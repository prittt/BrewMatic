/*
  This file contains all the useful functions to handle sparge
*/

#ifndef SPARGE_H_
#define SPARGE_H_

#include <OneWire.h>
#include "utilities.h"
#include "MegaPins.h"

//************************************************************
// VARIABLES
//************************************************************


class SpargeClass{
private:
    float tempSparge;
    float tareSparge = 0; // Not used yet
    float tared_temp_; 

    bool are_temps_ok_;

    float targetSparge = 79;

    bool is_sparge_; // To control sparge water
    
    unsigned long spargeChangeStateTime;

    OneWire *ds_; 

    LiquidCrystal* lcd_;
    
public:
    SpargeClass(OneWire * ds, LiquidCrystal* lcd){
      spargeChangeStateTime = millis() + FIRE_MAX_FREQUENCY;
      is_sparge_ = true;
      ds_ = ds;
      are_temps_ok_ = false;

      lcd_ = lcd;
    }
    
    //************************************************************
    // VARIABLES
    //************************************************************

    bool IsSparge() { return is_sparge_; }
    void Complete() { is_sparge_ = false;}
    
    void FireOn(void){
        // The sparge fire can be turned ON only if are passed more than "FIRE_MAX_FREQUENCY" millisecond after previuos state change.
        if(millis() - spargeChangeStateTime >= FIRE_MAX_FREQUENCY){
            digitalWrite(SPARGE_FIRE_PIN, ON);  // Sparge electrovalve ON 
            //delay(500);
            digitalWrite(SPARGE_LED_PIN, HIGH); // Switch ON sparge fire LED
            spargeChangeStateTime = millis(); 
        }
    }
    
    void FireOff(void){
        // The sparge fire can be always turned OFF
        digitalWrite(SPARGE_FIRE_PIN, OFF); // Sparge electrovalve OFF
        digitalWrite(SPARGE_LED_PIN, LOW);  // Switch OFF sparge fire LED
        spargeChangeStateTime = millis();
    }

    void SetTemperatures(){        
        tempSparge = getTemp(ds_);

        if(temperatureIsOk(tempSparge)){
            tared_temp_ = tempSparge + tareSparge;
            are_temps_ok_ = true;
        }else{
            are_temps_ok_ = false;
        }
    }

    void SetFire(){
       if(are_temps_ok_){
          if(tared_temp_ <= targetSparge - hysteresis){
              FireOn(); // Fire ON
          }else if(tared_temp_ >= targetSparge + hysteresis - inertia){
              FireOff();
          }
       }else{ // temperatureIsOk if
          FireOff(); // We cannot take decision 
       }  
      
    }

    
    void DisplayLcd(void){
      String targetTempString = String(((int)(targetSparge*10) >= 1000 ? "" : " ")) + String(targetSparge,2);
      
      lcd_->setCursor(0, 0);
      lcd_->print("Target:" + targetTempString + degree + "C ");
    
      lcd_->setCursor(0, 1);
      String spargeTempString; 
      if (are_temps_ok_){
        spargeTempString = String(((int)(tared_temp_*10)/10.0 < 100.0 ? " " : "")) + String(tared_temp_, 2);
        lcd_->print("    " + spargeTempString + degree + "C ");
      }else{
        spargeTempString = temperatureError(tempSparge, true); 
        lcd_->print("    " + spargeTempString + "     ");
      }
    }

    String sparge_backup_file = "sparge.txt";
    // ATTENTION: Deserialize function copied in more parts of the project
    void Deserialize() {
        char character;
        String setting_name;
        String setting_value;
    
        File backup = SD.open(sparge_backup_file, FILE_READ);
    
        if (backup) {
            while (backup.available()) {
                character = backup.read();
                while ((backup.available()) && (character != '[')) {
                    character = backup.read();
                }
                character = backup.read();
                while ((backup.available()) && (character != '=')) {
                    setting_name = setting_name + character;
                    character = backup.read();
                }
                character = backup.read();
                while ((backup.available()) && (character != ']')) {
                    setting_value = setting_value + character;
                    character = backup.read();
                }
                if (character == ']') {
    
                    DebugSerialPrint(F("Name: "));
                    DebugSerialPrintln(setting_name);
                    DebugSerialPrint(F("Value: "));
                    DebugSerialPrintln(setting_value);
    
                    // Apply the value to the parameter
                    ApplySetting(setting_name, setting_value);
    
                    // Reset Strings
                    setting_name = "";
                    setting_value = "";
                }
            }
            // close the file:
            backup.close();
        }
        else {
            DebugSerialPrint(F("Error opening "));
            DebugSerialPrintln(sparge_backup_file);
            backup.close(); // Is this necessary ??
    
        }
    }

    void ApplySetting(String setting_name, String setting_value) {
        
        if (setting_name == "is_sparge_") {
            is_sparge_ = String2Bool(setting_value);
            return;
        }
    }
    
    // ATTENTION: WriteToSd function copied in more parts of the project
    template<typename T>
    void WriteToSd(File& f, String v_name, T value) {
        f.print("[");
        String complete_name = v_name + "=";
        f.print(complete_name);
        f.print(value);
        f.println("]");
    }

    void Serialize() {
        SD.remove(sparge_backup_file); // Delete the old One
        File backup = SD.open(sparge_backup_file, FILE_WRITE); // Create new one
        
        if (backup) {
            WriteToSd(backup, "is_sparge_", is_sparge_);
            
            backup.close();
            DebugSerialPrint(sparge_backup_file);
            DebugSerialPrintln(F(" writing done!"));
        }
        else {
            DebugSerialPrint(F("Error opening "));
            DebugSerialPrintln(sparge_backup_file);
            backup.close(); // Is this necessary?
        }
    }
};

#endif //SPARGE_H_
