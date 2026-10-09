#ifndef _CBBASUPPORT
#define _CBBASUPPORT

#include "types.h"

#include "Kyoto/TFunctor.hpp"
#include "rstl/string.hpp"

class CGuid;

// The broadband adapter link to the development host: messages, remote commands and host file
// access. Every member is static. The class name is from the unit (CBBASupport.cpp); the member
// names are guessed.
class CBBASupport {
public:
  // 0x8053EB74: formats into a 0x800-byte buffer and sends it over the broadband adapter
  // when connected, otherwise prints it with rs_debugger_printf ("rs_bba_printf: " prefix).
  static void Printf(const char* format, ...);

  // 0x80541E78: sends the string as a message when connected (the guid is generated when null).
  // Returns false when there is no connection.
  static bool SendString(const rstl::string& text, int channel, const CGuid* guid);

  // Host file access over the broadband adapter. Open returns 0 on success and stores the handle;
  // CGameDebug passes mode 2 to create a file, 1 to append and 0 to probe for an existing file.
  static int BBAOpen(const char* path, int mode, int* handle);
  static void BBAWrite(int handle, const void* data, int size);
  static void BBAClose(int handle);

  // Guessed names. What the host returns for a remote command: its output, then a status word.
  struct SRemoteCommandResult {
    rstl::string mOutput;
    int mStatus;
  };
  // Guessed name. 0x805400C8: runs a shell command on the host, waiting up to timeout seconds.
  static SRemoteCommandResult ExecuteRemoteSystemCommand(const rstl::string& command,
                                                         float timeout);
  // Guessed name. 0x805406F4: reads an environment variable on the host.
  static rstl::string GetRemoteEnvironmentVariable(const rstl::string& name);

  // CMain's subsystem setup and teardown. InitializeBBA returns nonzero on failure.
  static int InitializeBBA(int);
  static void RegisterStringMessageCallback(int,
                                            const TFunctor2< int, const rstl::string& >& callback);
  static void Shutdown();
  // CMain's frame loop polls for host messages once per frame.
  static void PollMessages();

  // 0x80540988: when connected, asks the host for the list of assets it serves and waits for it.
  static void RefreshNetworkAssets();
};

#endif // _CBBASUPPORT
