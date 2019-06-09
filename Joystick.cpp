    /*     Simple Stepper Motor Control Exaple Code
     *      
     *  by Dejan Nedelkovski, www.HowToMechatronics.com
     *  
     */
    // defines pins numbers

#include "Arduino.h"
#include "Joystick.h"


Joystick::Joystick(){
	this->reference  = 0;	// cleanup
}

void Joystick::setReference(int32_t value)
{
	this->reference = value;
}

int32_t Joystick::RelativeValue(int32_t absoluteValue)
{
	return absoluteValue - reference;
}
