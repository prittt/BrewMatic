/*
GUI library for Arduino TFT and OLED displays

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
#include "libs/AWind/MainWindow.h"
#include "TabButton.h"
#include "libs/AWind/Button.h"

///TabControl. Control element wich allows intersactive switch between chidl windows
class BrewWindow : public Window, public ITouchEventReceiver
{
	LinkedList<TabButton> buttons_list; // list of tub buttons
	LinkedList<Window> windows_list;  //list of child windows
  static const int _szy = 20;
  static const int _szx = 120;
  static const int _overlapping = -4;
  int cur_x_ = 4;

public:
	///Constructor
	/**
	\param left left coordinate relative to parent indow
	\param top top coordinate relative to parent indow
	\param width window width
	\param height window height
	*/
	BrewWindow(const __FlashStringHelper * name, int left, int top, int width, int height):Window(name, left, top, width, height)
	{
    SetDecorators(Environment::Get()->FindDecorators(F("BrewWindow"))); // TODO, allow decorater selection from constructor ?!!
    //AddDecorator(new DecoratorRoundRect(Color::White));
	}
	
	// Adds pair: button + corresponding window. The size of added window is adjusted automatically
	void AddTab(const __FlashStringHelper *button_name, Window *window)
	{
    TabButton * button = new TabButton(cur_x_, 0, _szx, _szy, button_name);
    button->SetFont(F("Small"));

    if(windows_list.Count() > 0){
      button->SetDecorators(Environment::Get()->FindDecorators(F("BrewOtherTab")));
      window->SetVisible(false);
    }
    else{
      button->SetDecorators(Environment::Get()->FindDecorators(F("BrewCurTab")));
      window->SetVisible(true);
    }
    button->SetMargins(0,5);
    button->RegisterTouchEventReceiver(this);
    AddChild(button);
    AddChild(window);
    buttons_list.Add(button);
    windows_list.Add(window);
    
    cur_x_ += _szx - _overlapping; // Update cur_x_ for the next tab
  }

  void OnDraw(DC *dc){
    dc->SetColor(Color::White);
    dc->DrawRoundRect(0, _szy, Width(), Height()); // To draw rectangle on the window
  }
    
	// Events routing for gui interaction (see RegisterTouchEventReceiver and public ITouchEventReceiver declaration)
	void NotifyTouch(Window *window)
	{
		int sel_index=-1;
		for(int i=0;i<buttons_list.Count();i++)
		{
			if(window == buttons_list[i])
			{
				sel_index=i;
				break;
			}
		}
		if(sel_index >=0)
		{
			for(int i=0;i<buttons_list.Count();i++)
			{
				windows_list[i]->SetVisible(i==sel_index);
				if(i==sel_index)
				{
          //out<<"invaldate!\n";
					buttons_list[i]->SetDecorators(Environment::Get()->FindDecorators(F("BrewCurTab")));

				}
				else
				{
					buttons_list[i]->SetDecorators(Environment::Get()->FindDecorators(F("BrewOtherTab")));
				}
        buttons_list[i]->Invalidate();
        windows_list[i]->Invalidate();
			}
		}
		//out<<F("Window selected: ")<<sel_index<<endl;
	}
};
