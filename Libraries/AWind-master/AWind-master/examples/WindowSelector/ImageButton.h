#pragma once
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

#include "Window.h"


///Implements pictogram buttons
class ImageButton : public Window
{
private:
	unsigned short *img_;
  __FlashStringHelper * text_;
  //TextBoxTString<__FlashStringHelper> *text_;
public:
	///Constructor
	/**
	\param pictogram defines button appearance
	\param left left coordinate relative to parent indow
	\param top top coordinate relative to parent indow
	\param width window width
	\param height window height
	*/
	ImageButton(const __FlashStringHelper *button_name, unsigned short* img,int left,int top,int width,int height):Window(button_name,left,top,width,height)
	{
		img_=img;
    text_ = button_name;
//    out << F("name") << button_name << endln;
//    DecoratorList *list = new DecoratorList();
//    list->Add(new DecoratorRectFill(Color::SteelBlue, true));
//    //list->Add(new DecoratorRoundRect(tSteelBlue));
//    Environment::Get()->RegisterDecoratorsGroup(F("Red"), list);
//    text_ = new TextBoxTString<__FlashStringHelper>(left, top + height - 20, width, 20, button_name, F("Red"));
    
		SetDecorators(Environment::Get()->FindDecorators(F("MenuButton")));
	}
	///Implements drawing code
	/**
	\param dc Device context
	*/
	void OnDraw(DC *dc)
	{
    int img_rows = 25; //TODO to be calculated
    int img_cols = 25; //TODO to be calculated
    int left = (Width() - img_cols)/2 > 0 ? Left() + (Width() - img_cols)/2 : Left();
    int top = (Height() - img_rows)/2 > 0 ? Top() + (Height() - img_cols)/2 : Top();
   
		dc->DrawImage(img_, left, top, img_rows, img_cols);

    dc->SetFont(F("Small"));
    dc->SetColor(Color(255,255, 255));
    dc->DrawText(text_, 0, Height() - 20, DC::HorizontalAlignment::Center, Width());
	}
};
