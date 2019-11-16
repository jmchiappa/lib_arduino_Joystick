#include <Joystick.h>

Joystick x(A0);
// Joystick y(A1);
//Joystick x(A0,BACKWARD_WHEN_POSITIVE,8, BACKWARD_WHEN_POSITIVE );

#define PRINT(A,B)	{Serial.print(A);Serial.print(B);};
#define PRINTLN(A,B)	{Serial.print(A);Serial.println(B);};
void setup() {
	x.begin(5);
	// y.begin(5);

	Serial.begin(115200);
	//x.setJoystickReference(512);
	PRINTLN("Référence X : ",x.getJoystickReference());
	// PRINTLN("Référence Y : ",y.getJoystickReference());
}

void loop() {
	// teste toute la dynamique
/*	
	for(uint8_t coef=100;coef>0;coef-=20) {
		x.setOutputCoefficient(coef);
		for(uint32_t input=0;input<1023;input++) {
			PRINT("POSITION : ", input);
			PRINT("\tCOEF : ", coef);
			PRINT("\t VITESSE MOTEUR : ", x.getSpeedValue(input));
			PRINTLN("\t DIRECTION : ", x.getMotorDirection());
		}
	}
	*/
	Serial.println(x.getMotorSpeed());
	PRINTLN("dir=",x.getMotorDirection());
	delay(1);
}
