set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -target aarch64-linux-gnueabihf -mcpu=cortex-a53 -mfpu=neon-fp-armv8 --sysroot=/Users/borky/toolchains/aarch64-linux-gnueabihf/sysroot/sys-root/ --verbose")

include_directories(/Users/borky/toolchains/aarch64-linux-gnueabihf/include)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
