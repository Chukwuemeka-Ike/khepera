#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/aruco.hpp>
#include <iostream>

// Important namespaces for less typing
using namespace std;
using namespace cv;

// Write my IP address, so I know it
#define MY_ID 2
#define MY_IP "192.168.43.233"

int main()
{
  // Create CV Matrices for the images, camera matrix, and distortion coefficients
  cv::Mat image, imageCopy, cameraMatrix, distCoeffs;

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

    // Open and read my previous estimate of the average, then close the file
    cout << "Reading my previous estimate file\n";
    string myEstimate = "myEstimate.yaml";
    FileStorage myEstim(myEstimate, FileStorage::READ);

    if(myEstim.isOpened())
    {
      myEstim["Estimate"] >> currentEstimate;
      myEstim["Paired With"] >> pairedID;
      myEstim["Engage Flag"] >> myEngageFlag;
      myEstim["Acknowledge Flag"] >> myAckFlag;
      myEstim["Close Flag"] >> myCloseFlag;
      cout << "Estimate: " << currentEstimate << endl;
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
        recEstim["Estimate"] >> otherEstimate;
      }

      cout << "Sender ID: " << senderID << endl;
      cout << "Engage Flag: " << engageFlag << endl;
      cout << "Acknowledge Flag: " << ackFlag << endl;
      cout << "Close Flag: " << closeFlag << "\n\n";
    }
    recEstim.release();


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
        myEstim << "Acknowledge Flag" << 0;
        myEstim << "Close Flag" << 1;
        currentEstimate = (currentEstimate + otherEstimate)/2;
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
      myEstim << "Estimate" << currentEstimate;
      cout << currentEstimate << endl;

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

}
