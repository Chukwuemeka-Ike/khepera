#include "../include/khepera.h"
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

// The two namespaces used in this code
using namespace std;
using namespace cv;

// My IP address
#define MY_IP "192.168.43.233"

int main(int argc, char *argv[])
{
  int rc;
  // Set up variables to take in the information from the received estimate
  // file
  std::string senderID = " ";
  std::string pairedID = " ";
  int engageFlag = 0;
  int ackFlag = 0;
  int closeFlag = 0;
  std::vector<float> otherEstimate(2);
  int myEngageFlag = 0;
  int myAckFlag = 0;
  int myCloseFlag = 0;
  int i;
  std::vector<double> currentEstimate(2);
  float currentPos[2] = {0,0};
  float prevPos[2] = {0,0};

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

  // Open and read my previous estimate of the average, then close the file
  printf("\nReading Previous File\n");
  printf("--------------------------------------------------------------\n");
  string myEstimate = "myEstimate.yaml";
  FileStorage myEstim(myEstimate, FileStorage::READ);

  // Check if the file opened successfully
  if(myEstim.isOpened())
  {
    // Create a node to read the Estimate sequence
    FileNode mNode = myEstim["Estimate"];
    if (mNode.type() != FileNode::SEQ)
    {
        cerr << "Estimate is not a sequence! FAIL" << endl;
        return 1;
    }
    // Go through the node
    FileNodeIterator im = mNode.begin(), im_end = mNode.end();
    i = 0;
    for (; im != im_end; ++im, i++)
    {
      currentEstimate[i] = (float)(*im);
    }
    // Redefine the node to read the 'My Position' sequence
    mNode = myEstim["My Position"];
    if (mNode.type() != FileNode::SEQ)
    {
        cerr << "'My Position' is not a sequence! FAIL" << endl;
        return 1;
    }
    // Go through the node
    im = mNode.begin(), im_end = mNode.end();
    i = 0;
    for (; im != im_end; ++im, i++)
    {
      prevPos[i] = (float)(*im);
    }
    myEstim["Paired With"] >> pairedID;
    myEstim["Engage Flag"] >> myEngageFlag;
    myEstim["Acknowledge Flag"] >> myAckFlag;
    myEstim["Close Flag"] >> myCloseFlag;
    cout << "Previously Paired With: " << pairedID << "\n";
    printf("Previous Engage Flag: %d\nAcknowledge: %d\nClose Flag: %d\n",
                                      myEngageFlag, myAckFlag, myCloseFlag );
    printf("Previous Estimate (x,z): (%f, %f)\n", currentEstimate[0],currentEstimate[1]);
    printf("Previous Position (x,z): (%f, %f)\n\n", prevPos[0], prevPos[1]);
    printf("--------------------------------------------------------------\n");
  }

  myEstim.release();

  // Open and read the received estimate file
  printf("Reading Received File\n");
  string recEstimate = "receivedEstimate.yaml";
  FileStorage recEstim(recEstimate, FileStorage::READ);

  // Check if the file was available for use
  if (!recEstim.isOpened())
  {
    cout << "No request to engage received!\n";
  }
  else
  {
    // Grab the Sender ID along with other pertinent information to
    // ensure we are speaking to the right Khepera
    recEstim["Sender ID"] >> senderID;
    recEstim["Engage Flag"] >> engageFlag;
    recEstim["Acknowledge Flag"] >> ackFlag;
    recEstim["Close Flag"] >> closeFlag;
    if((ackFlag) && (senderID == pairedID))
    {
      FileNode tNode = recEstim["Estimate"];
      if (tNode.type() != FileNode::SEQ)
      {
          cerr << "Estimate is not a sequence! FAIL" << endl;
          return 1;
      }
      // Go through the node
      FileNodeIterator it = tNode.begin(), it_end = tNode.end();
      i = 0;
      for (; it != it_end; ++it, i++)
      {
          otherEstimate[i] = (float)(*it) ;
      }
    }

    cout << "Sender ID: " << senderID << endl;
    printf("Engage Flag: %d\nAcknowledge: %d\nClose Flag: %d\n",
                                      engageFlag, ackFlag, closeFlag );
    printf("Other Estimate (x,z): (%f, %f)\n", otherEstimate[0], otherEstimate[1]);
    printf("--------------------------------------------------------------\n");
  }
  recEstim.release();

  // Declare some variables to allow for ArUco detection
  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f> > markerCorners, rejectedCandidates;
  cv::Ptr<cv::aruco::DetectorParameters> parameters =
                                      cv::aruco::DetectorParameters::create();
  cv::Ptr<cv::aruco::Dictionary> dictionary =
                    cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

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
      cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.053,
                                  cameraMatrix, distCoeffs, rvecs, tvecs);

      // Draw the axes for each marker visible in the image
      for(int i=0; i<markerIds.size(); i++)
      {
        cv::aruco::drawAxis(frame, cameraMatrix, distCoeffs, rvecs[i], tvecs[i], 0.1);
        cout << "Marker " << markerIds[i] << ": " << endl;
        cout << "Rotation: " << rvecs[i] << endl;
        cout << "Translation: " << tvecs[i] << endl << endl;
      }
      imwrite("processedImg.jpg", frame);


      // Declare the Identity Transformation Matrix
      double transTagToBase[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};

      // Modify the transformation matrix based on which Tag we are looking at
      // All uncommented cases are correct based on the current
      // testing environment
      switch (markerIds[0])
      {
        case 1:
          transTagToBase[0][0] = -1;
          transTagToBase[0][3] = 0.3;
          break;
        case 2:
          transTagToBase[0][0] = -1;
          transTagToBase[0][3] = 0.6;
          break;
        case 3:
          transTagToBase[0][0] = 0;
          transTagToBase[0][2] = -1;
          transTagToBase[0][3] = 1;
          transTagToBase[2][0] = -1;
          transTagToBase[2][2] = 0;
          transTagToBase[2][3] = 0.3;
          break;
        case 4:
          transTagToBase[0][0] = 0;
          transTagToBase[0][2] = -1;
          transTagToBase[0][3] = 1;
          transTagToBase[2][0] = -1;
          transTagToBase[2][2] = 0;
          transTagToBase[2][3] = 0.6;
          break;
        case 5:
          transTagToBase[0][3] = 0.6;
          transTagToBase[2][2] = -1;
          transTagToBase[2][3] = 1;
          break;
        case 6:
          transTagToBase[0][3] = 0.3;
          transTagToBase[2][2] = -1;
          transTagToBase[2][3] = 1;
          break;
        case 7:
          transTagToBase[0][0] = 0;
          transTagToBase[0][2] = 1;
          transTagToBase[2][0] = 1;
          transTagToBase[2][2] = 0;
          transTagToBase[2][3] = 0.6;
          break;
        case 8:
          transTagToBase[0][0] = 0;
          transTagToBase[0][2] = 1;
          transTagToBase[2][0] = 1;
          transTagToBase[2][2] = 0;
          transTagToBase[2][3] = 0.3;
          break;
        default:
          break;
      }

      // Convert the rotation vector into a rotation matrix
      cv::Rodrigues(rvecs[0], rotMatrix);

      // Set up the the base and tag transformation matrices for the camera
      double transCameraToTag[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,1}};
      double transCameraToBase[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};

      // Transpose the camera to tag rotation matrix and
      // add it to the transformation matrix
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

      // // Output the tag to camera rotation matrix
      // cout << "Rotation Matrix: \n" << rotMatrix << endl;
      //
      // // Camera to Tag Transformation Matrix
      // cout << "Camera To Tag: \n";
      // for(int i = 0; i < 4; i++)
      // {
      //   for (int j = 0; j < 4; j++)
      //   {
      //     cout << transCameraToTag[i][j] << "|";
      //   }
      //   cout << "\n";
      // }
      // cout << "\n\n";
      //
      // // Transformation Matrix from the Tag to the Base Frame
      // cout << "Tag To Base: \n";
      // for(int i = 0; i < 4; i++)
      // {
      //   for (int j = 0; j < 4; j++)
      //   {
      //     cout << transTagToBase[i][j] << "|";
      //   }
      //   cout << "\n";
      // }
      // cout << "\n\n";

      // Multiply the Tag to Base and Camera to Tag Transformation
      // Matrices to yield the Final Transformation Matrix from
      // the Camera to the Base Frame
      for(int i = 0; i < 4; i++)
      {
        for(int j = 0; j < 4; j++)
        {
          for (int k = 0; k < 4; k++)
          {
            transCameraToBase[i][j] += (double) transTagToBase[i][k]
                                              * transCameraToTag[k][j];
          }
        }
      }

      // // Output the Final Transformation Matrix
      // cout << "Camera To Base: \n";
      // for(int i = 0; i < 4; i++)
      // {
      //   for (int j = 0; j < 4; j++)
      //   {
      //     cout << transCameraToBase[i][j] << "|";
      //   }
      //   cout << "\n";
      // }
      // cout << "\n\n";

      currentPos[0] = transCameraToBase[0][3] - currentPos[0];
      currentPos[1] = transCameraToBase[2][3] - currentPos[1];

      printf("X-Position: %f\n", currentPos[0]);
      printf("Z-Position: %f\n\n", currentPos[1]);

      // Reopen myEstimate.yaml to write the new information available
      myEstim.open(myEstimate, FileStorage::WRITE);
      if(!myEstim.isOpened())
      {
        cout << "Couldn't open my estimate file" << endl;
      }
      else
      {
        printf("--------------------------------------------------------------\n");
        myEstim << "Sender ID" << MY_IP;

        // Check if this is a hail and if I am available, initiate handshake
        if(engageFlag && !myAckFlag)
        {
          printf("New Hail Received\n");

          myEstim << "Paired With" << senderID;
          myEstim << "Engage Flag" << 0;
          myEstim << "Acknowledge Flag" << 1;
          myEstim << "Close Flag" << 0;

          myEstim << "Estimate" << "[" << currentEstimate[0] << currentEstimate[1] << "]";
          myEstim << "My Position" << "[" << currentPos[0] << currentPos[1] << "]";
          myEstim.release();
          // Copy the estimate so we can send it to the paired Khepera
          system("cp myEstimate.yaml my.yaml");
        }
        else if (engageFlag && myAckFlag)
        {
          printf("Currently Busy\n");

          myEstim << "Paired With" << pairedID;
          myEstim << "Engage Flag" << myEngageFlag;
          myEstim << "Acknowledge Flag" << myAckFlag;
          myEstim << "Close Flag" << myCloseFlag;
          myEstim << "Estimate" << "[" << currentEstimate[0] << currentEstimate[1] << "]";
          myEstim << "My Position" << "[" << currentPos[0] << currentPos[1] << "]";
          myEstim.release();
          // Copy the estimate so we can send it to the paired Khepera
          system("cp myEstimate.yaml my.yaml");
        }
        // If I receive an acknowledge from a previously paired Khepera,
        // send my estimate and then update
        else if(ackFlag && (myAckFlag) && (!myCloseFlag) && (senderID == pairedID))
        {
          printf("Sending Previous Estimate\n\n");

          myEstim << "Paired With" << senderID;
          myEstim << "Engage Flag" << 0;
          myEstim << "Acknowledge Flag" << 1;
          myEstim << "Close Flag" << 1;
          myEstim << "Estimate" << "[" << currentEstimate[0] << currentEstimate[1] << "]";
          myEstim << "My Position" << "[" << prevPos[0] << prevPos[1] << "]";
          myEstim.release();
          // Copy the estimate so we can send it to the paired Khepera
          system("cp myEstimate.yaml my.yaml");

          printf("Updating Estimate\n\n");

          myEstim.open(myEstimate, FileStorage::WRITE);
          myEstim << "Sender ID" << MY_IP;
          myEstim << "Paired With" << senderID;
          myEstim << "Engage Flag" << 0;
          myEstim << "Acknowledge Flag" << 1;
          myEstim << "Close Flag" << 1;
          currentEstimate[0] = ((currentEstimate[0] + otherEstimate[0])/2)
                                                  + (currentPos[0] - prevPos[0]);
          currentEstimate[1] = ((currentEstimate[1] + otherEstimate[1])/2)
                                                  + (currentPos[1] - prevPos[1]);
          myEstim << "Estimate" << "[" << currentEstimate[0] << currentEstimate[1] << "]";
          myEstim << "My Position" << "[" << currentPos[0] << currentPos[1] << "]";
          myEstim.release();
        }
        // If the received closed flag is on, send my estimate, and
        // set my close flag on
        else if(closeFlag && (senderID == pairedID))
        {
          printf("Shutting Down Comms\n");

          myEstim << "Paired With" << senderID;
          myEstim << "Engage Flag" << 0;
          myEstim << "Acknowledge Flag" << 0;
          myEstim << "Close Flag" << 1;
          myEstim << "Estimate" << "[" << currentEstimate[0] << currentEstimate[1] << "]";
          myEstim << "My Position" << "[" << currentPos[0] << currentPos[1] << "]";
          myEstim.release();
          // Copy the estimate so we can send it to the paired Khepera
          system("cp myEstimate.yaml my.yaml");
        }
        // If I received no hail and am not in the middle of comms,
        // try hailing a random khepera within the set range of IPs
        // Update my estimate with my new position
        else //if(!engageFlag && !ackFlag && !closeFlag)
        {
          srand (time(NULL));
          char targetIP[20];
          sprintf(targetIP, "192.168.43.%d", (rand()  % 4) + 233 );

          printf("Initiating Comms with %s\n", targetIP);

          pairedID = targetIP;
          myEstim << "Paired With" << targetIP;
          myEstim << "Engage Flag" << 1;
          myEstim << "Acknowledge Flag" << 1;
          myEstim << "Close Flag" << 0;
          currentEstimate[0] = currentEstimate[0] + (currentPos[0] - prevPos[0]);
          currentEstimate[1] = currentEstimate[1] + (currentPos[1] - prevPos[1]);
          myEstim << "Estimate" << "[" << currentEstimate[0] << currentEstimate[1] << "]";
          myEstim << "My Position" << "[" << currentPos[0] << currentPos[1] << "]";
          myEstim.release();
          // Copy the estimate so we can send it to the paired Khepera
          system("cp myEstimate.yaml my.yaml");
        }
        printf("--------------------------------------------------------------\n");
      }
      // string sendCommand = "scp my.yaml root@" + pairedID
      //                      + ":/home/root/pose-estimate/receivedEstimate.yaml";
      string sendCommand = "scp my.yaml chukwuemeka@192.168.43.234:~/Downloads/receivedEstimate.yaml";
      char* cpy = new char [sendCommand.length()+1];
      strcpy(cpy, sendCommand.c_str());
      printf("Sending Estimate to Paired Khepera\n");
      system(cpy);
      system("h3ll1ng4r");
      printf("\nSuccess!\n");

  }
  else
  {
    printf("No Markers Detected!\n");
  }



  return 0;
}
