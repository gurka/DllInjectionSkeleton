#include <thread>
#include <windows.h>

#include "console.h"

static Console console;

void injectedMain()
{
  console.init("Injected DLL");

  console.write("hey its me ur brother");
}

extern "C" __declspec(dllexport) BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
  static std::thread injectedMainThread;

  switch (fdwReason)
  {
    case DLL_PROCESS_ATTACH:
    {
      injectedMainThread = std::thread(injectedMain);
      break;
    }

    case DLL_PROCESS_DETACH:
    {
      if (injectedMainThread.joinable())
      {
        injectedMainThread.join();
      }
      break;
    }
  }

  return TRUE;
}
