#include "Arduino.h"

#define PERIOD 4000

int led = 13;
	
void setup() {
	pinMode(led,OUTPUT);
}


void loop() {
	delay(PERIOD / 2);
	digitalWrite(led,1);
	delay(PERIOD / 2);
	digitalWrite(led,0);
}
