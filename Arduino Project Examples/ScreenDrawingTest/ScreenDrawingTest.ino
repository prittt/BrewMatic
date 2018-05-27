/***************************************************
//Web: http://www.buydisplay.com
EastRising Technology Co.,LTD
Examples for ER-TFTM080-1(800x480 Pixels)
Software SPI,8080 16-bit Interface 5V Power Supply

This program is a demo of how to use most of the functions
of the library with a supported display modules.

Tested and worked with:
Arduino Mega 2560,Arduino Due
Works with Arduino 1.6.0 IDE 
****************************************************/

#include <UTFT.h>
#include <SD_adafruit.h> // To handle SD card

// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// Set the pins to the correct ones for your development shield
// ------------------------------------------------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41

// Remember to change the model parameter to suit your display module!
UTFT myGLCD(SSD1963_800480,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)

extern unsigned short flames25pxls[625];

#define SD_CS 47 
int dispx, dispy;   

void setup()
{
  Serial.begin(9600);
  // Setup the LCD
  myGLCD.InitLCD();
  
  // -------------------------------------------------------------
  pinMode(8, OUTPUT);  //backlight 
  digitalWrite(8, HIGH);//on
  // -------------------------------------------------------------
 
  myGLCD.setFont(SmallFont);

  // ---- SD card ----
  //Serial.print("Initializing SD card...");
  //pinMode(53, OUTPUT);

  // SINGATURE in the SD library:
  // boolean SDClass::begin(uint8_t csPin, int8_t mosi, int8_t miso, int8_t sck)
  //while (!SD.begin(47, 44, 45, 43)) {
  //  Serial.println("failed!");
  //}
  //Serial.println("done!");
  // ---- SD card ----
}

typedef struct Point2d{
  uint16_t x, y;  
}Point2d;

typedef struct Size2d{
  uint16_t x, y;
}Size2d;

typedef struct Rectangle{
  Point2d topleft, bottomright;
  Size2d sizes;

  Rectangle(){}

  Rectangle(Point2d p1, Point2d p2){
    topleft = p1;
    bottomright = p2; 
    sizes = {p2.x - p1.x, p2.y - p1.y};
  }
  
}Rectangle;

class Button{
private: 
  Rectangle rect_;
  String text_;
  UTFT *lcd_;

public:
  Button(){}
  Button(Rectangle rect, String text, UTFT *lcd){
    rect_ = rect;
    text_ = text;

    lcd_ = lcd;
  }

  void InDraw(){
    lcd_->setColor(255,0,0);
    lcd_->fillRoundRect(rect_.topleft.x, rect_.topleft.y, rect_.bottomright.x, rect_.bottomright.y);   

    lcd_->setColor(0, 0, 0);
    lcd_->setBackColor(255,0,0);
    lcd_->print(text_, rect_.topleft.x + (rect_.sizes.x - text_.length()*lcd_->getFontXsize())/2, rect_.topleft.y + 1);
  }

  void ConDraw(){
    
  }

  void ConPresDraw(){
    
  }

  void HandleSingleTouch(int x, int y){
    if(x >= rect_.topleft.x && x <= rect_.bottomright.x && y >= rect_.topleft.y && y <= rect_.bottomright.y){
        
    }  
  }

  void WaitForButton(){
      
  }
};

class Box{
private:
  Rectangle rect_;
  String title_;

  UTFT *lcd_; 
public:
  Box(Rectangle rect, String title, UTFT *lcd){
    rect_ = rect; 
    title_ = title;
    lcd_ = lcd;
    Init();
  }

  void Init(){
    // Set color
    lcd_->setColor(255, 0, 0);
    
    // External box
    lcd_->drawRoundRect(rect_.topleft.x, rect_.topleft.y, rect_.bottomright.x, rect_.bottomright.y);
  
    // Title box 
    lcd_->fillRoundRect(rect_.topleft.x, rect_.topleft.y, rect_.bottomright.x, rect_.topleft.y + 12);  
  
    // Title text
    lcd_->setColor(0, 0, 0);
    lcd_->setBackColor(255,0,0);
    lcd_->print(title_, rect_.topleft.x + (rect_.sizes.x - title_.length()*lcd_->getFontXsize())/2, rect_.topleft.y + 1);
  }

  void DrawRoundRect(int x1, int y1, int x2, int y2){
    lcd_->drawRoundRect(rect_.topleft.x + x1, rect_.topleft.y + y1, rect_.topleft.x + x2, rect_.topleft.y + y2); 
  }

  void FillRoundRect(int x1, int y1, int x2, int y2){
    lcd_->fillRoundRect(rect_.topleft.x + x1, rect_.topleft.y + y1, rect_.topleft.x + x2, rect_.topleft.y + y2); 
  }

  void DrawBitmap(int x1, int y1, int x2, int y2){
    myGLCD.drawBitmap (150, 150, 25, 25, flames25pxls); // TODO
    lcd_->fillRoundRect(rect_.topleft.x + x1, rect_.topleft.y + y1, rect_.topleft.x + x2, rect_.topleft.y + y2); 
  }

  void DrawButton(int x1, int y1, int x2, int y2, String text){
    
  }
};

class MenuBar{
private:
  Rectangle rect_; 
  Button voices_[10];
  uint8_t voices_len_;
  uint8_t *font_;
  uint8_t font_x_;
  uint8_t font_y_; 
  
  UTFT *lcd_; 
  
public:
  MenuBar(UTFT *lcd, uint8_t *font = SmallFont){
    lcd_ = lcd;
    font_ = font;

    lcd_->setFont(font_);
    font_x_ = lcd_->getFontXsize();
    font_y_ = lcd_->getFontYsize();
    
    rect_ = Rectangle({0,0},{lcd_->getDisplayXSize() - 1, font_y_ + 2 + 2 });
    voices_len_ = 0;

    Init();
  }

  Rectangle GetMenuRectangle(){
    return rect_;
  }

  void Init(){
    // Fill menu bar with white
    lcd_->setColor(225, 225, 225);
    lcd_->fillRoundRect(rect_.topleft.x, rect_.topleft.y, rect_.bottomright.x, rect_.bottomright.y);  

    // Draw separator line
    lcd_->setColor(160,160,160);
    lcd_->drawLine(rect_.topleft.x, rect_.bottomright.y - 1, rect_.bottomright.x, rect_.bottomright.y - 1);
    
    lcd_->setColor(0, 0, 0);
    lcd_->setBackColor(225, 225, 225);
    lcd_->print("Prova", 1, 1); //print test to remove
  }

  void AddVoice(String voice_text){

    uint16_t start_x = 2; 
//    if (voices_len_ != 0){
//      //start_x = voices_[voices_len_].rect_.bottomright.x + 2;
//    }
//    
//    if (voices_len_ < 10){
//      voices_[voices_len_++] = MenuVoice(voice_text, Rectangle({start_x, rect_.topleft.y}, { start_x + voice_text.length()*lcd_->getFontXsize() + 4 /*gap between border of touch area and text*/, rect_.bottomright.y - 2}));    
//      voices_[voices_len_].Init();
//    }
    
  }
};


void loop()
{
  int buf[798];
  int x, x2;
  int y, y2;
  int r;

  myGLCD.clrScr();
  //myGLCD.fillScr(255, 255, 255); // Fill screen background 


  // Menu bar
  MenuBar menu(&myGLCD);
  Rectangle menurect = menu.GetMenuRectangle();
  
  
  uint8_t box_gap = 3;

  // Mash box  
  Rectangle mashrect({ 0, menurect.bottomright.y + 4 }, { myGLCD.getDisplayXSize()/2 - box_gap, myGLCD.getDisplayYSize() - 1 });
  Box mashbox(mashrect, String("MASH"), &myGLCD);
  
  // Mash images //TODO
  //myGLCD.drawBitmap (150, 150, 25, 25, flames25pxls);
  //bmpDraw("TunFire.pbm",10,10); // Todo, read from sd
  
  // Sparge box
  Rectangle spargerect({ mashrect.bottomright.x + box_gap*2, 0 }, { myGLCD.getDisplayXSize() - 1, myGLCD.getDisplayYSize()/2 - box_gap });
  Box spargebox(spargerect, String("SPARGE"), &myGLCD);

  // Settings box
  Rectangle settingsrect({ mashrect.bottomright.x + box_gap*2, spargerect.bottomright.y + box_gap*2 }, { myGLCD.getDisplayXSize() - 1, myGLCD.getDisplayYSize() - 1 });
  Box settingsbox(settingsrect, String("SETTINGS"), &myGLCD);
  
  delay(100000000);
}

#define BUFFPIXEL 20

void bmpDraw(char *filename, int x, int y) {
  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= dispx) || (y >= dispy)) return;

  Serial.println();
  Serial.print(F("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');

  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    Serial.println(F("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    Serial.println(F("File size: ")); 
    Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    Serial.print(F("Image Offset: ")); 
    Serial.println(bmpImageoffset, DEC);

    // Read DIB header
    Serial.print(F("Header size: ")); 
    Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);

    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      Serial.print(F("Bit Depth: ")); 
      Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
        goodBmp = true; // Supported BMP format -- proceed!
        Serial.print(F("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= dispx)  w = dispx  - x;
        if((y+h-1) >= dispy) h = dispy - y;

        // Set TFT address window to clipped image bounds

        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
          pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                myGLCD.setColor(lcdbuffer[lcdidx]);
                myGLCD.drawPixel(col, row);
                lcdidx = 0;
                first  = false;
              }

              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            myGLCD.setColor(r,g,b);
            myGLCD.drawPixel(col, row);
          } // end pixel

        } // end scanline

        // Write any remaining data to LCD
        if(lcdidx > 0) {
           myGLCD.setColor(lcdbuffer[lcdidx]);
          myGLCD.drawPixel(col, row);
        } 

        Serial.print(F("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");

      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) Serial.println(F("BMP format not recognized."));

}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}



