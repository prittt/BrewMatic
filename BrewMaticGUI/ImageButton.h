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
class ImageButton : public Window
{
private:
	Image *img_;
  __FlashStringHelper * text_; // Button name
  bool with_frame_; // Whether to display or not the button frame around the image (default is true)
  bool with_text_; // Wheter to display or not button name (default is true)
  Color color_; // Color of the frame (default is white)
public:
	///Constructor
	/**
  TODO, describe constructor params
	*/
	ImageButton(const __FlashStringHelper *button_name, Image *img, int left, int top, int width, int height, bool with_text = true, bool with_frame = true, Color color = Color(255,255,255)):Window(button_name, left, top, width, height)
	{
		img_=img;
    text_ = button_name;
    with_text_ = with_text;
    with_frame_ = with_frame;
    color_ = color;

    if(with_frame_){
		  SetDecorators(Environment::Get()->FindDecorators(F("MenuButton"))); // TODO, allow decorater selection from constructor ?!!
    }
	}
	// Implements drawing code
	/**
	\param dc Device context
	*/
	void OnDraw(DC *dc)
	{
    int text_shift = 0;
    int img_rows = img_->rows_; 
    int img_cols = img_->cols_; 

    if(with_text_){
      text_shift = 20;
      dc->SetFont(F("Small"));
      dc->SetColor(Color(255,255, 255)); // TODO, allow color selection from constructor ?!!
      dc->DrawText(text_, 0, Height() - text_shift, DC::HorizontalAlignment::Center, Width());
    }

    int left = (Width() - img_cols)/2 > 0 ? Left() + (Width() - img_cols)/2 : Left();
    int top = (Height() - img_rows - text_shift)/2 > 0 ? Top() + (Height() - img_cols - text_shift)/2 : Top();
   
    dc->DrawImage(img_->data_, left, top, img_rows, img_cols);
	}
};
