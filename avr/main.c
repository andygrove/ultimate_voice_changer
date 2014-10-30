/**
 * Ultimate Voice Changer for Arduino/AVR.
 *
 * Requires an external board/shield hosting an MCP3208 ADC and an MCP4921 DAC.
 *
 * Created by Andy Grove.
 * http://theotherandygrove.com
 */

#include <avr/io.h>
#include <avr/power.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "wave.h"

// PORTB pin assignments
const int LDAC = 0;     // PB0 / Arduino pin 8
const int CS_DAC = 1;   // PB1 / Arduino pin 9
const int CS_ADC = 2;   // PB2 / Arduino pin 10
const int DATAOUT = 3;  // PB3 / Arduino pin 11
const int DATAIN = 4;   // PB4 / Arduino pin 12
const int SPICLOCK = 5; // PB5 / Arduino pin 13

// SPI command for DAC
const int cmd = 0x7000;

const long n = 10000;

int myindex = 0;
int fword = 0;
uint8_t data = 0;
int counter = 0;
long last_time = 0;

void setup(void) {

  // set clock to full speed (8 MHz)
  //clock_prescale_set(clock_div_1);

  // disable interrupts
  //cli();

  // we're not using the onboard ADC so may as well power it down
  //power_adc_disable();

  // LDAC, CS_DAC, CS_ADC, DATAOUT to OUTPUT
  DDRB |= (1 << LDAC);
  DDRB |= (1 << CS_ADC);
  DDRB |= (1 << CS_DAC);
  DDRB |= (1 << DATAOUT);
  DDRB |= (1 << SPICLOCK);

  // set DATAIN to INPUT
  DDRB &= ~_BV(DATAIN);

  // LED 
  DDRD |= _BV(7);

  // disable both the ADC and DAC by writing them HIGH
  PORTB |= (1 << CS_ADC);
  PORTB |= (1 << CS_DAC);

  // set the clock low
  PORTB &= ~(1 << SPICLOCK);

}

inline int pot_to_incr(int pot) {
  return 1 + (pot/64);
}

/** Set CLK HIGH then LOW */
inline void cycle_clock(void) {
  PORTB |= (1 << SPICLOCK);
  PORTB &= ~(1 << SPICLOCK);
}

/** Read from ADC */
int read_adc(int channel){

  int adcvalue = 0;

  // command bits - start, mode, chn (3), dont care (3)
  int commandbits = 0xC0; //b11000000; 

  // allow channel selection
  commandbits|=((channel-1)<<3);

  // select ADC
  PORTB &= ~(1 << CS_ADC);

  // setup bits to be written
  for (int i=7; i>=3; i--){
    if (commandbits&1<<i) {
      PORTB |= (1 << DATAOUT);
    } else {
      PORTB &= ~(1 << DATAOUT);
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

  PORTB |= (1 << CS_ADC);

  return adcvalue;
}

/** Write to DAC */
void write_dac(int data) {

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

int incr = 1;

int dc_bias = 0;

int audio_in;

int sample_counter = 0;

/** debug routine to make sure AVR is working. */
void blink(void) {
  setup();
  while (1) {
    PORTD ^= (1 << 7);
    _delay_ms(200);
  }
}

/** play the sine wave through the DAC. */
void play_sine_wave(void) {
  setup();
  int multiplier = 16;
  int increment = 1; // 1=30 Hz, 2=60Hz, 3=90Hz ... 7=210 Hz
  int i = 0;
  int j = 0;
  while (1) {

    if (++j==400) {
      j = 0;
      increment = pot_to_incr(read_adc(2));
    }

    uint8_t s = pgm_read_byte(&sineWave[i]);

    write_dac((multiplier*127)+((s-127)*multiplier));

    // increment the index in the wave table
    i += increment;
    if (i>=NUM_SINE_WAVE_POINTS) {
      i -= NUM_SINE_WAVE_POINTS;
    }
  }
}

void ring_modulator(void) {

  setup();

  while(1) {

    ++sample_counter;

    if (sample_counter == 400) {
      incr = pot_to_incr(read_adc(2));
      sample_counter = 0;
    } else {
      // sample audio input
      audio_in = read_adc(1);
    }

    // get next point in sine wave table
    myindex += incr;
    if (myindex >= NUM_SINE_WAVE_POINTS)  {
      myindex -= NUM_SINE_WAVE_POINTS;
    }
    //data = sineWave[myindex];
    data = pgm_read_byte(&sineWave[myindex]);

    // mix audio with sine wave
    audio_in = 2047 + ((audio_in-2047) * ((data-127) / 127.0));

    // clip the signal
    if (audio_in<0) audio_in = 0;
    else if (audio_in>4095) audio_in = 4095;
    
    // write output
    write_dac(audio_in);
  }

}

int main(void) {
  // choose a routine to call	  
  //blink();
  //play_sine_wave();
  ring_modulator();
  return 0;
}

    

