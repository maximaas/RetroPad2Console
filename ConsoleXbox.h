/**
 * Original Xbox controller protocol and console communication
 *
 * Copyright (C) 2015,2017 Maximaas
 * Version 1.0 - completed on 2015.12.09
 * Contact: mingzo@gmail.com
 */
#ifndef ConsoleXbox_h
#define ConsoleXbox_h

#include "Arduino.h"

class ConsoleXbox {

  public:
	ConsoleXbox();
	void init();

	bool detect();

	void snes(int state);
	void saturn(int* state);
	void idle();
};

#endif

