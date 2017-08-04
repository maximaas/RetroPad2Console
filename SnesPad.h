/**
 * SNES pad buttons events
 *
 * Copyright (C) 2015,2017 Maximaas
 * Version 1.0 - completed on 2015.12.09
 * Contact: mingzo@gmail.com
 */
#ifndef SnesPad_h
#define SnesPad_h

#include "Arduino.h"

#include <inttypes.h>

#define PIN_SNES_LATCH A4
#define PIN_SNES_CLOCK A5
#define PIN_SNES_DATA  A3

#define SNES_B       0b0000000000000001  //0x01
#define SNES_Y       0b0000000000000010  //0x02
#define SNES_SELECT  0b0000000000000100  //0x04
#define SNES_START   0b0000000000001000  //0x08
#define SNES_UP      0b0000000000010000  //0x10
#define SNES_DOWN    0b0000000000100000  //0x20
#define SNES_LEFT    0b0000000001000000  //0x40
#define SNES_RIGHT   0b0000000010000000  //0x80
#define SNES_A       0b0000000100000000  //0x100
#define SNES_X       0b0000001000000000  //0x200
#define SNES_L       0b0000010000000000  //0x400
#define SNES_R       0b0000100000000000  //0x800

class SnesPad {

  public:
	int connected;

	SnesPad();
	void init();
	void wait();
	int buttons();

  private:
    void latch();
    int shift();

};

#endif

