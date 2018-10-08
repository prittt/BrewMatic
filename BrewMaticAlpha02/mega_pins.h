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

#ifndef BREWMATIC_MEGA_PINS_H_
#define BREWMATIC_MEGA_PINS_H_

//************************************************************
// ROTATIVE ENCODER
//************************************************************

#define BTN_3 18
#define SW 18

#define CLK 3
#define DT 2

//************************************************************
// BUTTONs
//************************************************************

#define BTN_1 19
#define BTN_2 A15

//************************************************************
// LEDs - RELEs
//************************************************************

#define MASH_FIRE_PIN 6
#define SPARGE_FIRE_PIN 7 

#define SSR_1 A10
#define SSR_2 A11

//************************************************************
// DFPlayer
//************************************************************

// It is on Arduino's Serial1

//************************************************************
// TEMPERATURE SENSORs
//************************************************************

#define MASH_DS_PIN_UP A7
#define MASH_DS_PIN_DOWN A6

//#define MASH_DS_REPLACEMENT_PIN A9 // Internal temperature
#define INTERNAL_TEMP A9

#define SPARGE_DS_PIN 4
#define SPARGE_DS_REPLACEMENT_PIN 5 

#endif // ! BREWMATIC_MEGA_PINS_H_
