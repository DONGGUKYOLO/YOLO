#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
 
#include <wiringPi.h>
#include <softPwm.h>

using namespace std;
using namespace cv;
 
#define SERVO 0
#define SW 22

cv::String face_name = "/home/pi/opencv/opencv-3.2.0/data/haarcascades/haarcascade_frontalface_default.xml";

int main ( int argc,char **argv )
{
	if(wiringPiSetup() == -1) return 1;
	
	pinMode(SW, INPUT);
	pinMode(SERVO, OUTPUT);
	softPwmCreate(SERVO, 0, 200);
	digitalWrite(SERVO, LOW);

	raspicam::RaspiCam_Cv Camera;
	Mat image;

	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3);
	Camera.set( CV_CAP_PROP_FRAME_WIDTH, 320 );
	Camera.set( CV_CAP_PROP_FRAME_HEIGHT, 240 );
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}


	CascadeClassifier face_cascade;
	
	Mat grayframe;
	int servo_count = 15;
	softPwmWrite(SERVO, servo_count);


	while(digitalRead(SW) == 0){
	        Camera.grab();
        	Camera.retrieve (image);
		face_cascade.load(face_name);
		cvtColor(image, grayframe, COLOR_BGR2GRAY);
		equalizeHist(grayframe, grayframe);

		vector<Rect> faces;

		face_cascade.detectMultiScale(grayframe, faces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT|CV_HAAR_SCALE_IMAGE, Size(30,30));

		for(int i=0; i<faces.size(); i++){
			Point lb(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
			Point tr(faces[i].x, faces[i].y);

			rectangle(image, lb, tr, Scalar(0,255,0), 3,4,0);
		}

		if(faces.size() != 0 && (faces[0].x + (faces[0].x + faces[0].width))/2 < 120){
			if(servo_count <= 18){
				cout << "spin1" << endl;
				servo_count++;
				cout << "servo_count = " << servo_count << endl;
				softPwmWrite(SERVO, servo_count);
				delay(200);
			}
			else{
				digitalWrite(SERVO, LOW);
				servo_count = 18;
			}

		}
		else if(faces.size() != 0 && (faces[0].x + (faces[0].x + faces[0].width)/2) > 200){
			if(servo_count >= 12){
				cout << "spin2" << endl;
				servo_count--;
				cout << "servo_count = " << servo_count<<endl;
				softPwmWrite(SERVO, servo_count);
				delay(200);
			}
			else{
				digitalWrite(SERVO, LOW);
				servo_count = 12;
			}
		}
		else{
			digitalWrite(SERVO, LOW);
			delay(200);
		}

 		imshow( "picamera test", image );
		if ( waitKey(20) == 27 ) break; //ESC키 누르면 종료
	}
 

	Camera.release();
}
