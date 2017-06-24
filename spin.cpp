#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>

#define SERVO 0
#define SWB 29

using namespace std;

int count = LOW;

// button check code at button release
void myInterrupt(void) {

        cout<<"Button Pressed\n"<< endl;

        if (count == 1 && digitalRead(SWB)== LOW){
                // debounce handling
                delay(1);
                if (digitalRead(SWB) == LOW) {
                //      printf("digitalRead : %d , count : %d \n", digitalRead(SWW), count);
                         cout<<"stopping motor...\n"<<endl;
                        count = 0;
                }
        }
        else if(count == 0 && digitalRead(SWB) == LOW){
                // debounce handling
                delay(1);
                if (digitalRead(SWB) == LOW) {
                //      printf("digitalRead : %d , count : %d \n", digitalRead(SWW), count);
                        cout<<"starting motor..\n"<<endl;
                        count = 1;
		}
	}
}


int main(){

        if(wiringPiSetup() == -1)
                return 1;
        int PWM = 50;
        pinMode(SWB, INPUT);
        digitalWrite (SERVO, LOW);
        softPwmCreate(SERVO, 0, 200);

        if (wiringPiISR (SWB, INT_EDGE_FALLING, &myInterrupt) < 0){
                //fprintf(stderr, "Unable to setup ISR: %s\n", strerror (errno));
                return 1;
        }


//      int sww = HIGH;


        for(;;)
        {
                // printf("status = %d\n", count);
                // control switch

                // contron motor
                if (count) {
                        int i;
                        //printf("starting motor...\n");
                        for (i = 20; i >= 10; i--) {
                                //printf("changing angle to %d\n", i);
                                softPwmWrite(SERVO, i);
                                delay(200);
                                // check for the interrupt
                                if (count == 0)
                                        break;
                        }
                        for (i = 10; i <= 20; i++) {
                                //printf("changing angle to %d\n", i);
                               softPwmWrite(SERVO, i);
                                // check for the interrupt
                                delay(200);
                                if (count == 0)
                                        break;
                        }

                }
                else {
                        //printf("stopping motor...\n");
                        softPwmWrite(SERVO, 15);
                        //usleep(50000);
                }

                delay(5);

		}
        
        return 0;

              //  }
     //   } else {
//              printf("shouldn't be here...\n");
  //            printf("digitalRead : %d , count : %d \n", digitalRead(SWB), count);
        }
 
