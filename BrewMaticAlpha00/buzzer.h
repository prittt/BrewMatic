//#ifndef BUZZER_H_
//#define BUZZER_H_
//
//#include <aplayer.h> // To play music with buzzer
//#include "MegaPins.h"
//#include "Buttons.h"
//#include "utilities.h"
//
//class Buzzer{
//
//    APlayer* player_;
//    Buttons* btns_;
//
//    bool is_ring_;
//    bool is_posponed_; 
//
//    unsigned long start_pospone_ring_; 
//
//public: 
//    Buzzer(Buttons *btns, APlayer* player){
//        player_ = player;  
//        btns_ = btns;
//        is_ring_ = true;
//        is_posponed_ = false;
//    }
//    
//    void HandleRingStop(){
//        if(btns_->IsPressed(STOP_ALARM_BTN)){
//           is_ring_ = false; // Disable alarm
//           LedBlink(MSG_LED_PIN);
//        }
//    }
//
//    void HandleRingPospone(unsigned long pospone_time = 60000){ 
//        if(is_posponed_ && millis() - start_pospone_ring_ > pospone_time){
//             is_ring_ = true;
//             is_posponed_ = false;
//          }
//           
//          if(btns_->IsPressed(POSPONE_ALARM_BTN)){
//             start_pospone_ring_ = millis();
//             is_posponed_ = true;
//             is_ring_ = false;
//             LedBlink(MSG_LED_PIN);
          }
    }

    void HandleRingStopPospone(unsigned long pospone_time = 60000){
        HandleRingStop();
        HandleRingPospone(pospone_time);
    }

    void PlayWhenNeeded(bool additional_condition = true){
        if(is_ring_ && additional_condition){
            player_->play(ALARM_CLOCK);
        }
    }

    void ResetRing(){
        is_ring_ = true;
        is_posponed_ = false;        
    }
  
};

#endif // BUZZER_H_
