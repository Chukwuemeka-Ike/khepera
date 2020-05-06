// #include "../include/khepera.h"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <time.h>

// Important namespaces for less typing
using namespace std;
using namespace cv;

// This robot's ID
#define MY_ID 10


int main(int argc, char *argv[])
{
  int rc;

  /* Set the libkhepera debug level - Highly recommended for development. */
  // kb_set_debug_level(2);

  // Start the video capture
  cv::VideoCapture inputVideo(0 +cv::CAP_ANY);
  inputVideo.open(0);

  // Create CV Matrices for the images, camera matrix, and distortion coefficients
  cv::Mat image, imageCopy, cameraMatrix, distCoeffs;
  FileStorage fs("camera.yaml", FileStorage::READ);

  fs["camera_matrix"] >> cameraMatrix;
  fs["distortion_coefficients"] >> distCoeffs;

  cout << "camera matrix: " << cameraMatrix << endl
       << "distortion coeffs: " << distCoeffs << endl;
  fs.release();


  // Create the marker dictionary we'll use in our detection
  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::
                        getPredefinedDictionary(cv::aruco::DICT_4X4_50);


  std::vector<cv::Vec3d> receivedRvecs, receivedTvecs;

  // Open the received position file
  string pose = "position.yaml";
  FileStorage poseFile(pose, FileStorage::READ);

  // Check if the file was available for use
  if (!poseFile.isOpened())
  {
    cout << "No pose file received!\n";
  }
  else
  {
    // Check if the file was addressed to this Khepera, and if so,
    int receivedID;
    poseFile["Target ID"] >> receivedID;
    if(receivedID == MY_ID)
    {
      cout << "Pose file addressed to this Khepera. Processing." << endl;
      // Declare a FileNode type for the translation vector
      FileNode tNode = poseFile["tvecs"];
      if (tNode.type() != FileNode::SEQ)
      {
          cerr << "Tvecs is not a sequence! FAIL" << endl;
          return 1;
      }
      // Go through the node
      FileNodeIterator it = tNode.begin(), it_end = tNode.end();
      for (; it != it_end; ++it)
      {
          (*it) >> receivedTvecs;
      }
      // Declare a FileNode type for the rotation vector
      tNode = poseFile["rvecs"];
      if (tNode.type() != FileNode::SEQ)
      {
          cerr << "Rvecs is not a sequence! FAIL" << endl;
          return 1;
      }
      // Go through the node
      it = tNode.begin(), it_end = tNode.end();
      for (; it != it_end; ++it)
      {
          (*it) >> receivedRvecs;
      }
    }
    else
    {
      cout << "Pose file not addressed to this Khepera \n";
    }
  }
  poseFile.release();
  FileStorage myPose(pose, FileStorage::WRITE);

  // try
  // {
  //   cout << "\n" << "Received Tvecs:\n" << receivedTvecs[0] << "\n";
  //   cout << "Received Rvecs:\n" << receivedRvecs[0] << "\n\n\n";
  // }
  // catch (...)
  // {
  //   cout << "\nNot addressed to this Khepera \n";
  // }

  // While the video is running, detect markers, and display the estimated poses of the markers
  while (inputVideo.grab())
  {
      inputVideo.retrieve(image);
      image.copyTo(imageCopy);

      std::vector<int> markerIds;
      std::vector<std::vector<cv::Point2f> > markerCorners;
      // cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
      cv::aruco::detectMarkers(image, dictionary, markerCorners, markerIds);

      // if at least one marker detected
      if (markerIds.size() > 0)
      {
          cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);

          std::vector<cv::Vec3d> rvecs, tvecs;
          cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05,
                                      cameraMatrix, distCoeffs, rvecs, tvecs);

          // cout << "tvecs size: " << sizeof(tvecs) << "\n";
          // myPose << "tvecs" << "[";
          // for(int i = 0; i < sizeof(tvecs); i++){
          //   myPose << tvecs[i];
          //   cout << tvecs[i];
          // }
          // myPose << "]";
          // cout << "\n\n\n";
          //
          // cout << "rvecs size: " << sizeof(rvecs) << "\n";
          // myPose << "rvecs" << "[";
          // for(int i = 0; i < sizeof(rvecs); i++){
          //   myPose << rvecs[i];
          //   cout << rvecs[i];
          // }
          // myPose << "]";
          // cout << "\n\n\n";

          // Choose a random target ID between 1 and 15, and add it to the position file
          srand (time(NULL));
          int targetID = ((rand() % 15) + 1);
          cout << targetID << endl;
          myPose << "Target ID" << targetID;

          // draw axis for each marker
          for(int i=0; i<markerIds.size(); i++)
          {
            cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
          }
          cout << "tvecs:\n" << tvecs[0] << "\n";
          myPose << "tvecs" << "[";
          myPose << tvecs[0] << "]";

          cout << "rvecs:\n" << rvecs[0] << "\n\n\n";
          myPose << "rvecs" << "[";
          myPose << rvecs[0] << "]";
          myPose.release();


      }
      // Show the estimated poses and detected markers
      cv::imshow("Output", imageCopy);
      char key = (char) cv::waitKey(10);
      if (key == 27)
          break;
  }


  return 0;
}
