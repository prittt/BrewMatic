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

#include "Image.h"

///Implements pictogram buttons
class TabButton : public Button
{
private:
	Image *img_;
  __FlashStringHelper * text_; // Button name
  int left_, top_, width_, height_;
public:
	///Constructor
	TabButton(int left,int top,int width,int height,const __FlashStringHelper * text) : Button(left, top, width, height, text)
	{
    left_ = left;
    top_ = top;
    width_ = width;
    height_ = height;
	  //SetFont(F("Big"));
    //SetHorizontalAlignment(DC::Center);
	}
 
    virtual void OnTouching(DC *dc){
      // Faccio una cosa simile a quella nel default decorator "DecoratorChromeTabBox"
      int slope = 10; 
      dc->SetColor(Color::Red);

      PrepareDC(dc);
      dc->MoveTo(slope, 0);
      dc->LineTo(Width() - slope, 0);
      dc->LineTo(Width(), Height());
      dc->LineTo(0, Height());
                
      dc->MoveTo(slope, 0);
      dc->LineTo(0, Height());    
      
      dc->MoveTo(slope, 1);
      dc->LineTo(Width() - slope, 1);
      dc->LineTo(Width() - 1, Height() - 1);
      dc->LineTo(1, Height() - 1);
                
      dc->MoveTo(slope, 1);
      dc->LineTo(1, Height() - 1);
    }
};
