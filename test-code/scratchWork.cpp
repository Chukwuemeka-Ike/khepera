#include <stdio.h>
#include <stdlib.h>
// #include <opencv2/opencv.hpp>
// #include <opencv2/aruco.hpp>
#include <iostream>
#include <time.h>

// Important namespaces for less typing
using namespace std;
// using namespace cv;

int main()
{

  srand (time(NULL));
  char targetIP[20];
  sprintf(targetIP, "192.168.43.%d", (rand()  % 4) + 233 ) ;
  cout << targetIP << endl;

  // Generate and store a random IP address
  // char s[20];
  // sprintf(s, "%d.%d.%d.%d", rand() & 0xFF, rand() & 0xFF,
  //                             rand() & 0xFF, rand() & 0xFF ) ;
  // cout << s << endl;
  return 0;
}
