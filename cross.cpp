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

  // Start the video capture
  // VideoCapture inputVideo(6);
  // inputVideo.set(CAP_PROP_FRAME_WIDTH,752);
  // inputVideo.set(CAP_PROP_FRAME_HEIGHT,480);
  //
  // printf("----- Starting video capture -----\n");
  // if(!inputVideo.isOpened())
  // {
  //   cerr << "Couldn't open video!\n";
  //   return -1;
  // }
  // else{
  //   printf("Made it this far!\n");
  // }
  //
  // cout << "Frame width: " << inputVideo.get(CAP_PROP_FRAME_WIDTH) << endl;
  // cout << "     height: " << inputVideo.get(CAP_PROP_FRAME_HEIGHT) << endl;
  // int num_frames = static_cast<int>(inputVideo.get(CAP_PROP_FRAME_COUNT));
  // cout << num_frames << " frames." << endl;
  // while(inputVideo.grab()){

    cv::Mat inputImage, frame;
    // inputVideo >> inputImage;
    // if(image.empty())
    // {
    //   printf("no inputImage!\n" );
    //   // break;
    // }
    inputImage = imread("./image.jpg");


    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f> > markerCorners, rejectedCandidates;
    cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    printf("Dictionary set up!\n");


    cv::aruco::detectMarkers(inputImage, dictionary, markerCorners, markerIds);

    // if at least one marker detected
    if (markerIds.size() > 0)
    {

      inputImage.copyTo(frame);
      cv::aruco::drawDetectedMarkers(frame, markerCorners, markerIds);
      imwrite("processedImg.jpg", frame);

      // std::vector<cv::Vec3d> rvecs, tvecs;
      // cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05, cameraMatrix, distCoeffs, rvecs, tvecs);

      // draw axis for each marker
      // for(int i=0; i<markerIds.size(); i++){
      //   cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
      // }
    }

    // char key = (char) cv::waitKey(25);
    // if(key == 27)
    // {
    //
    //   break;
    // }
  // }
  // printf("%d\n", inputVideo.isOpened());
  // inputVideo.release();



  // // cv::Mat cameraMatrix, distCoeffs;
  // // readCameraParameters(cameraMatrix, distCoeffs);

  // while (inputVideo.grab())
  // {
  //     printf("Capturing\n");
  //     inputVideo.retrieve(image);
  //
  //     std::vector<int> markerIds;
  //     std::vector<std::vector<cv::Point2f> > markerCorners;

  //     // char key = (char) cv::waitKey(10);
  //     // if (key == 27)
  //     // {
  //     //
  //     //   break;
  //     // }
  //     imwrite("./image.jpg", image);
  //     cout << image << endl;
  //
  //
  // }
  return 0;
}
