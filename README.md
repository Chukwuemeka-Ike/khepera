# Khepera IV Pose Estimation Package

This package was developed by Chukwuemeka Osaretin Ike (2020) to facilitate a research project in the Julius Lab at Rensselaer Polytechnic Institute.

The tools described here employ the OpenCV and ArUco libraries in estimating the pose of a Khepera IV robot with respect to ArUco markers placed throughout an experimental space.

## Status
This project is a work in progress, but is nearing its final form.

## Purpose
The contents of this repository are intended to assist the user in setting up:
1. A cross-compilation environment using the Arm Linux Poky toolchain provided by the K-team.
2. A cross-compiled version of the OpenCV library allowing you build your applications on a Linux host, and run them on the Khepera.
3. A method to allow communications between a swarm of Kheppies.

I assume for this tutorial that you are using a Linux computer/virtual machine with apt as your package manager, and the bash shell.

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

This information is most useful when building the OpenCV library, as it determines the GNU toolchain and the versions of the C++ standard libraries that can be used.

## Preliminary Setup
Before any of the other tools can be setup, the following packages need to be installed on your Linux system:
1. Libv4l-0
2. v4l-utils
3. libv4l-dev
4. Libc6-armel-cross
5. Libc6-dev-armel-cross
6. Lib32z1
7. Binutils-arm-linux-gnueabi

I will admit, there is some redundancy in installing every one of those packages, but it doesn't hurt to cross-check. To install, run:
```bash
sudo apt install libv4l-0 v4l-utils libv4l-dev libc6-armel-cross libc6-dev-armel-cross lib32z1 binutils-arm-linux-gnueabi
```
Having these all installed before attempting to install the Khepera and OpenCV tools will make for a much smoother experience.

Once these are done, you can proceed to the next stage of the process.

## Khepera Tools
The manual suggests making a folder named 'khepera4_development' in the home folder, which is what was done, and this will be used for the rest of the tutorial.

#### Install Poky Toolchain
The version of the toolchain used is available [here](http://ftp.k-team.com/KheperaIV/software/Gumstix%20COM%20P/light%20tools/).

You then need to run the following:
```bash
sudo tar -xjf khepera4-yocto-light-kb1.0.tar.bz2 -C /usr/local
```
It is important that the toolchain is placed in this folder because of hardcoded links in the code. Of course, this can be changed if you really want to.

Following the extraction, we then need to set the environment variables to allow the shell recognize the installation. This can be done by running
```bash
export K4_YOCTO=/usr/local/khepera4-yocto/build/tmp/sysroots/i686-linux/usr/bin
export PATH=$PATH:$K4_YOCTO/armv7a-vfp-neon-poky-linux-gnueabi
```
These will have to be run in every bash instance to ensure the toolchain is available for use. To automate this process a little, I added those two lines to the end of the ~/.bashrc file. Doing so eliminates the need to run the commands every time the shell is opened.

#### Install Khepera Library
The version of the Khepera library used in this package is 2.0, and can be downloaded
[here](http://ftp.k-team.com/KheperaIV/software/Gumstix%20COM/library/).

You will need to run:
```bash
tar -xjf libkhepera-2.0.tar.bz2 -C ~/khepera4_development
cd ~/khepera4_development/libkhepera-2.0
make clean
make all
```
Assuming you made no changes to the library, you shouldn't have to transfer any of the built files to the robot. Should the need arise, the *libkhepera.so.2.0* and *libkhepera.a* files generated in the *'libkhepera-2.0/build-khepera-3.5.7-custom/lib'* folder will need to be transferred to the Khepera's */usr/lib* folder.

The header files that are generated in the *'libkhepera-2.0/build-khepera-3.5.7-custom/include'* folder are the same that are in the */include* folder of this repository, and are the ones used in building the sample applications.


## OpenCV
#### Required Setup
To get started, you will need to download the [OpenCV](https://github.com/opencv/opencv) and [OpenCV_contrib](https://github.com/opencv/opencv_contrib) library files. The first link contains the core OpenCV files, while the second contains a bunch of extra modules that add some capabilities, albeit with some stability issues. The ArUco library is contained in the *contrib* repo.

I suggest cloning both of these repos into the same OpenCV folder in your home directory. After doing so, you can move on to the next step of the process.

#### Building OpenCV for ARM
Instructions on how to cross-compile the OpenCV library for a generic ARM-based Linux system are available [here](
https://docs.opencv.org/2.4/doc/tutorials/introduction/crosscompilation/arm_crosscompile_with_cmake.html), but certain aspects of those instructions need to be adapted to fit our use case.

For starters, we do not need the *gcc-arm-linux-gnueabi* tools since our toolchain is the *arm-poky-linux-gnueabi* tools downloaded above.

Copy the files from the */opencv-toolchain-files* folder in this repo into the OpenCV folder where the OpenCV and OpenCV_contrib folders are located, then run the following commands in the shell.

```bash
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules/aruco -DOPENCV_ENABLE_CXX11=OFF -DSOFTFP=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_TOOLCHAIN_FILE=../arm-gnueabi.toolchain.cmake -DBUILD_OPENCV_PYTHON=OFF ../opencv
```
Information on these and more parameters is available [here](https://github.com/opencv/opencv/blob/master/CMakeLists.txt). I find that a number of them are self-explanatory, so I won't go into further detail.

One thing to note here is the
```bash
-DCMAKE_TOOLCHAIN_FILE=../arm-gnueabi.toolchain.cmake
```
parameter. This should point to the path where the files in the *opencv-toolchain-files* (from this repo) are located. These specify that the make utility uses the Poky toolchain, allowing us run the OpenCV binaries and our own code on the Khepera IV.

Should everything go well, you can then run:
```bash
make -j5
make install
```

The built OpenCV library should now be compatible for the Khepera robot.

The last thing that needs to be done to ensure the robot can use the library is to copy the files from the *lib/* folder in your build directory to the robot's */usr/lib* folder. This is a fairly crude way of doing this, but after the numerous issues I came across with getting the project started, it became the least of my worries.

I must mention that a number of the parameters used in building the library can probably be changed without adverse effects on the overall build, but I opted to not test every possible configuration, since I wasn't going for a large-scale deployment of this system. I leave it up to you to customize your own build as you'd like.

Now that you can use the OpenCV library on the Khepera, it's important that we first calibrate the camera, so as to allow for fairly accurate pose estimation.

## Camera Calibration
There are a million and one fairly exhaustive camera calibration tutorials, and I believe they all do it better than I can, so I will refrain from attempting. All that bears saying here is that in order for the robot to be able to consistently estimate the position of the ArUco tags relative to itself, it needs to have the intrinsic and extrinsic camera parameters.

The code in the *camera-calibration/* folder aims to extract these parameters by taking a series of photos using the camera, and then measuring the distortion over all these images.

## ArUco Tag Detection
The ArUco dictionary used by this project is DICT_4X4_50


The *camera-calibration* folder has instructions on using the contents therein. This was just a brief background on the necessity of calibration.

## Networking
While the ability of each Khepera to estimate its position in the environment was incredibly important, it was equally vital for it to be


## Usage
The remainder of this is written if you intend to use the code that I've already pre-written. To be sure, I assume in this section that each of the installed components were placed in the recommended folders. If you did not, I will do my best to point you to which aspects need to be tweaked in this tutorial.

I use the CMakeLists.txt file to instruct the cmake utility on how to build the poseEstimate executable. To use it, run
```bash
cmake .
make
```
The CMakeLists file specifies the target system architecture, the compiler to use, and the directory for the OpenCV package that we built specifically for Poky. Running ```cmake .``` creates a Makefile, which allows us build the executable that we can then run on the Khepera.
