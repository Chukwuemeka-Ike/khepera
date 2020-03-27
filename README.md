# Khepera IV Pose Estimation Package

This package was developed by Chukwuemeka Osaretin Ike (2020) to facilitate a research project in the Julius Lab at Rensselaer Polytechnic Institute.

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
export
export 
```

#### Install Khepera Library
The version of the Khepera library used in this package is 2.0, and can be downloaded from
[here](http://ftp.k-team.com/KheperaIV/software/Gumstix%20COM/library/).

You will need to then transfer this

The Aruco dictionary used by this project is DICT_4X4_50

The tools in this repo employ the OpenCV and ArUco libraries in estimating the pose of a Khepera IV robot with respect to ArUco markers placed throughout the experimental space.

The functionality was developed by Chukwuemeka Ike to aid Ruixuan Yan in his experiments which required that a swarm of robots be able to find their collective centroid.

## OpenCV
#### Required Setup
This is the mainThe proposed system uses the ArUco library to estimate the pose

#### Building OpenCV for ARM
Instructions on how to cross-compile the OpenCV library for a generic ARM-based Linux system are available [here](
https://docs.opencv.org/2.4/doc/tutorials/introduction/crosscompilation/arm_crosscompile_with_cmake.html), but certain aspects of those instructions need to be adapted to fit our use case.

For starters, we do not need the *gcc-arm-linux-gnueabi* tools since our toolchain is the *arm-poky-linux-gnueabi* tools downloaded above.

The main aspects of the  that need to be adapted from the
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules/aruco -DOPENCV_ENABLE_CXX11=OFF -DSOFTFP=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_TOOLCHAIN_FILE=../arm-gnueabi.toolchain.cmake -DBUILD_OPENCV_PYTHON=OFF ../opencv
```
