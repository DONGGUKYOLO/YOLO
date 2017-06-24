#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#include <csignal>
#include <cstdlib>
#include <raspicam/raspicam_cv.h>
#include <opencv2/imgproc.hpp>
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

#define SERVO 0
#define MO1 27
#define SWY 21
#define SWW 8
#define SWB 1
#define SWG 22

int countB = LOW;
int countY = LOW;
int countW = 6;
int countG = LOW;

using namespace std;

cv::String face_name = "/home/pi/opencv/opencv-3.2.0/data/haarcascades/haarcascade_frontalface_default.xml";


void digitalWrite_W2(int signal)
{
        //int count = 7;
        digitalWrite(MO1, HIGH);

        delay(100);
        if (countW<10) {
                digitalWrite(MO1, LOW);

                delay(500-countW*50);
        }
}

void digitalWrite_B(int signal)
{
	int i;
	for(i=20; i>=10; i--){
		softPwmWrite(SERVO,i);
		delay(200);
		if(countW) digitalWrite_W2(HIGH);
		if(countB == 0)
			break;
	}
	for(i=10; i<=20; i++){
		softPwmWrite(SERVO,i);
		delay(200);
		if(countW) digitalWrite_W2(HIGH);
		if(countB == 0)
			break;
	}
}

//SWB spin
void myInterruptB(void) {


        if (countB == 1 && digitalRead(SWB)== LOW){
                // debounce handling
                delay(1);
                if (digitalRead(SWB) == LOW) {
                         cout<<"stopping motor...\n"<<endl;
                        countB = 0;
                }
        }
        else if(countB == 0 && digitalRead(SWB) == LOW){
                // debounce handling
                delay(1);
                if (digitalRead(SWB) == LOW) {
                        cout<<"starting motor..\n"<<endl;
                        countB = 1;
		}
	}
}
//SWW speed
void myInterruptW(void) {

        if(digitalRead(SWW) == LOW){
                delay(1); // 1 ms
                if(digitalRead(SWW) == LOW) {

                        countW++;
			countW++;

                        if (countW <10) {
                  	 cout<<"turning on, speed="<< countW<<endl;

                        }
                                else {
                                cout<<"turning on, speed="<<countW<<endl;
                                countW = 4;
                        }
                }
        }
}

//SWY onoff
void myInterruptY(void) {

		 if(countY == 1 && digitalRead(SWY) == LOW)
                	{

                        cout<<"setting low"<<endl;
                        if (digitalRead(SWY)  == LOW) {
                                cout << "starting motor..."<<endl;
			//	digitalWrite(MO1,HIGH);
				countY = 0;
                        	}
		}
                else if(countY== LOW && digitalRead(SWY) == LOW)
                	{
                        cout << "setting high" <<endl;
			//digitalWrite(MO1, LOW);
			countY = 1;
                	}

		}


//SWG cam
void myInterruptG(void) {
		
                 if(countG == 1 && digitalRead(SWG) == LOW)
                        {
			digitalWrite(MO1, 60);
                        cout<<"setting low"<<endl;
                        if (digitalRead(SWG)  == LOW) {
                                cout << "starting cam..."<<endl;
                                countG = 0;
                                }
                }
                else if(countG== LOW && digitalRead(SWG) == LOW)
                        {
                        cout << "setting high" <<endl;
                        //if (digitalRead(SWG)  == LOW) {
                                cout << "stopping cam..."<<endl;
                                digitalWrite(MO1, LOW);
				countG = 1;
                       // }

                }

}
void sig_handler(int signo)
	{
       	 cout << "i'm dying..."<<endl;
       	digitalWrite(MO1, LOW);
       	//digitalWrite(LCD, LOW);
       	exit(1);
	}

int main(){

        if(wiringPiSetup() == -1)
                return 1;

	pinMode(SWY,INPUT);
        pinMode(SWW,INPUT);
	pinMode(SWB,INPUT);
	pinMode(SWG,INPUT);
        pinMode(MO1, OUTPUT);
	pinMode(SERVO, OUTPUT);

	digitalWrite(SERVO,LOW);
        digitalWrite (MO1, LOW);

	softPwmCreate(SERVO,0,200);

 	if(wiringPiISR(SWW, INT_EDGE_FALLING, &myInterruptW) < 0){
                return 1;
        }
  	if (wiringPiISR (SWB, INT_EDGE_FALLING, &myInterruptB) < 0){

                return 1;
        }
	 if (wiringPiISR (SWY, INT_EDGE_FALLING, &myInterruptY) < 0){

                return 1;
        }
	if (wiringPiISR (SWG, INT_EDGE_FALLING, &myInterruptG) < 0){

                return 1;
        }

	raspicam::RaspiCam_Cv Camera;
	Mat image;

	Camera.set( CV_CAP_PROP_FORMAT, CV_8UC3);
	Camera.set( CV_CAP_PROP_FRAME_WIDTH, 320 );
	Camera.set( CV_CAP_PROP_FRAME_HEIGHT, 240 );
	if (!Camera.open()) {cerr<<"Error opening the camera"<<endl;return -1;}

	CascadeClassifier face_cascade;


	 for(;;)
        {
		if(countG){
			//cout << "camera start"<<endl;
			Mat grayframe;
			int servo_count=15;
			softPwmWrite(SERVO,servo_count);
			while(1){
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
				if ( waitKey(20) == 27 ) break; //ESCÅ° Ž©ž£žé ÁŸ·á
			}
 		}

                // control onoff
                if (countY) {
                        //printf("motor on\n");
                        cout << "starting motor! " << endl;
                    	digitalWrite(MO1, 60);
                }
                else {
                        //printf("stopping motor...\n");

                        cout << "stopping motor" << endl;
			digitalWrite(MO1, LOW);
                }

		if(countY && countW){
			if(countB){
				cout << "spin with speed" << endl;
				digitalWrite_B(HIGH);
			}
			else if(countB==0){
				cout << "only speed" << endl;
				digitalWrite_W2(HIGH);
			}
		}
		else{
			digitalWrite(MO1, LOW);
			delay(100);
			softPwmWrite(SERVO, 15);
			delay(100);
		}
	}
	return 0;
}
