#include <stdio.h>
#include <stdlib.h>
// #include <opencv2/opencv.hpp>
// #include <opencv2/aruco.hpp>
#include <iostream>

// Important namespaces for less typing
using namespace std;
// using namespace cv;

int main()
{
  float transCameraToTag[3][3] = {{0.1, 2, 3},
                                 {4.1, 5.3, 6.2},
                                 {7, 8, 9}};

  double trial = (0.99309*0.3) + (-0.00289535*1) + ( -0.044251*1);
  cout << trial << endl;

  return 0;
}
