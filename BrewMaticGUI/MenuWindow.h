/*
This file is part of AWind library

Copyright (c) 2014-2018 Andrei Degtiarev

Licensed under the Apache License, Version 2.0 (the "License"); you
may not use this file except in compliance with the License.  You may
obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied.  See the License for the specific language governing
permissions and limitations under the License.
*/
#pragma once
#include "MainWindow.h"
#include "Button.h"

#include "ImageButton.h"

#include "SensorManager.h"
#include "TextBoxNumber.h"

extern Image home25x25;
extern Image home30x30;

// This is the menu window which controls menu and home buttons through the ITouchEventReceiver, and internal temperature sensor through ISensorHasDataEventReceiver and ISensorMeasuredEventReceiver.
class MenuWindow : public MainWindow, public ITouchEventReceiver,public ISensorHasDataEventReceiver, public ISensorMeasuredEventReceiver
{
	LinkedList<ImageButton> _listButtons; //list of buttons on the left scrren side
	LinkedList<Window> _listWindow;       //list of depended windows

  uint8_t _nbuttons;
  uint8_t _margins;
  unsigned _szx; 
  unsigned _szy;
  TextBoxString *temp_str_;
  TextBoxString *system_str_;
  
  ImageButton *home_btn;
  
public:
	MenuWindow(int wnd_width,int wnd_height):MainWindow(wnd_width,wnd_height)
	{
    // Init home button
    home_btn = new ImageButton(F("home"), &home30x30, 10, 10, 30, 30, false, false); //TODO change on click decorator
    home_btn->RegisterTouchEventReceiver(this);
    AddChild(home_btn);
    home_btn->SetVisible(false);
	
	  // Init internal temperature sensor TextBoxes
    system_str_ = new TextBoxString(650,450,140,22, "system:       $C"); 
    initTextBox(system_str_);
    temp_str_ = new TextBoxString(710,450,40,22, "");
    initTextBox(temp_str_);
	}

  // Initialize text box windows with "Sensor" decorator -> black background and white text with small font
  void initTextBox(TextBox *textBox)
  {
    textBox->SetMargins(0,0);
    textBox->SetDecorators(Environment::Get()->FindDecorators(F("Sensor")));
    textBox->SetFont(F("Small"));
    AddChild(textBox);
  }
 
	//Adds button and the corresponding window
	void AddMenuButton(const __FlashStringHelper *button_name /*name of the button and of the corresponding window*/, Window *win, Image *img = NULL)
	{
		int wnd_width=Width(); // Window width
		int wnd_height=Height(); // Window height
    int y = wnd_height / 2 - _szy; // Y value (fixed for each line)
    int x = (wnd_width - _szx*_nbuttons -_margins*(_nbuttons - 1))/2; // X starting value
    //out << y << endln;
    //out << x << endln;
		ImageButton *button=new ImageButton(button_name, img, x + (_listButtons.Count())*(_szx + _margins), y, _szx, _szy);
		button->RegisterTouchEventReceiver(this);
		AddChild(button);
    _listButtons.Add(button);
    
    AddChild(win);
    _listWindow.Add(win);
    win->SetVisible(false);
   
		//_listButtons[0]->SetDecorators(_listWindow[0]->GetDecorators());
	}
 
	void Initialize()
	{
		AddDecorator(new DecoratorRectFill(Color::Black,false));
		AddDecorator(new DecoratorColor(Color::Black));
	}
 
  // Must be called before "AddMenuButton": it defines the positioning of menu buttons
  void SetParams(uint8_t nbuttons, uint8_t margins, unsigned szx, unsigned szy){
    _nbuttons = nbuttons;
    _margins = margins;  
    _szx = szx; 
    _szy = szy;
  }
  
	//Events routing for gui interaction (see RegisterTouchEventReceiver and public ITouchEventReceiver declaration)
	void NotifyTouch(Window *window)
	{
      out<<F("NotifyTouch")<<endln;

      int sel_index=-1;
      for(int i=0;i<_listButtons.Count();i++)
      {
        if(window == _listButtons[i])
        {
          sel_index=i;
          break;
        }
      }

      if(window == home_btn){

        // HOME
        //out<<F("Home")<<endln;  

        // Disable home button 
        home_btn->SetVisible(false);
        // Restore menu view (set all buttons visible)
        for(int i = 0; i<_nbuttons; ++i){
          _listButtons[i]->SetVisible(true);  
        }
      } // HOME
      else{

        // SUBWINDOW        
        //out<<F("Menu")<<endln;

        // Enable home button 
        home_btn->SetVisible(true);
        // Disable all menu buttons except the home one
        for(int i = 0; i<_nbuttons; ++i){
          _listButtons[i]->SetVisible(false);  
        }

        // Disable all windows but the selected one
        if(sel_index >=0)
        {
          //DecoratorList *buttonDeco=Environment::Get()->FindDecorators(F("Button"));
          for(int i=0;i<_listButtons.Count();i++)
          {
            _listWindow[i]->SetVisible(i==sel_index);
            if(i==sel_index)
            {
              _listWindow[i]->Invalidate();
              //_listButtons[i]->SetDecorators(_listWindow[i]->GetDecorators());
            }
            else
            {
              //if(_listButtons[i]->GetDecorators()!=buttonDeco)
              //{
              //  _listButtons[i]->SetDecorators(buttonDeco);
              //  _listButtons[i]->Invalidate();
              //}
            }
          }
        } 
        //out<<F("Window selected: ")<<sel_index<<endl;
      } // SUBWINDOWS
      // Force windows rendering
      this->Invalidate();
	}

  void SetSystemTemp(float val){
    String str_val = "";
    if (val < 100.0){
      if(val < 10.0){
        str_val += "  ";
      }else{
        str_val += " ";  
      }
    }
    str_val += String(val) + " ";
    char *buf = malloc(str_val.length() + 1);
    str_val.toCharArray(buf, str_val.length());
    buf[str_val.length() - 1] = '\0';
    temp_str_->SetText(buf);
  }

  // Event sensor has new data. If data is the same as measured previosly. This event is not generated.
  void NotifySensorHasData(SensorManager *sensorManager)
  {
    SetSystemTemp(sensorManager->GetData());
  }
  
  // Event is generated after each measurement
  void NotifySensorMeasured(SensorManager *sensorManager)
  {
    //this->InvalidateOnlyChartArea();
  }
};
