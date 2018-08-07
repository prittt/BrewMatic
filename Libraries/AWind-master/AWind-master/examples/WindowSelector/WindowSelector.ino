/*
This file is part of AWind library

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
#include "DC_UTFT.h"
#include "TouchUTFT.h"


#include "Log.h"
#include "LinkedList.h"
#include "WindowsManager.h"
#include "DefaultDecorators.h"
#include "Window1.h"
#include "Window2.h"
#include "Window3.h"

#include "WindowSelector.h"

#include "ImageButton.h"

extern unsigned short flames25pxls[625];

UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch  myTouch(43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq

DC_UTFT dc(&myGLCD);
TouchUTFT touch(&myTouch);

//manager which is responsible for window updating process
WindowsManager<MenuWindow> windowsManager(&dc,&touch);

void InitMenuButtonDecorator(){

  DecoratorList *list = new DecoratorList();
  list->Add(new DecoratorRectFill(Color::SteelBlue, true));
  //list->Add(new DecoratorRoundRect(tSteelBlue));
  Environment::Get()->RegisterDecoratorsGroup(F("MenuButton"), list);

}

void setup()
{

	//setup log (out is wrap about Serial class)
	out.begin(9600);
	out<<F("Setup")<<endln;

	//initialize display
	myGLCD.InitLCD(); 
	myGLCD.clrScr();

  // -------------------------------------------------------------
  pinMode(8, OUTPUT);  //backlight 
  digitalWrite(8, HIGH);//on
  // -------------------------------------------------------------

	//initialize touch
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_MEDIUM);

	DC_UTFT::RegisterDefaultFonts();
  
	//Initialize apperance. Create your own DefaultDecorators class if you would like different application look
  InitMenuButtonDecorator();
	DefaultDecorators::InitAll();
  
	//initialize window manager
	windowsManager.Initialize();

//  //create windows
//  WindowHome *win_home = new WindowHome(F("Home"),0,0,0,0);
//  
  WindowRecipe *win_mash = new WindowRecipe(F("Mash"),0,0,0,0);
//  WindowSettings *win_settings = new WindowSettings(F("Settings"),0,0,0,0);
  WindowRecipe *win_recipe = new WindowRecipe(F("Recipi"),0,0,0,0);

  //create buttons
	windowsManager.MainWnd()->Initialize();

 
  windowsManager.MainWnd()->SetParams(3, 10, 100, 100); // (num button, margins between buttons in pixels, size x, size y)
  windowsManager.MainWnd()->AddMenuButton(F("Recipe"), win_mash, flames25pxls);
  windowsManager.MainWnd()->AddMenuButton(F("Brew"), win_mash, flames25pxls);
  windowsManager.MainWnd()->AddMenuButton(F("Settings"), win_mash, flames25pxls);

  //windowsManager.MainWnd()->AddButton(F("Settings"));
  //windowsManager.MainWnd()->AddButton(F("Brewing"));

	AHelper::LogFreeRam();
	delay(1000); 
	out<<F("End setup")<<endln;
}

void loop()
{
	//give window manager an opportunity to update display
	windowsManager.loop();
}
