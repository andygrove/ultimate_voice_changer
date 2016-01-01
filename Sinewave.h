
#ifndef _SINEWAVE_H
#define _SINEWAVE_H

#include <Arduino.h>

class Sinewave {
public:
  Sinewave(unsigned int size);
  unsigned int get(unsigned int index);
private:
  byte *data;
};

#endif // _SINEWAVE_H
