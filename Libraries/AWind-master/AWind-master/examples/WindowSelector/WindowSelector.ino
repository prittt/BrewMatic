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

UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch  myTouch(43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq

DC_UTFT dc(&myGLCD);
TouchUTFT touch(&myTouch);

//manager which is responsible for window updating process
WindowsManager<MenuWindow> windowsManager(&dc,&touch);


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
	DefaultDecorators::InitAll();

	//initialize window manager
	windowsManager.Initialize();

	//create tabs
	windowsManager.MainWnd()->Initialize(); 
  windowsManager.MainWnd()->AddTab(F("Recipe"));
  windowsManager.MainWnd()->AddTab(F("Settings"));
  windowsManager.MainWnd()->AddTab(F("Brewing"));
//	Window1 *window1=new Window1(F("Window1"),0,0,0,0);
//	Window2 *window2=new Window2(F("Window2"),0,0,0,0);
//	Window3 *window3=new Window3(F("Window3"),0,0,0,0);
//	windowsManager.MainWnd()->AddTab(F("Red"),window1);
//	windowsManager.MainWnd()->AddTab(F("Yellow"),window2);
//	windowsManager.MainWnd()->AddTab(F("Green"),window3);

	AHelper::LogFreeRam();
	delay(1000); 
	out<<F("End setup")<<endln;

}

void loop()
{
	//give window manager an opportunity to update display
	windowsManager.loop();
}
