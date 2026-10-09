// NonMatching translation unit.
// G2MEAB .text: 0x8020727C..0x8020ACA4 (59 retained native functions).
// Original source basename verified by target assertions; complete family retained.
// Prototype-only: neither Echoes nor Prime has a console. The host sends command lines over the
// broadband adapter (CMain forwards them to ExecuteConsoleCommand) and the commands print to the
// debugger, the warning output and back to the host ("Print " messages).
#include "MetroidPrime/ConsoleCommands.hpp"

#include "Kyoto/Alloc/Assert.hpp"
#include "Kyoto/Math/CVector2f.hpp"
#include "Kyoto/Network/CBBASupport.hpp"
#include "Kyoto/Text/CStringTokenizer.hpp"
#include "MetroidPrime/CEntityInfo.hpp"
#include "MetroidPrime/CGameDebug.hpp"
#include "MetroidPrime/CInputGenerator.hpp"
#include "MetroidPrime/CMain.hpp"
#include "MetroidPrime/CScriptMsgUtils.hpp"
#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/CStateManagerCallbackLists.hpp"
#include "MetroidPrime/CStateManagerObject.hpp"
#include "MetroidPrime/Player/CGameState.hpp"
#include "MetroidPrime/Player/CPlayer.hpp"
#include "MetroidPrime/Player/CPlayerState.hpp"
#include "MetroidPrime/TGameTypes.hpp"
#include "MetroidPrime/Tweaks/CTweakContents.hpp"

#include "Kyoto/Audio/CAudioHandle.hpp"
#include "Kyoto/Math/CTransform4f.hpp"

#include "rstl/algorithm.hpp"
#include "rstl/auto_ptr.hpp"
#include "rstl/math.hpp"
#include "rstl/optional_object.hpp"
#include "rstl/string.hpp"
#include "rstl/vector.hpp"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Guessed names. The state the mouse commands store (0xC bytes). The host sends the Windows
// mouse key state, whose left, right and middle buttons are 0x1, 0x2 and 0x10.
struct SMouseInfo {
  SMouseInfo();

  CVector2f mPosition;
  bool mLeftButton : 1;
  bool mRightButton : 1;
  bool mMiddleButton : 1;
  bool mHasFocus : 1;
};

// Guessed names. A registered tuning value (0x18 bytes); the list is kept sorted by name.
struct SDebugVar {
  // The value kinds as the debug var commands print them.
  enum EType {
    kT_Real32,
    kT_Int32,
    kT_Uint32,
    kT_Invalid,
  };

  // The search key of the debug var commands.
  explicit SDebugVar(rstl::string name);
  SDebugVar(const char* name, EType type, void* value) : mName(name), mType(type), mValue(value) {}

  rstl::string mName;
  EType mType;
  void* mValue;
};

// Guessed names. One console command: its upper-case name and its handler, which gets the
// command word and the rest of the line.
struct SConsoleCommand {
  typedef void (*Handler)(const char* command, const char* args);

  const char* mName;
  Handler mHandler;
};

struct SConsoleCommandLess {
  bool operator()(const SConsoleCommand& a, const SConsoleCommand& b) const {
    return strcmp(a.mName, b.mName) < 0;
  }
};

static void AdvanceFrame(const char* command, const char* args);
void CaptureMovie(const char* command, const char* args);
static void ConsolePrint(const char* command, const char* args);
static void DropConnection(const char* command, const char* args);
void FastScriptCook(const char* command, const char* args);
static void GameSpeed(const char* command, const char* args);
static void GetAllDebugVars(const char* command, const char* args);
static void GetDebugVar(const char* command, const char* args);
static void GotMouseFocus(const char* command, const char* args);
static void MouseInfo(const char* command, const char* args);
void PlaySound(const char* command, const char* args);
static void PrintDebugMessages(const char* command, const char* args);
void RefreshNetworkAssets(const char* command, const char* args);
void ReloadAreaLights(const char* command, const char* args);
void ReloadNetworkAssets(const char* command, const char* args);
void ReloadTweaks(const char* command, const char* args);
void RestartGameArea(const char* command, const char* args);
static void Screenshot(const char* command, const char* args);
static void SendMessage(const char* command, const char* args);
static void SetDebugOption(const char* command, const char* args);
void SetDebugTransform(const char* command, const char* args);
static void SetDebugVar(const char* command, const char* args);
void SetTransform(const char* command, const char* args);
static void ShowDebugOptions(const char* command, const char* args);
static void ShowInventory(const char* command, const char* args);
void ShowLayerInfo(const char* command, const char* args);
static void StartProfile(const char* command, const char* args);
void StopSound(const char* command, const char* args);
void TitleScreen(const char* command, const char* args);

// Sorted by name for the binary search in ExecuteConsoleCommand.
static const SConsoleCommand skConsoleCommands[] = {
    {"ADVANCEFRAME", AdvanceFrame},
    {"CAPTUREMOVIE", CaptureMovie},
    {"CONSOLEPRINT", ConsolePrint},
    {"DROPCONNECTION", DropConnection},
    {"FASTSCRIPTCOOK", FastScriptCook},
    {"GAMESPEED", GameSpeed},
    {"GETALLDEBUGVARS", GetAllDebugVars},
    {"GETDEBUGVAR", GetDebugVar},
    {"GOTMOUSEFOCUS", GotMouseFocus},
    {"MOUSEINFO", MouseInfo},
    {"PLAYSOUND", PlaySound},
    {"PRINTDEBUGMESSAGES", PrintDebugMessages},
    {"REFRESHNETWORKASSETS", RefreshNetworkAssets},
    {"RELOADAREALIGHTS", ReloadAreaLights},
    {"RELOADNETWORKASSETS", ReloadNetworkAssets},
    {"RELOADTWEAKS", ReloadTweaks},
    {"RESTARTGAMEAREA", RestartGameArea},
    {"SCREENSHOT", Screenshot},
    {"SENDMESSAGE", SendMessage},
    {"SETDEBUGOPTION", SetDebugOption},
    {"SETDEBUGTRANSFORM", SetDebugTransform},
    {"SETDEBUGVAR", SetDebugVar},
    {"SETTRANSFORM", SetTransform},
    {"SHOWDEBUGOPTIONS", ShowDebugOptions},
    {"SHOWINVENTORY", ShowInventory},
    {"SHOWLAYERINFO", ShowLayerInfo},
    {"STARTPROFILE", StartProfile},
    {"STOPSOUND", StopSound},
    {"TITLESCREEN", TitleScreen},
};

// Guessed names.
static CStateManager* sStateManager;
static CInputGenerator* sInputGenerator;
static SMouseInfo sMouseInfo;
static rstl::vector< SDebugVar > sDebugVars;
// The camera transform the next post-update callback restores (FASTSCRIPTCOOK and
// RESTARTGAMEAREA keep the player camera where it was).
static rstl::optional_object< CTransform4f > sSavedCameraTransform;
static rstl::auto_ptr< IConnection > sStateManagerConnection;
static rstl::auto_ptr< IConnection > sPostUpdateConnection;
static CAudioHandle sSoundHandle;

// Guessed name. Formats the text and sends it to the host as a "Print " message.
static void ConsolePrintf(const char* format, ...) {
  va_list args;
  va_start(args, format);
  char buffer[1024];
  vsnprintf(buffer, sizeof(buffer), format, args);
  CBBASupport::SendString(rstl::string_l("Print ") + rstl::string(buffer), 0, nullptr);
}

// Guessed names. The debug-var commands print each message to the debugger, the warning output
// and the host. They are macros, not functions, because each of the three calls takes the
// variadic arguments directly. The uint32 messages go to the host through CBBASupport::Printf.
#define DEBUG_CONSOLE_PRINTF(...)                                                                  \
  do {                                                                                             \
    rs_debugger_printf(__VA_ARGS__);                                                               \
    gpfnWarningPrintf(__VA_ARGS__);                                                                \
    ConsolePrintf(__VA_ARGS__);                                                                    \
  } while (0)
#define DEBUG_BBA_PRINTF(...)                                                                      \
  do {                                                                                             \
    rs_debugger_printf(__VA_ARGS__);                                                               \
    gpfnWarningPrintf(__VA_ARGS__);                                                                \
    CBBASupport::Printf(__VA_ARGS__);                                                              \
  } while (0)

SMouseInfo::SMouseInfo()
: mPosition(CVector2f::skZeroVector)
, mLeftButton(false)
, mRightButton(false)
, mMiddleButton(false)
, mHasFocus(false) {}

void ExecuteConsoleCommand(const char* command) {
  if (*command != '\0') {
    char buffer[256];
    strncpy(buffer, command, sizeof(buffer) - 1);
    char* word = strtok(buffer, " \n\t");
    if (word != nullptr) {
      const char* args = buffer + rstl::min_val(strlen(word) + 1, strlen(command));
      for (char* c = word; *c != '\0'; ++c) {
        *c = toupper(*c);
      }

      const SConsoleCommand* end = skConsoleCommands + ARRAY_SIZE(skConsoleCommands);
      SConsoleCommand key = {word, nullptr};
      const SConsoleCommand* it =
          rstl::lower_bound(skConsoleCommands, end, key, SConsoleCommandLess());
      if (it != end && strcmp(it->mName, word) == 0) {
        it->mHandler(word, args);
      } else {
        printf("Unrecognized command: %s\nValid commands:\n", word);
        for (int i = 0; i < ARRAY_SIZE(skConsoleCommands); ++i) {
          printf("%s\n", skConsoleCommands[i].mName);
        }
      }
    }
  }
}

bool operator<(const SDebugVar& a, const SDebugVar& b) { return a.mName < b.mName; }

// Guessed name. Replaces the entry of the same name or inserts the new one and sorts the list.
void AddDebugVar(const SDebugVar& var) {
  rstl::vector< SDebugVar >::iterator it =
      rstl::lower_bound(sDebugVars.begin(), sDebugVars.end(), var);
  if (it != sDebugVars.end() && it->mName == var.mName) {
    *it = var;
  } else {
    const int needed = sDebugVars.size() + 1;
    if (needed > sDebugVars.capacity()) {
      int capacity = sDebugVars.capacity() * 2;
      if (capacity < 1) {
        capacity = 1;
      }
      while (capacity < needed) {
        capacity *= 2;
      }
      sDebugVars.reserve(capacity);
    }
    sDebugVars.push_back(var);
    rstl::sort(sDebugVars.begin(), sDebugVars.end());
  }
}

void AddDebugVar(const char* name, float* value) {
  AddDebugVar(SDebugVar(name, SDebugVar::kT_Real32, value));
}

void AddDebugVar(const char* name, uint* value) {
  AddDebugVar(SDebugVar(name, SDebugVar::kT_Uint32, value));
}

// Guessed name. CMain's post-update callback: puts the player camera back where the last
// FASTSCRIPTCOOK or RESTARTGAMEAREA found it.
void RestoreCameraTransform();

// Guessed name.
static void OnStateManagerDestroyed(CStateManager&) { sStateManager = nullptr; }

void InitializeStateManagerConsoleCommands(CStateManager* mgr) {
  sStateManager = mgr;
  if (mgr != nullptr) {
    sStateManagerConnection = mgr->CallbackLists().StateManagerDestroyed().Connect(
        TFunctor1FromFunction< CStateManager& >::Make(OnStateManagerDestroyed));
  }
}

void InitializeConsoleCommands(CInputGenerator* inputGenerator) {
  sInputGenerator = inputGenerator;
  sPostUpdateConnection = gpMain->FrameCallbacks().x78_postUpdate.Connect(
      TFunctor0FromFunction::Make(RestoreCameraTransform));
}

void ShutdownConsoleCommands() {
  sDebugVars = rstl::vector< SDebugVar >();
  sPostUpdateConnection = rstl::auto_ptr< IConnection >();
  sStateManagerConnection = rstl::auto_ptr< IConnection >();
}

// Not implemented yet; they need members CStateManager.hpp does not expose (see the report).
void FastScriptCook(const char* command, const char* args);
void RestartGameArea(const char* command, const char* args);

static void GameSpeed(const char*, const char* args) {
  float speed = 1.f;
  if (args != nullptr && *args != '\0') {
    speed = rstl::min_val(rstl::max_val(static_cast< float >(atof(args)), 0.f), 1.f);
  }
  sInputGenerator->GetRecorder().SetGameSpeed(speed);
  rs_debugger_printf("Gamespeed changed to: %f.\n", speed);
}

static void AdvanceFrame(const char*, const char*) {
  sInputGenerator->GetRecorder().SetStepFrame();
  rs_debugger_printf("Advanced 1 frame.\n");
}

static void Screenshot(const char*, const char*) {
  rs_debugger_printf("Taking screenshot.\n");
  gpMain->TakeScreenshot();
}

static void DropConnection(const char*, const char*) {
  gpfnWarningPrintf("Dropping BBA connection.\n");
  rs_debugger_printf("Dropping BBA connection.\n");
  CBBASupport::Shutdown();
}

static void ConsolePrint(const char*, const char* args) { gpfnWarningPrintf("%s", args); }

static void SendMessage(const char*, const char* args) {
  if (args != nullptr && *args != '\0') {
    uint editorId;
    int message;
    if (sscanf(args, "%x %d", &editorId, &message) == 2) {
      if (sStateManager != nullptr) {
        // The editor id is taken as relative to the player's area.
        const CPlayer* player = sStateManager->ObjectManager().GetPlayer();
        editorId =
            (editorId & ~0x03FF0000) | ((player->GetCurrentAreaId().Value() << 16) & 0x03FF0000);
        CStateManagerObject::TIdListResult range =
            sStateManager->ObjectManager().GetIdListForScript(TEditorId(editorId));
        for (CStateManagerObject::TIdList::const_iterator it = range.first; it != range.second;
             ++it) {
          TUniqueId uid = it->second;
          if (sStateManager->ObjectManager().GetObjectById(uid) != nullptr) {
            sStateManager->ObjectManager().SendScriptMsg(
                CScriptMsg(static_cast< EScriptObjectMessage >(message), kInvalidUniqueId, uid,
                           MakeScriptMsgOriginator(player), kSS_InvalidState));
          }
        }
      }
    } else {
      rs_debugger_printf("Internal error? Could not extract editor id and message from \n%s\n",
                         args);
    }
  }
}

static void SetDebugVar(const char*, const char* args) {
  CStringTokenizer tokenizer(args);
  rstl::string name = tokenizer.ReadToken(nullptr, '"');
  rstl::string value = tokenizer.ReadToken(nullptr, '"');
  SDebugVar key = SDebugVar(rstl::string(name));
  rstl::vector< SDebugVar >::iterator it =
      rstl::lower_bound(sDebugVars.begin(), sDebugVars.end(), key);
  if (it != sDebugVars.end()) {
    SDebugVar var = *it;
    if (var.mName == key.mName) {
      switch (var.mType) {
      case SDebugVar::kT_Int32: {
        int* pointer = static_cast< int* >(var.mValue);
        int oldValue = *pointer;
        int newValue = atoi(value.data());
        DEBUG_CONSOLE_PRINTF("Int32 Var: %s: old value:%d, new value:%d\n", var.mName.data(),
                             oldValue, newValue);
        *pointer = newValue;
        break;
      }
      case SDebugVar::kT_Real32: {
        float* pointer = static_cast< float* >(var.mValue);
        float oldValue = *pointer;
        float newValue = atof(value.data());
        DEBUG_CONSOLE_PRINTF("Real32 Var: %s: old value:%f, new value:%f\n", var.mName.data(),
                             oldValue, newValue);
        *pointer = newValue;
        break;
      }
      case SDebugVar::kT_Uint32: {
        uint* pointer = static_cast< uint* >(var.mValue);
        uint oldValue = *pointer;
        uint newValue = 0;
        int count = sscanf(value.data(), "0x%x", &newValue);
        if (count == 0) {
          count = sscanf(value.data(), "%x", &newValue);
        }
        if (count == 0) {
          DEBUG_BBA_PRINTF("Bad format on uint32 var, use hexadecimal eg. 'ffffffff' or "
                           "'0xffffffff'\n");
        } else {
          DEBUG_BBA_PRINTF("Uint32 Var: %s: old value:0x%08x, new value:0x%08x\n", var.mName.data(),
                           oldValue, newValue);
          *pointer = newValue;
        }
        break;
      }
      }
    }
  } else {
    DEBUG_CONSOLE_PRINTF("SetDebugVar: Can't find var: %s\n", name.data());
  }
}

SDebugVar::SDebugVar(rstl::string name) : mName(name), mType(kT_Invalid), mValue(nullptr) {}

static void GetDebugVar(const char*, const char* args) {
  CStringTokenizer tokenizer(args);
  rstl::string name = tokenizer.ReadToken(nullptr, '"');
  SDebugVar key = SDebugVar(rstl::string(name));
  rstl::vector< SDebugVar >::iterator it =
      rstl::lower_bound(sDebugVars.begin(), sDebugVars.end(), key);
  if (it != sDebugVars.end()) {
    SDebugVar var = *it;
    if (var.mName == key.mName) {
      switch (var.mType) {
      case SDebugVar::kT_Int32: {
        int value = *static_cast< int* >(var.mValue);
        DEBUG_CONSOLE_PRINTF("Int32 Var: %s: value:%d\n", var.mName.data(), value);
        break;
      }
      case SDebugVar::kT_Real32: {
        float value = *static_cast< float* >(var.mValue);
        DEBUG_CONSOLE_PRINTF("Real32 Var: %s: value:%f\n", var.mName.data(), value);
        break;
      }
      case SDebugVar::kT_Uint32: {
        uint value = *static_cast< uint* >(var.mValue);
        DEBUG_BBA_PRINTF("Uint32 Var: %s: value:0x%08x\n", var.mName.data(), value);
        break;
      }
      }
    }
  } else {
    DEBUG_CONSOLE_PRINTF("GetDebugVar: Can't find var: %s\n", name.data());
  }
}

// Also writes every var to DebugVars.txt on the host.
static void GetAllDebugVars(const char*, const char*) {
  if (sDebugVars.begin() == sDebugVars.end()) {
    DEBUG_CONSOLE_PRINTF("GetAllDebugVars: No vars in list.\n");
    return;
  }

  char line[1024] = "";
  int file = -1;
  CBBASupport::BBAOpen("DebugVars.txt", 2, &file);
  for (rstl::vector< SDebugVar >::iterator it = sDebugVars.begin(); it != sDebugVars.end(); ++it) {
    SDebugVar var = *it;
    switch (var.mType) {
    case SDebugVar::kT_Int32: {
      int value = *static_cast< int* >(var.mValue);
      DEBUG_CONSOLE_PRINTF("Int32 Var: %s: value:%d\n", var.mName.data(), value);
      sprintf(line, "Int32 Var: %s: value:%d\n", var.mName.data(), value);
      break;
    }
    case SDebugVar::kT_Real32: {
      float value = *static_cast< float* >(var.mValue);
      DEBUG_CONSOLE_PRINTF("Real32 Var: %s: value:%f\n", var.mName.data(), value);
      sprintf(line, "Real32 Var: %s: value:%f\n", var.mName.data(), value);
      break;
    }
    case SDebugVar::kT_Uint32: {
      uint value = *static_cast< uint* >(var.mValue);
      DEBUG_BBA_PRINTF("Uint32 Var: %s: value:0x%08x\n", var.mName.data(), value);
      sprintf(line, "Uint32 Var: %s: value:0x%08x\n", var.mName.data(), value);
      break;
    }
    }
    const int length = strlen(line);
    line[length - 1] = '\r';
    line[length] = '\n';
    if (file != -1) {
      CBBASupport::BBAWrite(file, line, length + 1);
    }
  }
  if (file != -1) {
    CBBASupport::BBAClose(file);
  }
}

static void SetDebugOption(const char*, const char* args) {
  CStringTokenizer tokenizer(args);
  while (!tokenizer.IsExhausted()) {
    rstl::string index = tokenizer.ReadToken(nullptr, '"');
    rstl::string value = tokenizer.ReadToken(nullptr, '"');
    int option = atoi(index.data());
    float optionValue = atof(value.data());
    gpGameDebug->ReadEngineState();
    gpGameDebug->GetOption(option)->SetValue(optionValue);
    gpGameDebug->ApplyOptions();
  }
}

// Lists the registered options of every category with their index and value.
static void ShowDebugOptions(const char*, const char*) {
  rs_debugger_printf("\n");
  for (int category = 0; category < CGameDebug::kC_Count; ++category) {
    int count = 0;
    for (int i = 0; i < CGameDebug::kDO_Count; ++i) {
      CDebugOption* option = gpGameDebug->GetOption(i);
      if (option != nullptr && option->GetCategory() == category) {
        ++count;
      }
    }
    if (count != 0) {
      rs_debugger_printf("\n%s\n****\n", CGameDebug::GetCategoryName(category));
      for (int i = 0; i < CGameDebug::kDO_Count; ++i) {
        CDebugOption* option = gpGameDebug->GetOption(i);
        if (option != nullptr && option->GetCategory() == category) {
          rs_debugger_printf("%3d \t%-40s\t%10.4f\n", i, option->GetName().data(),
                             option->GetValue());
        }
      }
    }
  }
  rs_debugger_printf("\n");
}

static void ShowInventory(const char*, const char*) {
  rs_debugger_printf("Player inventory\nAmount, Capacity, Time\n");
  if (sStateManager != nullptr) {
    CPlayerState* playerState = gpGameState->GetPlayerState();
    for (int i = 0; i < CPlayerState::kIT_Max; ++i) {
      const CPlayerState::CPowerUp& powerUp =
          playerState->PowerUp(static_cast< CPlayerState::EItemType >(i));
      rs_debugger_printf("%02d:\t%d, %d, %f\t%s\n", i, powerUp.mAmount, powerUp.mCapacity,
                         powerUp.mTimeLeft,
                         CPlayerState::GetItemName(static_cast< CPlayerState::EItemType >(i)));
    }
    rs_debugger_printf("Player inventory\nAmount, Capacity, Time\n");
  }
}

static void StartProfile(const char* command, const char* args) {
  rs_debugger_printf("StartProfile received: %s, %s\n", command, args);
}

static void GotMouseFocus(const char*, const char* args) { sMouseInfo.mHasFocus = atoi(args) != 0; }

static void MouseInfo(const char*, const char* args) {
  CStringTokenizer tokenizer(args);
  float x = atof(tokenizer.ReadToken(nullptr, '"').data());
  float y = atof(tokenizer.ReadToken(nullptr, '"').data());
  int buttons = atoi(tokenizer.ReadToken(nullptr, '"').data());
  sMouseInfo.mLeftButton = buttons & 0x1;
  sMouseInfo.mPosition.SetX(x);
  sMouseInfo.mRightButton = (buttons & 0x2) != 0;
  sMouseInfo.mMiddleButton = (buttons & 0x10) != 0;
  sMouseInfo.mPosition.SetY(y);
}

static void PrintDebugMessages(const char*, const char*) {
  for (int i = 0; i < CGameDebug::kDO_Count; ++i) {
    CDebugOption* option = gpGameDebug->GetOption(i);
    if (option != nullptr && option->GetMessages().size() != 0) {
      static const char* kSeparator = "-------------------\n";
      rs_debugger_printf("%s%s\n%s", kSeparator, option->GetName().data(), kSeparator);
      for (int j = 0; j < option->GetMessages().size(); ++j) {
        rs_debugger_printf("%s\n", option->GetMessages()[j].data());
      }
      rs_debugger_printf(kSeparator);
    }
  }
}
