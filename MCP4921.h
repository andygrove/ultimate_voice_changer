
#ifndef _MCP4921_H
#define _MCP4921_H

const int LDAC = 0;     // Arduino pin 8
const int CS_DAC = 1;   // Arduino pin 9
// SPI command for DAC
const int cmd = 0x7000;


class MCP4921 {
public:
  void write(unsigned int value);
}

#endif //_MCP4921_H
