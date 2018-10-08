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

#include "Log.h"

#include "DC_UTFT.h"
#include "TouchUTFT.h"

#include "LinkedList.h"
#include "WindowsManager.h"
#include "DefaultDecorators.h"


#include "SettingsWindow.h"
#include "RecipeWindow.h"
#include "BrewWindow.h"

#include "OneWire.h"

#include "SensorManager.h"
#include "DS18B20Sensor.h"
#include "MeasurementNode.h"

#include "MenuWindow.h"

#include "ImageButton.h"

#include "mega_pins.h"

#include "Mash.h"
#include "Sparge.h"
#include "Boil.h"

extern Image brew60x60;
extern Image settings60x60;
extern Image recipe60x60;

// CONTROLLER
#include "Recipe.h"

UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch  myTouch(43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq

DC_UTFT dc(&myGLCD);
TouchUTFT touch(&myTouch);

// Manager which is responsible for window updating process
WindowsManager<MenuWindow> windowsManager(&dc,&touch);

// List in which all sensors are collected
LinkedList<SensorManager> sensors;
MeasurementNode measurementNode(sensors,NULL);

// Mash window + controller
Mash *mash;

// Sparge window + controller
Sparge *sparge;

void InitMenuButtonDecorator(){
  DecoratorList *list = new DecoratorList();
  list->Add(new DecoratorRectFill(Color::SteelBlue, true));
  //list->Add(new DecoratorRoundRect(tSteelBlue));
  Environment::Get()->RegisterDecoratorsGroup(F("MenuButton"), list);
}

void InitSensorDecorator(){
  DecoratorList *list = new DecoratorList();
  list->Add(new DecoratorRectFill(Color::Black, false));
  list->Add(new DecoratorColor(Color::White));
  Environment::Get()->RegisterDecoratorsGroup(F("Sensor"), list);
}

void InitBrewTabCurrentButtonsDecorator(){
  DecoratorList *list = new DecoratorList();
  list->Add(new DecoratorChromeTabBox(true));
  list->Add(new DecoratorColor(Color::Black));
  //list->Add(new DecoratorRoundRect(Color::White));
  Environment::Get()->RegisterDecoratorsGroup(F("BrewCurTab"), list);
}

void InitBrewTabOtherButtonsDecorator(){
  DecoratorList *list = new DecoratorList();
  list->Add(new DecoratorChromeTabBox(false));
  list->Add(new DecoratorColor(Color::White));
  //list->Add(new DecoratorRoundRect(Color::White));
  Environment::Get()->RegisterDecoratorsGroup(F("BrewOtherTab"), list);
}

void InitBrewWindowDecorator(){
  DecoratorList *list = new DecoratorList();
  list->Add(new DecoratorRectFill(Color::Black, false));
  list->Add(new DecoratorColor(Color::White));
  Environment::Get()->RegisterDecoratorsGroup(F("BrewWindow"), list);
}

void setup()
{

	// Setup log (out is wrap about Serial class)
	out.begin(9600);
	out<<F("Setup")<<endln;

	// Initialize display
	myGLCD.InitLCD(); 
	myGLCD.clrScr();

  // -------------------------------------------------------------
  pinMode(8, OUTPUT);    //backlight 
  digitalWrite(8, HIGH); //on
  // -------------------------------------------------------------

	// Initialize touch
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_MEDIUM);

	DC_UTFT::RegisterDefaultFonts();
  
	// Initialize apperance. Create your own DefaultDecorators class if you would like different application look
  InitSensorDecorator();
  InitMenuButtonDecorator();

  InitBrewTabCurrentButtonsDecorator();
  InitBrewTabOtherButtonsDecorator();
  
  
	DefaultDecorators::InitAll();
  
	// Initialize window manager
	windowsManager.Initialize();

  RecipeWindow *win_recipe = new RecipeWindow(F("Recipe"),0,0,200,200);
  SettingsWindow *win_settings = new SettingsWindow(F("Settings"),0,0,200,2000);
  BrewWindow *brew_win = new BrewWindow(F("TabControl"), 50, 40, 700, 380);

  mash = new Mash(F("Mash"), 5, 25, 690, 350); // (+5 pixel top e left - 10 widt, - 30 heigh, perchè diventeranno i figli di brew_win) Parametri collegati alle dimensioni dei tab nella brew_win -> TODO trasformarein variabili 
  sparge = new Sparge(F("Sparge"), 5, 25, 690, 350); // (+5 pixel top e left - 10 widt, - 30 heigh, perchè diventeranno i figli di brew_win) Parametri collegati alle dimensioni dei tab nella brew_win -> TODO trasformarein variabili
  Boil *boil = new Boil(F("Boil"), 5, 25, 690, 350);  // (+5 pixel top e left - 10 widt, - 30 heigh, perchè diventeranno i figli di brew_win) Parametri collegati alle dimensioni dei tab nella brew_win -> TODO trasformarein variabili
  brew_win->AddTab(F("Mash"), mash); 
  brew_win->AddTab(F("Sparge"),sparge);
  brew_win->AddTab(F("Boil"), boil);

  // Menu buttons
	windowsManager.MainWnd()->Initialize();

  windowsManager.MainWnd()->SetParams(3, 10, 100, 100); // (num button, margins between buttons in pixels, size x, size y)
  windowsManager.MainWnd()->AddMenuButton(F("Recipe"), win_recipe, &recipe60x60);
  windowsManager.MainWnd()->AddMenuButton(F("Brew"), brew_win, &brew60x60);
  windowsManager.MainWnd()->AddMenuButton(F("Settings"), win_settings, &settings60x60);

  // SensorManager signature
  // SensorManager(ISensor *sensor,
  //               float low_application_limit,
  //               float high_application_limit,
  //               unsigned long pause_length_ms,
  //               bool withBuffer=true, 
  //               const __FlashStringHelper* applicationName=NULL, 
  //               SensorCalibrator *sensorCalibrator=NULL)

  // System temperature variables
  DS18B20Sensor *internal_temp = new DS18B20Sensor(INTERNAL_TEMP,1);
  SensorManager *internal_temp_manager = new SensorManager(internal_temp,-10,110,1000*10, false, F("INTERNAL"));
  sensors.Add(internal_temp_manager);

  internal_temp_manager->RegisterHasDataEventReceiver(windowsManager.MainWnd());
  internal_temp_manager->RegisterMeasuredEventReceiver(windowsManager.MainWnd());

  // Mash temperature variables
  DS18B20Sensor *mash_up_temp = new DS18B20Sensor(MASH_DS_PIN_UP,1);
  DS18B20Sensor *mash_down_temp = new DS18B20Sensor(MASH_DS_PIN_DOWN,1);
    
  SensorManager *internal_temp_manager_up  = new SensorManager(mash_up_temp,-10,110,1000*10, false, F("UP"));
  SensorManager *internal_temp_manager_down = new SensorManager(mash_down_temp,-10,110,1000*10, false, F("DOWN"));
  sensors.Add(internal_temp_manager_up);
  sensors.Add(internal_temp_manager_down);

  internal_temp_manager_up->RegisterHasDataEventReceiver(mash);
  internal_temp_manager_up->RegisterMeasuredEventReceiver(mash);

  internal_temp_manager_down->RegisterHasDataEventReceiver(mash);
  internal_temp_manager_down->RegisterMeasuredEventReceiver(mash);

  DS18B20Sensor *sparge_temp = new DS18B20Sensor(SPARGE_DS_PIN,1);
  SensorManager *spargel_temp_manager = new SensorManager(sparge_temp,-10,110,1000*10, false, F("SPARGE"));
  sensors.Add(spargel_temp_manager);

  spargel_temp_manager->RegisterHasDataEventReceiver(sparge);
  spargel_temp_manager->RegisterMeasuredEventReceiver(sparge);
 
  // In order to avoid pause in the touch interactions, windows manager is defined as critical process
  //measurementNode.SetCriticalProcess(&windowsManager);

  //CONTROLLER SETUP
  mash->InitializeMashController(mashSteps, sizeof(mashSteps)/sizeof(mashStep), mash_water_weight, mash_malts_weight);

  // RELEs
  pinMode(SPARGE_FIRE_PIN, OUTPUT);
  pinMode(MASH_FIRE_PIN, OUTPUT);
  pinMode(SSR_1, OUTPUT);
  pinMode(SSR_2, OUTPUT);

  mash->FireOff();   // Turn off mash fire
  sparge->FireOff(); // Turn off sparge fire
  digitalWrite(SSR_1, HIGH);
  digitalWrite(SSR_2, HIGH);

	AHelper::LogFreeRam();
	delay(1000); 
	out<<F("End setup")<<endln;
}

unsigned long long t = millis();

void loop()
{
  measurementNode.Measure(); // Get data from sensors
	windowsManager.loop(); // Give window manager an opportunity to update display

  if(mash->IsMash()){
     mash->CheckTemperatures(); // Commentare per bollitura
     mash->SetFire();  // Commentare per bollitura
     mash->HandleStep(); // Commentare per bollitura
     mash->UpdateScreenStepData();
  }

   if(sparge->IsSparge()){ 
    sparge->SetTemperatures(); 
    sparge->SetFire(); 
   } 

  if(millis() - t > 5000){
  
//  String a = "prova.txt";
//     out << "a:" << int(&a) << "\n";
//  String b;
//       out << "b_prim:" << int(&b) << "\n";
//  b = f();
//     out << "b:" << int(&b) << "\n";
//  out << "ac: " << a << "\n";
//  out << "bc: " << b << "\n";
//
//  out << "up: " << mash->mash_up_temp_str_->GetText() << "\n";
//  out << "up_string: " << String(mash->buf_up) << "\n";
//  out << "down: " << mash->mash_down_temp_str_->GetText() << "\n";
//  out << "down_string: " << String(mash->buf_down) << "\n";
//
//  out << "avg: " << mash->mash_avg_temp_str_->GetText() << "\n";
//  out << "avg_string: " << String(mash->buf_avg) << "\n";
//
//  out << "fire: " << mash->flames_img_->GetText() << "\n";
//  out << "fire_string: " << String(mash->buf_flames) << "\n";
   t = millis();
  AHelper::LogFreeRam();

  }

  //windowsManager.MainWnd()->UpdateSystemTemp(150.00);
  
}

// Returns the temperature from one DS18S20 in DEG Celsius
float getTemp(OneWire ds){

  byte data[12];
  byte addr[8];

  if ( !ds.search(addr)) {
    // No more sensors on chain, reset search
    ds.reset_search();
    return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    //DebugSerialPrintln("Sensor getTemp says: CRC is not valid!");
    return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    //DebugSerialPrintln("Sensor getTemp says:; Device is not recognized");
    return -1000;
  }

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1); // Start conversion, with parasite power on at the end

  byte present = ds.reset();
  ds.select(addr);
  ds.write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // We need 9 bytes
    data[i] = ds.read();
  }

  ds.reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); // Using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
}
