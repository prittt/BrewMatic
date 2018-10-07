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
#include <Window.h>
///Example window with yellow background
class Sparge : public Window, public ISensorHasDataEventReceiver, public ISensorMeasuredEventReceiver
{

  // VIEW
  TextBoxString *sparge_temp_str_;
  //TextBoxString *mash_down_temp_str_;
  //TextBoxString *mash_avg_temp_str_;

  ImageWindow *flames_img_;

  size_t tune_x_ = 50;
  size_t tune_y_ = 70;
  size_t tune_dim_ = 200;
  shape tune_shape_ = shape::VERTICAL;

  String buf_tmp_ = "ERROR!"; // needed to display temperature up 000.00   


  // CONTROLLER
  float tempSparge;
  
  bool are_temps_ok_;

  float targetSparge = 79;

  bool is_sparge_; // To control sparge water
    
  unsigned long spargeChangeStateTime;
  
public:
	Sparge(const __FlashStringHelper * name,int left,int top,int width,int height):Window(name,left,top,width,height)
	{
    // VIEW
    int big_font_width_ = 8;
    int big_font_height_ = 12;
    sparge_temp_str_ = new TextBoxString(tune_x_ + (tune_dim_ - big_font_width_*6) / 2 - 25/* just to fix small disalignment*/, tune_y_ + tune_dim_ - big_font_height_ - 30, 100, 20, buf_tmp_.c_str());
    initTextBox(sparge_temp_str_);

    flames_img_ = new ImageWindow(&flames25x25, tune_x_ + (tune_dim_ - 25) / 2 /*- 25 just to fix small disalignment*/, tune_y_ + tune_dim_ + 3, Color::Black);
    AddChild(flames_img_);
    
	  // CONTROLLER
    spargeChangeStateTime = millis() + FIRE_MAX_FREQUENCY;
    is_sparge_ = true;
    are_temps_ok_ = false;
	}

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
    //String sensor_name = sensorManager->AppName();
    MeasurementStatus ms = sensorManager->Status();
    float cur_data = sensorManager->GetData();
    
    if(ms == MeasurementStatus::MStOK){
      tempSparge = cur_data;
    }else{
      tempSparge = -1000;
    }
    buf_tmp_ = SetSystemTemp(cur_data, ms);
    sparge_temp_str_->SetText(buf_tmp_.c_str());

    SetTemperatures();
  }

  // Event is generated after each measurement
  void NotifySensorMeasured(SensorManager *sensorManager)
  {
    // This event is not generated in case of error
  }
  
  /***********************/
  /*        VIEW         */
  /***********************/
  
  void OnDraw(DC *dc)
  {
    dc->SetColor(Color::Black);
    dc->FillRoundRect(0, 0, Width(), Height());
    dc->SetColor(Color::White);
    DrawTune(dc);
  }
  
  /***********************/
  /*     CONTROLLER      */
  /***********************/

  // TODO, udate according to the ASensor protocol
  bool temperatureIsOk(float temp){
    if (temp > -100.0 && (int)(temp * 10)/10 != 85.00){
      return true;
    }
  
    return false; 
  }
  
  bool IsSparge() { return is_sparge_; }
  void Complete() { is_sparge_ = false;}
  
  void FireOn(void){
      // The sparge fire can be turned ON only if are passed more than "FIRE_MAX_FREQUENCY" millisecond after previuos state change.
      if(millis() - spargeChangeStateTime >= FIRE_MAX_FREQUENCY){
          digitalWrite(SPARGE_FIRE_PIN, ON);  // Sparge electrovalve ON 
          spargeChangeStateTime = millis();

          // To draw flames on screen
          if (!flames_img_->GetVisible()){
            //out << F("Sparge Fire ON\n");
            // We must change screen state only if it is strictly necessary to not overload the system
            flames_img_->SetVisible(true);
            flames_img_->Invalidate();
          }
      }
  }
  
  void FireOff(void){
      // The sparge fire can be always turned OFF
      digitalWrite(SPARGE_FIRE_PIN, OFF); // Sparge electrovalve OFF
      spargeChangeStateTime = millis();

      // To cancel flames on screen
      if(flames_img_->GetVisible()){
        //out << F("Sparge fire OFF\n");
        // We must change screen state only if it is strictly necessary to not overload the system
        flames_img_->SetVisible(false);
        flames_img_->Invalidate();
      }
  }

  void SetTemperatures(){        
      if(temperatureIsOk(tempSparge)){
          are_temps_ok_ = true;
      }else{
          are_temps_ok_ = false;
      }
  }

  void SetFire(){
     if(are_temps_ok_){
        if(tempSparge <= targetSparge - hysteresis){
            FireOn(); // Fire ON
        }else if(tempSparge >= targetSparge + hysteresis - inertia){
            FireOff();
        }
     }else{ // temperatureIsOk if
        FireOff(); // We cannot take decision 
     }  
    
  }
 
};
