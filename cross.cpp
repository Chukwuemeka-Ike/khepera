#include "khepera/khepera.h"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace std;
using namespace cv;

int main(int argc, char *argv[])
{
  int rc;

  /* Set the libkhepera debug level - Highly recommended for development. */
  kb_set_debug_level(2);


  printf("Hello, World!\n");

  cv::Mat inputImage, imageCopy;
  inputImage = imread("./test images/image02.jpg");


  // cv::Mat cameraMatrix, distCoeffs;
  // readCameraParameters(cameraMatrix, distCoeffs);

  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f> > markerCorners, rejectedCandidates;
  cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
  cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

  inputImage.copyTo(imageCopy);

  // if at least one marker detected
  if (markerIds.size() > 0){
    cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);

    // std::vector<cv::Vec3d> rvecs, tvecs;
    // cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

    // draw axis for each marker
    // for(int i=0; i<markerIds.size(); i++){
    //   cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
    // }
  }

  return 0;
}
