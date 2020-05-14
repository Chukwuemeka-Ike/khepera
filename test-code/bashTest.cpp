#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>

// The two namespaces used in this code
using namespace std;
using namespace cv;

// My IP address
#define MY_IP "192.168.43.233"

int main()
{
  // Start the video capture
  cv::VideoCapture inputVideo(0+cv::CAP_ANY);
  inputVideo.open(0);

  // Create CV Matrices for the images, camera matrix, and distortion coefficients
  cv::Mat image, imageCopy, cameraMatrix, distCoeffs, rotMatrix;

  // Read the camera data file into the cameraMatrix and distCoeffs variables
  FileStorage fs("camera.yaml", FileStorage::READ);
  fs["camera_matrix"] >> cameraMatrix;
  fs["distortion_coefficients"] >> distCoeffs;
  fs.release();

  // while(1)
  // {
    // Create the marker dictionary we'll use in our detection
    cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::
                          getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    std::vector<cv::Vec3d> receivedRvecs, receivedTvecs;

    // Set up variables to take in the information from the received estimate
    // file
    std::string senderID = " ";
    std::string pairedID = " ";
    int engageFlag = 0;
    int ackFlag = 0;
    int closeFlag = 0;
    std::vector<float> otherEstimate;
    int myEngageFlag = 0;
    int myAckFlag = 0;
    int myCloseFlag = 0;
    std::vector<float> currentEstimate;
    float myPosition[2] = {0,0};

    // Open and read my previous estimate of the average, then close the file
    cout << "Reading my previous estimate file\n";
    string myEstimate = "myEstimate.yaml";
    FileStorage myEstim(myEstimate, FileStorage::READ);

    if(myEstim.isOpened())
    {
      myEstim["Estimate"] >> currentEstimate;
      FileNode mNode = myEstim["Estimate"];
      if (mNode.type() != FileNode::SEQ)
      {
          cerr << "Tvecs is not a sequence! FAIL" << endl;
          return 1;
      }
      // Go through the node
      FileNodeIterator im = mNode.begin(), im_end = mNode.end();
      for (; im != im_end; ++im)
      {
          (*im) >> currentEstimate;
      }
      myEstim["Paired With"] >> pairedID;
      myEstim["Engage Flag"] >> myEngageFlag;
      myEstim["Acknowledge Flag"] >> myAckFlag;
      myEstim["Close Flag"] >> myCloseFlag;
      // cout << "Estimate: " << currentEstimate << endl;
      cout << "Previously Paired With: " << pairedID << "\n\n";
    }

    myEstim.release();

    // Open and read the received estimate file
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
      if(ackFlag && (senderID == pairedID))
      {
        FileNode tNode = recEstim["Estimate"];
        if (tNode.type() != FileNode::SEQ)
        {
            cerr << "Tvecs is not a sequence! FAIL" << endl;
            return 1;
        }
        // Go through the node
        FileNodeIterator it = tNode.begin(), it_end = tNode.end();
        for (; it != it_end; ++it)
        {
            (*it) >> otherEstimate;
        }
      }


      cout << "Sender ID: " << senderID << endl;
      cout << "Engage Flag: " << engageFlag << endl;
      cout << "Acknowledge Flag: " << ackFlag << endl;
      cout << "Close Flag: " << closeFlag << "\n\n";
      cout << "Other Estimate: " << otherEstimate[0] << endl;
      cout << "\t" <<  otherEstimate[1] << endl;
    }
    // recEstim.release();



    // std::vector<int> markerIds;
    // std::vector<std::vector<cv::Point2f> > markerCorners;
    // // cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::
    // //                                        DetectorParameters::create();
    // while (inputVideo.grab())
    // {
    //     inputVideo.retrieve(image);
    //     image.copyTo(imageCopy);
    //
    //     cv::aruco::detectMarkers(image, dictionary, markerCorners, markerIds);
    //
    //     // if at least one marker detected
    //     if (markerIds.size() > 0)
    //     {
    //         cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);
    //         std::vector<cv::Vec3d> rvecs, tvecs;
    //         cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05,
    //                                     cameraMatrix, distCoeffs, rvecs, tvecs);
    //
    //         // Draw the axes for each marker
    //         for(int i=0; i<markerIds.size(); i++)
    //         {
    //           cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs,
    //                                                   rvecs[i], tvecs[i], 0.1);
    //         }
    //         cout << "tvecs:\n" << tvecs[0] << "\n";
    //         cout << "rvecs:\n" << rvecs[0] << "\n\n\n";
    //
    //         // Declare the Identity Transformation Matrix
    //         double transTagToBase[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
    //
    //         // Modify the transformation matrix based on which Tag we are looking at
    //         // All uncommented cases are correct based on the current
    //         // testing environment
    //         switch (markerIds[0])
    //         {
    //           case 1:
    //             transTagToBase[0][0] = -1;
    //             transTagToBase[0][3] = 0.3;
    //             break;
    //           case 2:
    //             transTagToBase[0][0] = -1;
    //             transTagToBase[0][3] = 0.6;
    //             break;
    //           case 3:
    //             transTagToBase[0][0] = 0;
    //             transTagToBase[0][2] = -1;
    //             transTagToBase[0][3] = 1;
    //             transTagToBase[2][0] = -1;
    //             transTagToBase[2][2] = 0;
    //             transTagToBase[2][3] = 0.3;
    //             break;
    //           case 4:
    //             transTagToBase[0][0] = 0;
    //             transTagToBase[0][2] = -1;
    //             transTagToBase[0][3] = 1;
    //             transTagToBase[2][0] = -1;
    //             transTagToBase[2][2] = 0;
    //             transTagToBase[2][3] = 0.6;
    //             break;
    //           case 5:
    //             transTagToBase[0][3] = 0.6;
    //             transTagToBase[2][2] = -1;
    //             transTagToBase[2][3] = 1;
    //             break;
    //           case 6:
    //             transTagToBase[0][3] = 0.3;
    //             transTagToBase[2][2] = -1;
    //             transTagToBase[2][3] = 1;
    //             break;
    //           case 7:
    //             transTagToBase[0][0] = 0;
    //             transTagToBase[0][2] = 1;
    //             transTagToBase[2][0] = 1;
    //             transTagToBase[2][2] = 0;
    //             transTagToBase[2][3] = 0.6;
    //             break;
    //           case 8:
    //             transTagToBase[0][0] = 0;
    //             transTagToBase[0][2] = 1;
    //             transTagToBase[2][0] = 1;
    //             transTagToBase[2][2] = 0;
    //             transTagToBase[2][3] = 0.3;
    //             break;
    //           default:
    //             break;
    //         }
    //
    //         // Convert the rotation vector into a rotation matrix
    //         cv::Rodrigues(rvecs[0], rotMatrix);
    //
    //         // Set up the the base and tag transformation matrices for the camera
    //         double transCameraToTag[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,1}};
    //         double transCameraToBase[4][4] = {{0,0,0,0}, {0,0,0,0}, {0,0,0,0}, {0,0,0,0}};
    //
    //         // Transpose the camera to tag rotation matrix and add it to the transformation matrix
    //         for(int i = 0; i < rotMatrix.rows; i++)
    //         {
    //           const double* rotMatI = rotMatrix.ptr<double>(i);
    //           for(int j = 0; j < rotMatrix.cols; j++)
    //           {
    //             transCameraToTag[j][i] = rotMatI[j];
    //           }
    //         }
    //         // Compute the transformed translation from the camera to the tag frame
    //         for (int i = 0; i < 3; i++)
    //         {
    //           for (int j = 0; j < 3; j++)
    //           {
    //               transCameraToTag[i][3] += transCameraToTag[i][j]*tvecs[0][j];
    //           }
    //           transCameraToTag[i][3] = -transCameraToTag[i][3];
    //         }
    //
    //         // Output the tag to camera rotation matrix
    //         cout << "Rotation Matrix: \n" << rotMatrix << endl;
    //
    //         // Camera to Tag Transformation Matrix
    //         cout << "Camera To Tag: \n";
    //         for(int i = 0; i < 4; i++)
    //         {
    //           for (int j = 0; j < 4; j++)
    //           {
    //             cout << transCameraToTag[i][j] << "|";
    //           }
    //           cout << "\n";
    //         }
    //         cout << "\n\n";
    //
    //         // Transformation Matrix from the Tag to the Base Frame
    //         cout << "Tag To Base: \n";
    //         for(int i = 0; i < 4; i++)
    //         {
    //           for (int j = 0; j < 4; j++)
    //           {
    //             cout << transTagToBase[i][j] << "|";
    //           }
    //           cout << "\n";
    //         }
    //         cout << "\n\n";
    //
    //         // Multiply the Tag to Base and Camera to Tag Transformation
    //         // Matrices to yield the Final Transformation Matrix from
    //         // the Camera to the Base Frame
    //         for(int i = 0; i < 4; i++)
    //         {
    //           for(int j = 0; j < 4; j++)
    //           {
    //             for (int k = 0; k < 4; k++)
    //             {
    //               transCameraToBase[i][j] += (double) transTagToBase[i][k] * transCameraToTag[k][j];
    //             }
    //           }
    //
    //         }
    //
    //         // Output the Final Transformation Matrix
    //         cout << "Camera To Base: \n";
    //         for(int i = 0; i < 4; i++)
    //         {
    //           for (int j = 0; j < 4; j++)
    //           {
    //             cout << std::setprecision(4) << transCameraToBase[i][j] << "|";
    //           }
    //           cout << "\n";
    //         }
    //         cout << "\n\n";
    //
    //         myPosition[0] = transCameraToBase[0][3];
    //         myPosition[1] = transCameraToBase[2][3];
    //
    //         printf("X-Position: %f\n", myPosition[0]);
    //         printf("Z-Position: %f\n\n", myPosition[1]);
    //     }
    //     // Show the estimated poses and detected markers
    //     cv::imshow("Output", imageCopy);
    //     char key = (char) cv::waitKey(10);
    //     if (key == 27)
    //         break;
    // }


    // Reopen myEstimate.yaml to write the new information available
    myEstim.open(myEstimate, FileStorage::WRITE);

    if(!myEstim.isOpened())
    {
      cout << "Couldn't open my estimate file" << endl;
    }
    else
    {
      myEstim << "Sender ID" << MY_IP;
      // Check if this is a hail and if I am available
      if(engageFlag && !myAckFlag)
      {
        myEstim << "Paired With" << senderID;
        myEstim << "Engage Flag" << 0;
        myEstim << "Acknowledge Flag" << 1;
        myEstim << "Close Flag" << 0;
      }
      // else if(ackFlag && (senderID == pairedID))
      // {
      //   myEstim << "Paired With" << senderID;
      //   myEstim << "Engage Flag" << 0;
      //   myEstim << "Acknowledge Flag" << 1;
      //   myEstim << "Close Flag" << 0;
      // }
      // Check if they have received an acknowledge
      else if(ackFlag && myAckFlag && (senderID == pairedID))
      {
        myEstim << "Paired With" << senderID;
        myEstim << "Engage Flag" << 0;
        myEstim << "Acknowledge Flag" << 1;
        myEstim << "Close Flag" << 1;
        currentEstimate[0] = (currentEstimate[0] + otherEstimate[0])/2;
        currentEstimate[1] = (currentEstimate[1] + otherEstimate[1])/2;
      }
      // If the received closed flag is on, send my estimate, and
      // set my close flag on
      else if(closeFlag)
      {
        myEstim << "Paired With" << senderID;
        myEstim << "Engage Flag" << 0;
        myEstim << "Acknowledge Flag" << 0;
        myEstim << "Close Flag" << 1;
      }
      // If I received no estimate and am not in the middle of comms
      else if(!myEngageFlag && !myAckFlag && !myCloseFlag)
      {
        myEstim << "Paired With" << senderID;
        myEstim << "Engage Flag" << 0;
        myEstim << "Acknowledge Flag" << 0;
        myEstim << "Close Flag" << 0;
      }
      myEstim << "Estimate" << "[" << currentEstimate[0] << currentEstimate[1] << "]";
      cout << currentEstimate[0] << endl << currentEstimate[1] << endl;

    }

    // Release the estimate file and copy it to the new one so we can send it
    // to the
    myEstim.release();
    cout << "Copying to a new file\n\n";
    system("cp myEstimate.yaml my.yaml");
    system("echo 'Success!'");

    // string copyCommand = "scp myEstimate.yaml root@" + pairedID + ":/home/root";
    // char* cpy = new char [copyCommand.length()+1];
    // strcpy(cpy, copyCommand.c_str());
    // system("echo 'Attempting Copy Now'");
    // system(cpy);

  // }

  return 0;
}
