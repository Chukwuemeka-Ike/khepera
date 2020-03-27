# Khepera IV Pose Estimation Package

This package was developed by Chukwuemeka Osaretin Ike (2020) to facilitate a research project in the Julius Lab at Rensselaer Polytechnic Institute.

The tools described here employ the OpenCV and ArUco libraries in estimating the pose of a Khepera IV robot with respect to ArUco markers placed throughout an experimental space.

## Purpose
The contents of this repository are intended to assist the user in setting up:
1. A cross-compilation environment using the Arm Linux Poky toolchain provided by the K-team.
2. A cross-compiled version of the OpenCV library allowing you build your applications on a Linux host, and run them on the Khepera.
3. A method to allow communications between a swarm of Kheppies.

You will need:
1. Linux computer/virtual machine.
2.

## Background
There are some important things to understand about the Khepera IV robot which will help understand why certain things can or can't be done on the bot.

A quick search will lead you to the product page at:
 https://www.k-team.com/khepera-iv
 <div align="center"></div>

The robot possesses the following:
1. 800 MHz ARM Cortex-A8
2. Gumstix Overo Processor with armv7l architecture
3. Angstrom Distribution
4. Poky as the packager
5.

This information is most useful when building the OpenCV library, as it determines the GNU toolchain and the versions of the C++ standard libraries that can be used.


## Khepera Tools
The manual suggests making a folder named 'khepera4_development' in the home folder, which is what was done, and this will be used for the rest of the tutorial.

#### Install Poky Toolchain
The version of the toolchain used is available [here](http://ftp.k-team.com/KheperaIV/software/Gumstix%20COM%20P/light%20tools/).

You then need to run the following:
```bash
sudo tar -xjf khepera4-yocto-light-kb1.0.tar.bz2 -C /usr/local
```
It is important that the toolchain is placed in this folder because of hardcoded links in the code. Of course, this can be changed if a savvy user really wants to.

Following the extraction, we then need to set the environment variables to allow the shell recognize the installation. This can be done by running
```bash
export K4_YOCTO=/usr/local/khepera4-yocto/build/tmp/sysroots/i686-linux/usr/bin/armv7a-vfp-neon-poky-linux-gnueabi
export PATH=$K4_YOCTO/arm-poky-linux-gnueabi
```
These will have to be run in every bash instance to ensure the toolchain is available for use. To automate this process a little, I like to add those two lines to the end of the ~/.bashrc file. Doing so eliminates the need to run them every time the shell is opened.

#### Install Khepera Library
The version of the Khepera library used in this package is 2.0, and can be downloaded
[here](http://ftp.k-team.com/KheperaIV/software/Gumstix%20COM/library/).

You will need to run
```bash
tar -xjf libkhepera-2.0.tar.bz2 -C ~/khepera4_development
cd ~/khepera4_development/libkhepera-2.0
make clean
make all
```
Assuming you made no changes to the library, you shouldn't have to transfer any of the built files to the robot. The header files that are generated in the *'libkhepera-2.0/build-khepera-3.5.7-custom/include'* folder are the same that are in the *include* folder of this repo


The Aruco dictionary used by this project is DICT_4X4_50


## OpenCV
#### Required Setup
This is the main

#### Building OpenCV for ARM
Instructions on how to cross-compile the OpenCV library for a generic ARM-based Linux system are available [here](
https://docs.opencv.org/2.4/doc/tutorials/introduction/crosscompilation/arm_crosscompile_with_cmake.html), but certain aspects of those instructions need to be adapted to fit our use case.

For starters, we do not need the *gcc-arm-linux-gnueabi* tools since our toolchain is the *arm-poky-linux-gnueabi* tools downloaded above.


```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules/aruco -DOPENCV_ENABLE_CXX11=OFF -DSOFTFP=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_TOOLCHAIN_FILE=../arm-gnueabi.toolchain.cmake -DBUILD_OPENCV_PYTHON=OFF ../opencv
```
Information on what all these parameters mean is available [here]().

Of note here is the
```bash
-DCMAKE_TOOLCHAIN_FILE=../arm-gnueabi.toolchain.cmake
```
parameter. This should point to the path where the files in the *opencv-toolchain-files* (from this repo) are located. These specify that the make utility uses the Poky toolchain, allowing us run the OpenCV binaries and our own code on the Khepera IV.

Should everything go well, you can then run:
```bash
make -j7
make install
```

The built OpenCV library should now be compatible for the Khepera robot.

The last thing that needs to be done to ensure the robot can use the library is to copy the files from the /build/lib
