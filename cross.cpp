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



  // cv::Mat edges;
  //
  // VideoCapture cap(6);
  // if(!cap.isOpened())
  // {
  //   return -1;
  // }
  // for(;;)
  // {
  //   cv::Mat frame;
  //   cap >> frame;
  //   // cv::cvtColor(frame, edges, CV_BGR2GRAY);
  //   GaussianBlur(edges, edges, Size(7,7), 1.5, 1.5);
  //   Canny(edges, edges, 0, 30, 3);
  //   // if(cv::waitKey(30) >= 0)
  //   // {
  //   //   imwrite("./image.jpg", frame);
  //   //   break;
  //   // }
  //   // char key = (char) cv::waitKey(10);
  //   // if (key == 27)
  //   // {
  //     imwrite("./image.jpg", edges);
  //   //   break;
  //   // }
  // }

  // Start the video capture
  cv::VideoCapture inputVideo(6);
  inputVideo.open(6);

  printf("----- Starting video capture -----\n");
  if(!inputVideo.isOpened())
  {
    printf("Couldn't open video\n");
  }
  cv::Mat image, imageCopy;

  // cv::Mat cameraMatrix, distCoeffs;
  // readCameraParameters(cameraMatrix, distCoeffs);

  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f> > markerCorners, rejectedCandidates;
  cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

  while (inputVideo.grab())
  {
      printf("Capturing\n");
      inputVideo.retrieve(image);

      std::vector<int> markerIds;
      std::vector<std::vector<cv::Point2f> > markerCorners;
      cv::aruco::detectMarkers(image, dictionary, markerCorners, markerIds);

      // if at least one marker detected
      if (markerIds.size() > 0)
      {

        image.copyTo(imageCopy);
        cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);

        // std::vector<cv::Vec3d> rvecs, tvecs;
        // cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

        // draw axis for each marker
        // for(int i=0; i<markerIds.size(); i++){
        //   cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
        // }
      }

      // char key = (char) cv::waitKey(10);
      // if (key == 27)
      // {
      //
      //   break;
      // }
      imwrite("./image.jpg", image);
      cout << image << endl;


  }
  return 0;
}
