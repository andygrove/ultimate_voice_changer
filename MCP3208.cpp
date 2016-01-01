#include "MCP3208.h"

MCP3208::MCP3208() {

  // CS_DAC, DATAOUT to OUTPUT
  DDRB |= _BV(CS_DAC);
  DDRB |= _BV(DATAOUT);

  // set DATAIN to INPUT
  DDRB &= ~_BV(DATAIN);

}

/** Read from ADC */
unsigned int MCP3208::read(unsigned int channel){

  int adcvalue = 0;

  // command bits - start, mode, chn (3), dont care (3)
  byte commandbits = B11000000;

  // allow channel selection
  commandbits|=((channel-1)<<3);

  // select ADC
  PORTB &= ~_BV(CS_ADC);

  // setup bits to be written
  for (int i=7; i>=3; i--){
    if (commandbits&1<<i) {
      PORTB |= _BV(DATAOUT);
    } else {
      PORTB &= ~_BV(DATAOUT);
    }
    cycle_clock();
  }

  //cycle clock twice to ignore two null bits
  cycle_clock();
  cycle_clock();

  //read bits from adc
  for (int i=11; i>=0; i--){
    adcvalue += ((PINB & _BV(DATAIN)) >> 4) << i;
    cycle_clock();
  }

  PORTB |= _BV(CS_ADC);

  return adcvalue;
}
