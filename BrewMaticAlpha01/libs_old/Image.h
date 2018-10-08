#pragma once

#include "Arduino.h"

struct Image{ 
  int rows_;
  int cols_;
  unsigned short  *data_;

  //Image(){}
  Image(unsigned short  *data, int rows, int cols) : data_(data), rows_(rows), cols_(cols)  {}  
};
