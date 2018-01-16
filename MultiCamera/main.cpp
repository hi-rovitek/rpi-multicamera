#include <iostream>
#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include <unistd.h>

using namespace std;
using namespace cv;

int main()
{

    // Intialize the wiringPi Library
    if (wiringPiSetup () == -1)
      return 1 ;

	
    pinMode(7,OUTPUT);
    pinMode(0,OUTPUT);
    pinMode(1,OUTPUT);   


	//turn on Camera A
    digitalWrite (7, 0) ; 
    digitalWrite (0, 0) ;
    digitalWrite (1, 1) ; 

	//turn on Camera B
	//digitalWrite (7,1) ;    
	//digitalWrite (0, 0) ; 
	//digitalWrite (1, 1) ; 

    namedWindow("MyVideo",CV_WINDOW_NORMAL); //create a window called "MyVideo"
    VideoCapture cap(0); // open the video camera no. 0
     


   cap.set(CV_CAP_PROP_FRAME_WIDTH,640);
   cap.set(CV_CAP_PROP_FRAME_HEIGHT,480);

    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }



    while (1)
    {
        Mat frame;
        double dvalue1 , dvalue2;
        usleep(30*1000);
        
		cap.grab();

		cap.retrieve(frame);
        dvalue1 = cap.get(CV_CAP_PROP_SPEED);
        dvalue2 = cap.get(CV_CAP_PROP_EXPOSURE);

        cout << dvalue1<< "  :  " <<dvalue2 <<endl;

        imshow("MyVideo", frame); //show the frame in "MyVideo" window

        if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            Camera.release();
            break;
        }

    }


    return 0;
}

