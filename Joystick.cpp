    /*     Simple Stepper Motor Control Exaple Code
     *      
     *  by Dejan Nedelkovski, www.HowToMechatronics.com
     *  
     */
    // defines pins numbers

#include "Arduino.h"
#include "Joystick.h"


Joystick::Joystick(uint8_t PinNumber, uint8_t Polar, uint8_t range, bool Revert, uint32_t SamplingPeriod):
	pin(PinNumber),
	Polarity(Polar),
	revert_speed(Revert),
	range(range),
	SamplingPeriod(SamplingPeriod)
{
	// cleanup
	this->reference  = 0;	
	this->sumSamples =0;
	this->MaxOutputSpeedCoef=100;
	this->MaxValue=0;
	this->output_speed=0;
	this->output_direction = FORWARD;
	// évite le débordement du calcul par 2^n -1 si n=32
	for(uint8_t i=0;i<range;i++)
		this->MaxValue |= (1<<i);	// allume les bits jusqu'à range-1
}
void Joystick::begin(uint32_t Period) {
	this->reference = analogRead(this->pin);
	this->SamplingPeriod = Period;
	this->nbSamples=0;
	this->PreviousRefreshTime = millis();
	this->initialized_ = true;
	PRINTLN("MxValue=",this->MaxValue);
}

void Joystick::setJoystickReference()
{
	if(!initialized_) begin(this->SamplingPeriod);
	this->reference = analogRead(this->pin);
}

void Joystick::setOutputCoefficient(uint8_t coefficient){
	this->MaxOutputSpeedCoef = constrain(coefficient,0,100);
}

void Joystick::setSamplingPeriod(uint32_t SamplingPeriod) {
	this->SamplingPeriod = SamplingPeriod;
}

void Joystick::setJoystickPolarity(uint8_t polarity) {
	this->Polarity = polarity;
}

int32_t Joystick::getJoystickRelativeValue()
{
	if(!initialized_) begin(this->SamplingPeriod);
	return analogRead(this->pin) - reference;
}

uint32_t Joystick::getMotorSpeed() {
	if(!initialized_) begin(SamplingPeriod);

	// Ajoute tous les échantillons
	sumSamples+=analogRead(pin);
	nbSamples++;
	if(millis() > (PreviousRefreshTime+SamplingPeriod)) {
		PRINT("Nbsamples :",this->nbSamples);
		PRINT("\tsumsamples :",this->sumSamples);
		PreviousRefreshTime = millis();
		sumSamples/=nbSamples;
		PRINT("\tsample moyen:",this->sumSamples);
		// la dynamique du joystick est toujours sur 10 bits
		int32_t j_value = constrain(sumSamples,0,1023);
		PRINT("\tsample contraint:",j_value);
		nbSamples=0;
		sumSamples=0;
		j_value -= reference;	// soustrait le point milieu
		PRINTLN("\tsample relatif:",j_value);
		// j_value est relatif et centré
		setSpeedValue(j_value);
		// A ce point là, la variable output_speed a été mise à jour
	}
	return output_speed;
}

void Joystick::setSpeedValue(int32_t joystick_input){

	PRINT("joystick input =",joystick_input);
    if (joystick_input>=(int32_t)Joystick_dead_band) {
    	// positif et au-dessus de la bande morte
    	this->output_speed = map(joystick_input,Joystick_dead_band,1023-this->reference,0, this->MaxValue);
    	// PRINTLN(" Positif : intermédiaire 1 ",this->output_speed);

    	// OUTPUT est compris entre 0 et 2^range-1

    	// Applique le coefficient de sortie
		this->output_speed=(this->output_speed*MaxOutputSpeedCoef) / 100;
		this->output_direction = FORWARD;
    	// Si le mode Revert est actif il faut inverser la sortie
    	if(this->Polarity == BACKWARD_WHEN_POSITIVE) {
    		this->output_speed = MaxValue - this->output_speed;
    		this->output_direction = BACKWARD;
	    	// PRINTLN(" Positif : intermédiaire 2 ",this->output_speed);
	    	// PRINTLN(" Positif : intermédiaire 2 - dir=",this->output_direction);
    	}
    } else if (joystick_input<=-(int32_t)Joystick_dead_band) {

    	// négatif et au-dessous de la bande morte
		this->output_speed = map(joystick_input,-reference,-Joystick_dead_band,this->MaxValue,0);
    	// PRINTLN(" Négatif : intermédiaire 1 ",this->output_speed);

    	// Applique le coefficient de sortie
		this->output_speed=(this->output_speed*MaxOutputSpeedCoef) / 100;


		if(this->Polarity == FORWARD_WHEN_POSITIVE) {
			this->output_direction = BACKWARD;
			// Ajoute l'offset pour une sortie 
			this->output_speed = MaxValue - output_speed;
	    	// PRINTLN(" Négatif : intermédiaire 2 ",this->output_speed);
		} else {
			// on ne change pas l'offset et la direction est avant
			this->output_direction = FORWARD;
	    	// PRINT(" Négatif : intermédiaire 3 ",this->output_speed);
	    	// PRINTLN(" Négatif : intermédiaire 3 - dir =",this->output_direction);
		}
    } else {
    	// PRINTLN(" Nul","");    	
    	// dans la bande morte, vitesse nulle
    	this->output_direction=FORWARD;
    	this->output_speed=0;
    }
}

uint8_t Joystick::getMotorDirection() {
	return this->output_direction;
}

uint32_t Joystick::getJoystickReference() {
	return this->reference;
}

// debug helper functions
#ifdef DEBUG
void Joystick::setJoystickReference(uint32_t val){
	this->reference = val;
}

uint32_t Joystick::getSpeedValue(uint32_t val){
	setSpeedValue((int32_t)val-this->reference);
	return this->output_speed;
}
#endif