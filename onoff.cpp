#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
using namespace std;

#define MO1 27
//#define MO2 26
#define SWY 21
#define EN 18


int main(){

        if(wiringPiSetup() == -1)
                return 1;
        int PWM = 50;
        pinMode(SWY, INPUT);
        pinMode(MO1, OUTPUT);
        //pinMode(MO2, OUTPUT);
        digitalWrite (MO1, LOW);
        softPwmCreate(MO1, 0, 100);

        int swy = HIGH;

        int count = 0;


        for(;;)
        {
//              printf("status = %d\n", count);
                // control switch
                if(swy == HIGH && digitalRead(SWY) == LOW)
                {
                        swy = LOW;

                        cout<<"setting low"<<endl;
                        if (count == 0) {
                                cout << "starting motor..."<<endl;
                                count = 1;
                        }
                        else
                        {
                                cout<<"stopping motor..."<<endl;
                                count = 0;
                        }
                }
                else if(swy == LOW && digitalRead(SWY) == HIGH)
                {
                        swy = HIGH;
                        cout << "setting high" <<endl;

                }

                // contron motor
                if (count) {
                        //printf("motor on\n");
                        softPwmWrite(MO1,100);
                     delay(300);
                        //sleep(1);
                        //printf("motor off\n");
                        //digitalWrite(MO1,HIGH);
                        //sleep(1);
                        //digitalWrite(MO2, LOW);
                }
                else {
                        //printf("stopping motor...\n");
                        softPwmWrite(MO1,0);
                        //digitalWrite(MO1,LOW);
                        //digitalWrite(MO2,LOW);
                }

                delay(5);

        }
        return 0;
}


 
