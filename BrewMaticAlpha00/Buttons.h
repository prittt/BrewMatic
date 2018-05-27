#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "MegaPins.h"

# define BUTTONS_PRESS_TIME_SHORT 1000
# define BUTTONS_PRESS_TIME_LONG 3000

class Buttons{
  public:
      
    // Buttons from left to right
    // BTN_1 --> pospone ring
    // BTN_2 --> stop ring
    // BTN_3 
    // BTN_4 
    // BTN_5
    // BTN_6 --> reset recovering variables 

    // BTN_6 + BTN_5 --> rest restarting recipe
    
    //bool state_[6] = {false, false, false, false, false, false};
    //bool is_press_[6] = {false, false,false, false, false, false};
    //unsigned long start_press_[6] = {millis(), millis(), millis(), millis(), millis(), millis()}; 

    Buttons(){
        pinMode(BTN_1, INPUT);
        pinMode(BTN_2, INPUT);
        pinMode(BTN_3, INPUT);
    }

    bool IsPressed(int button_pin, unsigned long press_duration = BUTTONS_PRESS_TIME_SHORT, bool direct = 1){
      unsigned btn = GetButtonPos(button_pin);
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
        case BTN_3: return 2;  
        default: return 100;
      }
    }

// No serialization required
    
};

#endif //BUTTONS_H_
