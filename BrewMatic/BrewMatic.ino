//************************************************************
// INCLUDED LIBRARIES
//************************************************************

#include <SPI.h>
#include <string.h>

// Included for hadle Arduino WatchDog
#include <avr/wdt.h>
#include <avr/interrupt.h>

// Included to check free memory
#include <MemoryFree.h>

// Included to handle display
#include <LiquidCrystal.h>

// Included to handle EEPROM
#include <EEPROMex.h>

// Included to read temperature
#include <OneWire.h>

// It is really necessary??
#include "Arduino.h"

// My libs: 
#include "utilities.h"
#include "Boil.h" // To handle boil process
#include "Mash.h" // To handle mash process
#include "sparge.h" // To handle sparge process
#include "Buttons.h"  // To handle buttons
#include "MegaPins.h" // To handle pin numbers/connections
#include "temp.h" // To handle temperature sensor
#include "buzzer.h" // To handle buzzer

//************************************************************
// LCD/SETUP/STATE GLOBAL VARIABLES
//************************************************************
  
// Initialize the LCD display library with the numbers of the interface pins
LiquidCrystal lcdSparge( SPARGE_PIN_LCD_RS, SPARGE_PIN_LCD_EN, SPARGE_PIN_LCD_D4, SPARGE_PIN_LCD_D5, SPARGE_PIN_LCD_D6, SPARGE_PIN_LCD_D7 );
LiquidCrystal lcdMash( MASH_PIN_LCD_RS, MASH_PIN_LCD_EN, MASH_PIN_LCD_D4, MASH_PIN_LCD_D5, MASH_PIN_LCD_D6, MASH_PIN_LCD_D7 );

LiquidCrystal lcdBoil( BOIL_PIN_LCD_RS, BOIL_PIN_LCD_EN, BOIL_PIN_LCD_D4, BOIL_PIN_LCD_D5, BOIL_PIN_LCD_D6, BOIL_PIN_LCD_D7 );
LiquidCrystal lcdMsg( MSG_PIN_LCD_RS, MSG_PIN_LCD_EN, MSG_PIN_LCD_D4, MSG_PIN_LCD_D5, MSG_PIN_LCD_D6, MSG_PIN_LCD_D7 );



//************************************************************
// SENSOR GLOBAL VARIABLES
//************************************************************

OneWire mashUpDs(MASH_DS_PIN_UP);
OneWire mashDownDs(MASH_DS_PIN_DOWN);
OneWire mashEnvDs(MASH_DS_REPLACEMENT_PIN);

OneWire spargeDs(SPARGE_DS_PIN);
OneWire boilDs(BOIL_DS_PIN);

Buttons btns;
APlayer player(BUZZER);
Buzzer bzr(&btns, &player);

//************************************************************
// RECIPE VARIABLES
//************************************************************

#include "Recipe.h"

MashClass mash(mashSteps, sizeof(mashSteps)/sizeof(mashStep), mash_water_weight, mash_malts_weight, &mashUpDs, &mashDownDs, &mashEnvDs, &lcdMash, &LcdDisplayConfigParams, &btns, &bzr);
BoilClass boil(boilSteps, sizeof(boilSteps)/sizeof(boilStep), totalBoilMinutes, &boilDs, &lcdBoil, &LcdDisplayConfigParams, &btns, &bzr); 
SpargeClass sparge(&spargeDs, &lcdSparge);


//************************************************************
// PLANT VARIABLES
//************************************************************

#include "PlantParams.h"

/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)

*/

#include <SD.h>

void setup(void)
{
  // Setup serial port
#ifdef DEBUG
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  } 
#endif

  // LEDs
  pinMode(MSG_LED_PIN, OUTPUT);
  pinMode(SPARGE_LED_PIN, OUTPUT);
  pinMode(MASH_LED_PIN, OUTPUT);

 // RELEs
  pinMode(SPARGE_FIRE_PIN, OUTPUT);
  pinMode(MASH_FIRE_PIN, OUTPUT);
  
  mash.FireOff();   // Turn off mash fire
  sparge.FireOff(); // Turn off sparge fire 

  sparge.FireOn(); // Per accendere fornello di sparge se serve un fuoco durante il boil

  // Initialize SD card
  DebugSerialPrintln(F("Initializing SD card..."));
  if (!SD.begin(53)) {
      DebugSerialPrintln(F("initialization failed!"));
  }else{
      is_sd_ok = true;
      DebugSerialPrintln(F("initialization complete!"));
  }

  // Reading backup from files if necessary
  if(!btns.IsPressed(RESET_BTN, 5000)){
      // Restore previous state reading it from file
      mash.Deserialize(); 
      boil.Deserialize();
      UtilityDeserialize();
      
      LcdDisplayConfigParams();
  }

  // Setup LCDs
  lcdMash.begin(20, 4);
  lcdMash.createChar(topRightArrow, newTopRightArrow);
  
  lcdBoil.begin(20, 4);
  lcdSparge.begin(16, 2); 
  lcdMsg.begin(16, 2);

  // Display LCD's first values 
  mash.DisplayLcd();
  sparge.DisplayLcd();
  boil.DisplayLcd();
  LcdDisplayConfigParams();
} // setup

void loop()
{ 
  // Begin SD if necessary
  if (!is_sd_ok){
      DebugSerialPrintln(F("Initializing SD card..."));
      if (!SD.begin(53)) {
          DebugSerialPrintln(F("initialization failed!"));
      }else{
          is_sd_ok = true;
          DebugSerialPrintln(F("initialization complete!"));
      }
  }

  // Reset arduino if requested
  if(btns.IsPressed(RESTART_BTN, 1000)){
       Backup(true);
       wdt_enable(WDTO_15MS);
       while(1){}; // Rebooting
  }
  
  mash.SetTemperatures(); 
  mash.DisplayLcd();
  
  sparge.SetTemperatures();
  sparge.DisplayLcd();
  
  boil.SetTemperatures();
  boil.DisplayLcd();   

  LcdDisplayConfigParams();

  if(sparge.IsSparge()){ sparge.SetFire(); } 

  if(mash.IsMash()){
    
     mash.CheckTemperatures();   
     mash.SetFire();  
     
     mash.HandleStep();   
  } 

  if(mash.IsMashSpargeTransition()){
      if(mash.TestIfSpargeDone()){
          boil.SetIsToTestBoil();
      }
  }
  
  if(boil.IsToTestBoil()){
      //mash.FireOn(); // Decommenta per fare boil su fugone intermedio
      boil.TestIfStartBoil();
  }

  if(boil.IsBoiling()){
      boil.TestIfHoppedHop();
  }

  Backup(false); // Backup data on SD 
  delay(1000);
}// loop

//************************************************************
// LCD FUNCTIONS: begin
//************************************************************

void LcdDisplayConfigParams(){

  switch(lcd_message_value){
    case SETTINGS:      
      lcdMsg.setCursor(0, 0);
      lcdMsg.print(String(" Settings (") + degree + String("C): "));
    
      lcdMsg.setCursor(0, 1);  
      lcdMsg.print(" D:." + String((int)(maxDelta*10)) + " I:." + String((int)(inertia*10)) + " H:." + String((int)(hysteresis*10)) + " ");   
      break;
    case MASH_IN:
      lcdMsg.setCursor(0, 0);
      lcdMsg.print(" MASH           ");
    
      lcdMsg.setCursor(0, 1);  
      lcdMsg.print("  IN            ");
      break;
    case MASH_OUT:
      mash.PrintCurrentIodString(lcdMsg, mash.GetIodExtraMinutes());
    
      lcdMsg.setCursor(0, 1);  
      lcdMsg.print(" MOUT  SUB  ADD ");
      break;
    case SPARGE_COMPLETE:
      lcdMsg.setCursor(0,0);
      lcdMsg.print("SPARGE          ");

      lcdMsg.setCursor(0,1);
      lcdMsg.print(" DONE           ");
      break;
    case START_BOIL:
      lcdMsg.setCursor(0,0);
      lcdMsg.print("  GO            ");

      lcdMsg.setCursor(0,1);
      lcdMsg.print(" BOIL           ");
      break;
     case THROW_HOP:
      lcdMsg.setCursor(0,0);
      lcdMsg.print(" NEW            ");

      lcdMsg.setCursor(0,1);
      lcdMsg.print(" HOP            ");
      break;
  }
}

#define BACKUP_FREQUENCY 30000 // 30 seconds 150000 //Backup every 2.5 minutes 300000 //Backup every 5 minutes
unsigned long last_backup_time = millis();
void Backup(bool force){
    if((millis() - last_backup_time >= BACKUP_FREQUENCY) || force){
        mash.Serialize();
        boil.Serialize();
        UtilitySerialize();

        last_backup_time = millis();
    }
}
