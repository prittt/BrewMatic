#include "DC_UTFT.h"
#include "TouchUTFT.h"


#include "Log.h"
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
#include "FakeSensor.h"

#include "MenuWindow.h"

#include "ImageButton.h"

#include "mega_pins.h"

extern Image brew60x60;
extern Image settings60x60;
extern Image recipe60x60;


OneWire ds(INTERNAL_TEMP);

UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch  myTouch(43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq

DC_UTFT dc(&myGLCD);
TouchUTFT touch(&myTouch);

// Manager which is responsible for window updating process
WindowsManager<MenuWindow> windowsManager(&dc,&touch);

// List in which all sensors are collected
LinkedList<SensorManager> sensors;
MeasurementNode measurementNode(sensors,NULL);


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

void setup()
{

	// Setup log (out is wrap about Serial class)
	out.begin(9600);
	out<<F("Setup")<<endln;

	//initialize display
	myGLCD.InitLCD(); 
	myGLCD.clrScr();

  // -------------------------------------------------------------
  pinMode(8, OUTPUT);    //backlight 
  digitalWrite(8, HIGH); //on
  // -------------------------------------------------------------

	//initialize touch
	myTouch.InitTouch();
	myTouch.setPrecision(PREC_MEDIUM);

	DC_UTFT::RegisterDefaultFonts();
  
	//Initialize apperance. Create your own DefaultDecorators class if you would like different application look
  InitSensorDecorator();
  InitMenuButtonDecorator();
	DefaultDecorators::InitAll();
  
	//initialize window manager
	windowsManager.Initialize();

  RecipeWindow *win_recipe = new RecipeWindow(F("Recipe"),0,0,200,200);
  SettingsWindow *win_settings = new SettingsWindow(F("Settings"),0,0,200,2000);
  BrewWindow *win_brew = new BrewWindow(F("Recipi"),0,0,200,200);

  // Buttons creation
	windowsManager.MainWnd()->Initialize();

  windowsManager.MainWnd()->SetParams(3, 10, 100, 100); // (num button, margins between buttons in pixels, size x, size y)
  windowsManager.MainWnd()->AddMenuButton(F("Recipe"), win_recipe, &recipe60x60);
  windowsManager.MainWnd()->AddMenuButton(F("Brew"), win_brew, &brew60x60);
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
  SensorManager *internal_temp_manager = new SensorManager(internal_temp,-10,110,1000*10);
  sensors.Add(internal_temp_manager);

  internal_temp_manager->RegisterHasDataEventReceiver(windowsManager.MainWnd());
  internal_temp_manager->RegisterMeasuredEventReceiver(windowsManager.MainWnd());
  
  // In order to avoid pause in the touch interactions, windows manager is defined as critical process
  //measurementNode.SetCriticalProcess(&windowsManager);

	AHelper::LogFreeRam();
	delay(1000); 
	out<<F("End setup")<<endln;
}

unsigned long long time_t = millis();
float i = 0.8;
void loop()
{
  if(measurementNode.Measure()){
    out << "qui.\n";  
  }  
	// Give window manager an opportunity to update display
	windowsManager.loop();
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
