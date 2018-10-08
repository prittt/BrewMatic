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

#ifndef BREWMATIC_BUTTONS_H_
#define BREWMATIC_BUTTONS_H_

#include "mega_pins.h"

# define BUTTONS_PRESS_TIME_SHORT 1000
# define BUTTONS_PRESS_TIME_LONG 3000

// GENERAL BUTTONS
#define POSPONE_ALARM_BTN BTN_1
#define STOP_ALARM_BTN BTN_2

// MASH IN BUTTONS
#define MASH_IN_BTN BTN_3

// MASH OUT BUTTONS
#define MASH_OUT_BTN BTN_3
#define IOD_MINUS_MINUTES_BTN BTN_4
#define IOD_PLUS_MINUTES_BTN BTN_5

// MASH/SPARGE PHASE BUTTONS
#define SPARGE_DONE_BTN BTN_3

// BOIL BUTTONS
#define START_BOIL_BTN BTN_3
#define THROW_HOP_BTN BTN_3

// RESTART/RESET BUTTONS
#define RESET_BTN BTN_5
#define RESTART_BTN BTN_6

class Buttons{
  public:
      
    // Buttons from left to right
    // BTN_1 --> 
    // BTN_2 --> MASH IN, to be changed

    Buttons(){
        pinMode(BTN_1, INPUT);
        pinMode(BTN_2, INPUT);
    }

    bool IsPressed(int button_pin, unsigned long press_duration = BUTTONS_PRESS_TIME_SHORT, bool direct = 1){
      unsigned btn = GetButtonPos(button_pin);
      out << F("sono dentro is pressed\n");
      if (btn == 100){
        return false; // The specified button doesn't exists
      }
  
      if (!direct){
          if(digitalRead(button_pin) == LOW){
            unsigned long start_press = millis();
            while(digitalRead(button_pin) == LOW && (millis() - start_press < press_duration)){}
    
            if(digitalRead(button_pin) == LOW && (millis() - start_press >= press_duration)){
              return true;  
            }
          }
        
      }else{
          if(digitalRead(button_pin) == HIGH){
            unsigned long start_press = millis();
            while(digitalRead(button_pin) == HIGH && (millis() - start_press < press_duration)){}
    
            if(digitalRead(button_pin) == HIGH && (millis() - start_press >= press_duration)){
              return true;  
            }
          }
      }
      
      return false;
    }
    
    bool IsPressedInstantly(int button_pin){
      if(digitalRead(button_pin) == HIGH){
          return true;
      }
      return false;   
    }

    private:
    unsigned GetButtonPos(int button_pin){
      switch(button_pin){
        case BTN_1: return 0; 
        case BTN_2: return 1;
        default: return 100;
      }
    }

// No serialization required
    
};

#endif // ! BREWMATIC_BUTTONS_H_
