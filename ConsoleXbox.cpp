/**
 * Original Xbox controller protocol and console communication
 *
 * Copyright (C) 2015,2017 Maximaas
 * Version 1.0 - completed on 2015.12.09
 * Contact: mingzo@gmail.com
 */
#include "ConsoleXbox.h"

#include "XBOXPad.h"

#include "SnesPad.h"
#include "SaturnPad.h"

ConsoleXbox::ConsoleXbox()
{
}

void ConsoleXbox::init() {
//	//Initiate ports and settings for xbox console...
//	xbox_init(false);
//
//	//Serial.println("Establishing communication with xbox console...");
//	//Try 5 times, if not work, means the connected target is not a xbox.
//	for (int i=0; i<5; i++) {
//		xbox_reset_watchdog();
//		delayMicroseconds(10000); // 10ms delay
//		xbox_send_pad_state();
//
//		if (xbox_pad_detected()) {
//			xb = true;
//			break;
//		}
//	}
}

bool ConsoleXbox::detect() {

	//Initiate ports and settings for xbox console...
	xbox_init(false);

	//Serial.println("Establishing communication with xbox console...");
	//Try 5 times, if not work, means the connected target is not a xbox.
	for (int i=0; i<5; i++) {
		xbox_reset_watchdog();
		delayMicroseconds(10000); // 10ms delay
		xbox_send_pad_state();

		if (xbox_pad_detected()) {
			return true;
		}
	}

	return false;
}

void ConsoleXbox::snes(int state) {

		(state & 16)  ? bitSet(gamepad_state.digital_buttons, XBOX_DPAD_UP)    : bitClear(gamepad_state.digital_buttons, XBOX_DPAD_UP);
		(state & 32)  ? bitSet(gamepad_state.digital_buttons, XBOX_DPAD_DOWN)  : bitClear(gamepad_state.digital_buttons, XBOX_DPAD_DOWN);
		(state & 64)  ? bitSet(gamepad_state.digital_buttons, XBOX_DPAD_LEFT)  : bitClear(gamepad_state.digital_buttons, XBOX_DPAD_LEFT);
		(state & 128) ? bitSet(gamepad_state.digital_buttons, XBOX_DPAD_RIGHT) : bitClear(gamepad_state.digital_buttons, XBOX_DPAD_RIGHT);

		gamepad_state.a = ((state & 1) > 0) * 0xFF;
		gamepad_state.b = ((state & 256) > 0) * 0xFF;
		gamepad_state.x = ((state & 2) > 0) * 0xFF;
		gamepad_state.y = ((state & 512) > 0) * 0xFF;

		gamepad_state.l = ((state & 1024) > 0) * 0xFF;
		gamepad_state.r = ((state & 2048) > 0) * 0xFF;

		((state & 8) > 0) ? bitSet(gamepad_state.digital_buttons, XBOX_START) : bitClear(gamepad_state.digital_buttons, XBOX_START);

		((state & 4) > 0) ? bitSet(gamepad_state.digital_buttons, XBOX_BACK) : bitClear(gamepad_state.digital_buttons, XBOX_BACK);

		((state & 4) && (state & 8)) ? bitSet(gamepad_state.digital_buttons, XBOX_RIGHT_STICK) : bitClear(gamepad_state.digital_buttons, XBOX_RIGHT_STICK);

		//press B and L at same time is BLACK
		if (((state & 256) > 0) && ((state & 1024) > 0)) {
			gamepad_state.black = 0xFF;

			gamepad_state.l = 0;
			gamepad_state.b = 0;
		}
		else {
			gamepad_state.black = 0;
		}
		//press Y and L at same time is WHITE
		if (((state & 512) > 0) && ((state & 1024) > 0)) {
			gamepad_state.white = 0xFF;

			gamepad_state.l = 0;
			gamepad_state.y = 0;
		}
		else {
			gamepad_state.white = 0;
		}

		//xbox_send_pad_state();
}

void ConsoleXbox::saturn(int* state) {

		(state[1] & SATURN_UP)    ? bitSet(gamepad_state.digital_buttons, XBOX_DPAD_UP)    : bitClear(gamepad_state.digital_buttons, XBOX_DPAD_UP);
		(state[1] & SATURN_DOWN)  ? bitSet(gamepad_state.digital_buttons, XBOX_DPAD_DOWN)  : bitClear(gamepad_state.digital_buttons, XBOX_DPAD_DOWN);
		(state[1] & SATURN_LEFT)  ? bitSet(gamepad_state.digital_buttons, XBOX_DPAD_LEFT)  : bitClear(gamepad_state.digital_buttons, XBOX_DPAD_LEFT);
		(state[1] & SATURN_RIGHT) ? bitSet(gamepad_state.digital_buttons, XBOX_DPAD_RIGHT) : bitClear(gamepad_state.digital_buttons, XBOX_DPAD_RIGHT);

		gamepad_state.a = ((state[2] & SATURN_A) > 0) * 0xFF;
		gamepad_state.b = ((state[2] & SATURN_B) > 0) * 0xFF;
		gamepad_state.x = ((state[0] & SATURN_X) > 0) * 0xFF;
		gamepad_state.y = ((state[0] & SATURN_Y) > 0) * 0xFF;

		gamepad_state.black = ((state[2] & SATURN_C) > 0) * 0xFF;
		gamepad_state.white = ((state[0] & SATURN_Z) > 0) * 0xFF;

		gamepad_state.l = (SATURN_L == state[3]) * 0xFF;
		gamepad_state.r = (state[0] & SATURN_R) * 0xFF;

		(state[2] & SATURN_START) ? bitSet(gamepad_state.digital_buttons, XBOX_START) : bitClear(gamepad_state.digital_buttons, XBOX_START);

		//press UP and START at same time is RIGHT ANALOG CLICK
		if ((state[1] & SATURN_UP) && (state[2] & SATURN_START)) {
			bitSet(gamepad_state.digital_buttons, XBOX_RIGHT_STICK);

			bitClear(gamepad_state.digital_buttons, XBOX_DPAD_UP);
			bitClear(gamepad_state.digital_buttons, XBOX_START);
		}
		else {
			bitClear(gamepad_state.digital_buttons, XBOX_RIGHT_STICK);
		}

		//press L and R at same time is XBOX_BACK
		if ( (SATURN_L == state[3]) && (state[0] & SATURN_R) ) {
			bitSet(gamepad_state.digital_buttons, XBOX_BACK);

			gamepad_state.l = 0;
			gamepad_state.r = 0;
		}
		else {
			bitClear(gamepad_state.digital_buttons, XBOX_BACK);
		}
		//xbox_send_pad_state();

}

void ConsoleXbox::idle() {
		bitClear(gamepad_state.digital_buttons, XBOX_DPAD_UP);
		bitClear(gamepad_state.digital_buttons, XBOX_DPAD_DOWN);
		bitClear(gamepad_state.digital_buttons, XBOX_DPAD_LEFT);
		bitClear(gamepad_state.digital_buttons, XBOX_DPAD_RIGHT);

		gamepad_state.a = 0;

		gamepad_state.b = 0;

		gamepad_state.black = 0;

		gamepad_state.x = 0;

		gamepad_state.y = 0;

		gamepad_state.white = 0;

		gamepad_state.l = 0;

		gamepad_state.r = 0;

		bitClear(gamepad_state.digital_buttons, XBOX_START);

		bitClear(gamepad_state.digital_buttons, XBOX_BACK);

		bitClear(gamepad_state.digital_buttons, XBOX_RIGHT_STICK);

		//xbox_send_pad_state();
}

