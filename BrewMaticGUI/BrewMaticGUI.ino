#include "DC_UTFT.h"
#include "TouchUTFT.h"


#include "Log.h"
#include "LinkedList.h"
#include "WindowsManager.h"
#include "DefaultDecorators.h"
#include "SettingsWindow.h"
#include "RecipeWindow.h"
#include "BrewWindow.h"

#include "MenuWindow.h"

#include "ImageButton.h"

extern Image flames25x25;
extern Image settings60x60;
extern Image recipe60x60;


UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)
UTouch  myTouch(43, 42, 44, 45, 46);  //byte tclk, byte tcs, byte din, byte dout, byte irq

DC_UTFT dc(&myGLCD);
TouchUTFT touch(&myTouch);

// Manager which is responsible for window updating process
WindowsManager<MenuWindow> windowsManager(&dc,&touch);

void InitMenuButtonDecorator(){

  DecoratorList *list = new DecoratorList();
  list->Add(new DecoratorRectFill(Color::SteelBlue, true));
  //list->Add(new DecoratorRoundRect(tSteelBlue));
  Environment::Get()->RegisterDecoratorsGroup(F("MenuButton"), list);

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
  windowsManager.MainWnd()->AddMenuButton(F("Brew"), win_brew, &flames25x25);
  windowsManager.MainWnd()->AddMenuButton(F("Settings"), win_settings, &settings60x60);

	AHelper::LogFreeRam();
	delay(1000); 
	out<<F("End setup")<<endln;
}

void loop()
{
	// Give window manager an opportunity to update display
	windowsManager.loop();
}
