//#ifndef BOIL
//#define BOIL
//
//#include "utilities.h"
//#include "temp.h"
//#include "buzzer.h"
//
//typedef struct boilStep{
//    String description_; // step description
//    float minutes_; // minutes of boil for the current step
//    
//    // costructor
//    boilStep(String description, unsigned minutes) {
//        description_ = description;
//        minutes_ = minutes;
//    }  
//} boilStep;
//
//
//typedef struct BoilClass{
//    boilStep *boil_; // vector of boil steps
//    unsigned curStep_;  // current step
//    unsigned totalSteps_; // total number of steps in boil vecor
//    long start_at_; // start boiling time // TODO, check if the change of unsigned long to long ruine the functionalities of the code
//    unsigned totalBoilMinutes_; // total boil minites for the current batch
//    bool is_boil_;  // True if boil is already started
//    bool is_to_test_boil_; 
//
//    float tempBoil;
//    float tareBoil = 0;
//    float tared_temp_;
//
//    OneWire* ds_;
//
//    bool are_temps_ok_;
//
//    LiquidCrystal* lcd_; // Must be 4x20
//    LiquidCrystal* lcd_msg_; // Must be 2x16
//
//
//    void(*LcdDisplayConfigParams_)();
//
//    Buzzer* bzr_;
//    Buttons* btns_;
//         
//    // Constructor
//    BoilClass(boilStep *boil, unsigned totalSteps, unsigned totalBoilMinutes, OneWire* ds, LiquidCrystal* lcd, void(*LcdDisplayConfigParams)(), Buttons* btns, Buzzer* bzr){
//        curStep_ = 0;
//        totalSteps_ = totalSteps;
//        boil_ = boil;
//        totalBoilMinutes_ = totalBoilMinutes;
//        is_boil_ = false;
//        is_to_test_boil_ = false;
//        
//        ds_ = ds;
//        
//        lcd_ = lcd;
//        LcdDisplayConfigParams_ = LcdDisplayConfigParams;
//
//        bzr_ = bzr;
//        btns_ = btns;
//    }
//
//    // Return true if the boil is started, false othwerwise
//    bool IsBoiling(){ return is_boil_; }
//    bool IsToTestBoil(){ return is_to_test_boil_; }
//    void SetIsToTestBoil(){ is_to_test_boil_ = true; }
//
//    // Set all the variable to start boil
//    void StartBoil(){ 
//      is_boil_ = true;
//      start_at_ = millis();  
//    }
//    
//    bool GotoNextHop(void){
//        curStep_++;
//        if(curStep_ >= totalSteps_){
//            return false; //No more phases;
//        }
//    }
//
//    // Return the total seconds to the end of boil
//    unsigned secToEnd(){       
//        if(is_boil_){
//            unsigned long now = millis();
//            int seconds = totalBoilMinutes_*60 - (now - start_at_)/1000;
//            if (seconds < 0){
//                return 0;
//            }else{
//                return (unsigned)seconds;
//            }
//        }else{
//            return (unsigned)(totalBoilMinutes_ * 60);
//        }
//    }
//
//    // Return the total time to the end of boil splitted into minutes and seconds
//    void TimeToEnd(unsigned &minutes, unsigned &seconds){
//      seconds = secToEnd();
//      minutes = seconds/60;
//      seconds = seconds%60;
//    }
//
//    unsigned TimeToThrowCurrHop(){
//      unsigned seconds_to_end = secToEnd();
//      
//      if(curStep_ < totalSteps_ && (seconds_to_end < boil_[curStep_].minutes_*60 || seconds_to_end == 0)){
//          return true;  
//      }
//      return false; 
//    }
//    
//    float millis2minutes(unsigned long ms){
//        return (ms/1000)/60; 
//    }
//    
//    unsigned long minutes2millis(unsigned ms){
//        return ms*1000*60; 
//    }
//    
//    void printCurrentStateString(LiquidCrystal* lcd, bool blink_state = true){
//        String msg = "                    ";
//        if((curStep_) < totalSteps_){
//          if(!TimeToThrowCurrHop()){
//              msg = "*";
//          }else{
//            if(blink_state){
//              msg = rightArrow;  
//            }else{
//              msg = " ";  
//            }   
//          }
//          
//          unsigned minutes = boil_[curStep_].minutes_;
//          String totalMinutesString;
//          if(minutes>99){ totalMinutesString = String(minutes); }else if(minutes>9){totalMinutesString = "0" + String(minutes); }else{totalMinutesString = "00" + String(minutes);}
//          
//          String descr = boil_[curStep_].description_;
//          if (descr.length() > (20 - 11 /*"*(000m) - " .. " "*/)){
//              descr = descr.substring(0, 9);
//          }else{
//              for(int i = descr.length(); i < 9; ++i){
//                  descr += " ";
//              }
//          }
//            
//          msg += "(" + totalMinutesString + "m) - " + descr;
//        }
//        lcd->print(msg);
//    }
//    
//    void printFollowStateString(LiquidCrystal* lcd){
//        
//        String msg = "                    ";
//        if((curStep_+1) < totalSteps_){
//            unsigned minutes = boil_[curStep_ + 1].minutes_;
//            String totalMinutesString;
//            if(minutes>99){ totalMinutesString = String(minutes); }else if(minutes>9){totalMinutesString = "0" + String(minutes); }else{totalMinutesString = "00" + String(minutes);}
//            
//            String descr = boil_[curStep_ + 1].description_;
//            if (descr.length() > (20 - 11 /*"*(000m) - " .. " "*/)){
//              descr = descr.substring(0, 9);
//            }else{
//              for(int i = descr.length(); i < 9; ++i){
//                descr += " ";
//              }
//            }
//            msg = "*(" + totalMinutesString + "m) - " + descr;
//        }
//        lcd->print(msg);
//    }
//    
//    void printStringToEnd(LiquidCrystal* lcd, String temp){
//        
//        unsigned seconds_to_end, minutes_to_end;
//        TimeToEnd(minutes_to_end, seconds_to_end);
//        
//        String minutes_to_end_string;
//        if(minutes_to_end>99){ minutes_to_end_string = String(minutes_to_end); }else if(minutes_to_end>9){minutes_to_end_string = "0" + String(minutes_to_end); }else{minutes_to_end_string = "00" + String(minutes_to_end);}
//        
//        String seconds_to_end_string;
//        if(seconds_to_end>9){ seconds_to_end_string = String(seconds_to_end); }else{ seconds_to_end_string = "0" + String(seconds_to_end); }
//        
//        lcd->print(" " + minutes_to_end_string + "m" + seconds_to_end_string + "s - " + temp + "       ");
//    }
//
//    void DisplayLcd(){
//      String tempBooilString;
//      if(temperatureIsOk(tempBoil)){ if(tared_temp_ < 100) { tempBooilString = " "; } tempBooilString += String(tared_temp_,2) + degree + "C"; }else{ tempBooilString = temperatureError(tempBoil, true); }
//      
//      lcd_->setCursor(0,0); 
//      printStringToEnd(lcd_, tempBooilString); 
//      
//      lcd_->setCursor(0,1); 
//      lcd_->print("--------------------");
//      lcd_->setCursor(0,2);
//      printCurrentStateString(lcd_);  
//      lcd_->setCursor(0,3);
//      printFollowStateString(lcd_);
//    }
//
//    void SetTemperatures(){ 
//      tempBoil = getTemp(ds_);
//      
//      if(temperatureIsOk(tempBoil)){
//          tared_temp_ = tempBoil + tareBoil;
//          are_temps_ok_ = true; 
//      }else{
//          are_temps_ok_ = false;
//      }
//    }
//
//    void TestIfStartBoil(){
//        bzr_->HandleRingStopPospone(300000); // Pospone for 5 minutes
//        bzr_->PlayWhenNeeded(are_temps_ok_ && tared_temp_ >= 100);
//        
//        // Start boil if necessary 
//        if(btns_->IsPressed(START_BOIL_BTN)){
//            StartBoil();
//            lcd_message_value = SETTINGS;
//            LcdDisplayConfigParams_();          
//            bzr_->ResetRing();
//            LedBlink(MSG_LED_PIN);
//            is_to_test_boil_ = false;
//        }
//    }
//
//    void TestIfHoppedHop(){
//      
//        if(TimeToThrowCurrHop()){
//            lcd_message_value = THROW_HOP;
//            LcdDisplayConfigParams_();
//
//            bzr_->HandleRingStopPospone(30000); // Pospone for 30 seconds
//            bzr_->PlayWhenNeeded(); // TODO change 30 to 100
//      
//            // Check if the hop is hopped
//            if(btns_->IsPressed(THROW_HOP_BTN)){
//                bzr_->ResetRing();
//                lcd_message_value = SETTINGS;
//                LcdDisplayConfigParams_();
//                GotoNextHop(); // TODO handle case of last hop and case of end boil
//                LedBlink(MSG_LED_PIN);
//                DisplayLcd();
//            }  
//        }
//    }
//
//    unsigned long BoilElapsedMillis(){
//        unsigned long now = millis();
//        if(is_boil_){
//            return now - start_at_;  
//        }else{
//            return 0;  
//        }
//    }
//
//    String boil_backup_file = "boil.txt";
//    // ATTENTION: Deserialize function copied in more parts of the project
//    void Deserialize() {
//        char character;
//        String setting_name;
//        String setting_value;
//    
//        File backup = SD.open(boil_backup_file, FILE_READ);
//    
//        if (backup) {
//            while (backup.available()) {
//                character = backup.read();
//                while ((backup.available()) && (character != '[')) {
//                    character = backup.read();
//                }
//                character = backup.read();
//                while ((backup.available()) && (character != '=')) {
//                    setting_name = setting_name + character;
//                    character = backup.read();
//                }
//                character = backup.read();
//                while ((backup.available()) && (character != ']')) {
//                    setting_value = setting_value + character;
//                    character = backup.read();
//                }
//                if (character == ']') {
//    
//                    DebugSerialPrint(F("Name: "));
//                    DebugSerialPrintln(setting_name);
//                    DebugSerialPrint(F("Value: "));
//                    DebugSerialPrintln(setting_value);
//    
//                    // Apply the value to the parameter
//                    ApplySetting(setting_name, setting_value);
//    
//                    // Reset Strings
//                    setting_name = "";
//                    setting_value = "";
//                }
//            }
//            // close the file:
//            backup.close();
//        }
//        else {
//            DebugSerialPrint(F("Error opening "));
//            DebugSerialPrintln(boil_backup_file);
//            backup.close(); // Is this necessary ??
//    
//        }
//    }
//
//    void ApplySetting(String setting_name, String setting_value) {
//                
//        if (setting_name == "curStep_") {
//            curStep_ = String2Unsigned(setting_value);
//            return;
//        }
//        
//        if (setting_name == "is_boil_") {
//            is_boil_ = String2Bool(setting_value);
//            return;
//        }
//
//        if (setting_name == "is_to_test_boil_") {
//            is_to_test_boil_ = String2Bool(setting_value);
//            return;
//        }
//        
//        if (setting_name == "boil_elapsed_millist_") {
//            if(is_boil_){
//                start_at_ = millis() - String2Long(setting_value);
//            }else{
//                start_at_ = 0;   
//            }
//            DebugSerialPrint(F("boil start at (millis): "));
//            DebugSerialPrintln(String(start_at_));
//            return;
//        }
//    }
//    
//    // ATTENTION: WriteToSd function copied in more parts of the project
//    template<typename T>
//    void WriteToSd(File& f, String v_name, T value) {
//        f.print("[");
//        String complete_name = v_name + "=";
//        f.print(complete_name);
//        f.print(value);
//        f.println("]");
//    }
//
//    void Serialize() {
//        SD.remove(boil_backup_file); // Delete the old One
//        File backup = SD.open(boil_backup_file, FILE_WRITE); // Create new one
//        
//        if (backup) {
//          
//            // Backup of boil class values
//            WriteToSd(backup, "curStep_", curStep_); 
//            WriteToSd(backup, "is_boil_", is_boil_);  // It must be restored befor boil_elasped_millis!
//            WriteToSd(backup, "is_to_test_boil_", is_to_test_boil_); 
//
//            WriteToSd(backup, "boil_elapsed_millist_", BoilElapsedMillis()); 
//            
//            backup.close();
//            DebugSerialPrint(boil_backup_file);
//            DebugSerialPrintln(F(" writing done!"));
//        }
//        else {
//            DebugSerialPrint(F("Error opening "));
//            DebugSerialPrintln(boil_backup_file);
//            backup.close(); // Is this necessary?
//        }
//    }    
//}BoilClass;
//
//#endif // BOIL
