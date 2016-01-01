#include <Arduino.h>
#include "UVC_Const.h"
#include "MCP3202.h"


MCP3202::MCP3202() {
/*
  // CS_DAC, DATAOUT to OUTPUT
  DDRB |= _BV(CS_DAC);
  DDRB |= _BV(DATAOUT);

  // set DATAIN to INPUT
  DDRB &= ~_BV(DATAIN);

  // disable DAC by writing chip-select HIGH
  PORTB |= _BV(CS_C);


*/
}

/** Read from ADC */
unsigned int MCP3202::read(unsigned int channel){
  int adcvalue = 0;
  /*

  // command bits - start, sgl/diff, odd/sign, channel 0, channel 1
  byte commandbits = B11010000;

  // allow channel selection
  commandbits|=((channel-1)<<5);

  // select ADC
  PORTB &= ~_BV(CS_ADC);

  // setup bits to be written
  for (int i=7; i>=4; i--){
    if (commandbits&1<<i) {
      PORTB |= _BV(DATAOUT);
    } else {
      PORTB &= ~_BV(DATAOUT);
    }
    cycle_clock();
  }

  //cycle clock to ignore null bit
  clock.cycle();

  //read bits from adc
  for (int i=11; i>=0; i--){
    adcvalue += ((PINB & _BV(DATAIN)) >> 4) << i;
    clock.cycle();
  }

  PORTB |= _BV(CS_ADC);
*/
  return adcvalue;
}

inline void MCP3202::cycle_clock() {
  /** Set CLK HIGH then LOW */
  PORTB |= _BV(SPICLOCK);
  PORTB &= ~_BV(SPICLOCK);
}
