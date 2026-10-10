// Reconstructed from FMOD Ex 4.06.00 (PS3) debug information. Member layout and offsets are the 4.06 reference, not yet verified against G2MEAB.

#ifndef _FMOD_H
#define _FMOD_H

struct FMOD_ADVANCEDSETTINGS;
struct FMOD_CODEC_DESCRIPTION;
struct FMOD_CREATESOUNDEXINFO;
struct FMOD_DSP_DESCRIPTION;
struct FMOD_REVERB_CHANNELPROPERTIES;
struct FMOD_REVERB_PROPERTIES;
struct FMOD_TAG;
struct FMOD_VECTOR;
namespace FMOD {
    struct Reverb;
}

typedef int FMOD_BOOL;
typedef struct FMOD_SYSTEM FMOD_SYSTEM;
typedef struct FMOD_SOUND FMOD_SOUND;
typedef struct FMOD_CHANNEL FMOD_CHANNEL;
typedef struct FMOD_CHANNELGROUP FMOD_CHANNELGROUP;
typedef struct FMOD_DSP FMOD_DSP;
typedef struct FMOD_GEOMETRY FMOD_GEOMETRY;
typedef struct FMOD_SYNCPOINT FMOD_SYNCPOINT;
typedef unsigned int FMOD_MODE;
#ifndef FMOD_DEFAULT
// FMOD Ex mode bits. G2MEAB evidence: FMOD_3D (Output::getFreeChannel 0x8060ED74), FMOD_OPENMEMORY
// (AsyncThread::threadFunc), FMOD_ACCURATETIME (CodecMPEG::setPositionInternal).
#define FMOD_DEFAULT 0x00000000
#define FMOD_LOOP_OFF 0x00000001
#define FMOD_LOOP_NORMAL 0x00000002
#define FMOD_LOOP_BIDI 0x00000004
#define FMOD_2D 0x00000008
#define FMOD_3D 0x00000010
#define FMOD_HARDWARE 0x00000020
#define FMOD_SOFTWARE 0x00000040
#define FMOD_CREATESTREAM 0x00000080
#define FMOD_CREATESAMPLE 0x00000100
#define FMOD_CREATECOMPRESSEDSAMPLE 0x00000200
#define FMOD_OPENUSER 0x00000400
#define FMOD_OPENMEMORY 0x00000800
#define FMOD_OPENRAW 0x00001000
#define FMOD_OPENONLY 0x00002000
#define FMOD_ACCURATETIME 0x00004000
#define FMOD_MPEGSEARCH 0x00008000
#define FMOD_NONBLOCKING 0x00010000
#define FMOD_IGNORETAGS 0x02000000 // CodecTag::openInternal 0x805E73D4
#endif
typedef unsigned int FMOD_TIMEUNIT;
typedef unsigned int FMOD_INITFLAGS;
typedef unsigned int FMOD_CAPS;
#ifndef FMOD_CAPS_NONE
// FMOD Ex capability bits; OutputNoSound::getDriverCaps 0x8060F2BC sets 0x4..0x80.
#define FMOD_CAPS_NONE 0x00000000
#define FMOD_CAPS_HARDWARE 0x00000001
#define FMOD_CAPS_HARDWARE_EMULATED 0x00000002
#define FMOD_CAPS_OUTPUT_MULTICHANNEL 0x00000004
#define FMOD_CAPS_OUTPUT_FORMAT_PCM8 0x00000008
#define FMOD_CAPS_OUTPUT_FORMAT_PCM16 0x00000010
#define FMOD_CAPS_OUTPUT_FORMAT_PCM24 0x00000020
#define FMOD_CAPS_OUTPUT_FORMAT_PCM32 0x00000040
#define FMOD_CAPS_OUTPUT_FORMAT_PCMFLOAT 0x00000080
#endif
typedef unsigned int FMOD_DEBUGLEVEL;
typedef unsigned int FMOD_MEMORY_TYPE;

#ifndef FMOD_TIMEUNIT_MS
// FMOD Ex time unit and 3D mode bits. G2MEAB evidence: ChannelReal::getPosition 0x805B7320 (sentence
// units, BUFFERED stripped), ChannelReal::setMode 0x805B77CC (3D relative/rolloff bits).
#define FMOD_TIMEUNIT_MS 0x00000001
#define FMOD_TIMEUNIT_PCM 0x00000002
#define FMOD_TIMEUNIT_PCMBYTES 0x00000004
#define FMOD_TIMEUNIT_RAWBYTES 0x00000008
#define FMOD_INIT_SOFTWARE_DISABLE 0x00000004 // SystemI::createDSP 0x80620490 tests bit 2
#ifndef FMOD_INIT_3D_RIGHTHANDED
#define FMOD_INIT_3D_RIGHTHANDED 0x00000002 // ChannelI::calcVolumeAndPitchFor3D 0x805BD588 negates z on bit 1 (group B)
#endif
#define FMOD_TIMEUNIT_SENTENCE_MS 0x00010000
#define FMOD_TIMEUNIT_SENTENCE_PCM 0x00020000
#define FMOD_TIMEUNIT_SENTENCE_PCMBYTES 0x00040000
#define FMOD_TIMEUNIT_SENTENCE 0x00080000
#define FMOD_TIMEUNIT_SENTENCE_SUBSOUND 0x00100000
#define FMOD_TIMEUNIT_BUFFERED 0x10000000
#define FMOD_3D_HEADRELATIVE 0x00040000
#define FMOD_3D_WORLDRELATIVE 0x00080000
#define FMOD_3D_LOGROLLOFF 0x00100000
#define FMOD_3D_LINEARROLLOFF 0x00200000
#define FMOD_3D_CUSTOMROLLOFF 0x04000000
#endif
#define FMOD_TIMEUNIT_MODORDER 0x00000100
#define FMOD_TIMEUNIT_MODROW 0x00000200
#define FMOD_TIMEUNIT_MODPATTERN 0x00000400
struct FMOD_VECTOR
{
    float x; // offset 0x0
    float y; // offset 0x4
    float z; // offset 0x8
};

// G2MEAB values, derived from native return sites (see research FMOD.md). Compared with
// FMOD Ex 4.06 this older enum lacks FILE_UNWANTED, INVALID_ADDRESS, INVALID_FLOAT and SUBSOUNDS,
// and probably INVALID_VECTOR and MEMORY_CANTPOINT. Codes after VERSION are unobserved.
enum FMOD_RESULT {
    FMOD_OK, // 0
    FMOD_ERR_ALREADYLOCKED, // 1
    FMOD_ERR_BADCOMMAND, // 2
    FMOD_ERR_CDDA_DRIVERS, // 3
    FMOD_ERR_CDDA_INIT, // 4
    FMOD_ERR_CDDA_INVALID_DEVICE, // 5
    FMOD_ERR_CDDA_NOAUDIO, // 6
    FMOD_ERR_CDDA_NODEVICES, // 7
    FMOD_ERR_CDDA_NODISC, // 8
    FMOD_ERR_CDDA_READ, // 9
    FMOD_ERR_CHANNEL_ALLOC, // 10
    FMOD_ERR_CHANNEL_STOLEN, // 11
    FMOD_ERR_COM, // 12
    FMOD_ERR_DMA, // 13
    FMOD_ERR_DSP_CONNECTION, // 14
    FMOD_ERR_DSP_FORMAT, // 15
    FMOD_ERR_DSP_NOTFOUND, // 16
    FMOD_ERR_DSP_RUNNING, // 17
    FMOD_ERR_DSP_TOOMANYCONNECTIONS, // 18
    FMOD_ERR_FILE_BAD, // 19
    FMOD_ERR_FILE_COULDNOTSEEK, // 20
    FMOD_ERR_FILE_EOF, // 21
    FMOD_ERR_FILE_NOTFOUND, // 22
    FMOD_ERR_FORMAT, // 23
    FMOD_ERR_HTTP, // 24
    FMOD_ERR_HTTP_ACCESS, // 25
    FMOD_ERR_HTTP_PROXY_AUTH, // 26
    FMOD_ERR_HTTP_SERVER_ERROR, // 27
    FMOD_ERR_HTTP_TIMEOUT, // 28
    FMOD_ERR_INITIALIZATION, // 29
    FMOD_ERR_INITIALIZED, // 30
    FMOD_ERR_INTERNAL, // 31
    FMOD_ERR_INVALID_HANDLE, // 32
    FMOD_ERR_INVALID_PARAM, // 33
    FMOD_ERR_INVALID_SPEAKER, // 34
    FMOD_ERR_IRX, // 35; probable: IRX or INVALID_VECTOR; value unobserved
    FMOD_ERR_MEMORY, // 36
    FMOD_ERR_MEMORY_IOP, // 37; probable: two of MEMORY_IOP/SRAM/CANTPOINT; values unobserved
    FMOD_ERR_MEMORY_SRAM, // 38; probable: two of MEMORY_IOP/SRAM/CANTPOINT; values unobserved
    FMOD_ERR_NEEDS2D, // 39
    FMOD_ERR_NEEDS3D, // 40
    FMOD_ERR_NEEDSHARDWARE, // 41
    FMOD_ERR_NEEDSSOFTWARE, // 42
    FMOD_ERR_NET_CONNECT, // 43
    FMOD_ERR_NET_SOCKET_ERROR, // 44
    FMOD_ERR_NET_URL, // 45
    FMOD_ERR_NOTREADY, // 46
    FMOD_ERR_OUTPUT_ALLOCATED, // 47
    FMOD_ERR_OUTPUT_CREATEBUFFER, // 48
    FMOD_ERR_OUTPUT_DRIVERCALL, // 49
    FMOD_ERR_OUTPUT_FORMAT, // 50
    FMOD_ERR_OUTPUT_INIT, // 51
    FMOD_ERR_OUTPUT_NOHARDWARE, // 52
    FMOD_ERR_OUTPUT_NOSOFTWARE, // 53
    FMOD_ERR_PAN, // 54
    FMOD_ERR_PLUGIN, // 55
    FMOD_ERR_PLUGIN_MISSING, // 56
    FMOD_ERR_PLUGIN_RESOURCE, // 57
    FMOD_ERR_RECORD, // 58
    FMOD_ERR_REVERB_INSTANCE, // 59
    FMOD_ERR_SUBSOUND_ALLOCATED, // 60
    FMOD_ERR_TAGNOTFOUND, // 61
    FMOD_ERR_TOOMANYCHANNELS, // 62
    FMOD_ERR_UNIMPLEMENTED, // 63
    FMOD_ERR_UNINITIALIZED, // 64
    FMOD_ERR_UNSUPPORTED, // 65
    FMOD_ERR_UPDATE, // 66
    FMOD_ERR_VERSION, // 67
    FMOD_ERR_EVENT_FAILED, // 68
    FMOD_ERR_EVENT_INTERNAL, // 69
    FMOD_ERR_EVENT_INFOONLY, // 70
    FMOD_ERR_EVENT_NAMECONFLICT, // 71
    FMOD_ERR_EVENT_NOTFOUND, // 72
    FMOD_RESULT_FORCEINT = 65536
};

enum FMOD_OUTPUTTYPE {
    FMOD_OUTPUTTYPE_AUTODETECT = 0,
    FMOD_OUTPUTTYPE_UNKNOWN = 1,
    FMOD_OUTPUTTYPE_NOSOUND = 2,
    FMOD_OUTPUTTYPE_WAVWRITER = 3,
    FMOD_OUTPUTTYPE_NOSOUND_NRT = 4,
    FMOD_OUTPUTTYPE_WAVWRITER_NRT = 5,
    FMOD_OUTPUTTYPE_DSOUND = 6,
    FMOD_OUTPUTTYPE_WINMM = 7,
    FMOD_OUTPUTTYPE_ASIO = 8,
    FMOD_OUTPUTTYPE_OSS = 9,
    FMOD_OUTPUTTYPE_ALSA = 10,
    FMOD_OUTPUTTYPE_ESD = 11,
    FMOD_OUTPUTTYPE_SOUNDMANAGER = 12,
    FMOD_OUTPUTTYPE_COREAUDIO = 13,
    FMOD_OUTPUTTYPE_XBOX = 14,
    FMOD_OUTPUTTYPE_PS2 = 15,
    FMOD_OUTPUTTYPE_PS3 = 16,
    FMOD_OUTPUTTYPE_GC = 17,
    FMOD_OUTPUTTYPE_XBOX360 = 18,
    FMOD_OUTPUTTYPE_PSP = 19,
    FMOD_OUTPUTTYPE_WII = 20,
    FMOD_OUTPUTTYPE_MAX = 21,
    FMOD_OUTPUTTYPE_FORCEINT = 65536
};

enum FMOD_SPEAKERMODE {
    FMOD_SPEAKERMODE_RAW = 0,
    FMOD_SPEAKERMODE_MONO = 1,
    FMOD_SPEAKERMODE_STEREO = 2,
    FMOD_SPEAKERMODE_QUAD = 3,
    FMOD_SPEAKERMODE_SURROUND = 4,
    FMOD_SPEAKERMODE_5POINT1 = 5,
    FMOD_SPEAKERMODE_7POINT1 = 6,
    FMOD_SPEAKERMODE_PROLOGIC = 7,
    FMOD_SPEAKERMODE_MAX = 8,
    FMOD_SPEAKERMODE_FORCEINT = 65536
};

enum FMOD_SPEAKER {
    FMOD_SPEAKER_FRONT_LEFT = 0,
    FMOD_SPEAKER_FRONT_RIGHT = 1,
    FMOD_SPEAKER_FRONT_CENTER = 2,
    FMOD_SPEAKER_LOW_FREQUENCY = 3,
    FMOD_SPEAKER_BACK_LEFT = 4,
    FMOD_SPEAKER_BACK_RIGHT = 5,
    FMOD_SPEAKER_SIDE_LEFT = 6,
    FMOD_SPEAKER_SIDE_RIGHT = 7,
    FMOD_SPEAKER_MAX = 8,
    FMOD_SPEAKER_MONO = 0,
    FMOD_SPEAKER_BACK_CENTER = 3,
    FMOD_SPEAKER_FORCEINT = 65536
};

enum FMOD_PLUGINTYPE {
    FMOD_PLUGINTYPE_OUTPUT = 0,
    FMOD_PLUGINTYPE_CODEC = 1,
    FMOD_PLUGINTYPE_DSP = 2,
    FMOD_PLUGINTYPE_MAX = 3,
    FMOD_PLUGINTYPE_FORCEINT = 65536
};

enum FMOD_SOUND_TYPE {
    FMOD_SOUND_TYPE_UNKNOWN = 0,
    FMOD_SOUND_TYPE_AAC = 1,
    FMOD_SOUND_TYPE_AIFF = 2,
    FMOD_SOUND_TYPE_ASF = 3,
    FMOD_SOUND_TYPE_AT3 = 4,
    FMOD_SOUND_TYPE_CDDA = 5,
    FMOD_SOUND_TYPE_DLS = 6,
    FMOD_SOUND_TYPE_FLAC = 7,
    FMOD_SOUND_TYPE_FSB = 8,
    FMOD_SOUND_TYPE_GCADPCM = 9,
    FMOD_SOUND_TYPE_IT = 10,
    FMOD_SOUND_TYPE_MIDI = 11,
    FMOD_SOUND_TYPE_MOD = 12,
    FMOD_SOUND_TYPE_MPEG = 13,
    FMOD_SOUND_TYPE_OGGVORBIS = 14,
    FMOD_SOUND_TYPE_PLAYLIST = 15,
    FMOD_SOUND_TYPE_RAW = 16,
    FMOD_SOUND_TYPE_S3M = 17,
    FMOD_SOUND_TYPE_SF2 = 18,
    FMOD_SOUND_TYPE_USER = 19,
    FMOD_SOUND_TYPE_WAV = 20,
    FMOD_SOUND_TYPE_XM = 21,
    FMOD_SOUND_TYPE_XMA = 22,
    FMOD_SOUND_TYPE_VAG = 23,
    FMOD_SOUND_TYPE_MAX = 24,
    FMOD_SOUND_TYPE_FORCEINT = 65536
};

enum FMOD_SOUND_FORMAT {
    FMOD_SOUND_FORMAT_NONE = 0,
    FMOD_SOUND_FORMAT_PCM8 = 1,
    FMOD_SOUND_FORMAT_PCM16 = 2,
    FMOD_SOUND_FORMAT_PCM24 = 3,
    FMOD_SOUND_FORMAT_PCM32 = 4,
    FMOD_SOUND_FORMAT_PCMFLOAT = 5,
    FMOD_SOUND_FORMAT_GCADPCM = 6,
    FMOD_SOUND_FORMAT_IMAADPCM = 7,
    FMOD_SOUND_FORMAT_VAG = 8,
    FMOD_SOUND_FORMAT_XMA = 9,
    FMOD_SOUND_FORMAT_MPEG = 10,
    FMOD_SOUND_FORMAT_MAX = 11,
    FMOD_SOUND_FORMAT_FORCEINT = 65536
};

enum FMOD_OPENSTATE {
    FMOD_OPENSTATE_READY = 0,
    FMOD_OPENSTATE_LOADING = 1,
    FMOD_OPENSTATE_ERROR = 2,
    FMOD_OPENSTATE_CONNECTING = 3,
    FMOD_OPENSTATE_BUFFERING = 4,
    FMOD_OPENSTATE_SEEKING = 5,
    FMOD_OPENSTATE_MAX = 6,
    FMOD_OPENSTATE_FORCEINT = 65536
};

enum FMOD_CHANNEL_CALLBACKTYPE {
    FMOD_CHANNEL_CALLBACKTYPE_END = 0,
    FMOD_CHANNEL_CALLBACKTYPE_VIRTUALVOICE = 1,
    FMOD_CHANNEL_CALLBACKTYPE_SYNCPOINT = 2,
    FMOD_CHANNEL_CALLBACKTYPE_MAX = 3,
    FMOD_CHANNEL_CALLBACKTYPE_FORCEINT = 65536
};

typedef FMOD_RESULT (* FMOD_CHANNEL_CALLBACK)(FMOD_CHANNEL *, FMOD_CHANNEL_CALLBACKTYPE, int, unsigned int, unsigned int);
typedef FMOD_RESULT (* FMOD_SOUND_NONBLOCKCALLBACK)(FMOD_SOUND *, FMOD_RESULT);
typedef FMOD_RESULT (* FMOD_SOUND_PCMREADCALLBACK)(FMOD_SOUND *, void *, unsigned int);
typedef FMOD_RESULT (* FMOD_SOUND_PCMSETPOSCALLBACK)(FMOD_SOUND *, int, unsigned int, FMOD_TIMEUNIT);
typedef FMOD_RESULT (* FMOD_FILE_OPENCALLBACK)(const char *, int, unsigned int *, void * *, void * *);
typedef FMOD_RESULT (* FMOD_FILE_CLOSECALLBACK)(void *, void *);
typedef FMOD_RESULT (* FMOD_FILE_READCALLBACK)(void *, void *, unsigned int, unsigned int *, void *);
typedef FMOD_RESULT (* FMOD_FILE_SEEKCALLBACK)(void *, unsigned int, void *);
// G2MEAB: MemPool::alloc/realloc/free 0x8060BB58/0x8060BEE0/0x8060BDB0 call these with no memory type.
typedef void * (* FMOD_MEMORY_ALLOCCALLBACK)(unsigned int);
typedef void * (* FMOD_MEMORY_REALLOCCALLBACK)(void *, unsigned int);
typedef void (* FMOD_MEMORY_FREECALLBACK)(void *);
enum FMOD_DSP_FFT_WINDOW {
    FMOD_DSP_FFT_WINDOW_RECT = 0,
    FMOD_DSP_FFT_WINDOW_TRIANGLE = 1,
    FMOD_DSP_FFT_WINDOW_HAMMING = 2,
    FMOD_DSP_FFT_WINDOW_HANNING = 3,
    FMOD_DSP_FFT_WINDOW_BLACKMAN = 4,
    FMOD_DSP_FFT_WINDOW_BLACKMANHARRIS = 5,
    FMOD_DSP_FFT_WINDOW_MAX = 6,
    FMOD_DSP_FFT_WINDOW_FORCEINT = 65536
};

enum FMOD_DSP_RESAMPLER {
    FMOD_DSP_RESAMPLER_NOINTERP = 0,
    FMOD_DSP_RESAMPLER_LINEAR = 1,
    FMOD_DSP_RESAMPLER_CUBIC = 2,
    FMOD_DSP_RESAMPLER_SPLINE = 3,
    FMOD_DSP_RESAMPLER_MAX = 4,
    FMOD_DSP_RESAMPLER_FORCEINT = 65536
};

enum FMOD_TAGTYPE {
    FMOD_TAGTYPE_UNKNOWN = 0,
    FMOD_TAGTYPE_ID3V1 = 1,
    FMOD_TAGTYPE_ID3V2 = 2,
    FMOD_TAGTYPE_VORBISCOMMENT = 3,
    FMOD_TAGTYPE_SHOUTCAST = 4,
    FMOD_TAGTYPE_ICECAST = 5,
    FMOD_TAGTYPE_ASF = 6,
    FMOD_TAGTYPE_MIDI = 7,
    FMOD_TAGTYPE_PLAYLIST = 8,
    FMOD_TAGTYPE_FMOD = 9,
    FMOD_TAGTYPE_USER = 10,
    FMOD_TAGTYPE_MAX = 11,
    FMOD_TAGTYPE_FORCEINT = 65536
};

enum FMOD_TAGDATATYPE {
    FMOD_TAGDATATYPE_BINARY = 0,
    FMOD_TAGDATATYPE_INT = 1,
    FMOD_TAGDATATYPE_FLOAT = 2,
    FMOD_TAGDATATYPE_STRING = 3,
    FMOD_TAGDATATYPE_STRING_UTF16 = 4,
    FMOD_TAGDATATYPE_STRING_UTF16BE = 5,
    FMOD_TAGDATATYPE_STRING_UTF8 = 6,
    FMOD_TAGDATATYPE_CDTOC = 7,
    FMOD_TAGDATATYPE_MAX = 8,
    FMOD_TAGDATATYPE_FORCEINT = 65536
};

struct FMOD_TAG
{
    FMOD_TAGTYPE type; // offset 0x0
    FMOD_TAGDATATYPE datatype; // offset 0x4
    char * name; // offset 0x8
    void * data; // offset 0xC
    unsigned int datalen; // offset 0x10
    FMOD_BOOL updated; // offset 0x14
};

typedef FMOD_TAG FMOD_TAG;
// G2MEAB: 0x4C bytes. SystemI::createSoundInternal 0x8061C1C0 rejects any other cbsize, copies userdata
// (+0x44) into SoundI::mUserData and compares suggestedsoundtype (+0x48) with the codec type; the codecs
// that open embedded samples memset and set cbsize to 0x4C. 4.06 adds the user file callbacks after +0x48.
struct FMOD_CREATESOUNDEXINFO
{
    int cbsize; // offset 0x0
    unsigned int length; // offset 0x4
    unsigned int fileoffset; // offset 0x8
    int numchannels; // offset 0xC
    int defaultfrequency; // offset 0x10
    FMOD_SOUND_FORMAT format; // offset 0x14
    unsigned int decodebuffersize; // offset 0x18
    int initialsubsound; // offset 0x1C
    int numsubsounds; // offset 0x20
    int * inclusionlist; // offset 0x24
    int inclusionlistnum; // offset 0x28
    FMOD_SOUND_PCMREADCALLBACK pcmreadcallback; // offset 0x2C
    FMOD_SOUND_PCMSETPOSCALLBACK pcmsetposcallback; // offset 0x30
    FMOD_SOUND_NONBLOCKCALLBACK nonblockcallback; // offset 0x34
    const char * dlsname; // offset 0x38
    const char * encryptionkey; // offset 0x3C
    int maxpolyphony; // offset 0x40
    void * userdata; // offset 0x44
    FMOD_SOUND_TYPE suggestedsoundtype; // offset 0x48
};

typedef FMOD_CREATESOUNDEXINFO FMOD_CREATESOUNDEXINFO;
struct FMOD_REVERB_PROPERTIES
{
    int Instance; // offset 0x0
    int Environment; // offset 0x4
    float EnvSize; // offset 0x8
    float EnvDiffusion; // offset 0xC
    int Room; // offset 0x10
    int RoomHF; // offset 0x14
    int RoomLF; // offset 0x18
    float DecayTime; // offset 0x1C
    float DecayHFRatio; // offset 0x20
    float DecayLFRatio; // offset 0x24
    int Reflections; // offset 0x28
    float ReflectionsDelay; // offset 0x2C
    float ReflectionsPan[3]; // offset 0x30
    int Reverb; // offset 0x3C
    float ReverbDelay; // offset 0x40
    float ReverbPan[3]; // offset 0x44
    float EchoTime; // offset 0x50
    float EchoDepth; // offset 0x54
    float ModulationTime; // offset 0x58
    float ModulationDepth; // offset 0x5C
    float AirAbsorptionHF; // offset 0x60
    float HFReference; // offset 0x64
    float LFReference; // offset 0x68
    float RoomRolloffFactor; // offset 0x6C
    float Diffusion; // offset 0x70
    float Density; // offset 0x74
    unsigned int Flags; // offset 0x78
};

typedef FMOD_REVERB_PROPERTIES FMOD_REVERB_PROPERTIES;
// G2MEAB: the initializer SystemI::SystemI 0x8061E9B4 copies from 0x806B06F8 (FMOD Ex public preset).
#define FMOD_PRESET_OFF { 0, -1, 7.5f, 1.00f, -10000, -10000, 0, 1.00f, 1.00f, 1.0f, -2602, 0.007f, { 0.0f, 0.0f, 0.0f }, 200, 0.011f, { 0.0f, 0.0f, 0.0f }, 0.250f, 0.00f, 0.25f, 0.000f, -5.0f, 5000.0f, 250.0f, 0.0f, 0.0f, 0.0f, 0x33f }
struct FMOD_REVERB_CHANNELPROPERTIES
{
    int Direct; // offset 0x0
    int DirectHF; // offset 0x4
    int Room; // offset 0x8
    int RoomHF; // offset 0xC
    int Obstruction; // offset 0x10
    float ObstructionLFRatio; // offset 0x14
    int Occlusion; // offset 0x18
    float OcclusionLFRatio; // offset 0x1C
    float OcclusionRoomRatio; // offset 0x20
    float OcclusionDirectRatio; // offset 0x24
    int Exclusion; // offset 0x28
    float ExclusionLFRatio; // offset 0x2C
    int OutsideVolumeHF; // offset 0x30
    float DopplerFactor; // offset 0x34
    float RolloffFactor; // offset 0x38
    float RoomRolloffFactor; // offset 0x3C
    float AirAbsorptionFactor; // offset 0x40
    unsigned int Flags; // offset 0x44
};

typedef FMOD_REVERB_CHANNELPROPERTIES FMOD_REVERB_CHANNELPROPERTIES;

// Pre-4.06 instance bits (4.06 moved them to 0x10..0x80). G2MEAB evidence: ChannelGC::setReverbProperties
// 0x80622A70 tests 0x8 (aux A), 0x10 (aux B) and rejects 0x20 with FMOD_ERR_REVERB_INSTANCE (group B).
#ifndef FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0
#define FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT0 0x00000008 // Probable name
#define FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT1 0x00000010 // Probable name
#define FMOD_REVERB_CHANNELFLAGS_ENVIRONMENT2 0x00000020 // Probable name
#endif
struct FMOD_ADVANCEDSETTINGS
{
    int cbsize; // offset 0x0
    int maxMPEGcodecs; // offset 0x4
    int maxADPCMcodecs; // offset 0x8
    int maxXMAcodecs; // offset 0xC
};
// G2MEAB: 0x10 bytes, no ASIO members (SystemI+0xC60, SystemI::mUserData follows at +0xC70).

typedef FMOD_ADVANCEDSETTINGS FMOD_ADVANCEDSETTINGS;
enum FMOD_CHANNELINDEX {
    FMOD_CHANNEL_FREE = -1,
    FMOD_CHANNEL_REUSE = -2
};

#include "fmod_dsp.h"

#ifdef __cplusplus
extern "C" {
#endif

FMOD_RESULT FMOD_Memory_GetStats(int * currentalloced, int * maxalloced);
FMOD_RESULT FMOD_Memory_Initialize(void * poolmem, int poollen, FMOD_MEMORY_ALLOCCALLBACK useralloc, FMOD_MEMORY_REALLOCCALLBACK userrealloc, FMOD_MEMORY_FREECALLBACK userfree);
FMOD_RESULT FMOD_System_Create(FMOD_SYSTEM * * system);
FMOD_RESULT FMOD_System_Release(FMOD_SYSTEM * system);
FMOD_RESULT FMOD_System_SetOutput(FMOD_SYSTEM * system, FMOD_OUTPUTTYPE output);
FMOD_RESULT FMOD_System_GetOutput(FMOD_SYSTEM * system, FMOD_OUTPUTTYPE * output);
FMOD_RESULT FMOD_System_GetNumDrivers(FMOD_SYSTEM * system, int * numdrivers);
FMOD_RESULT FMOD_System_GetDriverName(FMOD_SYSTEM * system, int id, char * name, int namelen);
FMOD_RESULT FMOD_System_GetDriverCaps(FMOD_SYSTEM * system, int id, FMOD_CAPS * caps, int * minfrequency, int * maxfrequency, FMOD_SPEAKERMODE * controlpanelspeakermode);
FMOD_RESULT FMOD_System_SetDriver(FMOD_SYSTEM * system, int driver);
FMOD_RESULT FMOD_System_GetDriver(FMOD_SYSTEM * system, int * driver);
FMOD_RESULT FMOD_System_SetHardwareChannels(FMOD_SYSTEM * system, int min2d, int max2d, int min3d, int max3d);
FMOD_RESULT FMOD_System_SetSoftwareChannels(FMOD_SYSTEM * system, int numsoftwarechannels);
FMOD_RESULT FMOD_System_GetSoftwareChannels(FMOD_SYSTEM * system, int * numsoftwarechannels);
FMOD_RESULT FMOD_System_SetSoftwareFormat(FMOD_SYSTEM * system, int samplerate, FMOD_SOUND_FORMAT format, int numoutputchannels, int maxinputchannels, FMOD_DSP_RESAMPLER resamplemethod);
FMOD_RESULT FMOD_System_GetSoftwareFormat(FMOD_SYSTEM * system, int * samplerate, FMOD_SOUND_FORMAT * format, int * numoutputchannels, int * maxinputchannels, FMOD_DSP_RESAMPLER * resamplemethod, int * bits);
FMOD_RESULT FMOD_System_SetDSPBufferSize(FMOD_SYSTEM * system, unsigned int bufferlength, int numbuffers);
FMOD_RESULT FMOD_System_GetDSPBufferSize(FMOD_SYSTEM * system, unsigned int * bufferlength, int * numbuffers);
FMOD_RESULT FMOD_System_SetFileSystem(FMOD_SYSTEM * system, FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek, int blocksize);
FMOD_RESULT FMOD_System_AttachFileSystem(FMOD_SYSTEM * system, FMOD_FILE_OPENCALLBACK useropen, FMOD_FILE_CLOSECALLBACK userclose, FMOD_FILE_READCALLBACK userread, FMOD_FILE_SEEKCALLBACK userseek);
FMOD_RESULT FMOD_System_SetAdvancedSettings(FMOD_SYSTEM * system, FMOD_ADVANCEDSETTINGS * settings);
FMOD_RESULT FMOD_System_GetAdvancedSettings(FMOD_SYSTEM * system, FMOD_ADVANCEDSETTINGS * settings);
FMOD_RESULT FMOD_System_SetSpeakerMode(FMOD_SYSTEM * system, FMOD_SPEAKERMODE speakermode);
FMOD_RESULT FMOD_System_GetSpeakerMode(FMOD_SYSTEM * system, FMOD_SPEAKERMODE * speakermode);
FMOD_RESULT FMOD_System_SetPluginPath(FMOD_SYSTEM * system, const char * path);
FMOD_RESULT FMOD_System_LoadPlugin(FMOD_SYSTEM * system, const char * filename, FMOD_PLUGINTYPE * plugintype, int * index);
FMOD_RESULT FMOD_System_GetNumPlugins(FMOD_SYSTEM * system, FMOD_PLUGINTYPE plugintype, int * numplugins);
FMOD_RESULT FMOD_System_GetPluginInfo(FMOD_SYSTEM * system, FMOD_PLUGINTYPE plugintype, int index, char * name, int namelen, unsigned int * version);
FMOD_RESULT FMOD_System_UnloadPlugin(FMOD_SYSTEM * system, FMOD_PLUGINTYPE plugintype, int index);
FMOD_RESULT FMOD_System_SetOutputByPlugin(FMOD_SYSTEM * system, int index);
FMOD_RESULT FMOD_System_GetOutputByPlugin(FMOD_SYSTEM * system, int * index);
FMOD_RESULT FMOD_System_CreateCodec(FMOD_SYSTEM * system, FMOD_CODEC_DESCRIPTION * description);
FMOD_RESULT FMOD_System_Init(FMOD_SYSTEM * system, int maxchannels, FMOD_INITFLAGS flags, void * extradriverdata);
FMOD_RESULT FMOD_System_Close(FMOD_SYSTEM * system);
FMOD_RESULT FMOD_System_Update(FMOD_SYSTEM * system);
FMOD_RESULT FMOD_System_Set3DSettings(FMOD_SYSTEM * system, float dopplerscale, float distancefactor, float rolloffscale);
FMOD_RESULT FMOD_System_Get3DSettings(FMOD_SYSTEM * system, float * dopplerscale, float * distancefactor, float * rolloffscale);
FMOD_RESULT FMOD_System_Set3DNumListeners(FMOD_SYSTEM * system, int numlisteners);
FMOD_RESULT FMOD_System_Get3DNumListeners(FMOD_SYSTEM * system, int * numlisteners);
FMOD_RESULT FMOD_System_Set3DListenerAttributes(FMOD_SYSTEM * system, int listener, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel, const FMOD_VECTOR * forward, const FMOD_VECTOR * up);
FMOD_RESULT FMOD_System_Get3DListenerAttributes(FMOD_SYSTEM * system, int listener, FMOD_VECTOR * pos, FMOD_VECTOR * vel, FMOD_VECTOR * forward, FMOD_VECTOR * up);
FMOD_RESULT FMOD_System_SetSpeakerPosition(FMOD_SYSTEM * system, FMOD_SPEAKER speaker, float x, float y);
FMOD_RESULT FMOD_System_GetSpeakerPosition(FMOD_SYSTEM * system, FMOD_SPEAKER speaker, float * x, float * y);
FMOD_RESULT FMOD_System_SetStreamBufferSize(FMOD_SYSTEM * system, unsigned int filebuffersize, FMOD_TIMEUNIT filebuffersizetype);
FMOD_RESULT FMOD_System_GetStreamBufferSize(FMOD_SYSTEM * system, unsigned int * filebuffersize, FMOD_TIMEUNIT * filebuffersizetype);
FMOD_RESULT FMOD_System_GetVersion(FMOD_SYSTEM * system, unsigned int * version);
FMOD_RESULT FMOD_System_GetOutputHandle(FMOD_SYSTEM * system, void * * handle);
FMOD_RESULT FMOD_System_GetChannelsPlaying(FMOD_SYSTEM * system, int * channels);
FMOD_RESULT FMOD_System_GetHardwareChannels(FMOD_SYSTEM * system, int * num2d, int * num3d, int * total);
FMOD_RESULT FMOD_System_GetCPUUsage(FMOD_SYSTEM * system, float * dsp, float * stream, float * update, float * total);
FMOD_RESULT FMOD_System_GetSoundRAM(FMOD_SYSTEM * system, int * currentalloced, int * maxalloced, int * total);
FMOD_RESULT FMOD_System_GetNumCDROMDrives(FMOD_SYSTEM * system, int * numdrives);
FMOD_RESULT FMOD_System_GetCDROMDriveName(FMOD_SYSTEM * system, int drive, char * drivename, int drivenamelen, char * scsiname, int scsinamelen, char * devicename, int devicenamelen);
FMOD_RESULT FMOD_System_GetSpectrum(FMOD_SYSTEM * system, float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
FMOD_RESULT FMOD_System_GetWaveData(FMOD_SYSTEM * system, float * wavearray, int numvalues, int channeloffset);
FMOD_RESULT FMOD_System_CreateSound(FMOD_SYSTEM * system, const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, FMOD_SOUND * * sound);
FMOD_RESULT FMOD_System_CreateStream(FMOD_SYSTEM * system, const char * name_or_data, FMOD_MODE mode, FMOD_CREATESOUNDEXINFO * exinfo, FMOD_SOUND * * sound);
FMOD_RESULT FMOD_System_CreateDSP(FMOD_SYSTEM * system, FMOD_DSP_DESCRIPTION * description, FMOD_DSP * * dsp);
FMOD_RESULT FMOD_System_CreateDSPByType(FMOD_SYSTEM * system, FMOD_DSP_TYPE type, FMOD_DSP * * dsp);
FMOD_RESULT FMOD_System_CreateDSPByIndex(FMOD_SYSTEM * system, int index, FMOD_DSP * * dsp);
FMOD_RESULT FMOD_System_CreateChannelGroup(FMOD_SYSTEM * system, const char * name, FMOD_CHANNELGROUP * * channelgroup);
FMOD_RESULT FMOD_System_PlaySound(FMOD_SYSTEM * system, FMOD_CHANNELINDEX channelid, FMOD_SOUND * sound, FMOD_BOOL paused, FMOD_CHANNEL * * channel);
FMOD_RESULT FMOD_System_PlayDSP(FMOD_SYSTEM * system, FMOD_CHANNELINDEX channelid, FMOD_DSP * dsp, FMOD_BOOL paused, FMOD_CHANNEL * * channel);
FMOD_RESULT FMOD_System_GetChannel(FMOD_SYSTEM * system, int channelid, FMOD_CHANNEL * * channel);
FMOD_RESULT FMOD_System_GetMasterChannelGroup(FMOD_SYSTEM * system, FMOD_CHANNELGROUP * * channelgroup);
FMOD_RESULT FMOD_System_SetReverbProperties(FMOD_SYSTEM * system, const FMOD_REVERB_PROPERTIES * prop);
FMOD_RESULT FMOD_System_GetReverbProperties(FMOD_SYSTEM * system, FMOD_REVERB_PROPERTIES * prop);
FMOD_RESULT FMOD_System_GetDSPHead(FMOD_SYSTEM * system, FMOD_DSP * * dsp);
FMOD_RESULT FMOD_System_AddDSP(FMOD_SYSTEM * system, FMOD_DSP * dsp);
FMOD_RESULT FMOD_System_LockDSP(FMOD_SYSTEM * system);
FMOD_RESULT FMOD_System_UnlockDSP(FMOD_SYSTEM * system);
FMOD_RESULT FMOD_System_SetRecordDriver(FMOD_SYSTEM * system, int driver);
FMOD_RESULT FMOD_System_GetRecordDriver(FMOD_SYSTEM * system, int * driver);
FMOD_RESULT FMOD_System_GetRecordNumDrivers(FMOD_SYSTEM * system, int * numdrivers);
FMOD_RESULT FMOD_System_GetRecordDriverName(FMOD_SYSTEM * system, int id, char * name, int namelen);
FMOD_RESULT FMOD_System_GetRecordPosition(FMOD_SYSTEM * system, unsigned int * position);
FMOD_RESULT FMOD_System_RecordStart(FMOD_SYSTEM * system, FMOD_SOUND * sound, FMOD_BOOL loop);
FMOD_RESULT FMOD_System_RecordStop(FMOD_SYSTEM * system);
FMOD_RESULT FMOD_System_IsRecording(FMOD_SYSTEM * system, FMOD_BOOL * recording);
FMOD_RESULT FMOD_System_CreateGeometry(FMOD_SYSTEM * system, int maxpolygons, int maxvertices, FMOD_GEOMETRY * * geometry);
FMOD_RESULT FMOD_System_SetGeometrySettings(FMOD_SYSTEM * system, float maxworldsize);
FMOD_RESULT FMOD_System_GetGeometrySettings(FMOD_SYSTEM * system, float * maxworldsize);
FMOD_RESULT FMOD_System_LoadGeometry(FMOD_SYSTEM * system, const void * data, int datasize, FMOD_GEOMETRY * * geometry);
FMOD_RESULT FMOD_System_SetNetworkProxy(FMOD_SYSTEM * system, const char * proxy);
FMOD_RESULT FMOD_System_GetNetworkProxy(FMOD_SYSTEM * system, char * proxy, int proxylen);
FMOD_RESULT FMOD_System_SetNetworkTimeout(FMOD_SYSTEM * system, int timeout);
FMOD_RESULT FMOD_System_GetNetworkTimeout(FMOD_SYSTEM * system, int * timeout);
FMOD_RESULT FMOD_System_SetUserData(FMOD_SYSTEM * system, void * userdata);
FMOD_RESULT FMOD_System_GetUserData(FMOD_SYSTEM * system, void * * userdata);
FMOD_RESULT FMOD_Sound_Release(FMOD_SOUND * sound);
FMOD_RESULT FMOD_Sound_GetSystemObject(FMOD_SOUND * sound, FMOD_SYSTEM * * system);
FMOD_RESULT FMOD_Sound_Lock(FMOD_SOUND * sound, unsigned int offset, unsigned int length, void * * ptr1, void * * ptr2, unsigned int * len1, unsigned int * len2);
FMOD_RESULT FMOD_Sound_Unlock(FMOD_SOUND * sound, void * ptr1, void * ptr2, unsigned int len1, unsigned int len2);
FMOD_RESULT FMOD_Sound_SetDefaults(FMOD_SOUND * sound, float frequency, float volume, float pan, int priority);
FMOD_RESULT FMOD_Sound_GetDefaults(FMOD_SOUND * sound, float * frequency, float * volume, float * pan, int * priority);
FMOD_RESULT FMOD_Sound_SetVariations(FMOD_SOUND * sound, float frequencyvar, float volumevar, float panvar);
FMOD_RESULT FMOD_Sound_GetVariations(FMOD_SOUND * sound, float * frequencyvar, float * volumevar, float * panvar);
FMOD_RESULT FMOD_Sound_Set3DMinMaxDistance(FMOD_SOUND * sound, float min, float max);
FMOD_RESULT FMOD_Sound_Get3DMinMaxDistance(FMOD_SOUND * sound, float * min, float * max);
FMOD_RESULT FMOD_Sound_Set3DConeSettings(FMOD_SOUND * sound, float insideconeangle, float outsideconeangle, float outsidevolume);
FMOD_RESULT FMOD_Sound_Get3DConeSettings(FMOD_SOUND * sound, float * insideconeangle, float * outsideconeangle, float * outsidevolume);
FMOD_RESULT FMOD_Sound_Set3DCustomRolloff(FMOD_SOUND * sound, FMOD_VECTOR * points, int numpoints);
FMOD_RESULT FMOD_Sound_Get3DCustomRolloff(FMOD_SOUND * sound, FMOD_VECTOR * * points, int * numpoints);
FMOD_RESULT FMOD_Sound_SetSubSound(FMOD_SOUND * sound, int index, FMOD_SOUND * subsound);
FMOD_RESULT FMOD_Sound_GetSubSound(FMOD_SOUND * sound, int index, FMOD_SOUND * * subsound);
FMOD_RESULT FMOD_Sound_SetSubSoundSentence(FMOD_SOUND * sound, int * subsoundlist, int numsubsounds);
FMOD_RESULT FMOD_Sound_GetName(FMOD_SOUND * sound, char * name, int namelen);
FMOD_RESULT FMOD_Sound_GetLength(FMOD_SOUND * sound, unsigned int * length, FMOD_TIMEUNIT lengthtype);
FMOD_RESULT FMOD_Sound_GetFormat(FMOD_SOUND * sound, FMOD_SOUND_TYPE * type, FMOD_SOUND_FORMAT * format, int * channels, int * bits);
FMOD_RESULT FMOD_Sound_GetNumSubSounds(FMOD_SOUND * sound, int * numsubsounds);
FMOD_RESULT FMOD_Sound_GetNumTags(FMOD_SOUND * sound, int * numtags, int * numtagsupdated);
FMOD_RESULT FMOD_Sound_GetTag(FMOD_SOUND * sound, const char * name, int index, FMOD_TAG * tag);
FMOD_RESULT FMOD_Sound_GetOpenState(FMOD_SOUND * sound, FMOD_OPENSTATE * openstate, unsigned int * percentbuffered, FMOD_BOOL * starving);
FMOD_RESULT FMOD_Sound_ReadData(FMOD_SOUND * sound, void * buffer, unsigned int lenbytes, unsigned int * read);
FMOD_RESULT FMOD_Sound_SeekData(FMOD_SOUND * sound, unsigned int pcm);
FMOD_RESULT FMOD_Sound_GetNumSyncPoints(FMOD_SOUND * sound, int * numsyncpoints);
FMOD_RESULT FMOD_Sound_GetSyncPoint(FMOD_SOUND * sound, int index, FMOD_SYNCPOINT * * point);
FMOD_RESULT FMOD_Sound_GetSyncPointInfo(FMOD_SOUND * sound, FMOD_SYNCPOINT * point, char * name, int namelen, unsigned int * offset, FMOD_TIMEUNIT offsettype);
FMOD_RESULT FMOD_Sound_AddSyncPoint(FMOD_SOUND * sound, unsigned int offset, FMOD_TIMEUNIT offsettype, const char * name, FMOD_SYNCPOINT * * point);
FMOD_RESULT FMOD_Sound_DeleteSyncPoint(FMOD_SOUND * sound, FMOD_SYNCPOINT * point);
FMOD_RESULT FMOD_Sound_SetMode(FMOD_SOUND * sound, FMOD_MODE mode);
FMOD_RESULT FMOD_Sound_GetMode(FMOD_SOUND * sound, FMOD_MODE * mode);
FMOD_RESULT FMOD_Sound_SetLoopCount(FMOD_SOUND * sound, int loopcount);
FMOD_RESULT FMOD_Sound_GetLoopCount(FMOD_SOUND * sound, int * loopcount);
FMOD_RESULT FMOD_Sound_SetLoopPoints(FMOD_SOUND * sound, unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
FMOD_RESULT FMOD_Sound_GetLoopPoints(FMOD_SOUND * sound, unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype);
FMOD_RESULT FMOD_Sound_SetUserData(FMOD_SOUND * sound, void * userdata);
FMOD_RESULT FMOD_Sound_GetUserData(FMOD_SOUND * sound, void * * userdata);
FMOD_RESULT FMOD_Channel_GetSystemObject(FMOD_CHANNEL * channel, FMOD_SYSTEM * * system);
FMOD_RESULT FMOD_Channel_Stop(FMOD_CHANNEL * channel);
FMOD_RESULT FMOD_Channel_SetPaused(FMOD_CHANNEL * channel, FMOD_BOOL paused);
FMOD_RESULT FMOD_Channel_GetPaused(FMOD_CHANNEL * channel, FMOD_BOOL * paused);
FMOD_RESULT FMOD_Channel_SetVolume(FMOD_CHANNEL * channel, float volume);
FMOD_RESULT FMOD_Channel_GetVolume(FMOD_CHANNEL * channel, float * volume);
FMOD_RESULT FMOD_Channel_SetFrequency(FMOD_CHANNEL * channel, float frequency);
FMOD_RESULT FMOD_Channel_GetFrequency(FMOD_CHANNEL * channel, float * frequency);
FMOD_RESULT FMOD_Channel_SetPan(FMOD_CHANNEL * channel, float pan);
FMOD_RESULT FMOD_Channel_GetPan(FMOD_CHANNEL * channel, float * pan);
FMOD_RESULT FMOD_Channel_SetDelay(FMOD_CHANNEL * channel, unsigned int startdelay, unsigned int enddelay);
FMOD_RESULT FMOD_Channel_GetDelay(FMOD_CHANNEL * channel, unsigned int * startdelay, unsigned int * enddelay);
FMOD_RESULT FMOD_Channel_SetSpeakerMix(FMOD_CHANNEL * channel, float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
FMOD_RESULT FMOD_Channel_GetSpeakerMix(FMOD_CHANNEL * channel, float * frontleft, float * frontright, float * center, float * lfe, float * backleft, float * backright, float * sideleft, float * sideright);
FMOD_RESULT FMOD_Channel_SetSpeakerLevels(FMOD_CHANNEL * channel, FMOD_SPEAKER speaker, float * levels, int numlevels);
FMOD_RESULT FMOD_Channel_GetSpeakerLevels(FMOD_CHANNEL * channel, FMOD_SPEAKER speaker, float * levels, int numlevels);
FMOD_RESULT FMOD_Channel_SetMute(FMOD_CHANNEL * channel, FMOD_BOOL mute);
FMOD_RESULT FMOD_Channel_GetMute(FMOD_CHANNEL * channel, FMOD_BOOL * mute);
FMOD_RESULT FMOD_Channel_SetPriority(FMOD_CHANNEL * channel, int priority);
FMOD_RESULT FMOD_Channel_GetPriority(FMOD_CHANNEL * channel, int * priority);
FMOD_RESULT FMOD_Channel_SetPosition(FMOD_CHANNEL * channel, unsigned int position, FMOD_TIMEUNIT postype);
FMOD_RESULT FMOD_Channel_GetPosition(FMOD_CHANNEL * channel, unsigned int * position, FMOD_TIMEUNIT postype);
FMOD_RESULT FMOD_Channel_SetReverbProperties(FMOD_CHANNEL * channel, const FMOD_REVERB_CHANNELPROPERTIES * prop);
FMOD_RESULT FMOD_Channel_GetReverbProperties(FMOD_CHANNEL * channel, FMOD_REVERB_CHANNELPROPERTIES * prop);
FMOD_RESULT FMOD_Channel_SetChannelGroup(FMOD_CHANNEL * channel, FMOD_CHANNELGROUP * channelgroup);
FMOD_RESULT FMOD_Channel_GetChannelGroup(FMOD_CHANNEL * channel, FMOD_CHANNELGROUP * * channelgroup);
FMOD_RESULT FMOD_Channel_SetCallback(FMOD_CHANNEL * channel, FMOD_CHANNEL_CALLBACKTYPE type, FMOD_CHANNEL_CALLBACK callback, int command);
FMOD_RESULT FMOD_Channel_Set3DAttributes(FMOD_CHANNEL * channel, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel);
FMOD_RESULT FMOD_Channel_Get3DAttributes(FMOD_CHANNEL * channel, FMOD_VECTOR * pos, FMOD_VECTOR * vel);
FMOD_RESULT FMOD_Channel_Set3DMinMaxDistance(FMOD_CHANNEL * channel, float mindistance, float maxdistance);
FMOD_RESULT FMOD_Channel_Get3DMinMaxDistance(FMOD_CHANNEL * channel, float * mindistance, float * maxdistance);
FMOD_RESULT FMOD_Channel_Set3DConeSettings(FMOD_CHANNEL * channel, float insideconeangle, float outsideconeangle, float outsidevolume);
FMOD_RESULT FMOD_Channel_Get3DConeSettings(FMOD_CHANNEL * channel, float * insideconeangle, float * outsideconeangle, float * outsidevolume);
FMOD_RESULT FMOD_Channel_Set3DConeOrientation(FMOD_CHANNEL * channel, FMOD_VECTOR * orientation);
FMOD_RESULT FMOD_Channel_Get3DConeOrientation(FMOD_CHANNEL * channel, FMOD_VECTOR * orientation);
FMOD_RESULT FMOD_Channel_Set3DCustomRolloff(FMOD_CHANNEL * channel, FMOD_VECTOR * points, int numpoints);
FMOD_RESULT FMOD_Channel_Get3DCustomRolloff(FMOD_CHANNEL * channel, FMOD_VECTOR * * points, int * numpoints);
FMOD_RESULT FMOD_Channel_Set3DOcclusion(FMOD_CHANNEL * channel, float directocclusion, float reverbocclusion);
FMOD_RESULT FMOD_Channel_Get3DOcclusion(FMOD_CHANNEL * channel, float * directocclusion, float * reverbocclusion);
FMOD_RESULT FMOD_Channel_Set3DSpread(FMOD_CHANNEL * channel, float angle);
FMOD_RESULT FMOD_Channel_Get3DSpread(FMOD_CHANNEL * channel, float * angle);
FMOD_RESULT FMOD_Channel_Set3DPanLevel(FMOD_CHANNEL * channel, float level);
FMOD_RESULT FMOD_Channel_Get3DPanLevel(FMOD_CHANNEL * channel, float * level);
FMOD_RESULT FMOD_Channel_Set3DDopplerLevel(FMOD_CHANNEL * channel, float level);
FMOD_RESULT FMOD_Channel_Get3DDopplerLevel(FMOD_CHANNEL * channel, float * level);
FMOD_RESULT FMOD_Channel_GetDSPHead(FMOD_CHANNEL * channel, FMOD_DSP * * dsp);
FMOD_RESULT FMOD_Channel_AddDSP(FMOD_CHANNEL * channel, FMOD_DSP * dsp);
FMOD_RESULT FMOD_Channel_IsPlaying(FMOD_CHANNEL * channel, FMOD_BOOL * isplaying);
FMOD_RESULT FMOD_Channel_IsVirtual(FMOD_CHANNEL * channel, FMOD_BOOL * isvirtual);
FMOD_RESULT FMOD_Channel_GetAudibility(FMOD_CHANNEL * channel, float * audibility);
FMOD_RESULT FMOD_Channel_GetCurrentSound(FMOD_CHANNEL * channel, FMOD_SOUND * * sound);
FMOD_RESULT FMOD_Channel_GetSpectrum(FMOD_CHANNEL * channel, float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
FMOD_RESULT FMOD_Channel_GetWaveData(FMOD_CHANNEL * channel, float * wavearray, int numvalues, int channeloffset);
FMOD_RESULT FMOD_Channel_GetIndex(FMOD_CHANNEL * channel, int * index);
FMOD_RESULT FMOD_Channel_SetMode(FMOD_CHANNEL * channel, FMOD_MODE mode);
FMOD_RESULT FMOD_Channel_GetMode(FMOD_CHANNEL * channel, FMOD_MODE * mode);
FMOD_RESULT FMOD_Channel_SetLoopCount(FMOD_CHANNEL * channel, int loopcount);
FMOD_RESULT FMOD_Channel_GetLoopCount(FMOD_CHANNEL * channel, int * loopcount);
FMOD_RESULT FMOD_Channel_SetLoopPoints(FMOD_CHANNEL * channel, unsigned int loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int loopend, FMOD_TIMEUNIT loopendtype);
FMOD_RESULT FMOD_Channel_GetLoopPoints(FMOD_CHANNEL * channel, unsigned int * loopstart, FMOD_TIMEUNIT loopstarttype, unsigned int * loopend, FMOD_TIMEUNIT loopendtype);
FMOD_RESULT FMOD_Channel_SetUserData(FMOD_CHANNEL * channel, void * userdata);
FMOD_RESULT FMOD_Channel_GetUserData(FMOD_CHANNEL * channel, void * * userdata);
FMOD_RESULT FMOD_ChannelGroup_Release(FMOD_CHANNELGROUP * channelgroup);
FMOD_RESULT FMOD_ChannelGroup_GetSystemObject(FMOD_CHANNELGROUP * channelgroup, FMOD_SYSTEM * * system);
FMOD_RESULT FMOD_ChannelGroup_SetVolume(FMOD_CHANNELGROUP * channelgroup, float volume);
FMOD_RESULT FMOD_ChannelGroup_GetVolume(FMOD_CHANNELGROUP * channelgroup, float * volume);
FMOD_RESULT FMOD_ChannelGroup_SetPitch(FMOD_CHANNELGROUP * channelgroup, float pitch);
FMOD_RESULT FMOD_ChannelGroup_GetPitch(FMOD_CHANNELGROUP * channelgroup, float * pitch);
FMOD_RESULT FMOD_ChannelGroup_Stop(FMOD_CHANNELGROUP * channelgroup);
FMOD_RESULT FMOD_ChannelGroup_OverridePaused(FMOD_CHANNELGROUP * channelgroup, FMOD_BOOL paused);
FMOD_RESULT FMOD_ChannelGroup_OverrideVolume(FMOD_CHANNELGROUP * channelgroup, float volume);
FMOD_RESULT FMOD_ChannelGroup_OverrideFrequency(FMOD_CHANNELGROUP * channelgroup, float frequency);
FMOD_RESULT FMOD_ChannelGroup_OverridePan(FMOD_CHANNELGROUP * channelgroup, float pan);
FMOD_RESULT FMOD_ChannelGroup_OverrideMute(FMOD_CHANNELGROUP * channelgroup, FMOD_BOOL mute);
FMOD_RESULT FMOD_ChannelGroup_OverrideReverbProperties(FMOD_CHANNELGROUP * channelgroup, const FMOD_REVERB_CHANNELPROPERTIES * prop);
FMOD_RESULT FMOD_ChannelGroup_Override3DAttributes(FMOD_CHANNELGROUP * channelgroup, const FMOD_VECTOR * pos, const FMOD_VECTOR * vel);
FMOD_RESULT FMOD_ChannelGroup_OverrideSpeakerMix(FMOD_CHANNELGROUP * channelgroup, float frontleft, float frontright, float center, float lfe, float backleft, float backright, float sideleft, float sideright);
FMOD_RESULT FMOD_ChannelGroup_AddGroup(FMOD_CHANNELGROUP * channelgroup, FMOD_CHANNELGROUP * group);
FMOD_RESULT FMOD_ChannelGroup_GetNumGroups(FMOD_CHANNELGROUP * channelgroup, int * numgroups);
FMOD_RESULT FMOD_ChannelGroup_GetGroup(FMOD_CHANNELGROUP * channelgroup, int index, FMOD_CHANNELGROUP * * group);
FMOD_RESULT FMOD_ChannelGroup_GetDSPHead(FMOD_CHANNELGROUP * channelgroup, FMOD_DSP * * dsp);
FMOD_RESULT FMOD_ChannelGroup_AddDSP(FMOD_CHANNELGROUP * channelgroup, FMOD_DSP * dsp);
FMOD_RESULT FMOD_ChannelGroup_GetName(FMOD_CHANNELGROUP * channelgroup, char * name, int namelen);
FMOD_RESULT FMOD_ChannelGroup_GetNumChannels(FMOD_CHANNELGROUP * channelgroup, int * numchannels);
FMOD_RESULT FMOD_ChannelGroup_GetChannel(FMOD_CHANNELGROUP * channelgroup, int index, FMOD_CHANNEL * * channel);
FMOD_RESULT FMOD_ChannelGroup_GetSpectrum(FMOD_CHANNELGROUP * channelgroup, float * spectrumarray, int numvalues, int channeloffset, FMOD_DSP_FFT_WINDOW windowtype);
FMOD_RESULT FMOD_ChannelGroup_GetWaveData(FMOD_CHANNELGROUP * channelgroup, float * wavearray, int numvalues, int channeloffset);
FMOD_RESULT FMOD_ChannelGroup_SetUserData(FMOD_CHANNELGROUP * channelgroup, void * userdata);
FMOD_RESULT FMOD_ChannelGroup_GetUserData(FMOD_CHANNELGROUP * channelgroup, void * * userdata);
FMOD_RESULT FMOD_DSP_Release(FMOD_DSP * dsp);
FMOD_RESULT FMOD_DSP_GetSystemObject(FMOD_DSP * dsp, FMOD_SYSTEM * * system);
FMOD_RESULT FMOD_DSP_AddInput(FMOD_DSP * dsp, FMOD_DSP * target);
FMOD_RESULT FMOD_DSP_DisconnectFrom(FMOD_DSP * dsp, FMOD_DSP * target);
FMOD_RESULT FMOD_DSP_DisconnectAll(FMOD_DSP * dsp, FMOD_BOOL inputs, FMOD_BOOL outputs);
FMOD_RESULT FMOD_DSP_Remove(FMOD_DSP * dsp);
FMOD_RESULT FMOD_DSP_GetNumInputs(FMOD_DSP * dsp, int * numinputs);
FMOD_RESULT FMOD_DSP_GetNumOutputs(FMOD_DSP * dsp, int * numoutputs);
FMOD_RESULT FMOD_DSP_GetInput(FMOD_DSP * dsp, int index, FMOD_DSP * * input);
FMOD_RESULT FMOD_DSP_GetOutput(FMOD_DSP * dsp, int index, FMOD_DSP * * output);
FMOD_RESULT FMOD_DSP_SetInputMix(FMOD_DSP * dsp, int index, float volume);
FMOD_RESULT FMOD_DSP_GetInputMix(FMOD_DSP * dsp, int index, float * volume);
FMOD_RESULT FMOD_DSP_SetInputLevels(FMOD_DSP * dsp, int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
FMOD_RESULT FMOD_DSP_GetInputLevels(FMOD_DSP * dsp, int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
FMOD_RESULT FMOD_DSP_SetOutputMix(FMOD_DSP * dsp, int index, float volume);
FMOD_RESULT FMOD_DSP_GetOutputMix(FMOD_DSP * dsp, int index, float * volume);
FMOD_RESULT FMOD_DSP_SetOutputLevels(FMOD_DSP * dsp, int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
FMOD_RESULT FMOD_DSP_GetOutputLevels(FMOD_DSP * dsp, int index, FMOD_SPEAKER speaker, float * levels, int numlevels);
FMOD_RESULT FMOD_DSP_SetActive(FMOD_DSP * dsp, FMOD_BOOL active);
FMOD_RESULT FMOD_DSP_GetActive(FMOD_DSP * dsp, FMOD_BOOL * active);
FMOD_RESULT FMOD_DSP_SetBypass(FMOD_DSP * dsp, FMOD_BOOL bypass);
FMOD_RESULT FMOD_DSP_GetBypass(FMOD_DSP * dsp, FMOD_BOOL * bypass);
FMOD_RESULT FMOD_DSP_Reset(FMOD_DSP * dsp);
FMOD_RESULT FMOD_DSP_SetParameter(FMOD_DSP * dsp, int index, float value);
FMOD_RESULT FMOD_DSP_GetParameter(FMOD_DSP * dsp, int index, float * value, char * valuestr, int valuestrlen);
FMOD_RESULT FMOD_DSP_GetNumParameters(FMOD_DSP * dsp, int * numparams);
FMOD_RESULT FMOD_DSP_GetParameterInfo(FMOD_DSP * dsp, int index, char * name, char * label, char * description, int descriptionlen, float * min, float * max);
FMOD_RESULT FMOD_DSP_ShowConfigDialog(FMOD_DSP * dsp, void * hwnd, FMOD_BOOL show);
FMOD_RESULT FMOD_DSP_GetInfo(FMOD_DSP * dsp, char * name, unsigned int * version, int * channels, int * configwidth, int * configheight);
FMOD_RESULT FMOD_DSP_GetType(FMOD_DSP * dsp, FMOD_DSP_TYPE * type);
FMOD_RESULT FMOD_DSP_SetDefaults(FMOD_DSP * dsp, float frequency, float volume, float pan, int priority);
FMOD_RESULT FMOD_DSP_GetDefaults(FMOD_DSP * dsp, float * frequency, float * volume, float * pan, int * priority);
FMOD_RESULT FMOD_DSP_SetUserData(FMOD_DSP * dsp, void * userdata);
FMOD_RESULT FMOD_DSP_GetUserData(FMOD_DSP * dsp, void * * userdata);
FMOD_RESULT FMOD_Geometry_Release(FMOD_GEOMETRY * geometry);
FMOD_RESULT FMOD_Geometry_AddPolygon(FMOD_GEOMETRY * geometry, float directocclusion, float reverbocclusion, FMOD_BOOL doublesided, int numvertices, const FMOD_VECTOR * vertices, int * polygonindex);
FMOD_RESULT FMOD_Geometry_GetNumPolygons(FMOD_GEOMETRY * geometry, int * numpolygons);
FMOD_RESULT FMOD_Geometry_GetMaxPolygons(FMOD_GEOMETRY * geometry, int * maxpolygons, int * maxvertices);
FMOD_RESULT FMOD_Geometry_GetPolygonNumVertices(FMOD_GEOMETRY * geometry, int index, int * numvertices);
FMOD_RESULT FMOD_Geometry_SetPolygonVertex(FMOD_GEOMETRY * geometry, int index, int vertexindex, const FMOD_VECTOR * vertex);
FMOD_RESULT FMOD_Geometry_GetPolygonVertex(FMOD_GEOMETRY * geometry, int index, int vertexindex, FMOD_VECTOR * vertex);
FMOD_RESULT FMOD_Geometry_SetPolygonAttributes(FMOD_GEOMETRY * geometry, int index, float directocclusion, float reverbocclusion, FMOD_BOOL doublesided);
FMOD_RESULT FMOD_Geometry_GetPolygonAttributes(FMOD_GEOMETRY * geometry, int index, float * directocclusion, float * reverbocclusion, FMOD_BOOL * doublesided);
FMOD_RESULT FMOD_Geometry_SetActive(FMOD_GEOMETRY * geometry, FMOD_BOOL active);
FMOD_RESULT FMOD_Geometry_GetActive(FMOD_GEOMETRY * geometry, FMOD_BOOL * active);
FMOD_RESULT FMOD_Geometry_SetRotation(FMOD_GEOMETRY * geometry, const FMOD_VECTOR * forward, const FMOD_VECTOR * up);
FMOD_RESULT FMOD_Geometry_GetRotation(FMOD_GEOMETRY * geometry, FMOD_VECTOR * forward, FMOD_VECTOR * up);
FMOD_RESULT FMOD_Geometry_SetPosition(FMOD_GEOMETRY * geometry, const FMOD_VECTOR * position);
FMOD_RESULT FMOD_Geometry_GetPosition(FMOD_GEOMETRY * geometry, FMOD_VECTOR * position);
FMOD_RESULT FMOD_Geometry_SetScale(FMOD_GEOMETRY * geometry, const FMOD_VECTOR * scale);
FMOD_RESULT FMOD_Geometry_GetScale(FMOD_GEOMETRY * geometry, FMOD_VECTOR * scale);
FMOD_RESULT FMOD_Geometry_Save(FMOD_GEOMETRY * geometry, void * data, int * datasize);
FMOD_RESULT FMOD_Geometry_SetUserData(FMOD_GEOMETRY * geometry, void * userdata);
FMOD_RESULT FMOD_Geometry_GetUserData(FMOD_GEOMETRY * geometry, void * * userdata);

#ifdef __cplusplus
}
#endif

#endif
