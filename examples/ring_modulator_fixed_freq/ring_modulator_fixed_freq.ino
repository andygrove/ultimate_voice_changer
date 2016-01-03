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

#include <UVC_Const.h>
#include <MCP3208.h>
#include <MCP4921.h>
#include <Sinewave.h>
 
// comment this out if you don't want the LED working
#define ENABLE_LED

// the code now supports some different effects modes, mostly to help with debugging
#define MODE_NO_EFFECT 1 // this mode passes the mic input directly to the audio out without modification
#define MODE_RING_MOD  2 // this is the default ring modulator mode where the mic input is mixed with a sine wave then written to audio out
#define MODE_SINE_WAVE 3 // this mode writes the sine wave directly to audio out

const int mode = MODE_SINE_WAVE;

// number of points in sine wave data
#define NUM_SINE_WAVE_POINTS 1024

// speed to iterate over sine wave .. changing this affects the frequency
int incr = 2;

// PORTD pin assignments
const int PORTD_LED = 6;

MCP3208 adc;
MCP4921 dac;
Sinewave sinewave(NUM_SINE_WAVE_POINTS);

// threshold for tuning sensitivity of LED response to sound levels
const int threshold = 256;

int index = 0;
int data;
int counter = 0;
long last_time = 0;


// this variable is used for the LED PWM effect
int sound_level = 0;
int led_counter = 0;
int sample_counter = 0;
int audio_in = 0;
int audio_out = 0;
int dc_offset = 2063;

void setup() {

  // set clock low
  DDRB |= _BV(SPICLOCK);
  PORTB &= ~_BV(SPICLOCK);

  // disable interrupts (not needed for this sketch)
  cli();

  // set digital pin 6 to output for the LEDs
  DDRD |= _BV(PORTD_LED);

  // turn the LED on briefly to show startup
  PORTD |= _BV(PORTD_LED);
  delay(500);
  PORTD &= ~_BV(PORTD_LED);
}

void loop() {

  ++sample_counter;

  if (sample_counter == 400) {
    sample_counter = 0;

  } else {
    // sample audio input from channel 1
    audio_in = adc.read(1);
  }

  // get next point in sine wave table
  index += incr;
  if (index >= NUM_SINE_WAVE_POINTS)  {
    index -= NUM_SINE_WAVE_POINTS;
  }
  data = sinewave.get(index);

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

  // attempt to reduce noise by not amplifying around the midpoint
  if (audio_out > 2030 && audio_out < 2100) {
    audio_out = 2047;
  }

  // write output
  dac.write(audio_out);
}

