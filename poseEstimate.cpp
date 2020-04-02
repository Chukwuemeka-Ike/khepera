#include "include/khepera.h"
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

  // Declare the cv matrices to hold the images and necessary camera parameters
  cv::Mat inputImage, frame, cameraMatrix, distCoeffs;

  // Get the camera parameters from the cameraData.xml file
  FileStorage fs("./cameraData.xml", FileStorage::READ);
  if(!fs.isOpened())
  {
    cout << "Unable to access camera parameters!" << endl;
    return -1;
  }
  fs["camera_matrix"] >> cameraMatrix;
  fs["distortion_coefficients"] >> distCoeffs;
  fs.release();

  // Read the last image taken before the pose estimator was run
  inputImage = imread("./image.jpg");

  // Declare some variables to allow for ArUco detection
  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f> > markerCorners, rejectedCandidates;
  cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

  // Perform detection on the input image
  cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds);

  // If at least one marker is detected, we can attempt to estimate the pose
  if (markerIds.size() > 0)
  {
    // Copy the input image to a frame and draw the detected markers
    inputImage.copyTo(frame);
    cv::aruco::drawDetectedMarkers(frame, markerCorners, markerIds);

    // Estimate the pose of the markers
    std::vector<cv::Vec3d> rvecs, tvecs;
    cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

    // Draw the axes for each marker visible in the image
    for(int i=0; i<markerIds.size(); i++)
    {
      cv::aruco::drawAxis(frame, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
      cout << "Marker " << i << ": " << endl;
      cout << "Rotation: " << rvecs[i] << endl;
      cout << "Translation: " << tvecs[i] << endl << endl;
    }
    imwrite("processedImg.jpg", frame);
  }
  else
  {
    printf("No Markers Detected!\n");
  }


  return 0;
}
