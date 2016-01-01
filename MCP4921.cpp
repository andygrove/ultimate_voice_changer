
#include "MCP4921.h"

MCP4921::MCP4921() {
  // LDAC, CS_DAC, CS_ADC, DATAOUT to OUTPUT
  DDRB |= _BV(LDAC);
  DDRB |= _BV(CS_ADC);


}

void MCP4921::write(unsigned int value) {

  // combine command and data
  fword = cmd | data;

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
