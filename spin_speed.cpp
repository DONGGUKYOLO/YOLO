#include <iostream>
#include <wiringPi.h>
#include <softPwm.h>
#include <csignal>
#include <cstdlib>

#define SERVO 0
#define SWB 25
#define MO1 27
#define MO2 26
#define SWW 22
#define EN 18

using namespace std;

int count = LOW;
int count1 = 7;


void digitalWrite2(int signal)
{
	//int count1 = 7;
	digitalWrite(MO1, HIGH);
	//      digitalWrite(LCD, HIGH);
	delay(100);
	if (count1<9) {
		digitalWrite(MO1, LOW);
		//              digitalWrite(LCD, LOW);
		delay(1000 - count1 * 100);
	}
}
void myInterrupt(void) {

	if (digitalRead(SWW) == LOW) {
		delay(1); // 1 ms
		if (digitalRead(SWW) == LOW) {

			++count1;

			if (count1 <= 9) {
				cout << "turning on, speed=" << count1 << endl;

			}
			else {
				cout << "turning on, speed=7" << endl;
				count1 = 7;
			}
		}
	}
}
void sig_handler(int signo)
{
	cout << "i'm dying..." << endl;
	digitalWrite(MO1, LOW);
	//digitalWrite(LCD, LOW);
	exit(1);
}

// button check code at button release
void myInterrupt(void) {

	cout << "Button Pressed\n" << endl;

	if (count == 1 && digitalRead(SWB) == LOW) {
		// debounce handling
		delay(1);
		if (digitalRead(SWB) == LOW) {
			//      printf("digitalRead : %d , count : %d \n", digitalRead(SWW), count);
			cout << "stopping motor...\n" << endl;
			count = 0;
		}
	}
	else if (count == 0 && digitalRead(SWB) == LOW) {
		// debounce handling
		delay(1);
		if (digitalRead(SWB) == LOW) {
			//      printf("digitalRead : %d , count : %d \n", digitalRead(SWW), count);
			cout << "starting motor..\n" << endl;
			count = 1;
		}

		int main() {

			if (wiringPiSetup() == -1)
				return 1;
			int PWM = 50;
			pinMode(SWB, INPUT);
			digitalWrite(SERVO, LOW);
			softPwmCreate(SERVO, 0, 200);

			if (wiringPiISR(SWB, INT_EDGE_FALLING, &myInterrupt) < 0) {
				//fprintf(stderr, "Unable to setup ISR: %s\n", strerror (errno));
				return 1;
			}

			for (;;)
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


			signal(SIGINT, sig_handler);

			if (wiringPiSetup() == -1)
				return 1;

			pinMode(SWW, INPUT);
			pinMode(MO1, OUTPUT);
			pinMode(MO2, OUTPUT);
			//pinMode(LCD, OUTPUT);

			digitalWrite(0, LOW);
			digitalWrite(MO1, LOW);

			if (wiringPiISR(SWW, INT_EDGE_FALLING, &myInterrupt) < 0) {
				return 1;
			}


			for (;;)
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
					digitalWrite(MO1, LOW);
					//digitalWrite(LCD,LOW);
					//                      digitalWrite(MO2,LOW);
				}
				delay(5);

			}

			return 0;
		}




