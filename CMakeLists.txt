cmake_minimum_required(VERSION 2.8)

project( staticTest)


set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER ${tools}/usr/bin/arm-linux-gnueabi-gcc)
set(CMAKE_CXX_COMPILER ${tools}/usr/bin/arm-linux-gnueabi-g++)

# /usr/local/khepera4-yocto/build/tmp/sysroots/i686-linux/usr/bin/armv7a-vfp-neon-poky-linux-gnueabi/


find_package( OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} khepera)

#add_executable( staticTest staticTest.cpp )
#target_link_libraries( staticTest ${OpenCV_LIBS} )

#add_executable( markerMaker markerMaker.cpp)
#target_link_libraries(markerMaker ${OpenCV_LIBS} )

#add_executable( videoTest videoTest.cpp )
#target_link_libraries( videoTest ${OpenCV_LIBS} )

#add_executable( cameraCalib cameraCalib.cpp )
#target_link_libraries( cameraCalib ${OpenCV_LIBS} )

add_executable( cross cross.cpp )
target_link_libraries( cross PUBLIC ${OpenCV_LIBS} khepera)
