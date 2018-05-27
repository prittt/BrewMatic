#ifndef MASH
#define MASH

#include <OneWire.h>

#include <SPI.h>
#include <SD.h>

//#include <EEPROMex.h> // For F() function

#include "utilities.h"
#include "MegaPins.h"
#include "PlantParams.h"

typedef struct mashStep {
    float temperature_; // Temperature in °C
    unsigned minutes_; // Minutes of the step
    bool is_to_iod_test_; // Whether it is the "iod test" step
    long startAt_; // Step start at time (milliseconds)
    bool isStarted_; // whether the step is started or not
    bool isMashIn_; // Whether is a "mash in" step or not
    long extra_iod_time_start_at_ = 0; // Start time of iod extra minutes
    unsigned extra_iod_minutes_ = 0; // Total added minutes (by the user) for the iod test

    mashStep(float temperature, unsigned minutes, bool is_to_iod_test = false, bool isMashIn = false) {
        temperature_ = temperature;
        minutes_ = minutes;
        is_to_iod_test_ = is_to_iod_test;
        startAt_ = 0;
        isStarted_ = false;
        isMashIn_ = isMashIn;
    }
} mashStep;

typedef struct MashClass {
    mashStep *mash_; // Vector of steps
    unsigned cur_step_;  // Number of the current step
    unsigned totalSteps_; // Total number of steps in the "mash" variable 
    bool is_mash_; // Wheter the mash is in progress or not

    bool mash_in_ongoing_; // This variable is useful to handle the following case: when is the time to 
                           // "mash in" if you put the malt in the moisture and the temperature goes down 
                           // the button "mash in" may disappear. If the mash_in_ongoing_ is true the button will be forced to appear.
    unsigned long mash_change_state_time_;

    float tempMashUp;
    float tareMashUp = 0;
    float tared_temp_up_;

    float tempMashDown;
    float tareMashDown = 0;
    float tared_temp_down_;

    float tempMashAverage;

    bool are_temps_ok_;

    OneWire *up_ds_;
    OneWire *down_ds_;
    OneWire *env_ds_;

    bool danger_delta_zone_; // This variable, when set to true, means that mash fire is turned OFF due to too high sensors delta!
    bool danger_temp_zone_; // If true means there are some errors in read temperature!

    bool mash_sparge_transition_;

    Buzzer* bzr_;
    Buttons* btns_;

    LiquidCrystal* lcd_;
    void(*LcdDisplayConfigParams_)();

    float water_weight_; // Useful to calculate inertia for mash in optimal temperature
    float malts_weight_; // Useful to calculate inertia for mash in optimal temperature

    // Constructor
    MashClass(mashStep *mash, unsigned totalSteps, float water_weight, float malts_weight, OneWire* up_ds, OneWire* down_ds, OneWire* env_ds, LiquidCrystal* lcd, void(*LcdDisplayConfigParams)(), Buttons* btns, Buzzer* bzr) {
        cur_step_ = 0;
        totalSteps_ = totalSteps;
        mash_ = mash;
        is_mash_ = true;
        mash_change_state_time_ = millis() + FIRE_MAX_FREQUENCY;
        
        up_ds_ = up_ds;
        down_ds_ = down_ds;
        env_ds_ = env_ds;
        
        are_temps_ok_ = false;
        mash_sparge_transition_ = false;

        water_weight_ = water_weight;
        malts_weight_ = malts_weight;

        lcd_ = lcd;
        LcdDisplayConfigParams_ = LcdDisplayConfigParams;

        bzr_ = bzr;
        btns_ = btns;

        mash_in_ongoing_= false;
    }

    void Complete() { is_mash_ = false; }
    bool IsMash() { return is_mash_; }
    bool IsMashSpargeTransition() { return mash_sparge_transition_; }

    void AddIodMin() { if (mash_[cur_step_].extra_iod_minutes_ < 99) { mash_[cur_step_].extra_iod_minutes_++; } }
    void SubIodMin() { if (mash_[cur_step_].extra_iod_minutes_ > 0 ) { mash_[cur_step_].extra_iod_minutes_--; } }

    /*Return the extra minutes setted by the user during batch*/
    unsigned GetIodExtraMinutes() { return mash_[cur_step_].extra_iod_minutes_; }

    /*Return true if estra iod minutes are gone*/
    bool IsIodRing() {
        if (secOverEnd() / (double)60 >= mash_[cur_step_].extra_iod_minutes_) {
            return true;
        }
        return false;
    }

    void GotoNextPhase(void) {
        cur_step_++;
        if (cur_step_ >= totalSteps_) {
            //return false; //No more phases;
            DebugSerialPrintln(F("MASH OVER, SPARGING IN PROGRESS!"));

            FireOff(); // Turn OFF fire
            lcd_message_value = SPARGE_COMPLETE;
            LcdDisplayConfigParams_();
            mash_sparge_transition_ = true;
            Complete();
            DisplayLcd();
        }
    }

    float GetMashInTemp(float target_temp){

        // Get temperature from the sensor if available, set to 20°C otherwise
        float ti = getTemp(env_ds_);
        if (!temperatureIsOk(ti)){
            ti = 20;
        }
        
        return (WATER_CP*water_weight_*target_temp + STEEL_CP*MASH_TUN_WEIGHT*target_temp + MALTS_CP*malts_weight_*(target_temp - ti))/(WATER_CP*water_weight_ + STEEL_CP*MASH_TUN_WEIGHT);       
        /* Original formula: t_mash_in = (ca*ma*te + cp*mp*te + cm*mm*(te - ti))/(ca*ma + cp*mp);
           where: 
              ca = calore specifico acqua;
              ma = massa acqua; 
              cp = calore specifico pentola (acciaio inox)
              mp = massa pentola; 
              cm = calore specifico malto;
              mm = massa malto; 
              te = temperatura desiderata per il mash in; 
              ti = temperatura del malto (temperatura ambiente); 
        */
    }

    float GetTargetTemp(void) {
        if (IsMashIn()){
            return GetMashInTemp(mash_[cur_step_].temperature_);
        }else{
            return mash_[cur_step_].temperature_;
        }
    }

    bool IsOver() {
        unsigned long now = millis();
        if (millis2minutes(now - mash_[cur_step_].startAt_) >= mash_[cur_step_].minutes_) {
            return true;
        }
        return false;
    }

    bool IsMashIn() {
        return mash_[cur_step_].isMashIn_;
    }

    void MashInComplete() {
        mash_[cur_step_].isMashIn_ = false;
    }

    bool IsRamp(void) {
        return !(mash_[cur_step_].isStarted_);
    }

    bool IsToIodTest(void) {
        return mash_[cur_step_].is_to_iod_test_;
    }

    void StopIodTest(void) {
        mash_[cur_step_].is_to_iod_test_ = false;
    }

    void setExtraTimeStartPoint() {
        unsigned long now = millis();
        mash_[cur_step_].extra_iod_time_start_at_ = now - millisOverEnd();
    }

    float millis2minutes(unsigned long ms) {
        return (ms / 1000) / 60.0;
    }

    void StartStep(void) {
        mash_[cur_step_].isStarted_ = true;
        mash_[cur_step_].startAt_ = millis();
    }

    void PrintCurrentStateString() {
        // Specific for 20 characters lcd
        String msg;
        if (mash_[cur_step_].isStarted_) {
            lcd_->print(rightArrow);
        }
        else {
            lcd_->print(topRightArrow);
        }

        unsigned secondsToEnd = secToEnd();
        unsigned minutesToEnd = secondsToEnd / 60;
        secondsToEnd = secondsToEnd % 60;

        String minutesToEndString;
        if (minutesToEnd > 99) { minutesToEndString = String(minutesToEnd); }
        else if (minutesToEnd > 9) { minutesToEndString = "0" + String(minutesToEnd); }
        else { minutesToEndString = "00" + String(minutesToEnd); }

        String secondsToEndString;
        if (secondsToEnd > 9) { secondsToEndString = String(secondsToEnd); }
        else { secondsToEndString = "0" + String(secondsToEnd); }

        unsigned minutes = mash_[cur_step_].minutes_;
        String totalMinutesString;
        if (minutes > 99) { totalMinutesString = String(minutes); }
        else if (minutes > 9) { totalMinutesString = "0" + String(minutes); }
        else { totalMinutesString = "00" + String(minutes); }

        msg = "(" + String(mash_[cur_step_].temperature_, 0) + degree + "C)-";
        if (mash_[cur_step_].isStarted_) {
            msg += minutesToEndString + "m" + secondsToEndString + "s/" + totalMinutesString + "m";
        }
        else {
            msg += totalMinutesString + "m00s/" + totalMinutesString + "m";
        }
        lcd_->print(msg);
    }

    void PrintFollowStateString() {
        // Specific for 20 characters lcd
        String msg = "                    ";
        if ((cur_step_ + 1) < totalSteps_) {
            unsigned minutes = mash_[cur_step_ + 1].minutes_;
            String totalMinutesString;
            if (minutes > 99) { totalMinutesString = String(minutes); }
            else if (minutes > 9) { totalMinutesString = "0" + String(minutes); }
            else { totalMinutesString = "00" + String(minutes); }

            msg = "*(" + String(mash_[cur_step_ + 1].temperature_, 0) + degree + "C)-" + totalMinutesString + "m00s/" + totalMinutesString + "m";
        }
        lcd_->print(msg);
    }

    void PrintCurrentIodString(LiquidCrystal lcd, unsigned minutes_added) {

        // Specific for first line of 16 characters lcd        
        unsigned seconds_over_end = secOverEnd();
        unsigned minutes_over_end = seconds_over_end / 60;
        seconds_over_end -= minutes_over_end * 60;

        String minutes_over_end_string;
        if (minutes_over_end > 9) { minutes_over_end_string = String(minutes_over_end); }
        else { minutes_over_end_string = "0" + String(minutes_over_end); }

        String seconds_over_end_string;
        if (seconds_over_end > 9) { seconds_over_end_string = String(seconds_over_end); }
        else { seconds_over_end_string = "0" + String(seconds_over_end); }

        String minutes_added_string;
        if (minutes_added > 9) { minutes_added_string = String(minutes_added); }
        else { minutes_added_string = "0" + String(minutes_added); }

        // Message to display: "   00m00s/00m   "
        String msg = "   " + minutes_over_end_string + "m" + seconds_over_end_string + "s/" + minutes_added_string + "m   ";

        lcd.setCursor(0, 0);
        lcd.print(msg);
    }

    void DisplayLcd(void) {

        String tempMashUpString;
        if (temperatureIsOk(tempMashUp)) { tempMashUpString = String(tared_temp_up_, 2); }
        else { tempMashUpString = temperatureError(tempMashUp); }

        String tempMashDownString;
        if (temperatureIsOk(tempMashDown)) { tempMashDownString = String(tared_temp_down_, 2); }
        else { tempMashDownString = temperatureError(tempMashDown); }

        String tempMashAverageString;
        if (temperatureIsOk(tempMashAverage)) { tempMashAverageString = String(tempMashAverage, 2) + degree + "C"; }
        else { tempMashAverageString = temperatureError(tempMashAverage) + "  "; }

        if (is_mash_ || mash_sparge_transition_) {
            lcd_->setCursor(0, 0);
            lcd_->print(tempMashUpString + obelo + tempMashDownString + " " + rightArrow + tempMashAverageString);

            lcd_->setCursor(0, 1);
            lcd_->print("--------------------");

            if (mash_sparge_transition_) {
                // No step to display
                lcd_->setCursor(0, 2);
                lcd_->print("     MASH OVER,     ");

                lcd_->setCursor(0, 3);
                lcd_->print("turn off pilot flame");
            }
            else {
                // Display current ramp/step  
                lcd_->setCursor(0, 2);
                PrintCurrentStateString();

                // Display following step
                lcd_->setCursor(0, 3);
                PrintFollowStateString();
            }
        }
        else {
            // Display MASH OVER!
            lcd_->setCursor(0, 0);
            lcd_->print("      +------+      ");

            lcd_->setCursor(0, 1);
            lcd_->print("      | MASH |      ");

            lcd_->setCursor(0, 2);
            lcd_->print("      | OVER |      ");

            lcd_->setCursor(0, 3);
            lcd_->print("      +------+      ");
        }
    }

    unsigned long millisToEnd() {
        unsigned long now = millis();
        unsigned long millisToEnd = mash_[cur_step_].minutes_ * 60 * 1000 - (now - mash_[cur_step_].startAt_);
        if (millisToEnd > 0) {
            return millisToEnd;
        }
        else {
            return 0;
        }
    }

    unsigned long millisOverEnd() {
        unsigned long now = millis();
        unsigned long millisOverEnd = mash_[cur_step_].minutes_ * 60 * 1000 - (now - mash_[cur_step_].startAt_);
        if (millisOverEnd < 0) {
            return millisOverEnd * (-1);
        }
        else {
            return 0;
        }
    }

    unsigned secToEnd() {
        unsigned long now = millis();
        int seconds = mash_[cur_step_].minutes_ * 60 - (now - mash_[cur_step_].startAt_) / 1000;
        if (seconds < 0) {
            return 0;
        }
        else {
            return (unsigned)seconds;
        }
    }

    unsigned secOverEnd() {
        unsigned long now = millis();
        
        if(!mash_[cur_step_].isStarted_){
            return 0;  
        }
        
        int seconds = mash_[cur_step_].minutes_ * 60 - (now - mash_[cur_step_].startAt_) / 1000;
        if (seconds > 0) {
            return 0;
        }
        else {
            return (unsigned)seconds*(-1);
        }
    }

    unsigned long ElapsedMillis(){
        unsigned long now = millis();
        if(mash_[cur_step_].isStarted_){
            return now - mash_[cur_step_].startAt_;  
        }else{
            return 0;  
        }
    }

    unsigned long ElapsedIodMillis(){
        unsigned long now = millis();
        if(mash_[cur_step_].is_to_iod_test_ && mash_[cur_step_].extra_iod_time_start_at_ != 0){
            return now - mash_[cur_step_].extra_iod_time_start_at_;  
        }else{
            return 0;
        }  
    }

    void FireOn(void) {
        // The mash fire can be turned ON only if are passed more than "FIRE_MAX_FREQUENCY" millisecond after previuos state change.
        if (millis() - mash_change_state_time_ >= FIRE_MAX_FREQUENCY) {
            digitalWrite(MASH_FIRE_PIN, ON);  // Mash electrovalve ON 
            //delay(500);
            digitalWrite(MASH_LED_PIN, HIGH); // Switch ON mash fire LED
            mash_change_state_time_ = millis();
        }
    }

    void FireOff(void) {
        // The mash fire can be always turned OFF
        digitalWrite(MASH_FIRE_PIN, OFF);  // Mash electrovalve OFF 
        digitalWrite(MASH_LED_PIN, LOW);  // Switch OFF mash fire LED
        mash_change_state_time_ = millis();
    }

    void SetTemperatures() {
        tempMashUp = getTemp(up_ds_);     // Get temperature up
        tempMashDown = getTemp(down_ds_); // Get temperature down 

        tared_temp_up_ = tempMashUp + tareMashUp;       // To tare temperature value
        tared_temp_down_ = tempMashDown + tareMashDown; // To tare temperature value

        if (temperatureIsOk(tempMashUp) && temperatureIsOk(tempMashDown)) {
            tempMashAverage = (tared_temp_up_ + tared_temp_down_) / 2.0; // Average temperature
            are_temps_ok_ = true;
        }
        else {
            tempMashAverage = -1000; // Error case
            are_temps_ok_ = false;
        }
    }

    // Check temperatures and set fire off in case of errors
    void CheckTemperatures() {

        danger_temp_zone_ = false;
        danger_delta_zone_ = false;

        if (are_temps_ok_) {
            if (abs(tared_temp_up_ - tared_temp_down_) > maxDelta) {
                FireOff(); // Turn OFF the mash fire
                danger_delta_zone_ = true; // To set danger zone
                DebugSerialPrintln("DANGER DELTA ZONE: mash delta is higher than" + String(maxDelta) + "°C ...");
            }
        }
        else {
            // Temperatures are wrong! 
            FireOff(); // Turn off the fire because we cannot take the correct decision (waiting for next loop)
            danger_temp_zone_ = true;  // To set danger zone 
            DebugSerialPrintln(F("DANGER TEMPERATURE VALUE: one or both sensors return 'ERROR' values ..."));
        }
    }

    void SetFire() {
        if (!mash_sparge_transition_) {
            if (!danger_temp_zone_ && !danger_delta_zone_) {
                // No danger zone: we have to decide what to do with mash fire
                if (tempMashAverage >= GetTargetTemp() + hysteresis - inertia) {
                    FireOff(); //Turn OFF the mash fire
                }
                else {
                    if (tempMashAverage < GetTargetTemp() - hysteresis) {
                        FireOn(); //Turn ON the mash fire
                    }
                    else {
                        // Fire is ok, nothing to do!! 
                    }
                }
            }
        }
    }

    bool TestIfSpargeDone() {
        DebugSerialPrintln(F("MASH->SPARGING!"));

        bzr_->HandleRingStopPospone(120000);
        bzr_->PlayWhenNeeded();

        if (btns_->IsPressed(SPARGE_DONE_BTN)) {
            lcd_message_value = START_BOIL;
            LcdDisplayConfigParams_();
            bzr_->ResetRing();
            mash_sparge_transition_ = false; // To end the mash/sparge transition phase
            DisplayLcd();
            LedBlink(MSG_LED_PIN);
            return true;
        }

        return false;
    }

    void HandleStep() {

        if (IsRamp() /*&& !IsSpargeStartedAndMashStopped*/) {

            // Ramp phase: check if the temperature is ok for the next step
            if (tempMashAverage > GetTargetTemp() - hysteresis || mash_in_ongoing_) {

                // Temperature is ok to start the phase, is to mash in?
                if (IsMashIn()) { // TODO add check for malts inertia? 
                    // Mash in   
                    mash_in_ongoing_ = true;   
                    DebugSerialPrintln(F("\nMASH IN: Put malts in the mash tun and press button 3° to start mash"));
                    lcd_message_value = MASH_IN;
                    LcdDisplayConfigParams_();

                    bzr_->HandleRingStopPospone(120000);
                    bzr_->PlayWhenNeeded();

                    if (btns_->IsPressed(MASH_IN_BTN)) {
                        StartStep();
                        MashInComplete();
                        lcd_message_value = SETTINGS;
                        LcdDisplayConfigParams_();
                        DisplayLcd();
                        bzr_->ResetRing();
                        mash_in_ongoing_ = false;
                        LedBlink(MSG_LED_PIN);
                    }
                }
                else {
                    // No mash in to do
                    StartStep();
                }
            }
        }

        if (!IsRamp()) {

            // Step phase 
            if (IsOver()) {

                // Step over
                if (IsToIodTest()) {
                    // Iod test
                    DebugSerialPrintln("\nIOD TEST: press button 4 to remove minutes; pres button 5 to add minutes;");

                    lcd_message_value = MASH_OUT;
                    LcdDisplayConfigParams_();

                    bzr_->HandleRingStopPospone(60000);
                    bzr_->PlayWhenNeeded(IsIodRing());

                    if (btns_->IsPressed(MASH_OUT_BTN)) {
                        StopIodTest(); // set is_to_iod_test_ to false for the current step
                        GotoNextPhase();
                        lcd_message_value = SETTINGS;
                        LcdDisplayConfigParams_();
                        bzr_->ResetRing();
                        LedBlink(MSG_LED_PIN);
                    }

                    // GotoNextPhase() may break the mash so, before handle button we have to check if we are in the mash yet
                    if (IsMash()) {
                        unsigned long last_press_add_sub = millis();
                        if (btns_->IsPressedInstantly(IOD_MINUS_MINUTES_BTN) || btns_->IsPressedInstantly(IOD_PLUS_MINUTES_BTN)) {
                            while (millis() - last_press_add_sub < 3000) {
                                LcdDisplayConfigParams_();

                                if (btns_->IsPressedInstantly(IOD_MINUS_MINUTES_BTN)) {
                                    SubIodMin();
                                    last_press_add_sub = millis();
                                    delay(350);
                                }

                                if (btns_->IsPressedInstantly(IOD_PLUS_MINUTES_BTN)) {
                                    AddIodMin();
                                    last_press_add_sub = millis();
                                    delay(350);
                                }
                            }
                        }
                    }
                }
                else {

                    GotoNextPhase();
                    //                        // No iod test
                    //                        if(!GotoNextPhase()){
                    //                            // No more step
                    //                            DebugSerialPrintln(F("MASH OVER, SPARGING IN PROGRESS!")); 
                    //      
                    //                            FireOff(); // Turn OFF fire
                    //                            lcd_message_value = SPARGE_COMPLETE;
                    //                            mash_sparge_transition_ = true;
                    //                            Complete();
                    //                            LcdDisplayConfigParams_();
                    //                            //LcdDisplayMashParams(); ??
                    //                        }         
                }
            }// IsOver if
        }// !IsRamp if
    }

    String mash_backup_file = "mash.txt";
    // ATTENTION: Deserialize function copied in more parts of the project
    void Deserialize() {
        char character;
        String setting_name;
        String setting_value;
    
        File backup = SD.open(mash_backup_file, FILE_READ);
    
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
            DebugSerialPrintln(mash_backup_file);
            backup.close(); // Is this necessary ??
    
        }
    }

    void ApplySetting(String setting_name, String setting_value) {
        if (setting_name == "cur_step_") {
            cur_step_ = String2Unsigned(setting_value);
            return;
        }
        
        if (setting_name == "is_mash_") {
            is_mash_ = String2Bool(setting_value);
            return;
        }

        if (setting_name == "mash_sparge_transition_") {
            mash_sparge_transition_ = String2Bool(setting_value);
            return;
        }
        
        if (setting_name == "curstep_isStarted_") {
            mash_[cur_step_].isStarted_ = String2Bool(setting_value);
            return;
        }

        if (setting_name == "curstep_elapsed_millis_") {
            if(mash_[cur_step_].isStarted_){
                mash_[cur_step_].startAt_ = millis() - String2Long(setting_value);
            }else{
                mash_[cur_step_].startAt_ = 0;   
            }
            DebugSerialPrint(F("step start at (millis): "));
            DebugSerialPrintln(String(mash_[cur_step_].startAt_));
            return;
        }

        if (setting_name == "curstep_is_to_iod_test_") {
            mash_[cur_step_].is_to_iod_test_ = String2Bool(setting_value);
            return;
        }

        if (setting_name == "curstep_extra_iod_minutes_") {
            mash_[cur_step_].extra_iod_minutes_ = String2Unsigned(setting_value);
            return;
        }

        if (setting_name == "curstep_extra_iod_elapsed_millis_") {
            if(IsToIodTest()){
                mash_[cur_step_].extra_iod_time_start_at_ = millis() - String2Long(setting_value);
            }
            DebugSerialPrint(F("step start at (millis): "));
            DebugSerialPrintln(String(mash_[cur_step_].extra_iod_time_start_at_));
            return;
        }

        if (setting_name == "mash_in_ongoing_") {
            mash_in_ongoing_ = String2Bool(setting_value);
            DebugSerialPrint(F("mash in on going: "));
            DebugSerialPrintln(String(mash_in_ongoing_));
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
        SD.remove(mash_backup_file); // Delete the old One
        File backup = SD.open(mash_backup_file, FILE_WRITE); // Create new one
        
        if (backup) {

            // Backup of mash class values
            WriteToSd(backup, "cur_step_", cur_step_); // It must be the first variable to be read because the other are linked to its value
            WriteToSd(backup, "is_mash_", is_mash_);
            WriteToSd(backup, "mash_sparge_transition_", mash_sparge_transition_);

            // Backup of current step values
            WriteToSd(backup, "curstep_isStarted_", mash_[cur_step_].isStarted_); // It must be read before curstep_elapsed_millis_ so it must be stored before it
            WriteToSd(backup, "curstep_elapsed_millis_", ElapsedMillis()); // We cannot store mash_[cur_step_].startAt_ variable because it depende on the execution 

            WriteToSd(backup, "curstep_is_to_iod_test_", mash_[cur_step_].is_to_iod_test_); // When the iod test "step" ends up the variable is set to false .. in case of backup 
                                                                                            // recovering its value must be restored. The variable must be restored before the 
                                                                                            // "curstep_extra_iod_minutes_" and "curstep_extra_iod_elapsed_millis_" ones.          
            WriteToSd(backup, "curstep_extra_iod_minutes_", mash_[cur_step_].extra_iod_minutes_);
            WriteToSd(backup, "curstep_extra_iod_elapsed_millis_", ElapsedIodMillis()); // We cannot store extra_iod_time_start_at_ variable because it depende on the execution

            WriteToSd(backup, "mash_in_ongoing_", mash_in_ongoing_);
            
            backup.close();
            DebugSerialPrint(mash_backup_file);
            DebugSerialPrintln(F(" writing done!"));
        }
        else {
            DebugSerialPrint(F("Error opening "));
            DebugSerialPrintln(mash_backup_file);
            backup.close(); // Is this necessary?
        }
    }
}MashClass;

#endif // MASH
