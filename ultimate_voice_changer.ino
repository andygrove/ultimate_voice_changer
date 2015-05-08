/**
 * Ultimate Voice Changer for Arduino
 * Created by Andy Grove.
 *
 * Requires an external board/shield hosting an MCP3208 ADC 
 * and an MCP4921 DAC as well as a few passive components and
 * an audio jack.
 *
 * For more information, please visit the project page:
 *
 * http://theotherandygrove.com/projects/ultimate-voice-changer-arduino-shield/
 */

// comment this out if you don't want the LED working
#define ENABLE_LED 

// PORTD pin assignments
const int PORTD_LED = 6;

// uncomment this line to use MCP3202 instead of MCP3204/8
//#define MCP3202

// PORTB pin assignments
const int LDAC = 0;     // Arduino pin 8
const int CS_DAC = 1;   // Arduino pin 9
const int CS_ADC = 2;   // Arduino pin 10
const int DATAOUT = 3;  // Arduino pin 11
const int DATAIN = 4;   // Arduino pin 12
const int SPICLOCK = 5; // Arduino pin 13

// SPI command for DAC
const int cmd = 0x7000;

const long n = 10000;

int index = 0;
int fword;
int data;
int counter = 0;
long last_time = 0;

//TODO: external pots could be used to adjust these values in real-time
const int threshold = 256;


#define NUM_SINE_WAVE_POINTS 1024
byte sineWave[NUM_SINE_WAVE_POINTS];  

void setup() {
  
  cli();
  
  // set digital pin 6 to output for the LEDs
  DDRD |= _BV(PORTD_LED);

  // LDAC, CS_DAC, CS_ADC, DATAOUT to OUTPUT
  DDRB |= _BV(LDAC);
  DDRB |= _BV(CS_ADC);
  DDRB |= _BV(CS_DAC);
  DDRB |= _BV(DATAOUT);
  DDRB |= _BV(SPICLOCK);

  // set DATAIN to INPUT
  DDRB &= ~_BV(DATAIN);

  // disable both the ADC and DAC by writing them HIGH
  PORTB |= _BV(CS_ADC);
  PORTB |= _BV(CS_DAC);

  // set the clock low
  PORTB &= ~_BV(SPICLOCK);
  
  // turn the LED on briefly to show startup
  PORTD |= _BV(PORTD_LED);
  delay(500);
  PORTD &= ~_BV(PORTD_LED);

  // prepare sine wave
  fill_sinewave();
}

/** Set CLK HIGH then LOW */
inline void cycle_clock() {
  PORTB |= _BV(SPICLOCK);
  PORTB &= ~_BV(SPICLOCK);
}

/** Read from ADC */
int read_adc_mcp3202(int channel){

  int adcvalue = 0;

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
  cycle_clock();

  //read bits from adc
  for (int i=11; i>=0; i--){
    adcvalue += ((PINB & _BV(DATAIN)) >> 4) << i;
    cycle_clock();
  }

  PORTB |= _BV(CS_ADC);

  return adcvalue;
}

/** Read from ADC */
int read_adc_mcp3208(int channel){

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

int read_adc(int channel){
#ifdef MCP3202
  return read_adc_mcp3202(channel);
#else
  return read_adc_mcp3208(channel);
#endif
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

bool led_on = false;

int incr = 1;

int dc_bias = 0;

int audio_in;

// this variable is used for the LED PWM effect
int sound_level = 0;
int led_counter = 0;

int sample_counter = 0;

boolean up;



void loop() {

  ++sample_counter;

  if (sample_counter == 400) {
    int pot = read_adc(2);
    incr = 1 + (pot/128);

  } else {
    // sample audio input
    audio_in = read_adc(1);
  }

  // get next point in sine wave table
  index += incr;
  if (index >= NUM_SINE_WAVE_POINTS)  {
    index -= NUM_SINE_WAVE_POINTS;
  }
  data = sineWave[index];

  // mix audio with sine wave
  audio_in = 2047 + ((audio_in-2047) * ((data-127) / 127.0));
  
#ifdef ENABLE_LED

  if (audio_in > 2048+threshold || audio_in < 2048-threshold) {
    sound_level = 255;
  } else {
    if (sound_level > 0) {
      sound_level -= 1;
    }
  }
  
  // crude way to simulate PWM effect, this could be improved by using interrupts
  if (++led_counter > 255) {
    led_counter = 0;
  }
  if (led_counter < sound_level) {
    PORTD |= _BV(PORTD_LED);
  } else {
    PORTD &= ~_BV(PORTD_LED);
  }

#endif

  // clip the signal
  if (audio_in<0) audio_in = 0;
  else if (audio_in>4095) audio_in = 4095;
  
  // write output
  write_dac(audio_in);
}

void fill_sinewave(){
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
    

