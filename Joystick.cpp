    /*     Simple Stepper Motor Control Exaple Code
     *      
     *  by Dejan Nedelkovski, www.HowToMechatronics.com
     *  
     */
    // defines pins numbers

#include "Arduino.h"
#include "Joystick.h"


Joystick::Joystick(uint8_t PinNumber, uint8_t Polar, uint8_t range, bool Revert, uint32_t SamplingPeriod,int32_t DeadBand, uint32_t mark):
	pin(PinNumber),
	Polarity(Polar),
	revert_speed(Revert),
	range(range),
	SamplingPeriod(SamplingPeriod),
	Joystick_dead_band(DeadBand),
	_mark(mark)
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

void Joystick::begin() {
	begin(this->SamplingPeriod);
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
		if(nbSamples>0) {
			sumSamples/=nbSamples;
		}
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
	PRINTLN("vitesse : ",this->output_speed);
	return this->output_speed;
}

void Joystick::setSpeedValue(int32_t joystick_input){
	uint32_t tmp_output;
	uint8_t tmp_direction;
	PRINT("joystick input =",joystick_input);
    if (joystick_input>=Joystick_dead_band) {
    	// positif et au-dessus de la bande morte
    	tmp_output = map(joystick_input,Joystick_dead_band,1023-this->reference,0, this->MaxValue);
    	PRINTLN(" Positif : intermédiaire 1 ",tmp_output);

    	// OUTPUT est compris entre 0 et 2^range-1

    	// Applique le coefficient de sortie
		tmp_output=(tmp_output*MaxOutputSpeedCoef) / 100;
		tmp_direction = FORWARD;
    	// Si le mode Revert est actif il faut inverser la sortie
    	if(this->Polarity == BACKWARD_WHEN_POSITIVE) {
    		tmp_output = MaxValue - tmp_output;
    		tmp_direction = BACKWARD;
	    	PRINT(" Positif : intermédiaire 2 "," ");
	    	PRINTLN(tmp_output,tmp_direction);
    	}
    } else if (joystick_input<=-Joystick_dead_band) {

    	// négatif et au-dessous de la bande morte
		tmp_output = map(joystick_input,-reference,-Joystick_dead_band,this->MaxValue,0);
    	PRINTLN(" Négatif : intermédiaire 1 ",tmp_output);

    	// Applique le coefficient de sortie
		tmp_output=(tmp_output*MaxOutputSpeedCoef) / 100;


		if(this->Polarity == FORWARD_WHEN_POSITIVE) {
			tmp_direction = BACKWARD;
			// Ajoute l'offset pour une sortie 
			tmp_output = MaxValue - tmp_output;
	    	PRINT(" Négatif : intermédiaire 2 "," ");
	    	PRINTLN(tmp_output, tmp_direction);
		} else {
			// on ne change pas l'offset et la direction est avant
			tmp_direction = FORWARD;
	    	PRINT(" Négatif : intermédiaire 3 ", " ");
	    	PRINTLN(tmp_output,tmp_direction);
		}
    } else {
    	PRINTLN(" Nul","");    	
    	// dans la bande morte, vitesse nulle
    	tmp_direction=FORWARD;
    	tmp_output=0;
    }

    // Finally apply only if it isn't a marker value
    if(tmp_output!=_mark) {
    	this->output_speed = tmp_output;
    	this->output_direction = tmp_direction;
    }
}

uint8_t Joystick::getMotorDirection() {
	PRINTLN(" direction : ",this->output_direction);
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