#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#include <csignal>
#include <cstdlib>

#define MO1 27
#define MO2 26
#define SWW 22
#define EN 18
//#define LCD 2
using namespace std;

int count = 7;


void digitalWrite2(int signal)
{
        //int count = 7;
        digitalWrite(MO1, HIGH);
//      digitalWrite(LCD, HIGH);
        delay(100);
        if (count<9) {
                digitalWrite(MO1, LOW);
//              digitalWrite(LCD, LOW);
                delay(1000-count*100);
        }
}

void myInterrupt(void){

        if(digitalRead(SWW) == LOW){
                delay(1); // 1 ms
                if(digitalRead(SWW) == LOW) {

                        ++count;

                        if (count <= 9) {
                             cout<<"turning on, speed="<< count<<endl;

                        }
                                else {
                                cout<<"turning on, speed=7"<<endl;
                                count = 7;
                        }
                }
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

        signal(SIGINT, sig_handler);

        if(wiringPiSetup() == -1)
                return 1;

        pinMode(SWW, INPUT);
        pinMode(MO1, OUTPUT);
        pinMode(MO2, OUTPUT);
        //pinMode(LCD, OUTPUT);

        digitalWrite (0, LOW);
        digitalWrite (MO1, LOW);

        if(wiringPiISR(SWW, INT_EDGE_FALLING, &myInterrupt) < 0){
                return 1;
        }


        for(;;)
        {
//              printf("status = %d\n", count);

                // contron motor
                if (count) {
                        //printf("starting motor...\n");
                        digitalWrite2(HIGH);
                        //sleep(1);
                        //digitalWrite2(LOW);
                        //sleep(2);

                        //digitalWrite(MO2, LOW);
                }
                else {
                        //printf("stopping motor...\n");
                        digitalWrite(MO1,LOW);
                        //digitalWrite(LCD,LOW);
//                      digitalWrite(MO2,LOW);
                }

                delay(5);

        }
        return 0;
}





 
