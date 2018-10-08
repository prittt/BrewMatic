/*
  AWind.h - Arduino window library support for Color TFT LCD Boards
  Copyright (C)2014 Andrei Degtiarev. All right reserved
  
  You can find the latest version of the library at 
  https://github.com/AndreiDegtiarev/AWind

  This library is free software; you can redistribute it and/or
  modify it under the terms of the CC BY-NC-SA 3.0 license.
  Please see the included documents for further information.

  Commercial use of this library requires you to buy a license that
  will allow commercial use. This includes using the library,
  modified or not, as a tool to sell products.

  The license applies to all part of the library including the 
  examples and tools supplied with the library.
*/
#pragma once

//VIEW
#include "libs/AWind/Window.h"

#include "libs/AWind/Button.h"
#include "ImageButton.h"

#include "libs/AFrame/SensorManager.h"
#include "libs/AWind/TextBoxNumber.h"

#include "libs/AFrame/MeasurementStatus.h"

#include "ImageWindow.h"

//CONTROLLER
#include "utilities.h"
#include "PlantParams.h"
#include "Buttons.h"

//IMAGES
extern Image flames25x25;

enum shape{
    VERTICAL = 0,
    HORIZONTAL = 1
};

typedef struct mashStep {
    //String step_name_;  // Name of the step
    float temperature_; // Temperature in °C
    unsigned minutes_; // Minutes of the step
    bool is_to_iod_test_; // Whether it is the "iod test" step
    long startAt_; // Step start at time (milliseconds)
    bool isStarted_; // whether the step is started or not
    bool isMashIn_; // Whether is a "mash in" step or not
    long extra_iod_time_start_at_ = 0; // Start time of iod extra minutes
    unsigned extra_iod_minutes_ = 0; // Total added minutes (by the user) for the iod test

    mashStep(/*String step_name,*/ float temperature, unsigned minutes, bool is_to_iod_test = false, bool isMashIn = false) {
        //step_name_ = step_name;
        temperature_ = temperature;
        minutes_ = minutes;
        is_to_iod_test_ = is_to_iod_test;
        startAt_ = 0;
        isStarted_ = false;
        isMashIn_ = isMashIn;
    }
} mashStep;

// Mash Tab Window
class Mash : public Window, public ISensorHasDataEventReceiver, public ISensorMeasuredEventReceiver, public ITouchEventReceiver
{
public:

  // VIEW
  TextBoxString *mash_up_temp_str_;
  TextBoxString *mash_down_temp_str_;
  TextBoxString *mash_avg_temp_str_;
  //TextBoxString *flames_img_;

  ImageWindow *flames_img_;

  // Steps 
  #define N_MAX_STEPS 5 // 5 is the maximum number of step TODO change this to bepotentially infinite
  
  TextBoxString* step_state_[N_MAX_STEPS];      // > or ^ or x or v
  TextBoxString* step_fix_values_[N_MAX_STEPS]; // (78 °C): xxxmxxs/120m
  TextBoxString* step_min_to_end[N_MAX_STEPS];  // 120
  TextBoxString* step_sec_to_end[N_MAX_STEPS];  // 60

  Button *mash_button_;
  Button *mash_out_button_;
  
  String step_state_string_[N_MAX_STEPS];
  String step_fix_values_string_[N_MAX_STEPS]; 
  String step_min_to_end_string_[N_MAX_STEPS];
  String step_sec_to_end_string_[N_MAX_STEPS];
    
  size_t tune_x_ = 50;
  size_t tune_y_ = 70;
  size_t tune_dim_ = 200;
  shape tune_shape_ = shape::VERTICAL;

  String buf_up = "ERROR!"; // needed to display temperature up 000.00   
  String buf_down = "ERROR!"; // needed to display temperature down 000.00 
  String buf_avg = "ERROR!"; // needed to display temperature avg 000.00
  String buf_flames = "nofire"; // needed to display " fire "/"nofire" under the tune (this will be replaced by the flames image)

  bool flames_visible_;

  // CONTROLLER
  // Variable from previous implementation

  mashStep *mash_; // Vector of steps
  unsigned cur_step_;  // Number of the current step
  unsigned totalSteps_; // Total number of steps in the "mash" variable 
  bool is_mash_; // Wheter the mash is in progress or not

  bool mash_in_ongoing_; // This variable is useful to handle the following case: when is the time to 
                           // "mash in" if you put the malt in the moisture and the temperature goes down 
                           // the button "mash in" may disappear. If the mash_in_ongoing_ is true the button will be forced to appear.
  unsigned long mash_change_state_time_;

  float tempMashUp; 
  float tempMashDown;

  float tempMashAverage;

  bool are_temps_ok_;

  bool danger_delta_zone_; // This variable, when set to true, means that mash fire is turned OFF due to too high sensors delta!
  bool danger_temp_zone_; // If true means there are some errors in read temperature!

  bool mash_sparge_transition_; 
  
  Buttons *btns_;

  bool mash_in_ok_; // Usato per comunicare tra il touch dell'interfaccia e l'handle function del mash;
  bool mash_in_ok_button_visible_; // Usato per aggiornare il bottone mash_in una sola volta e non a ripetizione;

  bool mash_out_button_visible_; // Similar as mash_in_ok_button_visible_
    
  float water_weight_; // Useful to calculate inertia for mash in optimal temperature
  float malts_weight_; // Useful to calculate inertia for mash in optimal temperature
  
	Mash(const __FlashStringHelper * name,int left,int top,int width,int height):Window(name,left,top,width,height)
	{
    // VIEW
    int big_font_width_ = 8;
    int big_font_height_ = 12;
    mash_up_temp_str_ = new TextBoxString(tune_x_ + (tune_dim_ - big_font_width_*6) / 2 - 25/* just to fix small disalignment*/, tune_y_ + tune_dim_ - big_font_height_*2 - 60, 100, 20, buf_up.c_str());
    initTextBox(mash_up_temp_str_);
	  mash_down_temp_str_ = new TextBoxString(tune_x_ + (tune_dim_ - big_font_width_*6) / 2 - 25/* just to fix small disalignment*/, tune_y_ + tune_dim_ - big_font_height_ - 30, 100, 20, buf_down.c_str());
    initTextBox(mash_down_temp_str_);
    mash_avg_temp_str_ = new TextBoxString(tune_x_ + (tune_dim_ - big_font_width_*6) / 2 - 25/* just to fix small disalignment*/, tune_y_ - big_font_height_*2 - 10, 100, 20, buf_avg.c_str());
    initTextBox(mash_avg_temp_str_);

    flames_img_ = new ImageWindow(&flames25x25, tune_x_ + (tune_dim_ - 25) / 2 /*- 25 just to fix small disalignment*/, tune_y_ + tune_dim_ + 3, Color::Black);
    AddChild(flames_img_);

    mash_button_ = new Button(400, 200, 150, 20, F("MASH IN"));
    mash_button_->RegisterTouchEventReceiver(this);
    mash_button_->SetVisible(false);
    AddChild(mash_button_);

    mash_out_button_ = new Button(450, 200, 150, 20, F("MASH OUT")); 
    mash_out_button_->RegisterTouchEventReceiver(this);
    mash_out_button_->SetVisible(false);
    AddChild(mash_out_button_);

    mash_in_ok_button_visible_ = false;
    mash_out_button_visible_ = false;
    //flames_img_ = new TextBoxString(tune_x_ + (tune_dim_ - big_font_width_*6) / 2 - 25 /*just to fix small disalignment*/, tune_y_ + tune_dim_ + 3, 200, 25, buf_flames.c_str());
    //initTextBox(flames_img_);
    //flames_visible_ = false;

	  // CONTROLLER
	  // It is initialized by calling the function InitMashController()
	}

  /***********************/
  /*        VIEW         */
  /***********************/
  // Initialize text box windows with "Sensor" decorator -> black background and white text with small font
  void initTextBox(TextBox *textBox)
  {
    textBox->SetMargins(0,0);
    textBox->SetDecorators(Environment::Get()->FindDecorators(F("Sensor")));
    textBox->SetFont(F("Big"));
    AddChild(textBox);
  }

  void DrawTune(DC *dc){
    size_t x = tune_x_;
    size_t y = tune_y_;
    size_t dim = tune_dim_;
    shape s = tune_shape_;
     
    x = dc->ToDC_X(x); // mi riconduco alla finestra corrente
    y = dc->ToDC_Y(y); // mi riconduco alla finestra corrente
    dim = dim % 2 != 0 ? dim - 1 : dim; // "dim" must be even  
    size_t left_right = dim / 6;
    size_t handle = dim / 5;

    // Main
    dc->drawLine(x, y, x + dim, y); // cover line 
    dc->drawLine(x + left_right, y, x + left_right, y + dim - 2); // left line
    dc->drawLine(x + dim - left_right, y, x + dim - left_right, y + dim - 2); // righ line
    dc->drawPixel(x + left_right + 1, y + dim - 1); // left corner
    dc->drawPixel(x + dim - left_right - 1, y + dim - 1); // right corner
    dc->drawLine(x + left_right + 2, y + dim, x + dim - left_right - 2, y + dim); // bottom line

    // Handle
    dc->drawLine(x + handle * 2, y - 5, x + dim - handle * 2, y - 5); // top line
    dc->drawPixel(x + handle * 2 - 1, y - 4); // left corner
    dc->drawPixel(x + dim - handle * 2 + 1, y - 4); // right corner
    dc->drawLine(x + handle * 2 - 2, y - 3, x + handle * 2 - 2, y); // left line
    dc->drawLine(x + dim - handle * 2 + 2, y - 3, x + dim - handle * 2 + 2, y); // right line
  }

  void OnDraw(DC *dc)
  {
    dc->SetColor(Color::Black);
    dc->FillRoundRect(0, 0, Width(), Height());
    dc->SetColor(Color::White);
    DrawTune(dc);
    //LinkedList<Window> children = Children();
    //for(int i=0; i<children.Count(); ++i){
    //  children[i]->Redraw(dc);
    //}
  }

   String SetSystemTemp(float val, MeasurementStatus ms){
    String str_val = "";
    if (ms == MeasurementStatus::MStOK){ 
      if (val < 100.0){
        if(val < 10.0){
          str_val += "  ";
        }else{
          str_val += " ";  
        }
      }
      str_val += String(val) + " ";
    }else{
      str_val = "ERROR!";
    }
    //str_val.toCharArray(buf, 7);
    //buf[str_val.length()] = '\0'; 
    //out << "SetSystemTemp: " << str_val << "\n";
    return str_val;
  }

  // Event sensor has new data. If data is the same as measured previosly this event is not generated.
  void NotifySensorHasData(SensorManager *sensorManager)
  {
    // This event is not generated in case of error --> force to be generated modifying AFrame-SensorManager code
    String sensor_name = sensorManager->AppName();
    MeasurementStatus ms = sensorManager->Status();
    float cur_data = sensorManager->GetData();
    
    if (sensor_name == "DOWN"){
      if(ms == MeasurementStatus::MStOK){
        tempMashDown = cur_data;
      }else{
        tempMashDown = -1000;
      }
      //SetSystemTemp(cur_data, buf_down, ms);
      buf_down = SetSystemTemp(cur_data, ms);
      mash_down_temp_str_->SetText(buf_down.c_str());
      //mash_down_temp_str_->Invalidate();
    }else if (sensor_name == "UP"){
       if(ms == MeasurementStatus::MStOK){
        tempMashUp = cur_data;
      }else{
        tempMashUp = -1000;
      }
      //SetSystemTemp(cur_data, buf_up, ms);
      buf_up = SetSystemTemp(cur_data, ms);
      mash_up_temp_str_->SetText(buf_up.c_str());
      //mash_up_temp_str_->Invalidate();
    }

    CalculateAverage();
  }
  
  // Event is generated after each measurement
  void NotifySensorMeasured(SensorManager *sensorManager)
  {
    // This event is not generated in case of error
  }

  void NotifyTouch(Window *window){
    //out << "Mash Notified";
    if(window == mash_button_){
      //out << "Mash In toccato\n";
      mash_button_->AddDecorator(new DecoratorRectFill(Color::Black, false));
      mash_button_->SetVisible(false);
      mash_button_->Invalidate();
      mash_in_ok_ = true;
    }

    if(window == mash_out_button_){
      mash_[cur_step_].is_to_iod_test_ = false;
      mash_out_button_->AddDecorator(new DecoratorRectFill(Color::Black, false));
      mash_out_button_->SetVisible(false);
      mash_out_button_->Invalidate();
    }
  }

  /***********************/
  /*     CONTROLLER      */
  /***********************/
  InitializeMashController(mashStep *mash, unsigned totalSteps, float water_weight, float malts_weight /*, OneWire* up_ds, OneWire* down_ds, OneWire* env_ds, UTFT *lcd, Buttons* btns*/) {
    cur_step_ = 0;
    totalSteps_ = totalSteps;
    mash_ = mash;
    is_mash_ = true;
    mash_change_state_time_ = millis() + FIRE_MAX_FREQUENCY;

    tempMashDown = -1000;
    tempMashUp = -1000;
    tempMashAverage = -1000;
    
    //up_ds_ = up_ds;
    //down_ds_ = down_ds;
    //env_ds_ = env_ds;
    
    are_temps_ok_ = false;
    mash_sparge_transition_ = false;
  
    water_weight_ = water_weight;
    malts_weight_ = malts_weight;
  
    mash_in_ongoing_= false;
  
    //touch_ = touch;
    //lcd_ = lcd;
  
    //btns_ = new Buttons();

    mash_in_ok_ = false;

    // STEPS VIEW
    for(int i = 0; i<totalSteps_; ++i){
              
        if(i < cur_step_){
          step_state_string_[i] = "v";
        }else if(i == cur_step_){
          step_state_string_[i] = "^";
        }else if(i > cur_step_){
          step_state_string_[i] = "x";    
        }
                
        // > or ^ or x or v
        int base_x = 320;
        step_state_[i] = new TextBoxString(base_x, 50 + i*25, 15, 20, step_state_string_[i].c_str());
        initTextBox(step_state_[i]);
        
        unsigned minutes = mash_[i].minutes_;
        String totalMinutesString;
        if (minutes > 99) { totalMinutesString = String(minutes); }
        else if (minutes > 9) { totalMinutesString = "0" + String(minutes); }
        else { totalMinutesString = "00" + String(minutes); }

        // (78 °C): xxxmxxs/120m
        step_fix_values_string_[i] = "(" + String(mash_[i].temperature_, 0) + "C):   m  s/" + totalMinutesString + "m";
        step_fix_values_[i] = new TextBoxString(base_x + 15, 50 + i*25, 200, 20, step_fix_values_string_[i].c_str());
        initTextBox(step_fix_values_[i]);

        // minutes to end 120
        step_min_to_end_string_[i] = totalMinutesString;
        step_min_to_end[i] = new TextBoxString(base_x + 15 + 99, 50 + i*25, 50, 20, step_min_to_end_string_[i].c_str());
        initTextBox(step_min_to_end[i]);

        // second to end 60
        step_sec_to_end_string_[i] = "00";
        step_sec_to_end[i] = new TextBoxString(base_x + 15 + 95 + 68, 50 + i*25, 30, 20, step_sec_to_end_string_[i].c_str());
        initTextBox(step_sec_to_end[i]);
    }
  }

  void UpdateScreenStepData(){

      for(int i = 0; i< totalSteps_; ++i){
        if (cur_step_ == i){

          if(mash_[cur_step_].isStarted_){

            // Se lo step è iniziato e lo stato non è ancora stato aggiornato lo aggiorno
            if (step_state_string_[i] != ">"){
              step_state_string_[i] = ">";
              step_state_[i]->SetText(step_state_string_[i].c_str());
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
            
            if (minutesToEndString != step_min_to_end_string_[i]){
              // Il valore della stringa minutes to end è cambiato quindi lo aggiorno sullo schermo
              step_min_to_end_string_[i] = minutesToEndString;
              step_min_to_end[i]->SetText(step_min_to_end_string_[i].c_str());
            }
  
            if (secondsToEndString != step_sec_to_end_string_[i]){
              // Il valore della stringa seconds to end è cambiato quindi lo aggiorno sullo schermo
              step_sec_to_end_string_[i] = secondsToEndString;
              step_sec_to_end[i]->SetText(step_sec_to_end_string_[i].c_str());
            } 
          }
        }
      }
  }

  // TODO, udate according to the ASensor protocol
  bool temperatureIsOk(float temp){
    if (temp > -100.0 && (int)(temp * 10)/10 != 85.00){
      return true;
    }
  
    return false; 
  }
  void CalculateAverage() {
    if (temperatureIsOk(tempMashUp) && temperatureIsOk(tempMashDown)) {
        tempMashAverage = (tempMashUp + tempMashDown) / 2.0; // Average temperature
        are_temps_ok_ = true;
        //SetSystemTemp(tempMashAverage, buf_avg, MeasurementStatus::MStOK);
        buf_avg = SetSystemTemp(tempMashAverage, MeasurementStatus::MStOK);
        mash_avg_temp_str_->SetText(buf_avg.c_str());
        //mash_avg_temp_str_->Invalidate();
    }
    else {
        tempMashAverage = -1000; // Error case
        are_temps_ok_ = false;
        //SetSystemTemp(tempMashAverage, buf_avg, MeasurementStatus::Error);
        buf_avg = SetSystemTemp(tempMashAverage, MeasurementStatus::Error);
        mash_avg_temp_str_->SetText(buf_avg.c_str());
        //mash_avg_temp_str_->Invalidate();
    }
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
    // VIEW
    out << "goto next phase \n";
    // Update state string
    step_state_string_[cur_step_] = "v";
    step_state_[cur_step_]->SetText(step_state_string_[cur_step_].c_str());

    // Force second to end string to be "00"
    step_sec_to_end_string_[cur_step_] = "00";
    step_sec_to_end[cur_step_]->SetText(step_sec_to_end_string_[cur_step_].c_str());
    
    cur_step_++;
    if (cur_step_ >= totalSteps_) {       
        //return false; //No more phases;
        out << F("MASH OVER, SPARGING IN PROGRESS!\n");
  
        FireOff(); // Turn OFF fire
        lcd_message_value = SPARGE_COMPLETE;
        mash_sparge_transition_ = true;
        Complete();        
    }else{
      // Update state string
      step_state_string_[cur_step_] = "^";
      step_state_[cur_step_]->SetText(step_state_string_[cur_step_].c_str());  
    }
  }
  
  float GetMashInTemp(float target_temp){
  
    // Get temperature from the sensor if available, set to 20°C otherwise
    float ti = 20;//getTemp(env_ds_);
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
          mash_change_state_time_ = millis();

          // To draw flames on screen
          if (!flames_img_->GetVisible()){
            //out << F("Fire ON\n");
            // We must change screen state only if it is strictly necessary to not overload the system
            //buf_flames = " fire ";
            //flames_img_->SetText(buf_flames.c_str());
            flames_img_->SetVisible(true);
            flames_img_->Invalidate();
            //flames_visible_ = true;
          }
      }
  }

  void FireOff(void) {
      // The mash fire can be always turned OFF
      digitalWrite(MASH_FIRE_PIN, OFF);  // Mash electrovalve OFF 
      mash_change_state_time_ = millis();

      // To cancel flames on screen
      if(flames_img_->GetVisible()){
        //out << F("Fire OFF\n");
        // We must change screen state only if it is strictly necessary to not overload the system
        //buf_flames = "nofire";
        //flames_img_->SetText(buf_flames.c_str());
        flames_img_->SetVisible(false);
        flames_img_->Invalidate();
        //flames_visible_ = false;
      }
  }

//  void SetTemperatures() {
//      //tempMashUp = getTemp(up_ds_);     // Get temperature up
//      //tempMashDown = getTemp(down_ds_); // Get temperature down 
//      
//      
//      //tared_temp_up_ = tempMashUp + tareMashUp;       // To tare temperature value
//      //tared_temp_down_ = tempMashDown + tareMashDown; // To tare temperature value
//
//      if (temperatureIsOk(tempMashUp) && temperatureIsOk(tempMashDown)) {
//          tempMashAverage = (tared_temp_up_ + tared_temp_down_) / 2.0; // Average temperature
//          are_temps_ok_ = true;
//      }
//      else {
//          tempMashAverage = -1000; // Error case
//          are_temps_ok_ = false;
//      }
//  }

  // Check temperatures and set fire off in case of errors
  void CheckTemperatures() {

      danger_temp_zone_ = false;
      danger_delta_zone_ = false;

      if (are_temps_ok_) {
          if (abs(tempMashUp - tempMashDown) > maxDelta && !mash_[cur_step_].isStarted_) {
              FireOff(); // Turn OFF the mash fire
              danger_delta_zone_ = true; // To set danger zone
              out << F("DANGER DELTA ZONE: mash delta is higher than") << String(maxDelta) << F("°C ...\n");
          }
      }
      else {
          // Temperatures are wrong! 
          FireOff(); // Turn off the fire because we cannot take the correct decision (waiting for next loop)
          danger_temp_zone_ = true;  // To set danger zone 
          out << F("DANGER TEMPERATURE VALUE: one or both sensors return 'ERROR' values ...\n");
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

  // TODO, ths function has been commented in version ALPHA00
  bool TestIfSpargeDone() {
      out << F("MASH->SPARGING!\n");

      // A00 //bzr_->HandleRingStopPospone(120000);
      // A00 //bzr_->PlayWhenNeeded();

      // A00 //
//        if (btns_->IsPressed(SPARGE_DONE_BTN)) {
//            lcd_message_value = START_BOIL;
//            // A00 //LcdDisplayConfigParams_();
//            // A00 //bzr_->ResetRing();
//            mash_sparge_transition_ = false; // To end the mash/sparge transition phase
//            DisplayLcd();
//            // A00 //LedBlink(MSG_LED_PIN);
//            return true;
//        }

      return false;
  }

  void HandleStep() {

      if (IsRamp() /*&& !IsSpargeStartedAndMashStopped*/) {
          // Ramp phase: check if the temperature is ok for the next step
          if (tempMashAverage > GetTargetTemp() - hysteresis || mash_in_ongoing_) {

              // Temperature is ok to start the phase, is to mash in?
              if (IsMashIn()) { 
                
                  if(!mash_in_ok_button_visible_){
                    mash_button_->SetVisible(true);
                    mash_button_->Invalidate();
                    mash_in_ok_button_visible_ = true;
                  }
                  
                  // Mash in   
                  mash_in_ongoing_ = true;   
                  out << F("\nMASH IN: Put malts in the mash tun and press button on top-right of the structure to start mash\n"); // TODO change and add touch button
                  lcd_message_value = MASH_IN;

                  // A00 //bzr_->HandleRingStopPospone(120000);
                  // A00 //bzr_->PlayWhenNeeded();

                  if (mash_in_ok_) {
                      StartStep();
                      MashInComplete();
                      mash_in_ongoing_ = false;
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
                  //out << F("\nIOD TEST: press button 4 to remove minutes; pres button 5 to add minutes;\n");

                  if(!mash_out_button_visible_){
                    mash_out_button_->SetVisible(true);
                    mash_out_button_->Invalidate();
                    mash_out_button_visible_ = true;
                  }
          
                  // A00 // lcd_message_value = MASH_OUT;
                  // A00 // LcdDisplayConfigParams_();

                  // A00 // bzr_->HandleRingStopPospone(60000);
                  // A00 // bzr_->PlayWhenNeeded(IsIodRing());

                  // A00 //
//                    if (btns_->IsPressed(MASH_OUT_BTN)) {
//                        StopIodTest(); // set is_to_iod_test_ to false for the current step
//                        GotoNextPhase();
//                        lcd_message_value = SETTINGS;
//                        // A00 //LcdDisplayConfigParams_();
//                        // A00 //bzr_->ResetRing();
//                        // A00 // LedBlink(MSG_LED_PIN);
//                    }

                  // GotoNextPhase() may break the mash so, before handle button we have to check if we are in the mash yet
                  if (IsMash()) {
                      unsigned long last_press_add_sub = millis();
                      // A00 //
//                        if (btns_->IsPressedInstantly(IOD_MINUS_MINUTES_BTN) || btns_->IsPressedInstantly(IOD_PLUS_MINUTES_BTN)) {
//                            while (millis() - last_press_add_sub < 3000) {
//                                // A00 // LcdDisplayConfigParams_();
//
//                                if (btns_->IsPressedInstantly(IOD_MINUS_MINUTES_BTN)) {
//                                    SubIodMin();
//                                    last_press_add_sub = millis();
//                                    delay(350);
//                                }
//
//                                if (btns_->IsPressedInstantly(IOD_PLUS_MINUTES_BTN)) {
//                                    AddIodMin();
//                                    last_press_add_sub = millis();
//                                    delay(350);
//                                }
//                            }
//                        }
                  }
              }
              else {

                  GotoNextPhase();
              }
          }// IsOver if
      }// !IsRamp if
  }
 
};

//#include <OneWire.h>

//#include <SPI.h>
//#include <SD_adafruit.h>

//#include <UTFT.h>
//#include <UTouch.h>

//#include "MegaPins.h"
//#include "temp.h"
//#include "Buttons.h"

//#define MASH_IN_BTN 1

//typedef struct MashClass {
//    mashStep *mash_; // Vector of steps
//    unsigned cur_step_;  // Number of the current step
//    unsigned totalSteps_; // Total number of steps in the "mash" variable 
//    bool is_mash_; // Wheter the mash is in progress or not
//
//    bool mash_in_ongoing_; // This variable is useful to handle the following case: when is the time to 
//                           // "mash in" if you put the malt in the moisture and the temperature goes down 
//                           // the button "mash in" may disappear. If the mash_in_ongoing_ is true the button will be forced to appear.
//    unsigned long mash_change_state_time_;
//
//    float tempMashUp;
//    float tareMashUp = 0;
//    float tared_temp_up_;
//
//    float tempMashDown;
//    float tareMashDown = 0;
//    float tared_temp_down_;
//
//    float tempMashAverage;
//
//    bool are_temps_ok_;
//
//    OneWire *up_ds_;
//    OneWire *down_ds_;
//    OneWire *env_ds_;
//
//    bool danger_delta_zone_; // This variable, when set to true, means that mash fire is turned OFF due to too high sensors delta!
//    bool danger_temp_zone_; // If true means there are some errors in read temperature!
//
//    bool mash_sparge_transition_;
//
//    UTouch *touch_;
//    UTFT *lcd_;
//    Buttons *btns_;
//    
//    float water_weight_; // Useful to calculate inertia for mash in optimal temperature
//    float malts_weight_; // Useful to calculate inertia for mash in optimal temperature
//
//    // Constructor without screen, buttons and buzzer
//
//
//    void PrintCurrentStateString() {
//        // Specific for 20 characters lcd
//        String msg;
//        String beginc; 
//        if (mash_[cur_step_].isStarted_) {
//            beginc = ">";
//        }
//        else {
//            beginc = "^";
//        }
//
//        unsigned secondsToEnd = secToEnd();
//        unsigned minutesToEnd = secondsToEnd / 60;
//        secondsToEnd = secondsToEnd % 60;
//
//        String minutesToEndString;
//        if (minutesToEnd > 99) { minutesToEndString = String(minutesToEnd); }
//        else if (minutesToEnd > 9) { minutesToEndString = "0" + String(minutesToEnd); }
//        else { minutesToEndString = "00" + String(minutesToEnd); }
//
//        String secondsToEndString;
//        if (secondsToEnd > 9) { secondsToEndString = String(secondsToEnd); }
//        else { secondsToEndString = "0" + String(secondsToEnd); }
//
//        unsigned minutes = mash_[cur_step_].minutes_;
//        String totalMinutesString;
//        if (minutes > 99) { totalMinutesString = String(minutes); }
//        else if (minutes > 9) { totalMinutesString = "0" + String(minutes); }
//        else { totalMinutesString = "00" + String(minutes); }
//
//        msg = "(" + String(mash_[cur_step_].temperature_, 0) + " C)-";
//        if (mash_[cur_step_].isStarted_) {
//            msg += minutesToEndString + "m" + secondsToEndString + "s/" + totalMinutesString + "m";
//        }
//        else {
//            msg += totalMinutesString + "m00s/" + totalMinutesString + "m";
//        }
//        lcd_->print(beginc + " " + msg, 0, 40 + cur_step_*20);
//    }
//
//    void PrintStateString(int cnt) {
//        // Specific for 20 characters lcd
//        String msg = "                    ";
//        
//        unsigned minutes = mash_[cnt].minutes_;
//        String totalMinutesString;
//        if (minutes > 99) { totalMinutesString = String(minutes); }
//        else if (minutes > 9) { totalMinutesString = "0" + String(minutes); }
//        else { totalMinutesString = "00" + String(minutes); }
//
//        msg = "- (" + String(mash_[cnt].temperature_, 0) + " C)-" + totalMinutesString + "m00s/" + totalMinutesString + "m";
//        
//        lcd_->print(msg, 0, 40 + cnt*20);
//    }
//
////    void PrintCurrentIodString(LiquidCrystal lcd, unsigned minutes_added) {
////
////        // Specific for first line of 16 characters lcd        
////        unsigned seconds_over_end = secOverEnd();
////        unsigned minutes_over_end = seconds_over_end / 60;
////        seconds_over_end -= minutes_over_end * 60;
////
////        String minutes_over_end_string;
////        if (minutes_over_end > 9) { minutes_over_end_string = String(minutes_over_end); }
////        else { minutes_over_end_string = "0" + String(minutes_over_end); }
////
////        String seconds_over_end_string;
////        if (seconds_over_end > 9) { seconds_over_end_string = String(seconds_over_end); }
////        else { seconds_over_end_string = "0" + String(seconds_over_end); }
////
////        String minutes_added_string;
////        if (minutes_added > 9) { minutes_added_string = String(minutes_added); }
////        else { minutes_added_string = "0" + String(minutes_added); }
////
////        // Message to display: "   00m00s/00m   "
////        String msg = "   " + minutes_over_end_string + "m" + seconds_over_end_string + "s/" + minutes_added_string + "m   ";
////
////        lcd.setCursor(0, 0);
////        lcd.print(msg);
////    }
//
//
//    const char mash_backup_file[9] = {'m', 'a', 's', 'h', '.', 't', 'x', 't', '\0'};
//    // ATTENTION: Deserialize function copied in more parts of the project
//    void Deserialize() {
//        char character;
//        String setting_name;
//        String setting_value;
//    
//        File backup = SD.open(mash_backup_file, FILE_READ);
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
//            DebugSerialPrintln(mash_backup_file);
//            backup.close(); // Is this necessary ??
//    
//        }
//    }
//
//    void ApplySetting(String setting_name, String setting_value) {
//        if (setting_name == "cur_step_") {
//            cur_step_ = String2Unsigned(setting_value);
//            return;
//        }
//        
//        if (setting_name == "is_mash_") {
//            is_mash_ = String2Bool(setting_value);
//            return;
//        }
//
//        if (setting_name == "mash_sparge_transition_") {
//            mash_sparge_transition_ = String2Bool(setting_value);
//            return;
//        }
//        
//        if (setting_name == "curstep_isStarted_") {
//            mash_[cur_step_].isStarted_ = String2Bool(setting_value);
//            return;
//        }
//
//        if (setting_name == "curstep_elapsed_millis_") {
//            if(mash_[cur_step_].isStarted_){
//                mash_[cur_step_].startAt_ = millis() - String2Long(setting_value);
//            }else{
//                mash_[cur_step_].startAt_ = 0;   
//            }
//            DebugSerialPrint(F("step start at (millis): "));
//            DebugSerialPrintln(String(mash_[cur_step_].startAt_));
//            return;
//        }
//
//        if (setting_name == "curstep_is_to_iod_test_") {
//            mash_[cur_step_].is_to_iod_test_ = String2Bool(setting_value);
//            return;
//        }
//
//        if (setting_name == "curstep_extra_iod_minutes_") {
//            mash_[cur_step_].extra_iod_minutes_ = String2Unsigned(setting_value);
//            return;
//        }
//
//        if (setting_name == "curstep_extra_iod_elapsed_millis_") {
//            if(IsToIodTest()){
//                mash_[cur_step_].extra_iod_time_start_at_ = millis() - String2Long(setting_value);
//            }
//            DebugSerialPrint(F("step start at (millis): "));
//            DebugSerialPrintln(String(mash_[cur_step_].extra_iod_time_start_at_));
//            return;
//        }
//
//        if (setting_name == "mash_in_ongoing_") {
//            mash_in_ongoing_ = String2Bool(setting_value);
//            DebugSerialPrint(F("mash in on going: "));
//            DebugSerialPrintln(String(mash_in_ongoing_));
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
//        SD.remove(mash_backup_file); // Delete the old One
//        File backup = SD.open(mash_backup_file, FILE_WRITE); // Create new one
//        
//        if (backup) {
//
//            // Backup of mash class values
//            WriteToSd(backup, "cur_step_", cur_step_); // It must be the first variable to be read because the other are linked to its value
//            WriteToSd(backup, "is_mash_", is_mash_);
//            WriteToSd(backup, "mash_sparge_transition_", mash_sparge_transition_);
//
//            // Backup of current step values
//            WriteToSd(backup, "curstep_isStarted_", mash_[cur_step_].isStarted_); // It must be read before curstep_elapsed_millis_ so it must be stored before it
//            WriteToSd(backup, "curstep_elapsed_millis_", ElapsedMillis()); // We cannot store mash_[cur_step_].startAt_ variable because it depende on the execution 
//
//            WriteToSd(backup, "curstep_is_to_iod_test_", mash_[cur_step_].is_to_iod_test_); // When the iod test "step" ends up the variable is set to false .. in case of backup 
//                                                                                            // recovering its value must be restored. The variable must be restored before the 
//                                                                                            // "curstep_extra_iod_minutes_" and "curstep_extra_iod_elapsed_millis_" ones.          
//            WriteToSd(backup, "curstep_extra_iod_minutes_", mash_[cur_step_].extra_iod_minutes_);
//            WriteToSd(backup, "curstep_extra_iod_elapsed_millis_", ElapsedIodMillis()); // We cannot store extra_iod_time_start_at_ variable because it depende on the execution
//
//            WriteToSd(backup, "mash_in_ongoing_", mash_in_ongoing_);
//            
//            backup.close();
//            DebugSerialPrint(mash_backup_file);
//            DebugSerialPrintln(F(" writing done!"));
//        }
//        else {
//            DebugSerialPrint(F("Error opening "));
//            DebugSerialPrintln(mash_backup_file);
//            backup.close(); // Is this necessary?
//        }
//    }
//
//// Spostata qui per la versione A00 prima era nel main e la classe usava un riferimento    
//void LcdDisplayConfigParams_(){
//
//  switch(lcd_message_value){
//    case BASE:
//      //lcd_->remove(lcd_message_value);
//    case MASH_IN:
////      lcd_->setColor(0, 0, 255);
////      lcd_->fillRoundRect (0, 150, 80, 170);
////      lcd_->setColor(255, 255, 255);
////      lcd_->drawRoundRect (0, 150, 80, 170);
//      lcd_->print("MASH IN", 5, 155);
//      break;
////    case MASH_OUT:
////      mash.PrintCurrentIodString(lcdMsg, mash.GetIodExtraMinutes());
////    
////      lcdMsg.setCursor(0, 1);  
////      lcdMsg.print(" MOUT  SUB  ADD ");
////      break;
////    case SPARGE_COMPLETE:
////      lcdMsg.setCursor(0,0);
////      lcdMsg.print("SPARGE          ");
////
////      lcdMsg.setCursor(0,1);
////      lcdMsg.print(" DONE           ");
////      break;
////    case START_BOIL:
////      lcdMsg.setCursor(0,0);
////      lcdMsg.print("  GO            ");
////
////      lcdMsg.setCursor(0,1);
////      lcdMsg.print(" BOIL           ");
////      break;
////     case THROW_HOP:
////      lcdMsg.setCursor(0,0);
////      lcdMsg.print(" NEW            ");
////
////      lcdMsg.setCursor(0,1);
////      lcdMsg.print(" HOP            ");
////      break;
//  }
//}
//    
//}MashClass;

