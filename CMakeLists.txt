cmake_minimum_required(VERSION 3.0)

project(dll-injection-skeleton)

include(GenerateExportHeader)

add_definitions(-D_WIN32_WINNT=0x0502)

# Common compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")

# dll-injector.exe
set(dll-injector_src
  "src/dll-injector/dll_injector.cc"
)

set(dll-injector_lib
  "psapi"
)

add_executable(dll-injector ${dll-injector_src})
target_link_libraries(dll-injector ${dll-injector_lib})

# injected.dll
set(injected_src
  "src/injected/injected.cc"
  "src/injected/console.cc"
)

add_library(injected MODULE ${injected_src})
