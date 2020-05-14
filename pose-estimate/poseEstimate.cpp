#include "../include/khepera.h"
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
  cv::Mat inputImage, frame, cameraMatrix, distCoeffs, rotMatrix;
  cout << "\nSetting Pipes\n";
  system("media-pipes.sh");
  cout << "Setting Format\n";
  system("media-formats.sh 752 480");
  cout << "Taking Image\n\n";
  system("v4l2grab -d /dev/video6 -o image.jpg -W 752 -H 480 -q 85 -I -1");
  cout << "Made it thus far. Opening camera file, and detecting tags\n\n";

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
    cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.053, cameraMatrix, distCoeffs, rvecs, tvecs);

    // Draw the axes for each marker visible in the image
    for(int i=0; i<markerIds.size(); i++)
    {
      cv::aruco::drawAxis(frame, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
      cout << "Marker " << markerIds[i] << ": " << endl;
      cout << "Rotation: " << rvecs[i] << endl;
      cout << "Translation: " << tvecs[i] << endl << endl;
    }
    imwrite("processedImg.jpg", frame);


    // Convert the rotation vector to a rotation matrix
    cv::Rodrigues(rvecs[0], rotMatrix);
    double transCameraToTag[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,1}};
    double transTagToBase[4][4] = {{1,0,0,0.3}, {0,1,0,0}, {0,0,-1,1}, {0,0,0,1}};
    double transCameraToBase[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};

    // Transpose the rotation matrix and add it to the transformation matrix
    for(int i = 0; i < rotMatrix.rows; i++)
    {
      const double* rotMatI = rotMatrix.ptr<double>(i);
      for(int j = 0; j < rotMatrix.cols; j++)
      {
        transCameraToTag[j][i] = rotMatI[j];
      }
    }
    // Compute the transformed translation from the camera to the tag frame
    for (int i = 0; i < 3; i++)
    {
      for (int j = 0; j < 3; j++)
      {
          transCameraToTag[i][3] += transCameraToTag[i][j]*tvecs[0][j];
      }
      transCameraToTag[i][3] = -transCameraToTag[i][3];
    }

    cout << "Rotation Matrix: \n" << rotMatrix << endl;
    cout << "Camera To Tag: \n";
    for(int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        cout << transCameraToTag[i][j] << "|";
      }
      cout << "\n";
    }
    cout << "\n\n";
    cout << "Tag To Base: \n";
    for(int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        cout << transTagToBase[i][j] << "|";
      }
      cout << "\n";
    }
    cout << "\n\n";


    // Final Transformation Matrix from the Camera to the Base Frame
    for(int i = 0; i < 4; i++)
    {
      for(int j = 0; j < 4; j++)
      {
        for (int k = 0; k < 4; k++)
        {
          transCameraToBase[i][j] += (double) transTagToBase[i][k] * transCameraToTag[k][j];
        }
      }

    }
    cout << "Camera To Base: \n";
    for(int i = 0; i < 4; i++)
    {
      for (int j = 0; j < 4; j++)
      {
        cout << std::setprecision(4) << transCameraToBase[i][j] << "|";
      }
      cout << "\n";
    }
    cout << "\n\n";


  }
  else
  {
    printf("No Markers Detected!\n");
  }

  return 0;
}
