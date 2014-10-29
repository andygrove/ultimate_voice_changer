/**
 * Ultimate Voice Changer for Arduino
 *
 * Requires an external board/shield hosting an MCP3208 ADC and an MCP4921 DAC.
 *
 * Created by Andy Grove.
 * http://theotherandygrove.com
 */

#include <avr/io.h>
#include <util/delay.h>

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
int fword;
int data;
int counter = 0;
long last_time = 0;

#define NUM_SINE_WAVE_POINTS 1024

uint8_t sineWave[NUM_SINE_WAVE_POINTS];  

void setup(void) {

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

  // prepare sine wave
  float pi = 3.141592;
  float dx ;
  float fd ;
  float fcnt;
  dx=2 * pi / NUM_SINE_WAVE_POINTS;   // fill the  byte bufferarry
  int iw;
  for (iw = 0; iw < NUM_SINE_WAVE_POINTS; iw++){      // with 50 periods sinewawe
    fd= 127*sin(fcnt);                // fundamental tone
    fcnt=fcnt+dx;                     // in the range of 0 to 2xpi  and 1/512 increments
    int bb=127+fd;                        // add dc offset to sinewawe 
    sineWave[iw]=bb;                  // write value into array
    // uncomment this to see the sine wave numbers in the serial monitor
    /*
    Serial.print("Sine: ");
    Serial.println(bb);
    */
  }
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

int main(void) {

  setup();

  while(1) {

    PORTD ^= _BV(7);

//    _delay_ms(250);

    write_dac(read_adc(1));

/*

    ++sample_counter;


    if (sample_counter == 400) {
      int pot = read_adc(2);
      incr = pot >> 5; // (1=2048, 2=1024, 3=512, 4=256, 5=128,6=64, 5=32 )
      // setp through sine wave with increments between 1 and 40 approximately .. since the sampling rate is 
      // very roughly 8000/second and the sine wave contains 1024 points this translates to frequencies between 
      // 8 Hz and 320 Hz ... again, these are very rough calculations for now

    } else {
      // sample audio input
      audio_in = read_adc(1);
    }

    // get next point in sine wave table
    myindex += incr;
    if (myindex >= NUM_SINE_WAVE_POINTS)  {
      myindex -= NUM_SINE_WAVE_POINTS;
    }
    data = sineWave[myindex];

    // mix audio with sine wave
    audio_in = 2047 + ((audio_in-2047) * ((data-127) / 127.0));

    //audio_in += dc_bias;

    // clip the signal
    if (audio_in<0) audio_in = 0;
    else if (audio_in>4095) audio_in = 4095;
    
    // write output
    write_dac(audio_in);
*/
  }
}

    

