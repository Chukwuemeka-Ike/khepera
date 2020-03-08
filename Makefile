# Makefile to compile the pose estimator for the Khepera IV robot
#

objects = poseEstim.o
VPATH = src:../headers

poseEstim : $(objects)			# The default goal to create poseEstim exe
					cc -o poseEstim $(objects)
poseEstim.o : khepera.h opencv2.hpp aruco.hpp

.PHONY : clean
clean :
					-rm poseEstim $(objects)
