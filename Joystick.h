    /*     Simple Stepper Motor Control Exaple Code
     *      
     *  by Dejan Nedelkovski, www.HowToMechatronics.com
     *  
     */
    // defines pins numbers

#ifndef JOYSTICK
#define JOYSTICK

//#define DEBUG

//#define PRINT_DBG

#ifdef PRINT_DBG
#define PRINT(A,B)  {Serial.print(A);Serial.print(B);};
#define PRINTLN(A,B)    {Serial.print(A);Serial.println(B);};
#else
#define PRINT(A,B)  {}
#define PRINTLN(A,B) {}
#endif

enum 
{
    FORWARD_WHEN_POSITIVE=0,
    BACKWARD_WHEN_POSITIVE
};

enum {
    FORWARD=0,
    BACKWARD
};

class Joystick
{
public:
    // Constructor
	Joystick(uint8_t PinNumber, uint8_t Polar=FORWARD_WHEN_POSITIVE, uint8_t range=8, bool Revert = false, uint32_t SamplingPeriod = 5, uint32_t DeadBand=20);
	void begin(uint32_t Period);
    void begin(void);

    // getter
    int32_t getJoystickRelativeValue();
    uint32_t getJoystickReference();
    
    uint32_t getMotorSpeed();
    uint8_t getMotorDirection();
    
    // setter
    /* prend la valeur du joystick comme reference */
    void setJoystickReference();
#ifdef DEBUG
    void setJoystickReference(uint32_t val);
    uint32_t getSpeedValue(uint32_t val);
#endif    
    void setOutputCoefficient(uint8_t coefficient);
    void setSamplingPeriod(uint32_t SamplingPeriod);
    void setJoystickPolarity(uint8_t Polarity);

    /* Controle la sortie de vitesse :
        false : sortie filtrée
        true  : sortie directe provenant du joystick (bypass du filtrage)
    */
private:

    // does begin has been called yet ?
    bool initialized_ =false;
    // internal getter
    void  setSpeedValue(int32_t joystick_input);
    /* Stocke la valeur du point milieu */
	uint32_t	reference;
    
    /* Numéro de la broche d'entrée analogique*/
    uint8_t pin = NC;
    /* Permet d'inverser le sens de calcul de direction et vitesse du joystick 
    Par exemple pousser le joystick vers le haut est négatif alors que c'est la marche avant*/
    uint8_t Polarity= FORWARD_WHEN_POSITIVE;
    ;
    /* valeur de la direction : par convention
        0 : MARCHE AVANT
        1 : MARCHE ARRIERE
    */
    uint8_t output_direction;

    /* Calcul de la vitesse */
    uint32_t output_speed;
    
    /* range : uint8_t exprimant le nombre de bits pour le calcul de la vitesse*/
    uint8_t range;  // number of bits, for DC motor set to 8 bits

    /* afin d"='éviter de alculer à chaque le max, on lecalcule et on le stocke*/
    uint32_t MaxValue;

    /* revert_speed : Boolean speed must be revert when in backward
    false : 0 est toujours la vitesse la plus faible quelque soit le sens
    true : si le sens est avant, 0 est la vitesse la plus faible
           si le sens est arrière, 2^(range)-1 est la vitesse la plus faible
    */
    bool revert_speed=false; 
    
    /* période d'échantillonnage min pour calculer la vitesse en (ms) */
    uint32_t SamplingPeriod = 5;

    /* date de l'ancienne acquisition */
    uint64_t PreviousRefreshTime=0;

    /* Valeur cumulée de l'entrée pour filtrage*/
    uint64_t sumSamples=0;

    /* Nombre d'échantillons cumulées */
    uint16_t nbSamples;

    /* Applique un coefficient directement sur la sortie, compris entre 0 et 100
    0 : met la sortie vitesse au plus bas (0 ou 2^range-1)
    100 : Met la sortie vitesse au plus haut (0 ou 2^range-1)
    */
    uint8_t MaxOutputSpeedCoef = 0;

    /* zone autour de la référence 0 qui évite de fournir une vitesse.
    Dans cette zone, la vitesse est considérée nulle (soit 0, soit 2^range-1)
    Valeur exprimée en LSB d'entrée
    */
    uint32_t Joystick_dead_band;
};

#endif
