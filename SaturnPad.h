/**
 * Saturn pad buttons events
 *
 * Copyright (C) 2015,2017 Maximaas
 * Version 1.0 - completed on 2015.12.09
 * Contact: mingzo@gmail.com
 */
#ifndef SaturnPad_h
#define SaturnPad_h

#include "Arduino.h"

#include <inttypes.h>

/**
 * Saturn controller pinout
 *
 * 1   2   3   4   5   6   7   8   9
 * VCC D1  D0  S0  S1  5v  D3  D2  GND
 * VCC PD5 PD6 PD7 PB0 PC2 PB1 PB2 GND
 */

#define PIN_SATURN_D0  6  //PD6, ss pad pin3
#define PIN_SATURN_D1  5  //PD5, ss pad pin2
#define PIN_SATURN_D2 10  //PB2, ss pad pin8
#define PIN_SATURN_D3  9  //PB1, ss pad pin7
#define PIN_SATURN_S0  7  //PD7, ss pad pin4
#define PIN_SATURN_S1  8  //PB0, ss pad pin5


#define SATURN_Z      B00001000 //0x08  8
#define SATURN_Y      B00000100 //0x04  4
#define SATURN_X      B00000010 //0x02  2
#define SATURN_R      B00000001 //0x01  1

#define SATURN_B      B00001000
#define SATURN_C      B00000100
#define SATURN_A      B00000010
#define SATURN_START  B00000001

#define SATURN_UP     B00001000
#define SATURN_DOWN   B00000100
#define SATURN_LEFT   B00000010
#define SATURN_RIGHT  B00000001

#define SATURN_L      B00001101

class SaturnPad {

  public:
	int connected;

	SaturnPad();
    void init();
    void wait();
    int* buttons();

  private:

};

#endif
