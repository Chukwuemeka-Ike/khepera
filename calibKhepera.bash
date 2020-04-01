#!/bin/bash

SET_PIPES="Setting the pipes to allow capture"

# Set the pipes to allow you take a photo
echo $SET_PIPES
media-pipes.sh
media-formats 752 480

echo "Taking "
mkdir images
for((i=1; i<=3; i++))
do
	v4l2grab -d /dev/video6 -o ./images/image$i.jpg -W 752 -H 480 -I -1
done
echo "Done taking photos"
