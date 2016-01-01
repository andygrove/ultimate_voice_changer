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

// comment this out if you don't have a pot attached to the second input
#define ENABLE_POT

// the code now supports some different effects modes, mostly to help with debugging
#define MODE_NO_EFFECT 1 // this mode passes the mic input directly to the audio out without modification
#define MODE_RING_MOD  2 // this is the default ring modulator mode where the mic input is mixed with a sine wave then written to audio out
#define MODE_SINE_WAVE 3 // this mode writes the sine wave directly to audio out

const int mode = MODE_RING_MOD;

// number of points in sine wave data
#define NUM_SINE_WAVE_POINTS 1024

// PORTD pin assignments
const int PORTD_LED = 6;

// uncomment this line to use MCP3202 instead of MCP3204/8
//#define MCP3202

// PORTB pin assignments

const int SPICLOCK = 5; // Arduino pin 13


// threshold for tuning sensitivity of LED response to sound levels
const int threshold = 256;

int index = 0;
int fword;
int data;
int counter = 0;
long last_time = 0;

// speed to iterate over sine wave .. changing this affects the frequency
int incr = 2;

// this variable is used for the LED PWM effect
int sound_level = 0;
int led_counter = 0;
int sample_counter = 0;
int audio_in = 0;
int audio_out = 0;
int dc_offset = 2063;

byte sineWave[NUM_SINE_WAVE_POINTS];

void setup() {

  // prepare sine wave
  fill_sinewave();

  delay(1000);

  cli();

  // set digital pin 6 to output for the LEDs
  DDRD |= _BV(PORTD_LED);

  // LDAC, CS_DAC, CS_ADC, DATAOUT to OUTPUT
  DDRB |= _BV(LDAC);
  DDRB |= _BV(CS_ADC);

  // CS_DAC, DATAOUT to OUTPUT
  DDRB |= _BV(CS_DAC);
  DDRB |= _BV(DATAOUT);

  // set DATAIN to INPUT
  DDRB &= ~_BV(DATAIN);

  // disable both the ADC and DAC by writing them HIGH
  PORTB |= _BV(CS_ADC);
  PORTB |= _BV(CS_DAC);

  // set the clock low
  DDRB |= _BV(SPICLOCK);
  PORTB &= ~_BV(SPICLOCK);

  // turn the LED on briefly to show startup
  PORTD |= _BV(PORTD_LED);
  delay(500);
  PORTD &= ~_BV(PORTD_LED);
}

/** Set CLK HIGH then LOW */
inline void cycle_clock() {
  PORTB |= _BV(SPICLOCK);
  PORTB &= ~_BV(SPICLOCK);
}


void loop() {

  ++sample_counter;

  if (sample_counter == 400) {
    sample_counter = 0;

#ifdef ENABLE_POT
    int pot = read_adc(2);
    incr = 1 + (pot/128);
#endif

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
  if (mode == MODE_SINE_WAVE) {
    audio_in = 4095;
    audio_out = 2047 + ((audio_in-dc_offset) * ((data-127) / 127.0));
  } else if (mode == MODE_RING_MOD) {
    audio_out = 2047 + ((audio_in-dc_offset) * ((data-127) / 127.0));
  } else {
    audio_out = audio_in;
  }

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
  if (audio_out<0) audio_out = 0;
  else if (audio_out>4095) audio_out = 4095;

  // attempt to reduce noise
  if (audio_out > 2030 && audio_out < 2100) {
    audio_out = 2047;
  }

  // write output
  write_dac(audio_out);
}

void fill_sinewave(){
  float pi = 3.141592;
  float dx = dx = 2 * pi / NUM_SINE_WAVE_POINTS;   // fill the  byte bufferarry
  float fd = 0;
  float fcnt = 0;
  int iw;
  for (iw = 0; iw < NUM_SINE_WAVE_POINTS; iw++){      // with 50 periods sinewawe
    fd= 127*sin(fcnt);                // fundamental tone
    fcnt=fcnt+dx;                     // in the range of 0 to 2xpi  and 1/512 increments
    int bb=127+fd;                        // add dc offset to sinewawe
    sineWave[iw]=bb;                  // write value into array
    // uncomment this to see the sine wave numbers in the serial monitor

    Serial.print("Sine: ");
    Serial.println(bb);

  }
}
