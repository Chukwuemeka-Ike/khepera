The files contained in this folder are intended to streamline the calibration of the Khepera robot. A quick description of the important ones follows.

### calibKhepera.bash
This bash script automates the image capture process by capturing as many photos as are specified in the script.

The more photos are taken, the better the final calibration will be, but only up to a point (left vague here because I don't know what point). To calibrate the initial Khepera I had in my possession, I took 80 images. The subsequent accuracy proved sufficient for preliminary testing.


### imageList.xml
Having taken the images, it is then necessary to write an *imageList.xml* file which lists the location of the calibration images taken. A sample format of this list is available [here](https://github.com/opencv/opencv/blob/master/samples/cpp/tutorial_code/calib3d/camera_calibration/VID5.xml).


### config.xml
This file contains information necessary for calibrating the camera. An example is available [here](https://github.com/opencv/opencv/blob/master/samples/cpp/tutorial_code/calib3d/camera_calibration/in_VID5.xml).

### kheperaCalibration.cpp
This is the main program that calibrates the camera on the robot. Assuming you name your files exactly as I did mine, you should not have to edit any of this code.

Simply compile it using
```bash
cmake .
make
```
 and transfer the resulting binary onto the Khepera.
