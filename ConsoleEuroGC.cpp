/**
 * Gamecube controller protocol and console communication
 *
 * Copyright (C) 2015,2017 Maximaas
 * Version 1.0 - completed on 2015.12.09
 * Contact: mingzo@gmail.com
 */

#include "ConsoleEuroGC.h"

#include "SnesPad.h"
#include "SaturnPad.h"

byte PROBING_COMMAND = B00000000;

/*
 * TODO
 * Gameboy Interface controller protocol has two more probing command at the beginning.
 * the first probing command is B11111111,
 * the second probing command is like the normal,
 * after that the standard gamecube handshaking process will begin,
 * that means the controller still need to handle a probing command before next moves.
 *
 * byte PROBING_COMMAND_GBI = B00000000;
*/

byte FIRST_POLLING_COMMAND = B01000001;

//NGC -- 01000000 00000011 00000000 1
byte POLLING_COMMAND = B01000000;

//00001001 00000000 00100000 1
//    ^  ^                   ^
//	  |  |                   +--- Stop bit
//	  |  +-- 0: non standard controller, 1: Dolphin standard controller
//	  +----- 0: N64, 1: Dolphin
unsigned char DEFAULT_ID_PACKET[3] = {
		0x09, 0x00, 0x20
};

//Byte 0	0	0	0	Start	Y	X	B	A
//Byte 1	1	L	R	Z	D-Up	D-Down	D-Right	D-Left
//Byte 2	Joystick X Value (8 bit)
//Byte 3	Joystick Y Value (8 bit)
//Byte 4	C-Stick X Value (8 bit)
//Byte 5	C-Stick Y Value (8 bit)
//Byte 6	Left Button Value (8 bit) - may be 4-bit mode also?
//Byte 7	Right Button Value (8 bit) - may be 4-bit mode also?
//unsigned char FIRST_REPORT_PACKET[10] = {
//		B00000000, B10000000, B10000000, B10000000,
//		B10000000, B10000000, B00000000, B00000000,
//		B00000000, B00000000
//};
//unsigned char DEFAULT_REPORT_PACKET[8] = {
//		B00000000, B10000000, B10000000, B10000000,
//		B10000000, B10000000, B00000000, B00000000
//};

unsigned char FIRST_REPORT_PACKET[10] = {
		B00000000, B10000000, B01111100, B01111101,B10000000, B01111101, B00100000, B00100010,
		B00000000, B00000000
};
unsigned char DEFAULT_REPORT_PACKET[8] = {
		B00000000, B10000000, B01111100, B01111101,B10000000, B01111101, B00100000, B00100010
};

unsigned char report_packet[8];

//pin 2
ConsoleEuroGC::ConsoleEuroGC()
{
	//pinMode(PIN_NGC_DATA, INPUT);
	DDRD |= ~(1 << PIN_NGC_DATA);
	counter = 0;
}

bool ConsoleEuroGC::communicate(bool detect) {
//	bool detected = false;

	byte flag = 0;

	noInterrupts();

	//read probing command then report controller id
	flag = probe_header();

	if (flag == PROBING_COMMAND) {

		//Detect if its a NGC console
		//On receiving PROBING_COMMAND, means the pad is connecting to a ngc console
		if (detect)
			return true;

		wait_finish_bit();
		send(DEFAULT_ID_PACKET, 3);

		flag = probe_header();

		if (flag == FIRST_POLLING_COMMAND) {
			wait_finish_bit();
			send(FIRST_REPORT_PACKET, 10);

			counter = 0;
		}
	}
	else if (flag == POLLING_COMMAND) {
		probe_suffix();
		send(report_packet, 8);

		counter++;
		idle();
	}
	else {
		skip();
	}

	interrupts();

	return false;
}

/**
 * In GBP mode, the regular interval between commands is about 570us,
 * but after every 18 commands the interval changes to 128us,
 * so the 18 commands composes a command set.
 *
 * Because the operations in the main loop need more time longer than 128us.
 * so we need to execute the 19th command right after the 18th command before it went back to the main loop
 *
 * we use a "counter" variable to locate where the 18th command is.
 * when polling stage the counter is marked as 0
 * and when we exe the 19th command, the counter is set to 1, to indicate a new command set starts.
 *
 */
void ConsoleEuroGC::idle() {
	if (counter == 18) {
		probe_header();
		probe_suffix();
		send(report_packet, 8);

		counter = 1;
	}
}

void ConsoleEuroGC::reset() {
	report_packet[0] = DEFAULT_REPORT_PACKET[0];
	report_packet[1] = DEFAULT_REPORT_PACKET[1];
	report_packet[2] = DEFAULT_REPORT_PACKET[2];
	report_packet[3] = DEFAULT_REPORT_PACKET[3];
	report_packet[4] = DEFAULT_REPORT_PACKET[4];
	report_packet[5] = DEFAULT_REPORT_PACKET[5];
	report_packet[6] = DEFAULT_REPORT_PACKET[6];
	report_packet[7] = DEFAULT_REPORT_PACKET[7];
}

void ConsoleEuroGC::snes2gc(int state_snes) {

	//BYTE 0
	if (state_snes & SNES_START) {
		report_packet[0] |= B00010000;
	}
	if (state_snes & SNES_Y) {
		report_packet[0] |= B00001000;
	}
	if (state_snes & SNES_X) {
		report_packet[0] |= B00000100;
	}
	if (state_snes & SNES_B) {
		report_packet[0] |= B00000010;
	}
	if (state_snes & SNES_A) {
		report_packet[0] |= B00000001;
	}

	//BYTE 1
	if (state_snes & SNES_L) {
		report_packet[1] |= B11000000;
	}
	if (state_snes & SNES_R) {
		report_packet[1] |= B10100000;
	}
	if (state_snes & SNES_SELECT) { //map SNES 'select' button to GC 'z' button
		report_packet[1] |= B10010000;
	}
	if (state_snes & SNES_UP) {
		report_packet[1] |= B10001000;
	}
	if (state_snes & SNES_DOWN) {
		report_packet[1] |= B10000100;
	}
	if (state_snes & SNES_RIGHT) {
		report_packet[1] |= B10000010;
	}
	if (state_snes & SNES_LEFT) {
		report_packet[1] |= B10000001;
	}

	//BYTE 2
	//skip joystick X
	report_packet[2] |= B01111100;

	//BYTE 3
	//skip joystick Y
	report_packet[3] |= B01111101;

	//BYTE 4
	if (state_snes == (SNES_LEFT | SNES_SELECT)) {
		report_packet[4] = B11111111;
	}
	if (state_snes == (SNES_RIGHT | SNES_SELECT)) {
		report_packet[4] = B00000000;
	}

	//BYTE 5
	if (state_snes == (SNES_UP | SNES_SELECT)) {
		report_packet[5] = B11111111;
	}
	if (state_snes == (SNES_DOWN | SNES_SELECT)) {
		report_packet[5] = B00000000;
	}

	//BYTE 6
	//skip L button value
	report_packet[6] |= B00100000;

	//BYTE 7
	//skip R button value
	report_packet[7] |= B00100010;

}

void ConsoleEuroGC::saturn2gc(int* state_saturn) {

	//BYTE 0
	if (state_saturn[2] & SATURN_START) {
		report_packet[0] |= B00010000;
	}
	if (state_saturn[0] & SATURN_Y) {
		report_packet[0] |= B00001000;
	}
	if (state_saturn[0] & SATURN_X) {
		report_packet[0] |= B00000100;
	}
	if (state_saturn[2] & SATURN_B) {
		report_packet[0] |= B00000010;
	}
	if (state_saturn[2] & SATURN_A) {
		report_packet[0] |= B00000001;
	}

	//BYTE 1
	if (state_saturn[0] & SATURN_Z) {
		report_packet[1] |= B11000000;
	}
	if (state_saturn[2] & SATURN_C) {
		report_packet[1] |= B10100000;
	}
	if (state_saturn[0] & SATURN_R) { //map SATURN 'r' button to GC 'z' button
		report_packet[1] |= B10010000;
	}
	if (state_saturn[1] & SATURN_UP) {
		report_packet[1] |= B10001000;
	}
	if (state_saturn[1] & SATURN_DOWN) {
		report_packet[1] |= B10000100;
	}
	if (state_saturn[1] & SATURN_RIGHT) {
		report_packet[1] |= B10000010;
	}
	if (state_saturn[1] & SATURN_LEFT) {
		report_packet[1] |= B10000001;
	}

	//BYTE 2
	//skip joystick X
	report_packet[2] |= B01111100;

	//BYTE 3
	//skip joystick Y
	report_packet[3] |= B01111101;

	//BYTE 4
	if ((state_saturn[1] == SATURN_LEFT) && (state_saturn[3] == SATURN_L)) {
		report_packet[4] = B11111111;
	}
	if ((state_saturn[1] == SATURN_RIGHT) && (state_saturn[3] == SATURN_L)) {
		report_packet[4] = B00000000;
	}

	//BYTE 5
	if ((state_saturn[1] == SATURN_UP) && (state_saturn[3] == SATURN_L)) {
		report_packet[5] = B11111111;
	}
	if ((state_saturn[1] == SATURN_DOWN) && (state_saturn[3] == SATURN_L)) {
		report_packet[5] = B00000000;
	}

	//BYTE 6
	//skip L button value
	report_packet[6] |= B00100000;

	//BYTE 7
	//skip R button value
	report_packet[7] |= B00100010;
}
