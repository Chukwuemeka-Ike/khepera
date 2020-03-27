# Khepera IV Pose Estimation Package

This package was developed by Chukwuemeka Osaretin Ike (2020) to facilitate a research project in the Julius Lab at Rensselaer Polytechnic Institute.

## Purpose
The contents of this repository are intended to assist the user in setting up:
1. A cross-compilation environment using the Arm Linux Poky toolchain provided by the K-team.
2. A cross-compiled version of the OpenCV library allowing you build your applications on a Linux host, and run them on the Khepera.
3. A method to allow communications between a swarm of Kheppies.

## Background
There are some important things to understand about the Khepera IV robot which will help understand why certain things can or can't be done on the bot.

A quick search will lead you to the product page at:
<div align="center"> https://www.k-team.com/khepera-iv </div>
which is a lightweight differential robot.

Most importantly, the robot
1. 800 MHz ARM Cortex-A8
2. Gumstix Overo Processor with armv7l architecture
3. Angstrom Distribution
4. Poky as the packager
5.

Workspace to develop the tools for the Khepera robot

The Aruco dictionary used by this project is DICT_4X4_50

The tools in this repo employ the OpenCV and ArUco libraries in estimating the pose of a Khepera IV robot with respect to ArUco markers placed throughout the experimental space.

The functionality was developed by Chukwuemeka Ike to aid Ruixuan Yan in his experiments which required that a swarm of robots be able to find their collective centroid.

## OpenCV
#### Required Setup
This is the mainThe proposed system uses the ArUco library to estimate the pose

#### Building OpenCV for ARM
The
```bash
cmake -DCMAKE_BUILD_TYPE=Release -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules/aruco -DOPENCV_ENABLE_CXX11=OFF -DSOFTFP=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_TOOLCHAIN_FILE=../arm-gnueabi.toolchain.cmake -DBUILD_OPENCV_PYTHON=OFF ../opencv
```
