set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arch64)

set(CMAKE_SYSROOT c:/SysGCC/raspberry64/aarch64-linux-gnu/sysroot)
set(CMAKE_STAGING_PREFIX ./bin)

set(CMAKE_C_COMPILER c:/SysGCC/raspberry64/bin/aarch64-linux-gnu-gcc.exe)
set(CMAKE_CXX_COMPILER c:/SysGCC/raspberry64/bin/aarch64-linux-gnu-g++.exe)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)