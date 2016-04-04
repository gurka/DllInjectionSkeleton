#include <cstdio>
#include <cstring>
#include "console.h"

Console::~Console()
{
  if (consoleAllocated)
  {
    FreeConsole();
  }
}

bool Console::init(const std::string& title)
{
  AllocConsole();
  consoleAllocated = true;

  consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
  if (consoleHandle == INVALID_HANDLE_VALUE)
  {
    return false;
  }

  CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
  GetConsoleScreenBufferInfo(consoleHandle, &consoleInfo);
  consoleInfo.dwSize.X = 80;
  consoleInfo.dwSize.Y = 400;
  SetConsoleScreenBufferSize(consoleHandle, consoleInfo.dwSize);

  SetConsoleTitleA(title.c_str());

  ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);

  return true;
}

void Console::write(const std::string& message)
{
  write(message.c_str());
}

void Console::write(const char* format, ...)
{
  if (consoleAllocated && consoleHandle != INVALID_HANDLE_VALUE)
  {
    char message[1024];

    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    WriteConsoleA(consoleHandle, message, strlen(message), nullptr, nullptr);
  }
}
