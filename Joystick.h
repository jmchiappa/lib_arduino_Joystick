    /*     Simple Stepper Motor Control Exaple Code
     *      
     *  by Dejan Nedelkovski, www.HowToMechatronics.com
     *  
     */
    // defines pins numbers

#ifndef JOYSTICK
#define JOYSTICK



class Joystick
{
public:
	Joystick();
  	void setReference(int32_t value);
	int32_t RelativeValue(int32_t absoluteValue);
private:
	int32_t	reference;
};

#endif
