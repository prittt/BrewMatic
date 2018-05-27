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

#include <SD_adafruit.h>

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

OneWire mashUpDs(MASH_DS_PIN_UP);
OneWire mashDownDs(MASH_DS_PIN_DOWN);
OneWire mashEnvDs(MASH_DS_REPLACEMENT_PIN);

OneWire spargeDs(SPARGE_DS_PIN);

Buttons btns;

//************************************************************
// TOUCHSCREEN VARIABLES
//************************************************************

#include <UTFT.h>
#include <UTouch.h>

UTFT glcd(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch touch(43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq

extern uint8_t BigFont[]; // Set the font

//************************************************************
// RECIPE VARIABLES
//************************************************************

#include "Recipe.h"

MashClass mash(mashSteps, sizeof(mashSteps)/sizeof(mashStep), mash_water_weight, mash_malts_weight, &mashUpDs, &mashDownDs, &mashEnvDs, &glcd, &btns);
SpargeClass sparge(&spargeDs, &glcd);


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

//  // open the file. note that only one file can be open at a time,
//  // so you have to close this one before opening another.
//  myFile = SD.open("test.txt", FILE_WRITE);
//  
//  // if the file opened okay, write to it:
//  if (myFile) {
//    Serial.print("Writing to test.txt...");
//    myFile.println("testing 1, 2, 3.");
//  // close the file:
//    myFile.close();
//    Serial.println("done.");
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//  
//  // re-open the file for reading:
//  myFile = SD.open("test.txt");
//  if (myFile) {
//    Serial.println("test.txt:");
//    
//    // read from the file until there's nothing else in it:
//    while (myFile.available()) {
//      Serial.write(myFile.read());
//    }
//    // close the file:
//    myFile.close();
//  } else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//}

void setup(void)
{
  // Setup serial port
#ifdef DEBUG
  Serial.begin(115200); // DFPlayer need 9600 rate
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  } 
#endif

 // RELEs
  pinMode(SPARGE_FIRE_PIN, OUTPUT);
  pinMode(MASH_FIRE_PIN, OUTPUT);
  pinMode(SSR_1, OUTPUT);
  pinMode(SSR_2, OUTPUT);

  digitalWrite(MASH_FIRE_PIN, ON);  // Mash electrovalve ON 
 // Modificato per bollitura
  //mash.FireOff();   // Turn off mash fire
  sparge.FireOff();  // Turn off sparge fire
  // A00 //sparge.FireOff(); // Turn off sparge fire 
  digitalWrite(SSR_2, HIGH);

  pinMode(53, OUTPUT); // Must be set as output even when using software SPI
  DebugSerialPrintln(F("Initializing SD card..."));
  if (!SD.begin(47, 44, 45, 43)) {
      DebugSerialPrintln(F("initialization failed!"));
  }else{
      is_sd_ok = true;
      DebugSerialPrintln(F("initialization complete!"));
  }


  // LCD
  pinMode(8, OUTPUT);  //backlight 3
  digitalWrite(8, HIGH);//on

  glcd.InitLCD();
  glcd.clrScr();

  //touch.InitTouch();
  //touch.setPrecision(PREC_MEDIUM);

  glcd.setFont(BigFont);
  glcd.setBackColor(0, 0, 255);

  // Initialize SD card
  

  // Reading backup from files if necessary
  if(!btns.IsPressed(BTN_1, 5000)){
      // Restore previous state reading it from file
      mash.Deserialize(); 
      //boil.Deserialize();
      UtilityDeserialize();
  }

  // Display LCD's first values 
  mash.DisplayLcd();
  sparge.DisplayLcd();

  // A00 // sparge.DisplayLcd();
  // A00 //boil.DisplayLcd();
  // A00 //LcdDisplayConfigParams();
} // setup

void loop()
{ 
  // Begin SD if necessary (removed because it doesn't work (contattp???)
//  if (!is_sd_ok){
//      DebugSerialPrintln(F("Initializing SD card..."));
//      if (!SD.begin(47, 44, 45, 43)) {
//          DebugSerialPrintln(F("initialization failed!"));
//      }else{
//          is_sd_ok = true;
//          DebugSerialPrintln(F("initialization complete!"));
//      }
//  }

  // Reset arduino if requested
  if(btns.IsPressed(BTN_1, 1000)){
       Backup(true);
       wdt_enable(WDTO_15MS);
       while(1){}; // Rebooting
  }
  
  mash.SetTemperatures(); 
  mash.DisplayLcd();
  
  sparge.SetTemperatures();
  sparge.DisplayLcd();
//  
//  boil.SetTemperatures();
//  boil.DisplayLcd();   
//
//  LcdDisplayConfigParams();

  if(sparge.IsSparge()){ sparge.SetFire(); } 

  if(mash.IsMash()){
    
     //mash.CheckTemperatures();   // Modificato per bollitura
     //mash.SetFire();  // Modificato per bollitura
     
     //mash.HandleStep(); // Modificato per bollitura
       
  } 

//  if(mash.IsMashSpargeTransition()){
//      if(mash.TestIfSpargeDone()){
//          boil.SetIsToTestBoil();
//      }
//  }
//  
//  if(boil.IsToTestBoil()){
//      //mash.FireOn(); // Decommenta per fare boil su fugone intermedio
//      boil.TestIfStartBoil();
//  }
//
//  if(boil.IsBoiling()){
//      boil.TestIfHoppedHop();
//  }

  Backup(false); // Backup data on SD 
  delay(1000);
}// loop

//************************************************************
// LCD FUNCTIONS: begin
//************************************************************

void LcdDisplayConfigParams(){

//  switch(lcd_message_value){
//    case SETTINGS:      
//      lcdMsg.setCursor(0, 0);
//      lcdMsg.print(String(" Settings (") + degree + String("C): "));
//    
//      lcdMsg.setCursor(0, 1);  
//      lcdMsg.print(" D:." + String((int)(maxDelta*10)) + " I:." + String((int)(inertia*10)) + " H:." + String((int)(hysteresis*10)) + " ");   
//      break;
//    case MASH_IN:
//      lcdMsg.setCursor(0, 0);
//      lcdMsg.print(" MASH           ");
//    
//      lcdMsg.setCursor(0, 1);  
//      lcdMsg.print("  IN            ");
//      break;
//    case MASH_OUT:
//      mash.PrintCurrentIodString(lcdMsg, mash.GetIodExtraMinutes());
//    
//      lcdMsg.setCursor(0, 1);  
//      lcdMsg.print(" MOUT  SUB  ADD ");
//      break;
//    case SPARGE_COMPLETE:
//      lcdMsg.setCursor(0,0);
//      lcdMsg.print("SPARGE          ");
//
//      lcdMsg.setCursor(0,1);
//      lcdMsg.print(" DONE           ");
//      break;
//    case START_BOIL:
//      lcdMsg.setCursor(0,0);
//      lcdMsg.print("  GO            ");
//
//      lcdMsg.setCursor(0,1);
//      lcdMsg.print(" BOIL           ");
//      break;
//     case THROW_HOP:
//      lcdMsg.setCursor(0,0);
//      lcdMsg.print(" NEW            ");
//
//      lcdMsg.setCursor(0,1);
//      lcdMsg.print(" HOP            ");
//      break;
//  }
}

#define BACKUP_FREQUENCY 30000 // 30 seconds 150000 //Backup every 2.5 minutes 300000 //Backup every 5 minutes
unsigned long last_backup_time = millis();
void Backup(bool force){
    if((millis() - last_backup_time >= BACKUP_FREQUENCY) || force){
        mash.Serialize();
        //boil.Serialize();
        UtilitySerialize();

        last_backup_time = millis();
    }
}
