
#include "Sinewave.h"

Sinewave::Sinewave(unsigned int size) {
  data = new byte[size];
  float pi = 3.141592;
  float dx = dx = 2 * pi / size;   // fill the  byte bufferarry
  float fd = 0;
  float fcnt = 0;
  int iw;
  for (iw = 0; iw < size; iw++){      // with 50 periods sinewawe
    fd= 127*sin(fcnt);                // fundamental tone
    fcnt=fcnt+dx;                     // in the range of 0 to 2xpi  and 1/512 increments
    int bb=127+fd;                    // add dc offset to sinewawe
    data[iw]=bb;                  // write value into array
  }
}

unsigned int Sinewave::get(unsigned int index) {
  return data[index];
}
