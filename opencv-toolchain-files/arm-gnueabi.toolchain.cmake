set(GCC_COMPILER_VERSION "" CACHE STRING "GCC Compiler version")
set(GNU_MACHINE "arm-poky-linux-gnueabi" CACHE STRING "GNU compiler triple")

# Set the CMAKE GNU Tools to match those of the Poky compiler used by Khepera
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(MY_SYSROOT /usr/local/khepera4-yocto/build/tmp/sysroots/i686-linux CACHE PATH "ARM cross compilation system root")
set(MY_CROSS_DIR /usr/local/khepera4-yocto/build/tmp/sysroots/i686-linux/usr/bin/armv7a-vfp-neon-poky-linux-gnueabi CACHE PATH "ARM cross compiler path")
set(CMAKE_C_COMPILER ${MY_CROSS_DIR}/arm-poky-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${MY_CROSS_DIR}/arm-poky-linux-gnueabi-g++)
set(CMAKE_AR ${MY_CROSS_DIR}/arm-poky-linux-gnueabi-ar)
set(CMAKE_LINKER ${MY_CROSS_DIR}/arm-poky-linux-gnueabi-ld)
set(CMAKE_NM ${MY_CROSS_DIR}/arm-poky-linux-gnueabi-nm)
set(CMAKE_OBJCOPY ${MY_CROSS_DIR}/arm-poky-linux-gnueabi-objcopy)
set(CMAKE_OBJDUMP ${MY_CROSS_DIR}/arm-poky-linux-gnueabi-objdump)
set(CMAKE_RANLIB ${MY_CROSS_DIR}/arm-poky-linux-gnueabi-ranlib)
set(CMAKE_STRIP ${MY_CROSS_DIR}/arm-poky-linux-gnueabi-strip)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -march=armv7-a ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a")

# --sysroot=${MY_SYSROOT}-mfpu=neon -cpu=cortex-a7 CACHE INTERNAL"" [FORCE]

include("${CMAKE_CURRENT_LIST_DIR}/arm.toolchain.cmake")
