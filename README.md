# khepera
Workspace to develop the tools for the Khepera robot

The Aruco dictionary used by this project is DICT_4X4_50

The tools in this repo employ the OpenCV and ArUco libraries in estimating the pose of a Khepera IV robot with respect to ArUco markers placed throughout the experimental space.

The functionality was developed by Chukwuemeka Ike to aid Ruixuan Yan in his experiments which required that a swarm of robots be able to find their collective centroid.


cmake -DCMAKE_BUILD_TYPE=Release -DOPENCV_EXTRA_MODULES_PATH=../opencv_contrib/modules/aruco -DOPENCV_ENABLE_CXX11=OFF -DSOFTFP=ON -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_TOOLCHAIN_FILE=../arm-gnueabi.toolchain.cmake -DBUILD_OPENCV_PYTHON=OFF ../opencv


