cmake_minimum_required(VERSION 2.8)

project(kheperaCalibration)


set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER ${tools}/usr/local/khepera4-yocto/build/tmp/sysroots/i686-linux/usr/bin/armv7a-vfp-neon-poky-linux-gnueabi/arm-poky-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${tools}/usr/local/khepera4-yocto/build/tmp/sysroots/i686-linux/usr/bin/armv7a-vfp-neon-poky-linux-gnueabi/arm-poky-linux-gnueabi-g++)


#/usr/bin/arm-linux-gnueabi-gcc
#/usr/bin/arm-linux-gnueabi-g++

find_package( OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} khepera v4l2)


add_executable( kheperaCalibration kheperaCalibration.cpp )
target_link_libraries( kheperaCalibration PUBLIC ${OpenCV_LIBS})
