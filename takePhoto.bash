#!/bin/bash

# Declare the echo variables to let you know what's going on
WiFI_RELOAD="Restarting WiFi adapter to make sure it's connected"
SET_PIPES="Setting the pipes to allow capture"
TAKING_PHOTO="Capturing photo"
RUNNING_PROC="Processing the image"

# Restart WiFi adapter to connect it to the XT1575 network
# echo $WiFI_RELOAD
#systemctl daemon-reload
#systemctl restart wpa_supplicant@wlan0
#ifdown wlan0
#ifup wlan0

# Set the pipes to allow you take a photo
echo $SET_PIPES
media-pipes.sh
media-formats.sh 752 480

# Take the photo
echo $TAKING_PHOTO
v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -I -1

# Run the image processing binary
echo $RUNNING_PROC
./cross

# If everything went well
echo Success!
