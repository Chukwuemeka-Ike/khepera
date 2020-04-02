#!/bin/bash

# Declare the echo variables to let you know what's going on
SET_PIPES="Setting the pipes to allow capture"
TAKING_PHOTO="Capturing photo"
RUNNING_PROC="Processing the image"


# Set the pipes to allow you take a photo
echo $SET_PIPES
media-pipes.sh
media-formats.sh 752 480

# Take the photo
echo $TAKING_PHOTO
v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -I -1

# Run the image processing binary
echo $RUNNING_PROC
./poseEstimate

# If everything went well
echo Success!
