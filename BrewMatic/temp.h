/*
  This file contains all the useful functions to handle dallas temperature sensor
*/

#ifndef TEMP_H_
#define TEMP_H_

#include "debug.h"

bool temperatureIsOk(float temp){

  if (temp > -100.0 && (int)(temp * 10)/10 != 85.00){
    return true;
  }

  return false; 
}

String temperatureError(float temp, bool complete = false){

  if (temp < -100.0){
      if(complete){
          return String(" ERROR!"); 
      }else{
          return String("ERR.!");  
      }
  }

  if ((int)(temp * 10)/10 == 85.00){
      if(complete){
          return String("LOADING!"); 
      }else{
          return String("LOAD!"); 
      }
  } 

  return String(""); 
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
    DebugSerialPrintln("Sensor getTemp says: CRC is not valid!");
    return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    DebugSerialPrintln("Sensor getTemp says:; Device is not recognized");
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
    DebugSerialPrintln("Sensor getTemp says: CRC is not valid!");
    return -1000;
  }

  if ( addr[0] != 0x10 && addr[0] != 0x28) {
    DebugSerialPrintln("Sensor getTemp says:; Device is not recognized");
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


#endif //TEMP_H_
