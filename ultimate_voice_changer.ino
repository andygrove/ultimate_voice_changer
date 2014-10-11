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

int fword;
int data;
int tmp;
int counter = 0;
long last_time = 0;

#define NUM_SINE_WAVE_POINTS 512

byte sineWave[NUM_SINE_WAVE_POINTS];  

void setup() {

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

int index = 0;

int read_adc(int channel){

  digitalWrite(SPICLOCK, LOW);
  int adcvalue = 0;

  byte commandbits = B11000000; //command bits - start, mode, chn (3), dont care (3)

  //allow channel selection
  commandbits|=((channel-1)<<3);

  digitalWrite(CS_ADC,LOW); //Select adc
  // setup bits to be written
  for (int i=7; i>=3; i--){
    digitalWrite(DATAOUT,commandbits&1<<i);
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);    
  }

    //cycle clock twice to ignore two null bits
  digitalWrite(SPICLOCK,HIGH);    
  digitalWrite(SPICLOCK,LOW);
  digitalWrite(SPICLOCK,HIGH);  
  digitalWrite(SPICLOCK,LOW);

  //read bits from adc
  for (int i=11; i>=0; i--){
    adcvalue+=digitalRead(DATAIN)<<i;
    //cycle clock
    digitalWrite(SPICLOCK,HIGH);
    digitalWrite(SPICLOCK,LOW);
  }
  digitalWrite(CS_ADC, HIGH); //turn off device

  // reset DATAOUT .. not needed but makes debugging easier
  digitalWrite(DATAOUT, LOW);  

  // clock is LOW
  return adcvalue;
}

/** Write to DAC */
void write_dac(int data) {

fword = cmd | data;
  
  // chip select
  digitalWrite(CS_DAC, LOW);
  
  // start writing data
  digitalWrite(LDAC, HIGH);
  for (int i1=0; i1<16; i1++) {
    tmp = fword & 0x8000;
    digitalWrite(DATAOUT, LOW);
    if (tmp) {
      digitalWrite(DATAOUT, HIGH);
    }

    // cycle clock
    digitalWrite(SPICLOCK, HIGH);
    digitalWrite(SPICLOCK, LOW);

    fword = fword << 1;
  }
  digitalWrite(CS_DAC, HIGH); // end of conversion
  digitalWrite(LDAC, LOW); // writing data to output buffer
}

bool led_on = false;

void loop() {
  if (++index == NUM_SINE_WAVE_POINTS)  {
    index = 0;
  }
  data = sineWave[index] * 5;

  int audio_in = read_adc(1);

  //TODO: apply modifications to audio
  
  write_dac(audio_in);

  // blink the LED after every n samples (it just wouldn't be 
  // an arduino sketch without a blinking LED)
  long n = 1000;
  if (++counter==n) {
    led_on = !led_on;
    digitalWrite(LED, led_on ? HIGH : LOW);
    counter = 0;
  }
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
    

