#include <cstdio>
#include <string>
#include <windows.h>
#include <psapi.h>

// Name of the program to inject the dll in
static const std::string programName = "test.exe";

// Name of the dll to inject
static const std::string dllName = "libinjected.dll";

DWORD findPid(const std::string& programName)
{
  // Enumerate all processes
  DWORD pids[1024];
  DWORD temp;
  if (!EnumProcesses(pids, sizeof(pids), &temp))
  {
    return 1;
  }

  // Find the first process with the given program name
  DWORD noPids = temp / sizeof(DWORD);
  for (DWORD i = 0; i < noPids; i++)
  {
    if (pids[i] == 0)
    {
      continue;
    }
    HANDLE tempHandle = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pids[i]);
    if (tempHandle == NULL)
    {
      continue;
    }
    HMODULE tempModule;
    if (EnumProcessModules(tempHandle, &tempModule, sizeof(tempModule), &temp))
    {
      TCHAR szProcessName[MAX_PATH];
      GetModuleBaseName(tempHandle, tempModule, szProcessName, sizeof(szProcessName) / sizeof(TCHAR));
      if (strcmp(programName.c_str(), szProcessName) == 0)
      {
        return pids[i];
      }
    }
  }
  return 0;
}

bool injectDLL(DWORD pid)
{
  // Get full path of our dll
  char fullDllName[1024];
  if (GetFullPathName(dllName.c_str(), sizeof(fullDllName), fullDllName, nullptr) == 0)
  {
    return false;
  }

  // Open process using pid
  HANDLE handle = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
  if (handle == NULL)
  {
    return false;
  }

  // Get the address to the function LoadLibraryA in kernel32.dll
  LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
  if (LoadLibAddr == NULL)
  {
    return false;
  }

  // Allocate memory inside the opened process
  LPVOID dereercomp = VirtualAllocEx(handle, NULL, strlen(fullDllName), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
  if (dereercomp == NULL)
  {
    return false;
  }

  // Write the DLL name to the allocated memory
  if (!WriteProcessMemory(handle, dereercomp, fullDllName, strlen(fullDllName), NULL))
  {
    return false;
  }

  // Create a thread in the opened process
  HANDLE remoteThread = CreateRemoteThread(handle, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
  if (remoteThread == NULL)
  {
    return false;
  }

  // Wait until thread have started (or stopped?)
  WaitForSingleObject(remoteThread, INFINITE);

  // Free the allocated memory
  VirtualFreeEx(handle, dereercomp, strlen(fullDllName), MEM_RELEASE);

  // Close the handles
  CloseHandle(remoteThread);
  CloseHandle(handle);

  return true;
}

int main(int argc, char* argv[])
{
  printf("Finding pid for: %s\n", programName.c_str());
  DWORD pid = findPid(programName);
  if (pid == 0)
  {
    fprintf(stderr, "Could not find process: %s\n", programName.c_str());
    return 1;
  }

  printf("Injecting DLL: %s\n", dllName.c_str());
  if (!injectDLL(pid))
  {
    fprintf(stderr, "Could not inject DLL: %s\n", dllName.c_str());
    return 1;
  }

  printf("Done!\n");
  return 0;
}
