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
#include <Window.h>

#include "Button.h"
#include "ImageButton.h"

#pragma once
///Example window with red background
class WindowMenu : public Window, public ITouchEventReceiver
{
  LinkedList<ImageButton> _listButtons; //list of menu buttons
  LinkedList<Window> _listWindow;       //list of windows associated to menu's voices
  
  uint8_t _nbuttons;
  uint8_t _margins;
  unsigned _szx; 
  unsigned _szy;
public:
	WindowMenu(const __FlashStringHelper * name,int left,int top,int width,int height):Window(name,left,top,width,height)
	{
		AddDecorator(new DecoratorRectFill(Color::Red,false));
		AddDecorator(new DecoratorColor(Color::Black));
	}

  //Adds button (eventually with backkground image) and the associated window
  void AddMenuButton(const __FlashStringHelper *button_name /*name of the button and of the corresponding window*/, Window *win, unsigned short* img = NULL)
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
      out<<F("Notify Touch")<<endln;
      if(window==_listButtons[0])
      {
        out<<F("Button 0")<<endln;
      }
//    int sel_index=-1;
//    for(int i=0;i<_listButtons.Count();i++)
//    {
//      if(window == _listButtons[i])
//      {
//        sel_index=i;
//        break;
//      }
//    }
//    if(sel_index >=0)
//    {
//      DecoratorList *buttonDeco=Environment::Get()->FindDecorators(F("Button"));
//      for(int i=0;i<_listButtons.Count();i++)
//      {
//        _listWindow[i]->SetVisible(i==sel_index);
//        if(i==sel_index)
//        {
//          _listWindow[i]->Invalidate();
//          _listButtons[i]->SetDecorators(_listWindow[i]->GetDecorators());
//        }
//        else
//        {
//          if(_listButtons[i]->GetDecorators()!=buttonDeco)
//          {
//            _listButtons[i]->SetDecorators(buttonDeco);
//            _listButtons[i]->Invalidate();
//          }
//        }
//      }
//    }
//    //out<<F("Window selected: ")<<sel_index<<endl;
  }

};
