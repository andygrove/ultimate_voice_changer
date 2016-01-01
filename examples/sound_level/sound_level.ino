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

void setup() {

  Serial.begin(9600);
  
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
  int fword = cmd | data;
  
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

void loop() {
  int mn=0, mx=0;
  for (int i=0; i<10000; i++) {
    int s = read_adc(1);
    if (mn==0 || s<mn) { mn = s; }
    if (s > mx) { mx = s; }
  }
  Serial.print(mn);
  Serial.print(",");
  Serial.println(mx);
  
}    
    

