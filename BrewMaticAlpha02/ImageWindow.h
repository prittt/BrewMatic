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

#ifndef BREWMATIC_IMAGE_WINDOW_H_
#define BREWMATIC_IMAGE_WINDOW_H_

#include "icons/Image.h"

#include "Window.h"

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

#endif // ! BREWMATIC_IMAGE_WINDOW_H_
