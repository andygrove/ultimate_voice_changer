// pin assigments
const int LED = 6;
const int LDAC = 8;
const int CS_DAC = 9;
const int CS_ADC = 10;
const int SPICLOCK = 13;
const int DATAOUT = 11;
const int DATAIN = 12;

// SPI command for DAC
const int cmd = 0x7000;

const long n = 10000;

int index = 0;
int fword;
int data;
int counter = 0;
long last_time = 0;

#define NUM_SINE_WAVE_POINTS 1024

byte sineWave[NUM_SINE_WAVE_POINTS];  

void setup() {

  // use regular arduino code here since this only runs once

  // set pin directions
  pinMode(LDAC, OUTPUT);
  pinMode(SPICLOCK, OUTPUT);
  pinMode(CS_DAC, OUTPUT);
  pinMode(CS_ADC, OUTPUT);
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);

  // disable both the ADC and DAC
  digitalWrite(CS_ADC, HIGH);
  digitalWrite(CS_DAC, HIGH);
  digitalWrite(SPICLOCK, LOW);

  // prepare sine wave
  fill_sinewave();
}

/** Set CLK HIGH then LOW */
inline void cycle_clock() {
  PORTB |= B00100000; //digitalWrite(SPICLOCK,HIGH);
  PORTB &= B11011111; //digitalWrite(SPICLOCK,LOW);    
}

/** Read from ADC */
int read_adc(int channel){

  int adcvalue = 0;

  // command bits - start, mode, chn (3), dont care (3)
  byte commandbits = B11000000; 

  // allow channel selection
  commandbits|=((channel-1)<<3);

  // select ADC
  PORTB &= B11111011; //digitalWrite(CS_ADC,LOW);

  // setup bits to be written
  for (int i=7; i>=3; i--){
    if (commandbits&1<<i) {
      PORTB |= B00001000; //digitalWrite(DATAOUT,HIGH);
    } else {
      PORTB &= B11110111; //digitalWrite(DATAOUT,LOW);
    }
    cycle_clock();
  }

  //cycle clock twice to ignore two null bits
  cycle_clock();
  cycle_clock();

  //read bits from adc
  for (int i=11; i>=0; i--){
    adcvalue += ((PINB & B00010000) >> 4) << i; //digitalRead(DATAIN)<<i;
    cycle_clock();
  }

  PORTB |= B00000100; //digitalWrite(CS_ADC, HIGH); //turn off device

  return adcvalue;
}

/** Write to DAC */
void write_dac(int data) {

  // combine command and data
  fword = cmd | data;
  
  // chip select
  PORTB &= B11111101; //digitalWrite(CS_DAC, LOW);
  
  // start writing data
  PORTB |= B00000001; //digitalWrite(LDAC, HIGH);

  for (int i1=0; i1<16; i1++) {
    if (fword & 0x8000) {
      PORTB |= B00001000; //digitalWrite(DATAOUT, HIGH);
    } else {
      PORTB &= B11110111; //digitalWrite(DATAOUT, LOW);
    }

    cycle_clock();

      fword = fword << 1;
  }

  // end of conversion
  PORTB |= B00000010; //digitalWrite(CS_DAC, HIGH); 
  
  // writing data to output buffer
  PORTB &= B11111110; //digitalWrite(LDAC, LOW); 
}

bool led_on = false;

int adj_counter = 0;

int incr = 1;

int audio_in;

void loop() {

  if (++adj_counter == 100) {
    int pot = read_adc(2);
    incr = pot / 100.0;
    adj_counter = 0;

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

  // clip the signal
  if (audio_in<0) audio_in = 0;
  else if (audio_in>4095) audio_in = 4095;
  
  // write output
  write_dac(audio_in);

  // blink the LED after every n samples (it just wouldn't be 
  // an arduino sketch without a blinking LED)
/*
  if (++counter==n) {
    led_on = !led_on;
    if (led_on) {
      //PORTD |= B01000000;
      digitalWrite(LED, HIGH);
    } else {
      //PORTD &= B10111111;
      digitalWrite(LED, LOW);
    }
    counter = 0;
  }
  */
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
    

