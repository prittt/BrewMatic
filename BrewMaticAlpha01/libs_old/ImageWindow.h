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

#include "libs/AWind/Window.h"

#include "Image.h"

class ImageWindow : public Window
{
private:
    Image *img_;
    Color background_color_; // Color of the background, to (default is white)
    bool is_visible_;
public:
	///Constructor
	/**
  T             TODO, describe constructor params
	*/
	ImageWindow(Image *img, int left, int top, Color background_color = Color::Black): Window(F("img_win"), left, top, img->cols_ - 1, img->rows_ - 1)
	{
            img_=img;
            background_color_ = background_color;
            is_visible_ = false;
            AddDecorator(new DecoratorRectFill(background_color,false));
  }
	// Implements drawing code
	/**
	\param dc Device context
	*/
  void SetVisible(bool visible){
    is_visible_ = visible;  
  }

  bool GetVisible(void){
    return is_visible_;  
  }
  
	void OnDraw(DC *dc)
	{
    //out << "On draw image window\n";
    if (is_visible_){ 
        dc->DrawImage(img_->data_, dc->ToDC_X(0), dc->ToDC_Y(0), img_->rows_, img_->cols_);
    }
	}
};
