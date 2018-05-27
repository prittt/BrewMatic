#define PINSSR A10
#define PIN A9 

#define PINRA 6
#define PINRB 7

#define encoderPin1 3
#define encoderPin2 2
#define SW 18

volatile int lastEncoded = 0;
volatile long encoderValue = 0;
long lastencoderValue = 0;
int lastMSB = 0;
int lastLSB = 0;

#include <OneWire.h>

OneWire ds(PIN);

// Required by the touchscreen
#include <UTFT.h> 

// Required by the sd card
#include <SD_adafruit.h> 

File myFile;
const int chipSelect = 47;

// Remember to change the model parameter to suit your display module!
UTFT myGLCD(TFTM080_1_16,38,39,40,41);  //(byte model, int RS, int WR, int CS, int RST, int SER)

//#define MEGA_SOFT_SPI 1
#define SD_CS 47 
int dispx, dispy;   

#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

void setup()
{

  pinMode(encoderPin1, INPUT_PULLUP);
  pinMode(encoderPin2, INPUT_PULLUP);
  pinMode(18, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(3), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(2), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(18), bottone, LOW);
  
  pinMode(PINRA, OUTPUT);
  pinMode(PINRB, OUTPUT);

  pinMode(A5, INPUT);
  
  digitalWrite(PINRA, LOW);
  digitalWrite(PINRB, LOW);
  
  // Setup the LCD
  myGLCD.InitLCD();
  // -------------------------------------------------------------
  pinMode(8, OUTPUT);  //backlight 
  digitalWrite(8, HIGH);//on
  // -------------------------------------------------------------

  attachInterrupt(digitalPinToInterrupt(19), callbackplayer, RISING);


  Serial.begin(115200);

  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 799, 479);  
  
  dispx=myGLCD.getDisplayXSize();
  dispy=myGLCD.getDisplayYSize();  

  Serial.print("Initializing SD card...");
  pinMode(53, OUTPUT);

  // SINGATURE in the SD library:
  // boolean SDClass::begin(uint8_t csPin, int8_t mosi, int8_t miso, int8_t sck)
  while (!SD.begin(47, 44, 45, 43)) {
    Serial.println("failed!");
  }
  Serial.println("done!");

  Serial2.begin(9600);
  
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  
  while (!myDFPlayer.begin(Serial2)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    
      delay(1000); // Code to compatible with ESP8266 watch dog.
    
  }
  Serial.println(F("DFPlayer Mini online."));

  
  pinMode(PIN, INPUT);
  pinMode(PINSSR, OUTPUT);
  
  myDFPlayer.volume(28);  //Set volume value. From 0 to 30
  //myDFPlayer.play(1);  //Play the first mp3
  //bmpDraw("BMP_1.bmp", 0, 0);
}

void loop()
{
  //myDFPlayer.play(1);
  Serial.println(encoderValue);

  float t = getTemp(&ds);
  Serial.print("Temp: ");
  Serial.println(t);
  //myDFPlayer.play(1);
  // put your main code here, to run repeatedly:
  digitalWrite(PINSSR, HIGH);
  delay(2000);
  digitalWrite(PINSSR, LOW);
  delay(2000);
  digitalWrite(PINRA, LOW);
  digitalWrite(PINRB, LOW);
  delay(2000);
  digitalWrite(PINRA, HIGH);
  digitalWrite(PINRB, LOW);
  delay(2000);
  digitalWrite(PINRA, LOW);
  digitalWrite(PINRB, HIGH);
  delay(2000);

  digitalWrite(PINSSR, HIGH);
  digitalWrite(PINRA, HIGH);
  digitalWrite(PINRB, HIGH);
  delay(2000);

}

void updateEncoder(){
  int MSB = digitalRead(encoderPin1); //MSB = most significant bit
  int LSB = digitalRead(encoderPin2); //LSB = least significant bit
  int encoded = (MSB << 1) |LSB; //converting the 2 pin value to single number
  int sum  = (lastEncoded << 2) | encoded; //adding it to the previous encoded value
  if(sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) encoderValue ++;
  if(sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) encoderValue --;
  lastEncoded = encoded; //store this value for next time
  Serial.println(encoderValue);

}

void bottone(){
  // put your main code here, to run repeatedly: 
  Serial.println("SPINTO");
}

void callbackplayer(){

  myDFPlayer.play(1);
  //delay(500);
}

// This function opens a Windows Bitmap (BMP) file and 
// displays it at the given coordinates.  It's sped up 
// by reading many pixels worth of data at a time 
// (rather than pixel by pixel).  Increasing the buffer 
// size takes more of the Arduino's precious RAM but 
// makes loading a little faster.  20 pixels seems a 
// good balance. 
   
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

// Returns the temperature from one DS18S20 in DEG Celsius
float getTemp(OneWire* ds){

  byte data[12];
  byte addr[8];

  if ( !ds->search(addr)) {
    // No more sensors on chain, reset search
    ds->reset_search();
    return -1000;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    Serial.println("Sensor getTemp says: CRC is not valid!");
    return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    Serial.println("Sensor getTemp says:; Device is not recognized");
    return -1000;
  }

  ds->reset();
  ds->select(addr);
  ds->write(0x44,1); // Start conversion, with parasite power on at the end

  byte present = ds->reset();
  ds->select(addr);
  ds->write(0xBE); // Read Scratchpad

  for (int i = 0; i < 9; i++) { // We need 9 bytes
    data[i] = ds->read();
  }

  ds->reset_search();

  byte MSB = data[1];
  byte LSB = data[0];

  float tempRead = ((MSB << 8) | LSB); // Using two's compliment
  float TemperatureSum = tempRead / 16;

  return TemperatureSum;
}

