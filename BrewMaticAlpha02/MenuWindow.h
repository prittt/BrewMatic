// Copyright(c) 2018 Federico Bolelli
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met :
//
// * Redistributions of source code must retain the above copyright notice, this
// list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and / or other materials provided with the distribution.
//
// * Neither the name of BrewMatic nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef BREWMATIC_MENU_WINDOW_H_
#define BREWMATIC_MENU_WINDOW_H_

#include "MainWindow.h"
#include "Button.h"

#include "ImageButton.h"

#include "SensorManager.h"
#include "TextBoxNumber.h"

extern Image home25x25;
extern Image home30x30;

// This is the menu window which controls menu and home buttons through the ITouchEventReceiver, and internal temperature sensor through ISensorHasDataEventReceiver and ISensorMeasuredEventReceiver.
class MenuWindow : public MainWindow, public ITouchEventReceiver, public ISensorHasDataEventReceiver, public ISensorMeasuredEventReceiver
{
	LinkedList<ImageButton> _listButtons; //list of buttons on the left scrren side
	LinkedList<Window> windows_list_;       //list of depended windows

  uint8_t _nbuttons;
  uint8_t _margins;
  unsigned _szx; 
  unsigned _szy;
  TextBoxString *temp_str_;
  TextBoxString *system_str_;
  
  ImageButton *home_btn;

  char buf[7]; //000.00
  
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
    windows_list_.Add(win);
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
      //out<<F("NotifyTouch")<<endln;

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

        // Disable all windows
        for(int i = 0; i<windows_list_.Count(); ++i){
          windows_list_[i]->SetVisible(false);  
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
            windows_list_[i]->SetVisible(i==sel_index);
            if(i==sel_index)
            {
              windows_list_[i]->Invalidate();
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
    str_val.toCharArray(buf, str_val.length());
    buf[str_val.length() - 1] = '\0';
    temp_str_->SetText(buf);
    //temp_str_->Invalidate();
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

#endif // ! BREWMATIC_MENU_WINDOW_H_
