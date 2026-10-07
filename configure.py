#!/usr/bin/env python3

###
# Generates build files for the project.
# This file also includes the project configuration,
# such as compiler flags and the object matching status.
#
# Usage:
#   python3 configure.py
#   ninja
#
# Append --help to see available options.
###

import argparse
import json
import sys
from pathlib import Path
from typing import Any, Dict, List

from tools.project import (
    Object,
    ProgressCategory,
    ProjectConfig,
    calculate_progress,
    generate_build,
    is_windows,
)

# Game versions
DEFAULT_VERSION = 0
VERSIONS = [
    "G2MEAB",  # 0
]

parser = argparse.ArgumentParser()
parser.add_argument(
    "mode",
    choices=["configure", "progress"],
    default="configure",
    help="script mode (default: configure)",
    nargs="?",
)
parser.add_argument(
    "-v",
    "--version",
    choices=VERSIONS,
    type=str.upper,
    default=VERSIONS[DEFAULT_VERSION],
    help="version to build",
)
parser.add_argument(
    "--build-dir",
    metavar="DIR",
    type=Path,
    default=Path("build"),
    help="base build directory (default: build)",
)
parser.add_argument(
    "--binutils",
    metavar="BINARY",
    type=Path,
    help="path to binutils (optional)",
)
parser.add_argument(
    "--compilers",
    metavar="DIR",
    type=Path,
    help="path to compilers (optional)",
)
parser.add_argument(
    "--map",
    action="store_true",
    help="generate map file(s)",
)
parser.add_argument(
    "--debug",
    action="store_true",
    help="build with debug info (non-matching)",
)
if not is_windows():
    parser.add_argument(
        "--wrapper",
        metavar="BINARY",
        type=Path,
        help="path to wibo or wine (optional)",
    )
parser.add_argument(
    "--dtk",
    metavar="BINARY | DIR",
    type=Path,
    help="path to decomp-toolkit binary or source (optional)",
)
parser.add_argument(
    "--objdiff",
    metavar="BINARY | DIR",
    type=Path,
    help="path to objdiff-cli binary or source (optional)",
)
parser.add_argument(
    "--sjiswrap",
    metavar="EXE",
    type=Path,
    help="path to sjiswrap.exe (optional)",
)
parser.add_argument(
    "--ninja",
    metavar="BINARY",
    type=Path,
    help="path to ninja binary (optional)",
)
parser.add_argument(
    "--verbose",
    action="store_true",
    help="print verbose output",
)
parser.add_argument(
    "--non-matching",
    dest="non_matching",
    action="store_true",
    help="builds equivalent (but non-matching) or modded objects",
)
parser.add_argument(
    "--warn",
    dest="warn",
    type=str,
    choices=["all", "off", "error"],
    help="how to handle warnings",
)
parser.add_argument(
    "--no-progress",
    dest="progress",
    action="store_false",
    help="disable progress calculation",
)
args = parser.parse_args()

config = ProjectConfig()
config.version = str(args.version)
version_num = VERSIONS.index(config.version)

# Apply arguments
config.build_dir = args.build_dir
config.dtk_path = args.dtk
config.objdiff_path = args.objdiff
config.binutils_path = args.binutils
config.compilers_path = args.compilers
config.generate_map = args.map
config.non_matching = args.non_matching
config.sjiswrap_path = args.sjiswrap
config.ninja_path = args.ninja
config.progress = args.progress
if not is_windows():
    config.wrapper = args.wrapper
# Don't build asm unless we're --non-matching
if not config.non_matching:
    config.asm_dir = None

# Tool versions
config.binutils_tag = "2.42-2"
config.compilers_tag = "20251118"
config.dtk_tag = "v1.8.3"
config.objdiff_tag = "v3.6.1"
config.sjiswrap_tag = "v1.2.2"
config.wibo_tag = "1.0.3"

# Project
config.config_path = Path("config") / config.version / "config.yml"
config.check_sha_path = Path("config") / config.version / "build.sha1"
config.asflags = [
    "-mgekko",
    "--strip-local-absolute",
    "-I include",
    f"-I build/{config.version}/include",
    f"--defsym BUILD_VERSION={version_num}",
]
config.ldflags = [
    "-fp hardware",
    "-nodefaults",
]
if args.debug:
    config.ldflags.append("-g")  # Or -gdwarf-2 for Wii linkers
if args.map:
    config.ldflags.append("-mapunused")
    # config.ldflags.append("-listclosure") # For Wii linkers

# Use for any additional files that should cause a re-configure when modified
config.reconfig_deps = []

# Optional numeric ID for decomp.me preset
# Can be overridden in libraries or objects
config.scratch_preset_id = None

# Base flags, common to most GC/Wii games.
# Generally leave untouched, with overrides added below.
cflags_base = [
    "-nodefaults",
    "-proc gekko",
    "-align powerpc",
    "-enum int",
    "-fp hardware",
    "-Cpp_exceptions off",
    "-O4,p",
    "-inline auto",
    '-pragma "cats off"',
    '-pragma "warn_notinlined off"',
    "-maxerrors 1",
    "-nosyspath",
    "-RTTI off",
    "-fp_contract on",
    "-str reuse",
    "-multibyte",  # For Wii compilers, replace with `-enc SJIS`
    "-i include",
    "-i include/libc",
    f"-i build/{config.version}/include",
    f"-DBUILD_VERSION={version_num}",
    f"-DVERSION_{config.version}",
    # dolsdk2004: April 20, 2004 SDK, May 21 Patch 1.
    "-DSDK_REVISION=1",
    "-D__GEKKO__",
]

# Debug flags
if args.debug:
    # Or -sym dwarf-2 for Wii compilers
    cflags_base.extend(["-sym on", "-DDEBUG=1"])
else:
    cflags_base.append("-DNDEBUG=1")

# Warning flags
if args.warn == "all":
    cflags_base.append("-W all")
elif args.warn == "off":
    cflags_base.append("-W off")
elif args.warn == "error":
    cflags_base.append("-W error")

# Metrowerks library flags
cflags_runtime = [
    *cflags_base,
    "-use_lmw_stmw on",
    "-str reuse,pool,readonly",
    "-gccinc",
    "-common off",
    "-inline auto",
]

# Retro game, Kyoto, and rstl code uses a separate compiler profile from the
# SDK/runtime libraries. Keep optimization and inlining independent of the
# generic flags so individual objects can be adjusted as they are matched.
cflags_retro = [
    *[flag for flag in cflags_base if flag not in ("-O4,p", "-inline auto", "-str reuse")],
    "-O4,p",
    "-str reuse,pool,readonly",
    "-use_lmw_stmw on",
    "-gccinc",
    "-inline deferred,noauto",
    "-common on",
]

# REL flags
cflags_rel = [
    *cflags_retro,
    "-sdata 0",
    "-sdata2 0",
]

config.linker_version = "GC/2.7"


# Imported from doldecomp/dolsdk2004 (see src/Dolphin/UPSTREAM).
# Keep these objects NonMatching until prototype-specific matching is done.
cflags_dolphin = [
    *cflags_base,
    "-char unsigned",
    "-ir src/Dolphin",
]


# Helper function for Dolphin libraries
def DolphinLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.2.5n",
        "cflags": cflags_dolphin,
        "progress_category": "sdk",
        "objects": objects,
    }


# Helper function for Metrowerks runtime libraries
def RuntimeLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        # Preserve the existing compiler profile independently of the linker.
        "mw_version": "GC/1.3.2",
        "cflags": cflags_runtime,
        "progress_category": "sdk",
        "objects": objects,
    }


# Tentative LZO compiler profile from Echoes; prototype optimization remains unverified.
def LzoLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/2.7",
        "cflags": cflags_runtime,
        "progress_category": "sdk",
        "objects": objects,
    }


# Provisional profile for FMOD scaffolds; historical compiler and flags are unverified.
# Keep this independent from the imported Dolphin SDK and Retro inlining settings.
cflags_fmod = [*cflags_base]


def FmodLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/2.7",
        "cflags": cflags_fmod,
        "progress_category": "sdk",
        "objects": objects,
    }


# Provisional bundled codec profiles; original compilers and flags remain unverified.
# Keep each library independent for later prototype-specific compilation checks.
cflags_ogg = [*cflags_base]
cflags_vorbis = [*cflags_base]


def OggLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/2.7",
        "cflags": cflags_ogg,
        "progress_category": "sdk",
        "objects": objects,
    }


def VorbisLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/2.7",
        "cflags": cflags_vorbis,
        "progress_category": "sdk",
        "objects": objects,
    }


# Lua C core settings; the LuaPlus C++ wrappers enable exceptions below.
cflags_lua = [
    *cflags_base,
    "-i include/Lua",
    "-DLUAPLUS_LIB",
    "-wchar_t off",
    "-requireprotos",
    "-use_lmw_stmw on",
    "-inline deferred,auto",
    "-str reuse,pool,readonly",
]


def LuaLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/2.7",
        "cflags": cflags_lua,
        "progress_category": "sdk",
        "objects": objects,
    }


# Helper function for Retro game, Kyoto, and rstl libraries
def RetroLib(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/2.7",
        "cflags": cflags_retro,
        "progress_category": "game",
        "objects": objects,
    }


# Helper function for REL script objects
def Rel(lib_name: str, objects: List[Object]) -> Dict[str, Any]:
    return {
        "lib": lib_name,
        "mw_version": "GC/1.3.2",
        "cflags": cflags_rel,
        "progress_category": "game",
        "objects": objects,
    }


Matching = True  # Object matches and should be linked
NonMatching = False  # Object does not match and should not be linked
Equivalent = config.non_matching  # Object should be linked when configured with --non-matching


# Object is only matching for specific versions
def MatchingFor(*versions):
    return config.version in versions


config.warn_missing_config = True
config.warn_missing_source = False
config.libs = [
    # SDK translation units investigated from the prototype.
    DolphinLib("AMCStubs", [
        Object(NonMatching, "Dolphin/amcstubs/AmcExi2Stubs.c"),
    ]),
    DolphinLib("AX", [
        Object(NonMatching, "Dolphin/ax/AX.c"),
        Object(NonMatching, "Dolphin/ax/AXAlloc.c"),
        Object(NonMatching, "Dolphin/ax/AXAux.c"),
        Object(NonMatching, "Dolphin/ax/AXCL.c"),
        Object(NonMatching, "Dolphin/ax/AXOut.c"),
        Object(NonMatching, "Dolphin/ax/AXSPB.c"),
        Object(NonMatching, "Dolphin/ax/AXVPB.c"),
        Object(NonMatching, "Dolphin/ax/AXProf.c"),
    ]),
    DolphinLib("AXFX", [
        Object(NonMatching, "Dolphin/axfx/AXFXReverbHi.c"),
        Object(NonMatching, "Dolphin/axfx/AXFXReverbStd.c"),
        Object(NonMatching, "Dolphin/axfx/AXFXHooks.c"),
        Object(NonMatching, "Dolphin/axfx/AXFXReverbHiDpl2.c"),
    ]),
    DolphinLib("Base64", [
        Object(NonMatching, "Dolphin/eth/base64.c"),
    ]),
    DolphinLib("Dolphin", [
        Object(NonMatching, "Dolphin/PPCArch.c"),
        Object(NonMatching, "Dolphin/os/OS.c"),
        Object(NonMatching, "Dolphin/os/OSAlarm.c"),
        Object(NonMatching, "Dolphin/os/OSAlloc.c"),
        Object(NonMatching, "Dolphin/os/OSArena.c"),
        Object(NonMatching, "Dolphin/os/OSAudioSystem.c"),
        Object(NonMatching, "Dolphin/os/OSCache.c"),
        Object(NonMatching, "Dolphin/os/OSContext.c"),
        Object(NonMatching, "Dolphin/os/OSError.c"),
        Object(NonMatching, "Dolphin/os/OSExec.c"),
        Object(NonMatching, "Dolphin/os/OSFatal.c"),
        Object(NonMatching, "Dolphin/os/OSFont.c"),
        Object(NonMatching, "Dolphin/os/OSInterrupt.c"),
        Object(NonMatching, "Dolphin/os/OSLink.c"),
        Object(NonMatching, "Dolphin/os/OSMemory.c"),
        Object(NonMatching, "Dolphin/os/OSMutex.c"),
        Object(NonMatching, "Dolphin/os/OSReboot.c"),
        Object(NonMatching, "Dolphin/os/OSReset.c"),
        Object(NonMatching, "Dolphin/os/OSResetSW.c"),
        Object(NonMatching, "Dolphin/os/OSRtc.c"),
        Object(NonMatching, "Dolphin/os/OSSem.c"),
        Object(NonMatching, "Dolphin/os/OSSync.c"),
        Object(NonMatching, "Dolphin/os/OSThread.c"),
        Object(NonMatching, "Dolphin/os/OSTime.c"),
        Object(NonMatching, "Dolphin/os/__ppc_eabi_init.cpp", extra_cflags=["-lang=c"]),
    ]),
    DolphinLib("ETH", [
        Object(NonMatching, "Dolphin/eth/eth.c"),
        Object(NonMatching, "Dolphin/eth/ethAuth.c"),
    ]),
    DolphinLib("HIO", [
        Object(NonMatching, "Dolphin/hio/hio.c"),
    ]),
    DolphinLib("IP", [
        Object(NonMatching, "Dolphin/ip/IPCommon.c"),
        Object(NonMatching, "Dolphin/ip/IPPacket.c"),
        Object(NonMatching, "Dolphin/ip/IPArp.c"),
        Object(NonMatching, "Dolphin/ip/IPIcmp.c"),
        Object(NonMatching, "Dolphin/ip/IPInterface.c"),
        Object(NonMatching, "Dolphin/ip/IPRoute.c"),
        Object(NonMatching, "Dolphin/ip/IPError.c"),
        Object(NonMatching, "Dolphin/ip/IPUdp.c"),
        Object(NonMatching, "Dolphin/ip/IPFragment.c"),
        Object(NonMatching, "Dolphin/ip/IPEthernet.c"),
        Object(NonMatching, "Dolphin/ip/IPRing.c"),
        Object(NonMatching, "Dolphin/ip/IPTcpInput.c"),
        Object(NonMatching, "Dolphin/ip/IPTcpOutput.c"),
        Object(NonMatching, "Dolphin/ip/IPTcpTimer.c"),
        Object(NonMatching, "Dolphin/ip/IPTcp.c"),
        Object(NonMatching, "Dolphin/ip/IPTcpTimeWait.c"),
        Object(NonMatching, "Dolphin/ip/IPDns.c"),
        Object(NonMatching, "Dolphin/ip/IPDhcp.c"),
        Object(NonMatching, "Dolphin/ip/IPAutoIP.c"),
        Object(NonMatching, "Dolphin/ip/IPOptions.c"),
        Object(NonMatching, "Dolphin/ip/IPSocket.c"),
        Object(NonMatching, "Dolphin/ip/PPP.c"),
        Object(NonMatching, "Dolphin/ip/PPPoE.c"),
        Object(NonMatching, "Dolphin/ip/PPPLcp.c"),
        Object(NonMatching, "Dolphin/ip/PPPIpcp.c"),
        Object(NonMatching, "Dolphin/ip/PPPPap.c"),
        Object(NonMatching, "Dolphin/ip/PPPChap.c"),
        Object(NonMatching, "Dolphin/ip/IPIgmp.c"),
    ]),
    DolphinLib("MCC", [
        Object(NonMatching, "Dolphin/mcc/mcc.c"),
        Object(NonMatching, "Dolphin/mcc/fio.c"),
        Object(NonMatching, "Dolphin/mcc/tty.c"),
    ]),
    DolphinLib("MD5", [
        Object(NonMatching, "Dolphin/eth/md5.c"),
    ]),
    DolphinLib("MIX", [
        Object(NonMatching, "Dolphin/mix/mix.c"),
    ]),
    DolphinLib("ODENotStub", [
        Object(NonMatching, "Dolphin/odenotstub/odenotstub.c"),
    ]),
    DolphinLib("OdemuExi2", [
        Object(NonMatching, "Dolphin/odemuexi2/DebuggerDriver.c"),
    ]),
    DolphinLib("Revolution", [
        Object(NonMatching, "Revolution/WPAD.c"),
        Object(NonMatching, "Revolution/KPAD.c"),
    ]),
    DolphinLib("ai", [
        Object(NonMatching, "Dolphin/ai.c"),
    ]),
    DolphinLib("ar", [
        Object(NonMatching, "Dolphin/ar/ar.c"),
        Object(NonMatching, "Dolphin/ar/arq.c"),
    ]),
    DolphinLib("card", [
        Object(NonMatching, "Dolphin/card/CARDBios.c"),
        Object(NonMatching, "Dolphin/card/CARDUnlock.c"),
        Object(NonMatching, "Dolphin/card/CARDRdwr.c"),
        Object(NonMatching, "Dolphin/card/CARDBlock.c"),
        Object(NonMatching, "Dolphin/card/CARDDir.c"),
        Object(NonMatching, "Dolphin/card/CARDCheck.c"),
        Object(NonMatching, "Dolphin/card/CARDMount.c"),
        Object(NonMatching, "Dolphin/card/CARDFormat.c"),
        Object(NonMatching, "Dolphin/card/CARDOpen.c"),
        Object(NonMatching, "Dolphin/card/CARDCreate.c"),
        Object(NonMatching, "Dolphin/card/CARDRead.c"),
        Object(NonMatching, "Dolphin/card/CARDWrite.c"),
        Object(NonMatching, "Dolphin/card/CARDDelete.c"),
        Object(NonMatching, "Dolphin/card/CARDStat.c"),
        Object(NonMatching, "Dolphin/card/CARDNet.c"),
    ]),
    DolphinLib("db", [
        Object(NonMatching, "Dolphin/db.c"),
    ]),
    DolphinLib("dsp", [
        Object(NonMatching, "Dolphin/dsp/dsp.c"),
        Object(NonMatching, "Dolphin/dsp/dsp_debug.c"),
        Object(NonMatching, "Dolphin/dsp/dsp_task.c"),
    ]),
    DolphinLib("dvd", [
        Object(NonMatching, "Dolphin/dvd/dvdlow.c"),
        Object(NonMatching, "Dolphin/dvd/dvdfs.c"),
        Object(NonMatching, "Dolphin/dvd/dvd.c"),
        Object(NonMatching, "Dolphin/dvd/dvdqueue.c"),
        Object(NonMatching, "Dolphin/dvd/dvderror.c"),
        Object(NonMatching, "Dolphin/dvd/dvdidutils.c"),
        Object(NonMatching, "Dolphin/dvd/dvdfatal.c"),
        Object(NonMatching, "Dolphin/dvd/fstload.c"),
    ]),
    DolphinLib("exi", [
        Object(NonMatching, "Dolphin/exi/EXIBios.c"),
        Object(NonMatching, "Dolphin/exi/EXIUart.c"),
    ]),
    DolphinLib("gx", [
        Object(NonMatching, "Dolphin/gx/GXInit.c"),
        Object(NonMatching, "Dolphin/gx/GXFifo.c"),
        Object(NonMatching, "Dolphin/gx/GXAttr.c"),
        Object(NonMatching, "Dolphin/gx/GXMisc.c"),
        Object(NonMatching, "Dolphin/gx/GXGeometry.c"),
        Object(NonMatching, "Dolphin/gx/GXFrameBuf.c"),
        Object(NonMatching, "Dolphin/gx/GXLight.c"),
        Object(NonMatching, "Dolphin/gx/GXTexture.c"),
        Object(NonMatching, "Dolphin/gx/GXBump.c"),
        Object(NonMatching, "Dolphin/gx/GXTev.c"),
        Object(NonMatching, "Dolphin/gx/GXPixel.c"),
        Object(NonMatching, "Dolphin/gx/GXDisplayList.c"),
        Object(NonMatching, "Dolphin/gx/GXTransform.c"),
        Object(NonMatching, "Dolphin/gx/GXPerf.c"),
    ]),
    DolphinLib("mtx", [
        Object(NonMatching, "Dolphin/mtx/mtx.c"),
        Object(NonMatching, "Dolphin/mtx/mtxvec.c"),
        Object(NonMatching, "Dolphin/mtx/mtx44.c"),
    ]),
    DolphinLib("si", [
        Object(NonMatching, "Dolphin/si/SIBios.c"),
        Object(NonMatching, "Dolphin/si/SISamplingRate.c"),
    ]),
    DolphinLib("thp", [
        Object(NonMatching, "Dolphin/thp/THPDec.c"),
        Object(NonMatching, "Dolphin/thp/THPAudio.c"),
    ]),
    DolphinLib("vi", [
        Object(NonMatching, "Dolphin/vi/vi.c"),
    ]),
    FmodLib("FMOD", [
        Object(NonMatching, "FMOD/fmod.cpp"),
        Object(NonMatching, "FMOD/fmod_async.cpp"),
        Object(NonMatching, "FMOD/fmod_channel.cpp"),
        Object(NonMatching, "FMOD/fmod_channel_emulated.cpp"),
        Object(NonMatching, "FMOD/fmod_channelbase.cpp"),
        Object(NonMatching, "FMOD/fmod_channeldsp.cpp"),
        Object(NonMatching, "FMOD/fmod_channelgroup.cpp"),
        Object(NonMatching, "FMOD/fmod_channelgroupi.cpp"),
        Object(NonMatching, "FMOD/fmod_channeli.cpp"),
        Object(NonMatching, "FMOD/fmod_channelpool.cpp"),
        Object(NonMatching, "FMOD/fmod_codec.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_aiff.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_dls.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_fsb.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_it.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_midi.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_mod.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_mpeg.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_oggvorbis.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_playlist.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_raw.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_s3m.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_tag.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_user.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_wav.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_wav_riff.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_xm.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_chorus.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_connection.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_connectionpool.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_distortion.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_echo.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_fft.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_filter.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_flange.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_highpass.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_itecho.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_itlowpass.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_lowpass.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_normalize.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_oscillator.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_parameq.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_pitchshift.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_resampler.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_reverb.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_soundcard.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_source.cpp"),
        Object(NonMatching, "FMOD/fmod_dspi.cpp"),
        Object(NonMatching, "FMOD/fmod_file.cpp"),
        Object(NonMatching, "FMOD/fmod_geometry.cpp"),
        Object(NonMatching, "FMOD/fmod_global.cpp"),
        Object(NonMatching, "FMOD/fmod_memory.cpp"),
        Object(NonMatching, "FMOD/fmod_metadata.cpp"),
        Object(NonMatching, "FMOD/fmod_music.cpp"),
        Object(NonMatching, "FMOD/fmod_octree.cpp"),
        Object(NonMatching, "FMOD/fmod_output.cpp"),
        Object(NonMatching, "FMOD/fmod_output_emulated.cpp"),
        Object(NonMatching, "FMOD/fmod_output_nosound.cpp"),
        Object(NonMatching, "FMOD/fmod_output_polling.cpp"),
        Object(NonMatching, "FMOD/fmod_output_software.cpp"),
        Object(NonMatching, "FMOD/fmod_plugin.cpp"),
        Object(NonMatching, "FMOD/fmod_pluginfactory.cpp"),
        Object(NonMatching, "FMOD/fmod_sample_software.cpp"),
        Object(NonMatching, "FMOD/fmod_sound.cpp"),
        Object(NonMatching, "FMOD/fmod_sound_sample.cpp"),
        Object(NonMatching, "FMOD/fmod_sound_stream.cpp"),
        Object(NonMatching, "FMOD/fmod_soundi.cpp"),
        Object(NonMatching, "FMOD/fmod_string.cpp"),
        Object(NonMatching, "FMOD/fmod_system.cpp"),
        Object(NonMatching, "FMOD/fmod_systemi.cpp"),
        Object(NonMatching, "FMOD/fmod_channel_gc.cpp"),
        Object(NonMatching, "FMOD/fmod_os_misc.cpp"),
        Object(NonMatching, "FMOD/fmod_output_gc.cpp"),
        Object(NonMatching, "FMOD/fmod_reverb_model.cpp"),
        Object(NonMatching, "FMOD/fmod_output_nosound_nrt.cpp"),
        Object(NonMatching, "FMOD/fmod_codec_gcadpcm.cpp"),
        Object(NonMatching, "FMOD/fmod_sample_converter.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_codec.cpp"),
        Object(NonMatching, "FMOD/fmod_dsp_codecpool.cpp"),
    ]),
    LuaLib("Lua", [
        Object(NonMatching, "Lua/lwstrlib.c"),
        Object(NonMatching, "Lua/LuaObject.cpp", extra_cflags=["-Cpp_exceptions on"]),
        Object(NonMatching, "Lua/LuaPlus.cpp", extra_cflags=["-Cpp_exceptions on"]),
        Object(Matching, "Lua/LuaPlusAddons.c"),
        Object(NonMatching, "Lua/LuaPlusFunctions.cpp", extra_cflags=["-Cpp_exceptions on"]),
        Object(NonMatching, "Lua/LuaState_DumpObject.cpp", extra_cflags=["-Cpp_exceptions on"]),
        Object(NonMatching, "Lua/lcode.c"),
        Object(NonMatching, "Lua/ldebug.c"),
        Object(NonMatching, "Lua/ldo.c"),
        Object(NonMatching, "Lua/ldump.c"),
        Object(Matching, "Lua/lfunc.c"),
        Object(Matching, "Lua/lgc.c"),
        Object(NonMatching, "Lua/llex.c"),
        Object(Matching, "Lua/lmem.c"),
        Object(NonMatching, "Lua/lobject.c"),
        Object(NonMatching, "Lua/lparser.c"),
        Object(NonMatching, "Lua/lstate.c"),
        Object(Matching, "Lua/lstring.c"),
        Object(NonMatching, "Lua/ltable.c"),
        Object(Matching, "Lua/ltm.c"),
        Object(NonMatching, "Lua/lundump.c"),
        Object(NonMatching, "Lua/lvm.c"),
        Object(NonMatching, "Lua/lzio.c"),
        Object(NonMatching, "Lua/lapi.c"),
        Object(NonMatching, "Lua/lbaselib.c"),
        Object(NonMatching, "Lua/ldblib.c"),
        Object(NonMatching, "Lua/lmathlib.c"),
        Object(NonMatching, "Lua/lstrlib.c"),
        Object(NonMatching, "Lua/ltablib.c"),
        Object(NonMatching, "Lua/lauxlib.c"),
        # Opcode tables have no established prototype data split yet.
        Object(NonMatching, "Lua/lopcodes.c", build_unlinked=True),
    ]),
    LzoLib("LZO", [
        Object(NonMatching, "LZO/lzo_init.c"),
        Object(NonMatching, "LZO/lzo_ptr.c"),
        Object(NonMatching, "LZO/lzo1x_d1.c"),
    ]),
    OggLib("Ogg", [
        Object(NonMatching, "Ogg/bitwise.c"),
        Object(NonMatching, "Ogg/framing.c"),
    ]),
    RetroLib("Game", [
        Object(NonMatching, "MetroidPrime/CTransitionDatabaseGame.cpp"),
        Object(NonMatching, "MetroidPrime/CAxisAngle.cpp"),
        Object(NonMatching, "MetroidPrime/CEulerAngles.cpp"),
        Object(NonMatching, "MetroidPrime/Collision/CJointCollisionDescription.cpp"),
    ]),
    RetroLib("Kyoto", [
        Object(NonMatching, "Kyoto/Graphics/CFoldySurface.cpp"),
        Object(NonMatching, "Collision/CCollidableAABox.cpp"),
        Object(NonMatching, "Collision/CCollisionInfo.cpp"),
        Object(NonMatching, "Collision/InternalColliders.cpp"),
        Object(NonMatching, "Collision/CCollisionPrimitive.cpp"),
        Object(NonMatching, "Collision/CMaterialList.cpp"),
        Object(NonMatching, "Collision/CollisionUtil.cpp"),
        Object(NonMatching, "Collision/CCollidableSphere.cpp"),
        Object(NonMatching, "Collision/CMaterialFilter.cpp"),
        Object(NonMatching, "Collision/COBBox.cpp"),
        Object(NonMatching, "Collision/CMRay.cpp"),
        Object(NonMatching, "Collision/CCharacterPrimitiveData.cpp"),
        Object(NonMatching, "Collision/CDelaunayTriangulation.cpp"),
        Object(NonMatching, "Collision/CCollidableOrientedBox.cpp"),
        Object(NonMatching, "Collision/CGjkSolver.cpp"),
        Object(NonMatching, "GuiSys/CAuiMeter.cpp"),
        Object(NonMatching, "GuiSys/CGuiCamera.cpp"),
        Object(NonMatching, "GuiSys/CGuiCompoundWidget.cpp"),
        Object(NonMatching, "GuiSys/CGuiFactories.cpp"),
        Object(NonMatching, "GuiSys/CGuiFrame.cpp"),
        Object(NonMatching, "GuiSys/CGuiHeadWidget.cpp"),
        Object(NonMatching, "GuiSys/CGuiLight.cpp"),
        Object(NonMatching, "GuiSys/CGuiModel.cpp"),
        Object(NonMatching, "GuiSys/CGuiObject.cpp"),
        Object(NonMatching, "GuiSys/CGuiPane.cpp"),
        Object(NonMatching, "GuiSys/CGuiSliderGroup.cpp"),
        Object(NonMatching, "GuiSys/CGuiTableGroup.cpp"),
        Object(NonMatching, "GuiSys/CGuiTextPane.cpp"),
        Object(NonMatching, "Kyoto/Text/CGuiTextSupport.cpp"),
        Object(NonMatching, "GuiSys/CGuiWidget.cpp"),
        Object(NonMatching, "GuiSys/CGuiWidgetIdDB.cpp"),
        Object(NonMatching, "GuiSys/CGuiWidgetDrawParms.cpp"),
        Object(NonMatching, "GuiSys/CAuiEnergyBarT01.cpp"),
        Object(NonMatching, "GuiSys/CAuiImagePane.cpp"),
        Object(NonMatching, "GuiSys/CRepeatState.cpp"),
        Object(NonMatching, "GuiSys/CAuiBarMeter.cpp"),
        Object(NonMatching, "GuiSys/DolphinCGuiFrameModels.cpp"),
        Object(NonMatching, "Kyoto/Basics/CBasics.cpp"),
        Object(NonMatching, "Kyoto/Basics/CStopwatch.cpp"),
        Object(NonMatching, "Kyoto/Basics/CBasicsDolphin.cpp"),
        Object(NonMatching, "Kyoto/Alloc/CCallStackDolphin.cpp"),
        Object(NonMatching, "Kyoto/Basics/COsContextDolphin.cpp"),
        Object(NonMatching, "Kyoto/Basics/CSWDataDolphin.cpp"),
        Object(NonMatching, "Kyoto/Basics/RAssertDolphin.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimation.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaAnimDatabase.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimationManager.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimSysContext.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeLoopIn.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeSequence.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaAnimBlend.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaAnimFactory.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaAnimPhaseBlend.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaAnimPlay.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaAnimRandom.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaAnimSequence.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaTransFactory.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaTransMetaAnim.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaTransPhaseTrans.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaTransSnap.cpp"),
        Object(NonMatching, "Kyoto/Animation/CMetaTransTrans.cpp"),
        Object(NonMatching, "Kyoto/Animation/CPASAnimInfo.cpp"),
        Object(NonMatching, "Kyoto/Animation/CPASAnimParm.cpp"),
        Object(NonMatching, "Kyoto/Animation/CPASAnimState.cpp"),
        Object(NonMatching, "Kyoto/Animation/CPASDatabase.cpp"),
        Object(NonMatching, "Kyoto/Animation/CPASParmInfo.cpp"),
        Object(NonMatching, "Kyoto/Animation/CPrimitive.cpp"),
        Object(NonMatching, "Kyoto/Animation/CSequenceHelper.cpp"),
        Object(NonMatching, "Kyoto/Animation/CTreeUtils.cpp"),
        Object(NonMatching, "Kyoto/Animation/IMetaAnim.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAdvancementDeltas.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimMathUtils.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimPOIData.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimSource.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimSourceReader.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimSourceReaderBase.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeAnimReaderContainer.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeBlend.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeDoubleChild.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeNode.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeSingleChild.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeTimeScale.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeTransition.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimTreeTweenBase.cpp"),
        Object(NonMatching, "Kyoto/Animation/CBoolPOINode.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCharAnimMemoryMetrics.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCharLayoutInfo.cpp"),
        Object(NonMatching, "Kyoto/Animation/CFBStreamedAnimReader.cpp"),
        Object(NonMatching, "Kyoto/Animation/CFBStreamedCompression.cpp"),
        Object(NonMatching, "Kyoto/Animation/CInt32POINode.cpp"),
        Object(NonMatching, "Kyoto/Animation/CParticlePOINode.cpp"),
        Object(NonMatching, "Kyoto/Animation/CPOINode.cpp"),
        Object(NonMatching, "Kyoto/Animation/CSegId.cpp"),
        Object(NonMatching, "Kyoto/Animation/CSegStatementSet.cpp"),
        Object(NonMatching, "Kyoto/Animation/CSteadyStateAnimInfo.cpp"),
        Object(NonMatching, "Kyoto/Animation/CTimeScaleFunctions.cpp"),
        Object(NonMatching, "Kyoto/Animation/IAnimReader.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAllFormatsAnimSource.cpp"),
        Object(NonMatching, "Kyoto/CDvdRequest.cpp"),
        Object(NonMatching, "Kyoto/Text/CColorInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CColorOverrideInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CDrawStringOptions.cpp"),
        Object(NonMatching, "Kyoto/Text/CFontInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CFontRenderState.cpp"),
        Object(NonMatching, "Kyoto/Text/CLineExtraSpaceInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CLineInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CLineSpacingInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CPopStateInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CPushStateInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CRasterFont.cpp"),
        Object(NonMatching, "Kyoto/Text/CRemoveColorOverrideInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CSaveableState.cpp"),
        Object(NonMatching, "Kyoto/Text/CTextParser.cpp"),
        Object(NonMatching, "Kyoto/Text/CWordInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CBlockInstruction.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CLight.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CCubeModel.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CGX.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CTevCombiners.cpp"),
        Object(NonMatching, "Kyoto/Graphics/DolphinCGraphics.cpp"),
        Object(NonMatching, "Kyoto/Graphics/DolphinCPalette.cpp"),
        Object(NonMatching, "Kyoto/Graphics/DolphinCTexture.cpp"),
        Object(Matching, "Kyoto/CCrc32.cpp"),
        Object(NonMatching, "Kyoto/Text/CStringTokenizer.cpp"),
        Object(NonMatching, "Kyoto/Alloc/CCircularBuffer.cpp"),
        Object(NonMatching, "Kyoto/Alloc/CMemory.cpp"),
        Object(NonMatching, "Kyoto/Alloc/IAllocator.cpp"),
        Object(NonMatching, "Kyoto/PVS/CPVSVisOctree.cpp"),
        Object(NonMatching, "Kyoto/PVS/CPVSVisSet.cpp"),
        Object(NonMatching, "Kyoto/Particles/CColorElement.cpp"),
        Object(NonMatching, "Kyoto/Particles/CElementGen.cpp"),
        Object(NonMatching, "Kyoto/Particles/CIntElement.cpp"),
        Object(NonMatching, "Kyoto/Particles/CModVectorElement.cpp"),
        Object(NonMatching, "Kyoto/Particles/CParticleDataFactory.cpp"),
        Object(NonMatching, "Kyoto/Particles/CParticleGlobals.cpp"),
        Object(NonMatching, "Kyoto/Particles/CParticleSwoosh.cpp"),
        Object(NonMatching, "Kyoto/Particles/CParticleSwooshDataFactory.cpp"),
        Object(NonMatching, "Kyoto/Particles/CRealElement.cpp"),
        Object(NonMatching, "Kyoto/Particles/CSpawnSystemKeyframeData.cpp"),
        Object(NonMatching, "Kyoto/Particles/CUVElement.cpp"),
        Object(NonMatching, "Kyoto/Particles/CVectorElement.cpp"),
        Object(NonMatching, "Kyoto/Math/CCylinder.cpp"),
        Object(NonMatching, "Kyoto/IObj.cpp"),
        Object(NonMatching, "Kyoto/CARAMManager.cpp"),
        Object(NonMatching, "Kyoto/Math/CFrustumPlanes.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CCubeMaterial.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CCubeSurface.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCharAnimTime.cpp"),
        Object(NonMatching, "Kyoto/Animation/CSegIdList.cpp"),
        Object(NonMatching, "Kyoto/Input/CFinalInput.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CColor.cpp"),
        Object(NonMatching, "Kyoto/DolphinCMemoryCardSys.cpp"),
        Object(NonMatching, "Kyoto/DolphinCDvdFile.cpp"),
        Object(NonMatching, "Kyoto/Alloc/CMediumAllocPool.cpp"),
        Object(NonMatching, "Kyoto/Alloc/CSmallAllocPool.cpp"),
        Object(NonMatching, "Kyoto/Alloc/CGameAllocator.cpp"),
        Object(NonMatching, "Kyoto/Animation/DolphinCSkinnedModel.cpp"),
        Object(NonMatching, "Kyoto/Animation/DolphinCSkinRules.cpp"),
        Object(NonMatching, "Kyoto/Animation/DolphinCVirtualBone.cpp"),
        Object(NonMatching, "Kyoto/Graphics/DolphinCModel.cpp"),
        Object(NonMatching, "Kyoto/Text/CStringTable.cpp"),
        Object(NonMatching, "Kyoto/Particles/CEmitterElement.cpp"),
        Object(NonMatching, "Kyoto/Animation/CNamedAnimPOIData.cpp"),
        Object(NonMatching, "Kyoto/CTimeProvider.cpp"),
        Object(NonMatching, "Kyoto/CARAMToken.cpp"),
        Object(NonMatching, "Kyoto/DolphinCFIOFileSupport.cpp"),
        Object(NonMatching, "Kyoto/Streams/CFIOOutStream.cpp"),
        Object(NonMatching, "Kyoto/Streams/CFIOInStream.cpp"),
        Object(NonMatching, "Kyoto/Text/CFontImageDef.cpp"),
        Object(NonMatching, "Kyoto/Text/CImageInstruction.cpp"),
        Object(NonMatching, "Kyoto/Text/CTextRenderBuffer.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CCubeMoviePlayer.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAdditiveAnimPlayback.cpp"),
        Object(NonMatching, "Kyoto/Particles/CParticleElectricDataFactory.cpp"),
        Object(NonMatching, "Kyoto/Particles/CParticleElectric.cpp"),
        Object(NonMatching, "Kyoto/Graphics/DolphinCColor.cpp"),
        Object(NonMatching, "Kyoto/CDependencyGroup.cpp"),
        Object(NonMatching, "Kyoto/Streams/CTextOutStream.cpp"),
        Object(NonMatching, "Kyoto/Streams/CTextInStream.cpp"),
        Object(NonMatching, "Kyoto/Audio/CStreamAudioManager.cpp"),
        Object(NonMatching, "Kyoto/Particles/CElectricDescription.cpp"),
        Object(NonMatching, "Kyoto/Particles/CSwooshDescription.cpp"),
        Object(NonMatching, "Kyoto/Particles/CGenDescription.cpp"),
        Object(NonMatching, "Kyoto/CPakFile.cpp"),
        Object(NonMatching, "Kyoto/Input/CRumbleVoice.cpp"),
        Object(NonMatching, "Kyoto/Input/RumbleAdsr.cpp"),
        Object(NonMatching, "Kyoto/Input/CRumbleGenerator.cpp"),
        Object(NonMatching, "Kyoto/Audio/g721.cpp"),
        Object(NonMatching, "Kyoto/Audio/DolphinCRSFAudio.cpp"),
        Object(NonMatching, "Kyoto/Audio/DolphinCAIInterruptManager.cpp"),
        Object(NonMatching, "Kyoto/CFrameDelayedKiller.cpp"),
        Object(NonMatching, "Kyoto/Animation/CTimeRemainderAndFraction.cpp"),
        Object(NonMatching, "Kyoto/Text/CCharacterExtraSpaceInstruction.cpp"),
        Object(NonMatching, "Kyoto/Network/CNetworkMessage.cpp"),
        Object(NonMatching, "Kyoto/Network/DolphinCBBACommon.cpp"),
        Object(NonMatching, "Kyoto/Network/CBBASupport.cpp"),
        Object(NonMatching, "Kyoto/Network/CBBACommon.cpp"),
        Object(NonMatching, "Kyoto/Streams/CBBAInStream.cpp"),
        Object(NonMatching, "Kyoto/Streams/CBBAOutStream.cpp"),
        Object(NonMatching, "Kyoto/Math/CMayaSpline.cpp"),
        Object(NonMatching, "Kyoto/Particles/CParticleSpawnSystemDataFactory.cpp"),
        Object(NonMatching, "Kyoto/Particles/CParticleSpawnSystem.cpp"),
        Object(NonMatching, "Kyoto/Particles/CSpawnSystemDescription.cpp"),
        Object(NonMatching, "Kyoto/Particles/CSortedParticleSystem.cpp"),
        Object(NonMatching, "Kyoto/Particles/CParticleSortedSystemDataFactory.cpp"),
        Object(NonMatching, "Kyoto/Particles/CSortedParticleSystemDescription.cpp"),
        Object(NonMatching, "Kyoto/Graphics/DolphinGPUMemory.cpp"),
        Object(NonMatching, "Kyoto/Math/CGameCameraSpline.cpp"),
        Object(NonMatching, "Kyoto/Math/CSpline.cpp"),
        Object(NonMatching, "Kyoto/Math/CMotionSpline.cpp"),
        Object(NonMatching, "Kyoto/Graphics/PortalPlane.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CDisplayListReader.cpp"),
        Object(NonMatching, "Kyoto/Audio/CDSPStreamManager.cpp"),
        Object(NonMatching, "Kyoto/Basics/CScopedProfiler.cpp"),
        Object(NonMatching, "Kyoto/Alloc/LockedCache.cpp"),
        Object(NonMatching, "Kyoto/Animation/CSoundPOINode.cpp"),
        Object(NonMatching, "Kyoto/Animation/CPoseAsTransforms_Linear.cpp"),
        Object(NonMatching, "Kyoto/Particles/CManagedParticleGen.cpp"),
        Object(NonMatching, "Kyoto/CRelFileDebugInfo.cpp"),
        Object(NonMatching, "Kyoto/Streams/CBitStreamWriter.cpp"),
        Object(NonMatching, "Kyoto/Streams/CBitStreamReader.cpp"),
        Object(NonMatching, "Kyoto/Streams/DolphinCInputStream.cpp"),
        Object(NonMatching, "Kyoto/Streams/DolphinCZipInputStream.cpp"),
        Object(NonMatching, "Kyoto/Streams/CLookaheadRes.cpp"),
        Object(NonMatching, "Kyoto/Streams/CLZOSupport.cpp"),
        Object(NonMatching, "Kyoto/Streams/DolphinCLZOInputStream.cpp"),
        Object(NonMatching, "Kyoto/Streams/CStreamPreloadedToken.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCompressedAnimSource.cpp"),
        Object(NonMatching, "Kyoto/Animation/CJointData_LinearStorage.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCompressedAnimSourceReader.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimChannelMask.cpp"),
        Object(NonMatching, "Kyoto/Animation/CQuantizedAnimFrame.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimBitStream.cpp"),
        Object(NonMatching, "Kyoto/Particles/CUserEvaluatorDescription.cpp"),
        Object(NonMatching, "Kyoto/Particles/CUserEvaluatorDataFactory.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAnimPOIDatabase.cpp"),
        Object(NonMatching, "Kyoto/Animation/CHalfTransition.cpp"),
        Object(NonMatching, "Kyoto/Animation/CTransition.cpp"),
        Object(NonMatching, "Kyoto/Animation/CParticleResData.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCECharacterInfo.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCEAnimationSet.cpp"),
        Object(NonMatching, "Kyoto/CUnpredictableRandom.cpp"),
        Object(NonMatching, "Kyoto/Particles/CBurstFireDescription.cpp"),
        Object(NonMatching, "Kyoto/Particles/CBurstFireDataFactory.cpp"),
        Object(NonMatching, "Kyoto/Graphics/CMaterialDataFactory.cpp"),
        Object(NonMatching, "Kyoto/Animation/CVisibilityAnimationData.cpp"),
        Object(NonMatching, "Kyoto/Animation/CVisibilityAnimationDataList.cpp"),
        Object(NonMatching, "Kyoto/Audio/CAudioHandle.cpp"),
        Object(NonMatching, "Kyoto/Audio/CAudioManager.cpp"),
        Object(NonMatching, "Kyoto/Audio/CAudioSampleDescription.cpp"),
        Object(NonMatching, "Kyoto/Audio/CAudioSample.cpp"),
        Object(NonMatching, "Kyoto/Audio/CAudioChannel.cpp"),
        Object(NonMatching, "Kyoto/Audio/CAudioSoundEffect.cpp"),
        Object(NonMatching, "Kyoto/Audio/CAudioVoice.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAssetSoundPOINode.cpp"),
        Object(NonMatching, "Kyoto/Audio/CSoundEvaluator.cpp"),
        Object(NonMatching, "Kyoto/Math/CAngularPIDController.cpp"),
        Object(NonMatching, "Kyoto/Animation/CAssetPOINode.cpp"),
        Object(NonMatching, "Kyoto/Input/RevolutionIController.cpp"),
        Object(NonMatching, "Kyoto/Input/CRevolutionController.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCECharacterFactoryBuilder.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCECharacterFactory.cpp"),
        Object(NonMatching, "Kyoto/Animation/CCharacterAudioManager.cpp"),
        Object(NonMatching, "Weapons/CProjectileWeapon.cpp"),
        Object(NonMatching, "Weapons/CProjectileWeaponDataFactory.cpp"),
        Object(NonMatching, "Weapons/CCollisionResponseData.cpp"),
        Object(NonMatching, "WorldFormat/CAreaOctTree_Tests.cpp"),
        Object(NonMatching, "WorldFormat/CCollisionSurface.cpp"),
        Object(NonMatching, "WorldFormat/CCollisionEdge.cpp"),
        Object(NonMatching, "WorldFormat/CMetroidModelInstance.cpp"),
        Object(NonMatching, "WorldFormat/DolphinCAreaOctTree.cpp"),
        Object(NonMatching, "WorldFormat/CMetroidAreaCollider.cpp"),
        Object(NonMatching, "WorldFormat/CWorldLight.cpp"),
        Object(NonMatching, "WorldFormat/COBBTree.cpp"),
        Object(NonMatching, "WorldFormat/CCollidableOBBTree.cpp"),
        Object(NonMatching, "WorldFormat/CCollidableOBBTreeGroup.cpp"),
        Object(NonMatching, "WorldFormat/CAreaPVS.cpp"),
        Object(NonMatching, "WorldFormat/CRenderingOctree.cpp"),
        Object(NonMatching, "WorldFormat/CGroupedFloatData.cpp"),
        Object(NonMatching, "WorldFormat/CGamePortalAreaData.cpp"),
        Object(NonMatching, "WorldFormat/CCollisionPrimitiveData.cpp"),
        Object(NonMatching, "WorldFormat/CTriangleCollisionCache.cpp"),
        Object(NonMatching, "WorldFormat/CCollidableTranslatedConvexPH.cpp"),
    ]),
    RetroLib("Kyoto.Math", [
        Object(NonMatching, "Kyoto/Math/CloseEnough.cpp"),
        Object(NonMatching, "Kyoto/Math/CMatrix3f.cpp"),
        Object(NonMatching, "Kyoto/Math/CMatrix4f.cpp"),
        Object(NonMatching, "Kyoto/Math/CNUQuaternion.cpp"),
        Object(NonMatching, "Kyoto/Math/CQuaternion.cpp"),
        Object(Matching, "Kyoto/CRandom16.cpp"),
        Object(NonMatching, "Kyoto/Math/CTransform4f.cpp"),
        Object(NonMatching, "Kyoto/Math/CUnitVector3f.cpp"),
        Object(NonMatching, "Kyoto/Math/CVector2d.cpp"),
        Object(NonMatching, "Kyoto/Math/CVector2f.cpp"),
        Object(Matching, "Kyoto/Math/CVector2i.cpp"),
        Object(NonMatching, "Kyoto/Math/CVector3d.cpp"),
        Object(NonMatching, "Kyoto/Math/CVector3f.cpp"),
        Object(Matching, "Kyoto/Math/CVector3i.cpp"),
        Object(NonMatching, "Kyoto/Math/RMathUtils.cpp"),
        Object(NonMatching, "Kyoto/Math/CLine.cpp"),
        Object(NonMatching, "Kyoto/Math/CLineSeg.cpp"),
        Object(NonMatching, "Kyoto/Math/CPlane.cpp"),
        Object(NonMatching, "Kyoto/Math/CTri.cpp"),
        Object(NonMatching, "Kyoto/Math/CQuad.cpp"),
        Object(NonMatching, "Kyoto/Math/CSphere.cpp"),
        Object(NonMatching, "Kyoto/Math/CAABox.cpp"),
    ]),
    RetroLib("Kyoto.Weapons", [
        Object(NonMatching, "Weapons/IWeaponRenderer.cpp"),
        Object(NonMatching, "Weapons/CDecalDataFactory.cpp"),
        Object(NonMatching, "Weapons/CDecal.cpp"),
        Object(NonMatching, "Weapons/CWeaponDescription.cpp"),
        Object(NonMatching, "Weapons/CDecalDescription.cpp"),
    ]),
    RetroLib("MetaRender", [
        Object(NonMatching, "MetaRender/CCubeRenderer.cpp"),
    ]),
    RetroLib("MetroidPrime", [
        Object(NonMatching, "MetroidPrime/main.cpp"),
        Object(NonMatching, "MetroidPrime/CControlMapper.cpp"),
        Object(NonMatching, "MetroidPrime/CArchMsgParmUserInput.cpp"),
        Object(NonMatching, "MetroidPrime/CInputGenerator.cpp"),
        Object(NonMatching, "MetroidPrime/CMainFlow.cpp"),
        Object(NonMatching, "MetroidPrime/CMFGame.cpp"),
        Object(NonMatching, "MetroidPrime/CPlayMovie.cpp"),
        Object(NonMatching, "MetroidPrime/CSplashScreen.cpp"),
        Object(NonMatching, "MetroidPrime/Tweaks/TweakGlobals.cpp"),
        Object(NonMatching, "MetroidPrime/Weapons/CGameProjectile.cpp"),
        Object(NonMatching, "MetroidPrime/CDbgDraw.cpp"),
        Object(NonMatching, "MetroidPrime/CArchMsgParmInt32.cpp"),
        Object(NonMatching, "MetroidPrime/CArchMsgParmInt32Int32VoidPtr.cpp"),
        Object(NonMatching, "MetroidPrime/CArchMsgParmNull.cpp"),
        Object(NonMatching, "MetroidPrime/CArchMsgParmReal32.cpp"),
        Object(NonMatching, "MetroidPrime/CIOWin.cpp"),
        Object(NonMatching, "MetroidPrime/CGameDebug.cpp"),
        Object(NonMatching, "MetroidPrime/CDebugMenu.cpp"),
        Object(NonMatching, "MetroidPrime/CArchMsgParmControllerStatus.cpp"),
        Object(NonMatching, "MetroidPrime/CExplosion.cpp"),
        Object(NonMatching, "MetroidPrime/CEffect.cpp"),
        Object(NonMatching, "MetroidPrime/HUD/CSamusHud.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptActor.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptTrigger.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptWaypoint.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CPatterned.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptDoor.cpp"),
        Object(NonMatching, "MetroidPrime/CMapArea.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptEffect.cpp"),
        Object(NonMatching, "MetroidPrime/Weapons/CBomb.cpp"),
        Object(NonMatching, "MetroidPrime/CGameDebugDraw.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerState.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptTimer.cpp"),
        Object(NonMatching, "MetroidPrime/CAutoMapper.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptCounter.cpp"),
        Object(NonMatching, "MetroidPrime/CMapWorld.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CAi.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptSound.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptPlatform.cpp"),
        Object(NonMatching, "MetroidPrime/UserNames.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptGenerator.cpp"),
        Object(NonMatching, "MetroidPrime/CGameLight.cpp"),
        Object(NonMatching, "MetroidPrime/CTargetReticles.cpp"),
        Object(NonMatching, "MetroidPrime/CWeaponMgr.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptPickup.cpp"),
        Object(NonMatching, "MetroidPrime/CDamageInfo.cpp"),
        Object(NonMatching, "MetroidPrime/CMemoryDrawEnum.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptDock.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptCameraHint.cpp"),
        Object(NonMatching, "MetroidPrime/CScriptMailbox.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptRelay.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptSpawnPoint.cpp"),
        Object(NonMatching, "MetroidPrime/HUD/CHUDMemoParms.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptHUDMemo.cpp"),
        Object(NonMatching, "MetroidPrime/CMappableObject.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerCameraBob.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptCameraFilterKeyframe.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptCameraBlurKeyframe.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CCameraFilter.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CMorphBall.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptDamageableTrigger.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptDebris.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptCameraShaker.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptActorKeyframe.cpp"),
        Object(NonMatching, "MetroidPrime/CConsoleOutputWindow.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptWater.cpp"),
        Object(NonMatching, "MetroidPrime/Weapons/CWeapon.cpp"),
        Object(NonMatching, "MetroidPrime/CDamageVulnerability.cpp"),
        Object(NonMatching, "MetroidPrime/CActorLights.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CPatternedInfo.cpp"),
        Object(NonMatching, "MetroidPrime/CSimpleShadow.cpp"),
        Object(NonMatching, "MetroidPrime/CActorParameters.cpp"),
        Object(NonMatching, "MetroidPrime/CInGameGuiManager.cpp"),
        Object(NonMatching, "MetroidPrime/CWorldShadow.cpp"),
        Object(NonMatching, "MetroidPrime/CAudioStateWin.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerVisor.cpp"),
        Object(NonMatching, "MetroidPrime/CModelData.cpp"),
        Object(NonMatching, "MetroidPrime/CDecalManager.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptSpiderBallWaypoint.cpp"),
        Object(NonMatching, "MetroidPrime/TGameTypes.cpp"),
        Object(NonMatching, "MetroidPrime/CPhysicsActor.cpp"),
        Object(NonMatching, "MetroidPrime/CPhysicsState.cpp"),
        Object(NonMatching, "MetroidPrime/CFluidUVMotion.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptCoverPoint.cpp"),
        Object(NonMatching, "MetroidPrime/CFluidPlane.cpp"),
        Object(NonMatching, "MetroidPrime/CFluidPlaneManager.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptGrapplePoint.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CHUDBillboardEffect.cpp"),
        Object(NonMatching, "MetroidPrime/CDebugOption.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBodyStateCmdMgr.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBodyStateInfo.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSAttack.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSDie.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSFall.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSGetup.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSKnockBack.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSLieOnGround.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSLocomotion.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSStep.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSTurn.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBodyController.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSLoopAttack.cpp"),
        Object(NonMatching, "MetroidPrime/Weapons/CTargetableProjectile.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSLoopReaction.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSGroundHit.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSSlide.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSHurled.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSCover.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSGenerate.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSProjectileAttack.cpp"),
        Object(NonMatching, "MetroidPrime/CSortedLists.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptSpiderBallAttractionSurface.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSScripted.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptDistanceFog.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSTaunt.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptSpecialFunction.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CSamusFaceReflection.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptPlayerHint.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptPointOfInterest.cpp"),
        Object(NonMatching, "MetroidPrime/CMapWorldInfo.cpp"),
        Object(NonMatching, "MetroidPrime/Factories/CScannableObjectInfo.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CScanDisplay.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptSteam.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptRipple.cpp"),
        Object(NonMatching, "MetroidPrime/CControllerRecorder.cpp"),
        Object(NonMatching, "MetroidPrime/CPlayerGuiManager.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptDamageableTriggerOrientated.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptRepulsor.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CABSPose.cpp"),
        Object(NonMatching, "MetroidPrime/CObjectListSmall.cpp"),
        Object(NonMatching, "MetroidPrime/CSurfacePathWalker.cpp"),
        Object(NonMatching, "MetroidPrime/CCollisionTracker.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CSwarmBasics.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptStreamedMovie.cpp"),
        Object(NonMatching, "MetroidPrime/Weapons/CControlHintProjectile.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptAIHint.cpp"),
        Object(NonMatching, "MetroidPrime/CControlHintManager.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerHints.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptControlHint.cpp"),
        Object(NonMatching, "MetroidPrime/CGameHint.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CHolsterKeeper.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptDestructibleBarrier.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptSoundModifier.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptLayerController.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CPlantScarabSwarm.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptSkyRipple.cpp"),
        Object(NonMatching, "MetroidPrime/HUD/CMannedTurretHud.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptFogOverlay.cpp"),
        Object(NonMatching, "MetroidPrime/CObjectTagToFilenameMapping.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptHUDHint.cpp"),
        Object(NonMatching, "MetroidPrime/CScriptObjectLoaderHelper.cpp"),
        Object(NonMatching, "MetroidPrime/SwarmRenderHelpers.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptGuiPlayerJoinManager.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CParasiteAiFunctions.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CWallWalker.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CGenericFSM2.cpp"),
        Object(NonMatching, "MetroidPrime/CStateManager.cpp"),
        Object(NonMatching, "MetroidPrime/CStateManagerObject.cpp"),
        Object(NonMatching, "MetroidPrime/CStateManagerCollision.cpp"),
        Object(NonMatching, "MetroidPrime/CRenderActor.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CDebugCamera.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CCameraShaker.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CCameraShakerData.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CCameraShakerManager.cpp"),
        Object(NonMatching, "MetroidPrime/CDisplayManager.cpp"),
        Object(NonMatching, "MetroidPrime/CRenderManager.cpp"),
        Object(NonMatching, "MetroidPrime/CViewportManager.cpp"),
        Object(NonMatching, "MetroidPrime/SViewportRenderData.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptActorTransform.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptRelayRandom.cpp"),
        Object(NonMatching, "MetroidPrime/Tweaks/CTweakPlayerRes.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CTransitionCamera.cpp"),
        Object(NonMatching, "MetroidPrime/CGameViewport.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptLUA.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerArms.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerBombController.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerFidget.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerLasso.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerLeftArm.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerRightArm.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerGrappleTargeting.cpp"),
        Object(NonMatching, "MetroidPrime/CElectricArcs.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CActorAiDebugRecorder.cpp"),
        Object(NonMatching, "MetroidPrime/CActorTriangleCollisionCache.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CGrappleBlock.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptEffectRepulsor.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CMysteryFlyer.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptPlayerUserAnimPoint.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CBallCamera.cpp"),
        Object(NonMatching, "MetroidPrime/CGameProfiler.cpp"),
        Object(NonMatching, "MetroidPrime/CShip.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptShipCommandIcon.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CDebugCameraBehavior.cpp"),
        Object(NonMatching, "MetroidPrime/PathFinding/CPathFindRegionAvoidance.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CKorbaSnatcher.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CKorbaMaw.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptShipCommandPath.cpp"),
        Object(NonMatching, "MetroidPrime/CRigidBody.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CNoseTurret.cpp"),
        Object(NonMatching, "MetroidPrime/CFootTracking.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptWeaponGenerator.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CPhazonLeech.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptGeneratedObjectDeleter.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptCrossAreaRelay.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptLoader.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CReptilicusHunter.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CBlinkWolf.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptShipProxy.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CMetroidHopper.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CSwarmBot.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptFalsePerspective.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CMantha.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CFacingNavigation.cpp"),
        Object(NonMatching, "MetroidPrime/Weapons/CReptilicusHunterChakram.cpp"),
        Object(NonMatching, "MetroidPrime/CSteeringBehaviors.cpp"),
        Object(NonMatching, "MetroidPrime/CTerrainAlignment.cpp"),
        Object(NonMatching, "MetroidPrime/CLineOfSightCache.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CWaypointNavigation.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CPathFindNavigation.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CSeedBoss1.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CMetroidHatcher.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CPirateDrone.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptCable.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CBerserker.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CDarkSamus.cpp"),
        Object(NonMatching, "MetroidPrime/Weapons/CSplitterBeamEffect.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CPhazonFlyerSwarm.cpp"),
        Object(NonMatching, "MetroidPrime/BodyState/CBSCodeDriven.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CSeedBoss1Orb.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CRidley1.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CSteamBot.cpp"),
        Object(NonMatching, "MetroidPrime/Weapons/CBeamProjectileManager.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CFlyingPirate.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CFishCloudModifier.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CSteamLord.cpp"),
        Object(NonMatching, "MetroidPrime/ScriptObjects/CScriptPositionRelay.cpp"),
        Object(NonMatching, "MetroidPrime/AudioDebug.cpp"),
        Object(NonMatching, "MetroidPrime/PathFinding/CPathFindRegionReservation.cpp"),
        Object(NonMatching, "MetroidPrime/Weapons/CAnimatedTargetableProjectile.cpp"),
        Object(NonMatching, "MetroidPrime/Player/CPlayerAimControl.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CSeedBoss1Hand.cpp"),
        Object(NonMatching, "MetroidPrime/CScriptMsgUtils.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CEyePod.cpp"),
        Object(NonMatching, "MetroidPrime/Enemies/CAtomicAlpha.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CCameraSurface.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CCylinderCameraSurface.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CSplineCylinderCameraSurface.cpp"),
        Object(NonMatching, "MetroidPrime/Cameras/CSplinePlaneCameraSurface.cpp"),
        Object(NonMatching, "MetroidPrime/CAnimData.cpp"),
        Object(NonMatching, "MetroidPrime/CParticleDatabase.cpp"),
        Object(NonMatching, "MetroidPrime/CParticleGenInfoGeneric.cpp"),
    ]),
    RuntimeLib("MetroTRK", [
        Object(NonMatching, "MetroTRK/mainloop.c"),
        Object(NonMatching, "MetroTRK/nubevent.c"),
        Object(NonMatching, "MetroTRK/nubinit.c"),
        Object(NonMatching, "MetroTRK/msg.c"),
        Object(NonMatching, "MetroTRK/msgbuf.c"),
        Object(NonMatching, "MetroTRK/serpoll.c"),
        Object(NonMatching, "MetroTRK/usr_put.c"),
        Object(NonMatching, "MetroTRK/dispatch.c"),
        Object(NonMatching, "MetroTRK/msghndlr.c"),
        Object(NonMatching, "MetroTRK/support.c"),
        Object(NonMatching, "MetroTRK/mutex_TRK.c"),
        Object(NonMatching, "MetroTRK/notify.c"),
        Object(NonMatching, "MetroTRK/flush_cache.c"),
        Object(NonMatching, "MetroTRK/mem_TRK.c"),
        Object(NonMatching, "MetroTRK/targimpl.c"),
        Object(NonMatching, "MetroTRK/targsupp.s"),
        Object(NonMatching, "MetroTRK/mpc_7xx_603e.c"),
        Object(NonMatching, "MetroTRK/dolphin_trk.c"),
        Object(NonMatching, "MetroTRK/main_TRK.c"),
        Object(NonMatching, "MetroTRK/dolphin_trk_glue.c"),
        Object(NonMatching, "MetroTRK/targcont.c"),
        Object(NonMatching, "MetroTRK/target_options.c"),
        Object(NonMatching, "MetroTRK/mslsupp.c"),
        Object(NonMatching, "MetroTRK/udp_cc.c"),
        Object(NonMatching, "MetroTRK/ddh_cc.c"),
        Object(NonMatching, "MetroTRK/circle_buffer.c"),
        Object(NonMatching, "MetroTRK/gdev_cc.c"),
        Object(NonMatching, "MetroTRK/MWTrace.c"),
        Object(NonMatching, "MetroTRK/critical_section.c"),
    ]),
    RuntimeLib("Runtime", [
        Object(NonMatching, "Runtime/__va_arg.c"),
        Object(NonMatching, "Runtime/global_destructor_chain.c"),
        Object(NonMatching, "Runtime/CPlusLibPPC.cpp"),
        Object(NonMatching, "Runtime/NMWException.cpp"),
        Object(NonMatching, "Runtime/ptmf.c"),
        Object(NonMatching, "Runtime/runtime.c"),
        Object(NonMatching, "Runtime/__init_cpp_exceptions.cpp"),
        Object(NonMatching, "Runtime/Gecko_ExceptionPPC.cpp"),
        Object(NonMatching, "Runtime/abort_exit.c"),
        Object(NonMatching, "Runtime/alloc.c"),
        Object(NonMatching, "Runtime/ansi_files.c"),
        Object(NonMatching, "Runtime/ansi_fp.c"),
        Object(NonMatching, "Runtime/arith.c"),
        Object(NonMatching, "Runtime/buffer_io.c"),
        Object(NonMatching, "Runtime/char_io.c"),
        Object(NonMatching, "Runtime/critical_regions.gamecube.c"),
        Object(NonMatching, "Runtime/ctype.c"),
        Object(NonMatching, "Runtime/direct_io.c"),
        Object(NonMatching, "Runtime/file_io.c"),
        Object(NonMatching, "Runtime/FILE_POS.c"),
        Object(NonMatching, "Runtime/mbstring.c"),
        Object(NonMatching, "Runtime/mem.c"),
        Object(NonMatching, "Runtime/mem_funcs.c"),
        Object(NonMatching, "Runtime/misc_io.c"),
        Object(NonMatching, "Runtime/printf.c"),
        Object(NonMatching, "Runtime/qsort.c"),
        Object(NonMatching, "Runtime/rand.c"),
        Object(NonMatching, "Runtime/sscanf.c"),
        Object(NonMatching, "Runtime/signal.c"),
        Object(NonMatching, "Runtime/strerror.c"),
        Object(NonMatching, "Runtime/string.c"),
        Object(NonMatching, "Runtime/strtold.c"),
        Object(NonMatching, "Runtime/strtoul.c"),
        Object(NonMatching, "Runtime/wstrtold.c"),
        Object(NonMatching, "Runtime/wstrtoul.c"),
        Object(NonMatching, "Runtime/wmem.c"),
        Object(NonMatching, "Runtime/wprintf.c"),
        Object(NonMatching, "Runtime/wscanf.c"),
        Object(NonMatching, "Runtime/wstring.c"),
        Object(NonMatching, "Runtime/wchar_io.c"),
        Object(NonMatching, "Runtime/uart_console_io.c"),
        Object(NonMatching, "Runtime/e_acos.c"),
        Object(NonMatching, "Runtime/e_asin.c"),
        Object(NonMatching, "Runtime/e_atan2.c"),
        Object(NonMatching, "Runtime/e_exp.c"),
        Object(NonMatching, "Runtime/e_fmod.c"),
        Object(NonMatching, "Runtime/e_log.c"),
        Object(NonMatching, "Runtime/e_log10.c"),
        Object(NonMatching, "Runtime/e_pow.c"),
        Object(NonMatching, "Runtime/e_rem_pio2.c"),
        Object(NonMatching, "Runtime/k_cos.c"),
        Object(NonMatching, "Runtime/k_rem_pio2.c"),
        Object(NonMatching, "Runtime/k_sin.c"),
        Object(NonMatching, "Runtime/k_tan.c"),
        Object(NonMatching, "Runtime/s_atan.c"),
        Object(NonMatching, "Runtime/s_ceil.c"),
        Object(NonMatching, "Runtime/s_copysign.c"),
        Object(NonMatching, "Runtime/s_cos.c"),
        Object(NonMatching, "Runtime/s_floor.c"),
        Object(NonMatching, "Runtime/s_frexp.c"),
        Object(NonMatching, "Runtime/s_ldexp.c"),
        Object(NonMatching, "Runtime/s_modf.c"),
        Object(NonMatching, "Runtime/s_nextafter.c"),
        Object(NonMatching, "Runtime/s_sin.c"),
        Object(NonMatching, "Runtime/s_tan.c"),
        Object(NonMatching, "Runtime/w_acos.c"),
        Object(NonMatching, "Runtime/w_asin.c"),
        Object(NonMatching, "Runtime/w_atan2.c"),
        Object(NonMatching, "Runtime/w_exp.c"),
        Object(NonMatching, "Runtime/w_fmod.c"),
        Object(NonMatching, "Runtime/w_log.c"),
        Object(NonMatching, "Runtime/w_log10.c"),
        Object(NonMatching, "Runtime/w_pow.c"),
        Object(NonMatching, "Runtime/e_sqrt.c"),
        Object(NonMatching, "Runtime/math_ppc.c"),
        Object(NonMatching, "Runtime/w_sqrt.c"),
        Object(NonMatching, "Runtime/stricmp.c"),
    ]),
    RuntimeLib("zlib", [
        Object(NonMatching, "zlib-1.1.3/adler32.c"),
        Object(NonMatching, "zlib-1.1.3/deflate.c"),
        Object(NonMatching, "zlib-1.1.3/infblock.c"),
        Object(NonMatching, "zlib-1.1.3/infcodes.c"),
        Object(NonMatching, "zlib-1.1.3/inffast.c"),
        Object(NonMatching, "zlib-1.1.3/inflate.c"),
        Object(NonMatching, "zlib-1.1.3/inftrees.c"),
        Object(NonMatching, "zlib-1.1.3/infutil.c"),
        Object(NonMatching, "zlib-1.1.3/trees.c"),
        Object(NonMatching, "zlib-1.1.3/zutil.c"),
    ]),
    VorbisLib("Vorbis", [
        Object(NonMatching, "Vorbis/block.c"),
        Object(NonMatching, "Vorbis/codebook.c"),
        Object(NonMatching, "Vorbis/envelope.c"),
        Object(NonMatching, "Vorbis/floor0.c"),
        Object(NonMatching, "Vorbis/floor1.c"),
        Object(NonMatching, "Vorbis/info.c"),
        Object(NonMatching, "Vorbis/lookup.c"),
        Object(NonMatching, "Vorbis/lsp.c"),
        Object(NonMatching, "Vorbis/mapping0.c"),
        Object(NonMatching, "Vorbis/mdct.c"),
        Object(NonMatching, "Vorbis/psy.c"),
        Object(NonMatching, "Vorbis/res0.c"),
        Object(NonMatching, "Vorbis/sharedbook.c"),
        Object(NonMatching, "Vorbis/smallft.c"),
        Object(NonMatching, "Vorbis/synthesis.c"),
        Object(NonMatching, "Vorbis/vorbisfile.c"),
        Object(NonMatching, "Vorbis/window.c"),
    ]),
    # End SDK translation units.
    RetroLib("Game",
             [
                 Object(NonMatching, "MetroidPrime/CObjectList.cpp"),
                 Object(NonMatching, "MetroidPrime/Player/CPlayer.cpp"),
                 Object(NonMatching, "MetroidPrime/CEntity.cpp"),
                 Object(NonMatching, "MetroidPrime/Decode.cpp"),
                 Object(NonMatching, "MetroidPrime/CIOWinManager.cpp"),
                 Object(NonMatching, "MetroidPrime/CActor.cpp"),
                 Object(NonMatching, "MetroidPrime/CWorld.cpp"),
                 Object(NonMatching, "MetroidPrime/CGameArea.cpp"),
        ],
    ),
    RetroLib("Kyoto1",
             [
                 Object(NonMatching, "Kyoto/Text/CTextExecuteBuffer.cpp"),
                 Object(NonMatching, "Kyoto/Text/CTextInstruction.cpp"),
                 Object(NonMatching, "Kyoto/CFactoryMgr.cpp"),
                 Object(NonMatching, "Kyoto/CResFactory.cpp"),
                 Object(NonMatching, "Kyoto/CResLoader.cpp"),
                 Object(NonMatching, "Kyoto/CObjectReference.cpp"),
                 Object(NonMatching, "Kyoto/CSimplePool.cpp"),
                 Object(Matching, "rstl/rstl_map.cpp"),
                 Object(Matching, "rstl/rstl_misc.cpp"),
                 Object(NonMatching, "rstl/rstl_strings.cpp"),
                 Object(NonMatching, "rstl/RstlExtras.cpp"),
             ],
    ),
    RetroLib(
        "Kyoto2",
        [
            Object(NonMatching, "Kyoto/Text/CFont.cpp"),
            Object(NonMatching, "Kyoto/CAssetTypesList.cpp"),
            Object(
                Matching,
                "Kyoto/CToken.cpp",
            ),
            Object(Matching, "Kyoto/Streams/CMemoryInStream.cpp"),
            Object(Matching, "Kyoto/Streams/CMemoryStreamOut.cpp"),
            Object(
                Matching,
                "Kyoto/Streams/COutputStream.cpp",
            ),
            Object(NonMatching, "Kyoto/Streams/CZipSupport.cpp"),
            Object(
                Matching,
                "Kyoto/Streams/CZipOutputStream.cpp",
            ),
        ],
    )
]


# Imported Kyoto sources use the normal project headers. Reference-only sources compile
# but cannot be linked until their prototype translation units are identified.
kyoto_manifest_path = Path("config/kyoto-imports.json")
config.reconfig_deps.append(kyoto_manifest_path)
kyoto_imports = json.loads(kyoto_manifest_path.read_text())
kyoto_flags = [kyoto_imports["cflags"], "-lang=c++", "-i include",
               "-i include/libc", "-i include/LZO", "-DSDK_REVISION=1", "-D__GEKKO__"]
kyoto_objects = {obj.name: obj for lib in config.libs for obj in lib["objects"]}
kyoto_references = []
for imported in kyoto_imports["files"]:
    name = imported["path"]
    if imported["mapped"]:
        obj = kyoto_objects[name]
        if obj.completed:
            raise ValueError(f"Import would override a prototype match: {name}")
    else:
        if name in kyoto_objects:
            raise ValueError(f"Reference unexpectedly has target configuration: {name}")
        obj = Object(NonMatching, name, build_unlinked=True)
        kyoto_references.append(obj)
    obj.options.update(cflags=kyoto_flags, extra_cflags=[], mw_version="GC/2.7")
    if name == "Kyoto/Math/CTransform4f.cpp":
        # The upstream unfinished GetInverse uses an uninitialized return
        # pointer. Keep the diagnostic visible without blocking this unlinked
        # NonMatching import; do not silently change the reference algorithm.
        obj.options["extra_cflags"] = ["-W noerror"]
config.libs.append(RetroLib("KyotoEchoesReferences", kyoto_references))


# Optional callback to adjust link order. This can be used to add, remove, or reorder objects.
# This is called once per module, with the module ID and the current link order.
#
# For example, this adds "dummy.c" to the end of the DOL link order if configured with --non-matching.
# "dummy.c" *must* be configured as a Matching (or Equivalent) object in order to be linked.
def link_order_callback(module_id: int, objects: List[str]) -> List[str]:
    # Don't modify the link order for matching builds
    if not config.non_matching:
        return objects
    if module_id == 0:  # DOL
        return objects + ["dummy.c"]
    return objects


# Uncomment to enable the link order callback.
# config.link_order_callback = link_order_callback


# Optional extra categories for progress tracking
# Adjust as desired for your project
config.progress_categories = [
    ProgressCategory("game", "Game Code"),
    ProgressCategory("sdk", "SDK Code"),
]
config.progress_each_module = args.verbose
# Optional extra arguments to `objdiff-cli report generate`
config.progress_report_args = [
    # Marks relocations as mismatching if the target value is different
    # Default is "functionRelocDiffs=none", which is most lenient
    # "--config functionRelocDiffs=data_value",
]

if args.mode == "configure":
    # Write build.ninja and objdiff.json
    generate_build(config)
elif args.mode == "progress":
    # Print progress information
    calculate_progress(config)
else:
    sys.exit("Unknown mode: " + args.mode)
