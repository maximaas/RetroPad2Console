/**
 * SNES pad buttons events
 *
 * Copyright (C) 2015,2017 Maximaas
 * Version 1.0 - completed on 2015.12.09
 * Contact: mingzo@gmail.com
 */
#include "SnesPad.h"

//A4, A5, A3
SnesPad::SnesPad()
{
	connected = false;
}

void SnesPad::init(void)
{
	pinMode(PIN_SNES_LATCH, OUTPUT);
	pinMode(PIN_SNES_CLOCK,  OUTPUT);
	pinMode(PIN_SNES_DATA, INPUT);
}

void SnesPad::wait(void)
{
	int d = analogRead(PIN_SNES_DATA);

//	Serial.print("sfc:");
//	Serial.println(d, DEC);

	//when pad connected, the reading is 1023, when disconnected the reading is about 43
	if (d > 1000)
		connected = true;
}

int SnesPad::buttons(void)
{
	int ret = 0;
	byte i;

	latch();

	for (i = 0; i < 16; i++) {
		ret |= shift() << i;
	}
	return ~ret;
}

void SnesPad::latch(void)
{
	digitalWrite(PIN_SNES_LATCH,HIGH);
	delayMicroseconds(12);
	digitalWrite(PIN_SNES_LATCH,LOW);

	delayMicroseconds(6);
}

int SnesPad::shift(void)
{
	int ret = digitalRead(PIN_SNES_DATA);

	delayMicroseconds(6);
	digitalWrite(PIN_SNES_CLOCK,HIGH);
	delayMicroseconds(6);
	digitalWrite(PIN_SNES_CLOCK,LOW);

	return ret;
}
