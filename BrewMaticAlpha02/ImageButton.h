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

#ifndef BREWMATIC_IMAGE_BUTTON_H_
#define BREWMATIC_IMAGE_BUTTON_H_

#include "Window.h"

#include "icons/Image.h"

// Implements button with images inside
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

#endif // ! BREWMATIC_IMAGE_BUTTON_H_
