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
#include <Window.h>

enum shape{
    VERTICAL = 0,
    HORIZONTAL = 1
  };


// Brew window
class BrewWindow : public Window
{
public:
	BrewWindow(const __FlashStringHelper * name,int left,int top,int width,int height):Window(name,left,top,width,height)
	{
	  SetDecorators(Environment::Get()->FindDecorators(F("BrewWindow"))); // TODO, allow decorater selection from constructor ?!!
    AddDecorator(new DecoratorRoundRect(Color::White));
   
	}

   
  
  void DrawTune(DC *dc, size_t x, size_t y, size_t dim, shape s = shape::VERTICAL){
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

    // Temp (test)
    dc->SetFont(F("Big"));
    dc->DrawText(F("000.00"), x + (x - dc->FontWidth()*6 / 2) , y + dim - dc->FontHeight()*2 - 30);
    dc->DrawText(F("000.00"), x + (x - dc->FontWidth()*6 / 2) , y + dim - dc->FontHeight() - 10);
  }
 
  // Implements drawing code
  /**
  \param dc Device context
  */
  void OnDraw(DC *dc)
  {
    dc->SetColor(Color(255,255, 255));
    DrawTune(dc, 100, 100, 200);
  }
};
