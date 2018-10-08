#ifndef MEGA_PINS
#define MEGA_PINS

//************************************************************
// ROTATIVE ENCODER
//************************************************************

#define BTN_3 18
#define SW 18

#define CLK 3
#define DT 2

//************************************************************
// BUTTONs
//************************************************************

#define BTN_1 19
#define BTN_2 A15

//************************************************************
// LEDs - RELEs
//************************************************************

#define MASH_FIRE_PIN 6
#define SPARGE_FIRE_PIN 7 

#define SSR_1 A10
#define SSR_2 A11

//************************************************************
// DFPlayer
//************************************************************

// It is on Arduino's Serial1

//************************************************************
// TEMPERATURE SENSORs
//************************************************************

#define MASH_DS_PIN_UP A7
#define MASH_DS_PIN_DOWN A6

//#define MASH_DS_REPLACEMENT_PIN A9 // Internal temperature
#define INTERNAL_TEMP A9

#define SPARGE_DS_PIN 4
#define SPARGE_DS_REPLACEMENT_PIN 5 

#endif // MEGA_PINS
