#ifndef MEGA_PINS
#define MEGA_PINS

//************************************************************
// BUTTONs
//************************************************************

//#define BTN_ALARM ??
#define BTN_1 A10
#define BTN_2 A11
#define BTN_3 A12
#define BTN_4 A13
#define BTN_5 A14
#define BTN_6 A15

//************************************************************
// LCDs
//************************************************************

// MASH LCD bright pin
//#define MASH_PWM_LED_PIN 2 // must be PWM
// MASH LCD other pins
#define MASH_PIN_LCD_RS 32//33//39
#define MASH_PIN_LCD_EN 22//35//41
#define MASH_PIN_LCD_D4 24//37//43
#define MASH_PIN_LCD_D5 26//39//45
#define MASH_PIN_LCD_D6 28//41//47
#define MASH_PIN_LCD_D7 30//43//49

// BOIL LCD bright pin
//#define BOIL_PWM_LED_PIN 3 // must be PWM
// BOIL LCD other pins
#define BOIL_PIN_LCD_RS 34//30//39
#define BOIL_PIN_LCD_EN 36//28//41
#define BOIL_PIN_LCD_D4 38//45//43
#define BOIL_PIN_LCD_D5 44//47//45
#define BOIL_PIN_LCD_D6 42//49//47
#define BOIL_PIN_LCD_D7 40//51//49

// SPARGE LCD bright pin
//#define SPARGE_PWM_LED_PIN 4 // must be PWM
// SPARGE other pins
#define SPARGE_PIN_LCD_RS 23//39
#define SPARGE_PIN_LCD_EN 25//41
#define SPARGE_PIN_LCD_D4 27//43
#define SPARGE_PIN_LCD_D5 29//45
#define SPARGE_PIN_LCD_D6 31//47
#define SPARGE_PIN_LCD_D7 33//49

// MESSAGE LCD bright pin
//#define MSG_PWM_LED_PIN 5 // must be PWM
// MESSAGE LCD other pins
#define MSG_PIN_LCD_RS 41
#define MSG_PIN_LCD_EN 37
#define MSG_PIN_LCD_D4 43
#define MSG_PIN_LCD_D5 45
#define MSG_PIN_LCD_D6 47
#define MSG_PIN_LCD_D7 49


//************************************************************
// LEDs - RELEs
//************************************************************

#define MASH_FIRE_PIN A0    // Pin of mash electrovalve's rele --> IN1 on rele board
#define MASH_LED_PIN A6     // Led that say when the mash's fire is ON

#define SPARGE_FIRE_PIN A1 // Pin of sparge electrovalve's rele --> IN2 on rele board
#define SPARGE_LED_PIN A7   // Led that say when the sparge's fire is ON

#define REPLACEMENT_RELE A2 // --> IN3 on rele board
#define REPLACEMENT_RELE A3 // --> IN4 on rele board

#define MSG_LED_PIN A8   // Led for messages

//************************************************************
// BUZZER
//************************************************************

#define BUZZER A9

//************************************************************
// TEMPERATURE SENSORs
//************************************************************

#define MASH_DS_PIN_UP 8
#define MASH_DS_PIN_DOWN 7
#define MASH_DS_REPLACEMENT_PIN 6 // Used to get external/environment temperature to calculate "mash in" correct temperature

#define SPARGE_DS_PIN 14
#define SPARGE_DS_REPLACEMENT_PIN 15 

#define BOIL_DS_PIN 13
#define BOIL_DS_REPLACEMENT_PIN 12

#endif // MEGA_PINS
