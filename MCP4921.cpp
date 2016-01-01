
#include <Arduino.h>
#include "UVC_Const.h"
#include "MCP4921.h"

// SPI command for DAC
const int cmd = 0x7000;

MCP4921::MCP4921() {
  // LDAC, CS_DAC, CS_ADC, DATAOUT to OUTPUT
  DDRB |= _BV(LDAC);
  DDRB |= _BV(CS_DAC);

  // disable the DAC by writing chip-select HIGH
  PORTB |= _BV(CS_DAC);
}

void MCP4921::write(unsigned int value) {

  // combine command and data
  fword = cmd | value;

  // chip select
  PORTB &= ~_BV(CS_DAC);

  // start writing data
  PORTB |= _BV(LDAC);

  for (int i1=0; i1<16; i1++) {
    if (fword & 0x8000) {
      PORTB |= _BV(DATAOUT);
    } else {
      PORTB &= ~_BV(DATAOUT);
    }

    cycle_clock();

    fword = fword << 1;
  }

  // end of conversion
  PORTB |= _BV(CS_DAC);

  // writing data to output buffer
  PORTB &= ~_BV(LDAC);

}

inline void MCP4921::cycle_clock() {
  /** Set CLK HIGH then LOW */
  PORTB |= _BV(SPICLOCK);
  PORTB &= ~_BV(SPICLOCK);
}
