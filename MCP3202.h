
#ifndef _MCP3202_H
#define _MCP3202_H

const int CS_ADC = 2;   // Arduino pin 10
const int DATAOUT = 3;  // Arduino pin 11
const int DATAIN = 4;   // Arduino pin 12


class MCP3202 {
public:
  MCP3208(unsigned int chipSelect);
  unsigned int read(unsigned int channel);
};

#endif //_MCP3202_H
