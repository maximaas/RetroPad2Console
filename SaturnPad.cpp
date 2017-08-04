/**
 * Saturn pad buttons events
 *
 * Copyright (C) 2015,2017 Maximaas
 * Version 1.0 - completed on 2015.12.09
 * Contact: mingzo@gmail.com
 */
#include "SaturnPad.h"

int state[5] = {0, 0, 0, 0, 0};

//#define D0_QUERY (PIND & 0x04)

SaturnPad::SaturnPad()
{
	connected = false;
}

void SaturnPad::init(void)
{
	pinMode(PIN_SATURN_D0, INPUT);
	pinMode(PIN_SATURN_D1, INPUT);
	pinMode(PIN_SATURN_D2, INPUT);
	pinMode(PIN_SATURN_D3, INPUT);
	pinMode(PIN_SATURN_S0, OUTPUT);
	pinMode(PIN_SATURN_S1, OUTPUT);
}

void SaturnPad::wait(void)
{
	int i = 10;
	while (i>0) {
		int d2 = analogRead(PIN_SATURN_D2);

//		Serial.print("ss:");
//		Serial.println(d2, DEC);

		if (d2 > 1000)
			connected = true;
		i--;
	}
}

int* SaturnPad::buttons(void)
{
	int idx = 0;
	for(int s0=0; s0<2; s0++){
		for(int s1=0; s1<2; s1++){
			digitalWrite(PIN_SATURN_S0, s0);
			digitalWrite(PIN_SATURN_S1, s1);
			int d0 = digitalRead(PIN_SATURN_D0);   //Stores the data lines
			int d1 = digitalRead(PIN_SATURN_D1);
			int d2 = digitalRead(PIN_SATURN_D2);
			int d3 = digitalRead(PIN_SATURN_D3);

			byte data = ~(d0<<3 | d1<<2 | d2<<1 | d3) << 4;
			data = data >> 4;
			state[idx] = data;
//			Serial.println(state[idx]);
			idx++;
		}
	}

	if ((state[0] == 0) && (state[1] == 0) && (state[2] == 0) && (state[3] == 12))
		state[4] = 0;
	else
		state[4] = 1;

	return state;
}
