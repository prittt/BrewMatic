#ifndef UTILITIES_H_
#define UTILITIES_H_

#define FIRE_MAX_FREQUENCY 15000 //30 secondi? 

//#include <SD_adafruit.h>
//#include "debug.h"

enum ReleOnOff{
  ON = HIGH,
  OFF = LOW
};

enum LcdMessages{
  BASE = 0, 
  MASH_IN = 1,
  MASH_OUT = 2,
  SPARGE_COMPLETE = 4,
  START_BOIL = 5,
  THROW_HOP = 6,
};
int lcd_message_value = BASE;

// Id of useful LCD characters
char obelo = 253;  
char rightArrow = 126;
char degree = 223;
char topRightArrow = 0;
char BottomArrow = 5; 

// Defining new characters for LCDs
byte newTopRightArrow[8] = { B00000, B01111, B00011, B00101, B01001, B10000, B00000, B00000 };

bool is_sd_ok = false;

void LedBlink(int led_pin){
  digitalWrite(led_pin, HIGH);
  delay(100);
  digitalWrite(led_pin, LOW);  
  delay(100);
  digitalWrite(led_pin, HIGH);
  delay(100);
  digitalWrite(led_pin, LOW);  
  delay(100);
  digitalWrite(led_pin, HIGH);
  delay(100);
  digitalWrite(led_pin, LOW);  
}

// Convert String to Float
float String2Float(String setting_value) {
    char* floatbuf = new char[setting_value.length() + 1];
    setting_value.toCharArray(floatbuf, sizeof(floatbuf));
    float f = atof(floatbuf);
    delete[] floatbuf;
    return f;
}

// Convert String to Long
long String2Long(String setting_value) {
    char longbuf[setting_value.length() + 1];
    setting_value.toCharArray(longbuf, sizeof(longbuf));
    long l = atol(longbuf);
    return l;
}

// Convert String to Long
unsigned String2Unsigned(String setting_value) {
    return unsigned(String2Long(setting_value)); // On mega long are 4 bytes long and unsigned are 2 bytes long!!
}

unsigned long String2uLong(String setting_value){
  unsigned long ul;
  
  // Big Endian
  //ul  = (unsigned long) setting_value[0] << 24 | (unsigned long) setting_value[1] << 16 | (unsigned long) setting_value[2] << 8 | setting_value[3];

  // Little Endian
  ul  = (unsigned long) setting_value[3] << 24 | (unsigned long) setting_value[2] << 16 | (unsigned long) setting_value[1] << 8 | setting_value[0];
  return ul;
}

// Convert String to Bool
bool String2Bool(String settingValue) {
    if (settingValue.toInt() == 1) {
        return true;
    }
    else {
        return false;
    }
}

void UtilityApplySetting(String setting_name, String setting_value) {
    if (setting_name == "lcd_message_value") {
        lcd_message_value = setting_value.toInt();
        return;
    }
}

//const char utility_backup_file[12] = {'u', 't', 'i', 'l', 'i', 't', 'y', '.', 't','x', 't', '\0'};
//// ATTENTION: Deserialize function copied in more parts of the project
//void UtilityDeserialize() {
//    char character;
//    String setting_name;
//    String setting_value;
//
//    File backup = SD.open(utility_backup_file, FILE_READ);
//
//    if (backup) {
//        while (backup.available()) {
//            character = backup.read();
//            while ((backup.available()) && (character != '[')) {
//                character = backup.read();
//            }
//            character = backup.read();
//            while ((backup.available()) && (character != '=')) {
//                setting_name = setting_name + character;
//                character = backup.read();
//            }
//            character = backup.read();
//            while ((backup.available()) && (character != ']')) {
//                setting_value = setting_value + character;
//                character = backup.read();
//            }
//            if (character == ']') {
//
//                DebugSerialPrint(F("Name: "));
//                DebugSerialPrintln(setting_name);
//                DebugSerialPrint(F("Value: "));
//                DebugSerialPrintln(setting_value);
//
//                // Apply the value to the parameter
//                UtilityApplySetting(setting_name, setting_value);
//
//                // Reset Strings
//                setting_name = "";
//                setting_value = "";
//            }
//        }
//        // close the file:
//        backup.close();
//    }
//    else {
//        DebugSerialPrint(F("Error opening "));
//        DebugSerialPrintln(utility_backup_file);
//        backup.close(); // Is this necessary ??
//    }
//}
//
//// ATTENTION: WriteToSd function copied in more parts of the project
//template<typename T>
//void UtilityWriteToSd(File& f, String v_name, T value) {
//    f.print("[");
//    String complete_name = v_name + "=";
//    f.print(complete_name);
//    f.print(value);
//    f.println("]");
//}
//
//void UtilitySerialize() {
//    SD.remove(utility_backup_file); // Delete the old One
//    File backup = SD.open(utility_backup_file, FILE_WRITE); // Create new one
//    
//    if (backup) {
//        // Backup of utility variables
//        UtilityWriteToSd(backup, "lcd_message_value", lcd_message_value);
//               
//        backup.close();
//        DebugSerialPrint(utility_backup_file);
//        DebugSerialPrintln(F(" writing done!"));
//    }
//    else {
//        DebugSerialPrint(F("Error opening "));
//        DebugSerialPrintln(utility_backup_file);
//        backup.close(); // Is this necessary?
//    }
//}

#endif // UTILITIES_H_
