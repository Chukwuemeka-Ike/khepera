#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>

// Important namespaces for less typing
using namespace std;
using namespace cv;

// My IP address
#define MY_IP "192.168.43.233"

// Coordinates of all 8 tags in the map
#define TAG1_X 0.3
#define TAG1_Y 0
#define TAG2_X 0.7
#define TAG2_Y 0
#define TAG3_X 1
#define TAG3_Y 0.3
#define TAG4_X 1
#define TAG4_Y 0.7
#define TAG5_X 0.7
#define TAG5_Y 1
#define TAG6_X 0.3
#define TAG6_Y 1
#define TAG7_X 0
#define TAG7_Y 0.7
#define TAG8_X 0
#define TAG8_Y 0.3


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
    float otherEstimate;
    int myEngageFlag = 0;
    int myAckFlag = 0;
    int myCloseFlag = 0;
    float currentEstimate;
    float myPosition[2] = {0,0};

    // // Open and read my previous estimate of the average, then close the file
    // cout << "Reading my previous estimate file\n";
    // string myEstimate = "myEstimate.yaml";
    // FileStorage myEstim(myEstimate, FileStorage::READ);
    //
    // if(myEstim.isOpened())
    // {
    //   myEstim["Estimate"] >> currentEstimate;
    //   myEstim["Paired With"] >> pairedID;
    //   myEstim["Engage Flag"] >> myEngageFlag;
    //   myEstim["Acknowledge Flag"] >> myAckFlag;
    //   myEstim["Close Flag"] >> myCloseFlag;
    //   cout << "Estimate: " << currentEstimate << endl;
    //   cout << "Previously Paired With: " << pairedID << "\n\n";
    // }
    //
    // myEstim.release();
    //
    // // Open and read the received estimate file
    // string recEstimate = "receivedEstimate.yaml";
    // FileStorage recEstim(recEstimate, FileStorage::READ);
    //
    // // Check if the file was available for use
    // if (!recEstim.isOpened())
    // {
    //   cout << "No request to engage received!\n";
    // }
    // else
    // {
    //   // Grab the Sender ID along with other pertinent information to
    //   // ensure we are speaking to the right Khepera
    //   recEstim["Sender ID"] >> senderID;
    //   recEstim["Engage Flag"] >> engageFlag;
    //   recEstim["Acknowledge Flag"] >> ackFlag;
    //   recEstim["Close Flag"] >> closeFlag;
    //   if(ackFlag && (senderID == pairedID))
    //   {
    //     recEstim["Estimate"] >> otherEstimate;
    //   }
    //
    //   cout << "Sender ID: " << senderID << endl;
    //   cout << "Engage Flag: " << engageFlag << endl;
    //   cout << "Acknowledge Flag: " << ackFlag << endl;
    //   cout << "Close Flag: " << closeFlag << "\n\n";
    // }
    // recEstim.release();



    std::vector<int> markerIds;
    std::vector<std::vector<cv::Point2f> > markerCorners;
    // cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::
    //                                        DetectorParameters::create();
    while (inputVideo.grab())
    {
        inputVideo.retrieve(image);
        image.copyTo(imageCopy);

        cv::aruco::detectMarkers(image, dictionary, markerCorners, markerIds);

        // if at least one marker detected
        if (markerIds.size() > 0)
        {
            cv::aruco::drawDetectedMarkers(imageCopy, markerCorners, markerIds);
            std::vector<cv::Vec3d> rvecs, tvecs;
            cv::aruco::estimatePoseSingleMarkers(markerCorners, 0.05,
                                        cameraMatrix, distCoeffs, rvecs, tvecs);
            // cout << markerIds.size() << endl;
            // draw axis for each marker
            // for(int i=0; i<markerIds.size(); i++)
            // {
            //   cv::aruco::drawAxis(imageCopy, cameraMatrix, distCoeffs,
            //                                           rvecs[i], tvecs[i], 0.1);
            // }
            cout << markerIds[0] << endl;
            cout << "tvecs:\n" << tvecs[0] << "\n";
            cout << "rvecs:\n" << rvecs[0] << "\n\n\n";

            // Calculate where you are based on which tag you're looking at
            switch (markerIds[0])
            {
              //
              case 6:
                myPosition[0] = TAG6_X - tvecs[0][0];
                myPosition[1] = TAG6_Y - tvecs[0][2];

            }
            // cout << "X Position: " << myPosition[0] << endl;
            // cout << "Y Position: " << myPosition[1] << endl;

            // float actualPosition[3] = {};
            cv::Rodrigues(rvecs[0], rotMatrix);
            double transCameraToTag[4][4] = {{},{},{},{0,0,0,1}};

            double transL[3] = {};
            for(int i = 0; i < rotMatrix.rows; i++)
            {
              const double* rotMatI = rotMatrix.ptr<double>(i);
              for(int j = 0; j < rotMatrix.cols; j++)
              {
                transCameraToTag[j][i] = rotMatI[j];
                // transL[i] += rotMatI[j]*tvecs[0][j];
              }
              // cout << transL[i] << "\n\n";
            }
            cout << "Rotation Matrix: \n" << rotMatrix << endl;
            cout << "Transposed: \n";
            for(int i = 0; i < 4; i++)
            {
              for (int j = 0; j < 4; j++)
              {
                cout << transCameraToTag[i][j] << "|";
              }
              cout << "\n";
            }
            cout << "\n\n";
            // actualPosition[0] = tvecs[0][0]*rvecs[0][0];
            // cout << actualPosition[0];

        }
        // Show the estimated poses and detected markers
        cv::imshow("Output", imageCopy);
        char key = (char) cv::waitKey(10);
        if (key == 27)
            break;
    }
    // // Reopen myEstimate.yaml to write the new information available
    // myEstim.open(myEstimate, FileStorage::WRITE);
    //
    // if(!myEstim.isOpened())
    // {
    //   cout << "Couldn't open my estimate file" << endl;
    // }
    // else
    // {
    //   myEstim << "Sender ID" << MY_IP;
    //   // Check if this is a hail and if I am available
    //   if(engageFlag && !myAckFlag)
    //   {
    //     myEstim << "Paired With" << senderID;
    //     myEstim << "Engage Flag" << 0;
    //     myEstim << "Acknowledge Flag" << 1;
    //     myEstim << "Close Flag" << 0;
    //   }
    //   // else if(ackFlag && (senderID == pairedID))
    //   // {
    //   //   myEstim << "Paired With" << senderID;
    //   //   myEstim << "Engage Flag" << 0;
    //   //   myEstim << "Acknowledge Flag" << 1;
    //   //   myEstim << "Close Flag" << 0;
    //   // }
    //   // Check if they have received an acknowledge
    //   else if(ackFlag && myAckFlag && (senderID == pairedID))
    //   {
    //     myEstim << "Paired With" << senderID;
    //     myEstim << "Engage Flag" << 0;
    //     myEstim << "Acknowledge Flag" << 0;
    //     myEstim << "Close Flag" << 1;
    //     currentEstimate = (currentEstimate + otherEstimate)/2;
    //   }
    //   // If the received closed flag is on, send my estimate, and
    //   // set my close flag on
    //   else if(closeFlag)
    //   {
    //     myEstim << "Paired With" << senderID;
    //     myEstim << "Engage Flag" << 0;
    //     myEstim << "Acknowledge Flag" << 0;
    //     myEstim << "Close Flag" << 1;
    //   }
    //   // If I received no estimate and am not in the middle of comms
    //   else if(!myEngageFlag && !myAckFlag && !myCloseFlag)
    //   {
    //     myEstim << "Paired With" << senderID;
    //     myEstim << "Engage Flag" << 0;
    //     myEstim << "Acknowledge Flag" << 0;
    //     myEstim << "Close Flag" << 0;
    //   }
    //   myEstim << "Estimate" << currentEstimate;
    //   cout << currentEstimate << endl;
    //
    // }
    //
    // // Release the estimate file and copy it to the new one so we can send it
    // // to the
    // myEstim.release();
    // cout << "Copying to a new file\n\n";
    // system("cp myEstimate.yaml my.yaml");
    // system("echo 'Success!'");

    // string copyCommand = "scp myEstimate.yaml root@" + pairedID + ":/home/root";
    // char* cpy = new char [copyCommand.length()+1];
    // strcpy(cpy, copyCommand.c_str());
    // system("echo 'Attempting Copy Now'");
    // system(cpy);

  // }

  return 0;
}
