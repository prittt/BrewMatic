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

#ifndef BREWMATIC_TAB_BUTTON_H_
#define BREWMATIC_TAB_BUTTON_H_

#include "Window.h"

#include "icons/Image.h"

// Implementation of tab box "Google Chrome" style
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

#endif // ! BREWMATIC_TAB_BUTTON_H_
