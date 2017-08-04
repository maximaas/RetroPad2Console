/**
 * RetroPad2Console
 * for SNES & Saturn pads using on original xbox and gamecube.
 *
 * Copyright (C) 2015,2017 Maximaas
 * Version 1.0 - completed on 2015.12.09
 *
 * GNU GPLv3 license
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Contact: mingzo@gmail.com
 */
#include "RetroPad2Console.h"

#include "ConsoleEuroGC.h"
#include "ConsoleXbox.h"

#include "XBOXPad.h"
#include "SnesPad.h"
#include "SaturnPad.h"

#define PIN_LED 3

//pads
SnesPad pad_snes = SnesPad();
SaturnPad pad_saturn = SaturnPad();

//consoles
ConsoleEuroGC ngc = ConsoleEuroGC();
ConsoleXbox xbox = ConsoleXbox();

bool xb = false;
bool gc = false;

void setup()
{
	//Serial.begin(9600);

	pinMode(PIN_LED, OUTPUT);

	//Detect which console currently connected
	gc = ngc.communicate(true);

	//if not a ngc, looping to wait for the xbox
	while (!gc && !xb) {
		//Initiate ports and settings for xbox console...
		xbox_init(false);

		//Serial.println("Establishing communication with xbox console...");
		//Try 5 times, if not work, means the connected target is not a xbox.
		for (int i=0; i<5; i++) {
			xbox_reset_watchdog();
			delayMicroseconds(10000); // 10ms delay
			xbox_send_pad_state();

			if (xbox_pad_detected()) {
				xb = true;
				break;
			}
		}
	}

	//turn the led on when any console detected
	if (gc || xb) {
		digitalWrite(PIN_LED, HIGH);
	}

	pad_snes.init();
	pad_snes.wait();

	pad_saturn.init();
	pad_saturn.wait();
}

void loop()
{

	//console is xbox
	if (xb) {
		xbox.idle();

		if (pad_snes.connected) {
			int snes_state = pad_snes.buttons();
			if (snes_state > 0) {
				xbox.snes(snes_state);
			}
		}
		if (pad_saturn.connected) {
			int* saturn_state = pad_saturn.buttons();
			if (saturn_state[4] > 0) {
				xbox.saturn(saturn_state);
			}
		}

		xbox_send_pad_state();
	}

	//console is gamecube
	if(gc) {
		ngc.reset();

		if (pad_snes.connected) {
			int snes_state = pad_snes.buttons();
			if (snes_state > 0) {
				ngc.snes2gc(snes_state);
			}
		}

		if (pad_saturn.connected) {
			int* saturn_state = pad_saturn.buttons();
			if (saturn_state[4] > 0) {
				ngc.saturn2gc(saturn_state);
			}
		}
		ngc.communicate(false);
	}
}

