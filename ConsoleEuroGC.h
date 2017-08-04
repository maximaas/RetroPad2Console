/**
 * Gamecube controller protocol and console communication
 *
 * Copyright (C) 2015,2017 Maximaas
 * Version 1.0 - completed on 2015.12.09
 * Contact: mingzo@gmail.com
 */

#ifndef ConsoleEuroGC_h
#define ConsoleEuroGC_h

#include "Arduino.h"

//#include <inttypes.h>

#define PIN_NGC_DATA  2
//#define GC_HIGH DDRD &= ~0x04
//#define GC_LOW DDRD |= 0x04
//#define GC_QUERY (PIND & 0x04)
//#define GC_QUERY (PIND & (1<<PIN_NGC_DATA))

class ConsoleEuroGC {

  public:
	ConsoleEuroGC();
	ConsoleEuroGC(int data);
	void init();
	void check();
	byte xxxxx();
	bool communicate(bool detect);
	void idle();
	int checkConnection(int bit_count, byte bits);

	void reset();
    void snes2gc(int snes_state);
    void saturn2gc(int* saturn_state);

  private:
    int counter;


    void skip() {
		asm volatile (
			"; START OF MANUAL ASSEMBLY BLOCK\n"

			"L%=_wait_for_line_low:\n"
			"sbis %[port], 2\n"
			"rjmp L%=_line_low\n" // line is low
			"rjmp L%=_wait_for_line_low\n"

			"L%=_line_low:\n"
			"ldi r25, lo8(40)\n" //(1)
			"L%=_waste_loop:\n"
			"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
			"nop\nnop\nnop\nnop\nnop\n" // (10)
			"dec r25\n" //(1)
			"brne L%=_waste_loop\n" //(1/2)
			:
			:
			[port] "I" (_SFR_IO_ADDR(PIND))
			: "r25"
		);
    }

    int probe_header()
    {
    	// listen for the expected 3 bytes of data back from the controller and
    	// and pack it into the gc_status struct.
    	asm volatile (";Starting to listen");

    	unsigned int retval = 0;

		asm volatile (
    			// r25 is our bit counter. We read 8 bits and increment the byte
    			// pointer every 8 bits
    			"ldi r25, lo8(0)\n"

				//init r23 to non-zero,
				//in order to return non-zero number when no probing cwhenommand received.
				//(because the probing command is a zero)
				"ldi r23, lo8(8)\n"

    			"L%=_wait_for_signal:\n"
    			"sbic %[port], 2\n" //(2) reg 9 bit 2 is PIND2, or arduino I/O 2
    			"rjmp L%=_wait_for_signal\n" // loop if the counter isn't 0

    			"L%=_when_line_low:\n"
    			"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\n" // (2)

				"lsl r23\n" //(1) left shift
    			"sbic %[port],2\n" //(1/2) read PIND2
    			"sbr r23, lo8(1)\n" //(1) set bit 1 in value if PIND2 is high
				"mov %[retval], r23\n" //(1)
				"inc r25\n" //(1) increment bitcount
    			"cpi r25,lo8(8)\n" //(1) == 8?
    			"breq L%=_exit\n" //(1/2) jump to exit

    			"L%=_wait_for_line_high:\n"

				//when plug on a non-gc console, "_wait_for_line_high" will be a dead loop.
				//using r24 as a timeout counter to avoid.
				"inc r24\n"
    			"cpi r24,250\n" //(1) == 8?
    			"breq L%=_exit\n" //(1/2) jump to exit

    			"sbic %[port], 2\n" //(1/2) reg 9 bit 2 is PIND2, or arduino I/O 2
    			"rjmp L%=_wait_for_signal\n" //(2) line is high
    			"rjmp L%=_wait_for_line_high\n" //(2)

    			"L%=_exit:\n" // waste 2us
//				"mov %[retval], r23\n" //(1)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\n" // (2)

				";END OF MANUAL ASSEMBLY BLOCK\n"
    			// ----------
    			// outputs:
    			:
    			[retval] "+a" (retval)
    			:
    			[port] "I" (_SFR_IO_ADDR(PIND))
    			: "r25","24","23"
    	);

    	return retval;
    }

    void wait_finish_bit()
    {
    	// listen for the expected 3 bytes of data back from the controller and
    	// and pack it into the gc_status struct.
    	asm volatile (";Starting to listen");

		asm volatile (
				"L%=_finish_bit:\n" //waste 78 cycle to wait for high end of finish bit
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (8)

//    			"L%=_exit:\n" // waste 2us
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\n" // (2)

    			// ----------
    			// outputs:
    			:
    			:
    			:
    	);
    }

    byte probe_suffix()
    {
    	// listen for the expected 3 bytes of data back from the controller and
    	// and pack it into the gc_status struct.
    	asm volatile (";Starting to listen");

    	byte retval = 0;

		asm volatile (
    			// r25 is our bit counter. We read 8 bits and increment the byte
    			// pointer every 8 bits
    			"ldi r25, lo8(0)\n"

				"ldi r23, lo8(0)\n"

    			"L%=_wait_for_signal:\n"
    			"sbic %[port], 2\n" //(2) reg 9 bit 2 is PIND2, or arduino I/O 2
    			"rjmp L%=_wait_for_signal\n" // loop if the counter isn't 0

    			"L%=_when_line_low:\n"
    			"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\n" // (2)

				"lsl r23\n" //(1) left shift
    			"sbic %[port],2\n" //(1/2) read PIND2
    			"sbr r23, lo8(1)\n" //(1) set bit 1 in value if PIND2 is high
				"mov %[retval], r23\n" //(1)
				"inc r25\n" //(1) increment bitcount
    			"cpi r25,lo8(16)\n" //(1) == 8?
    			"breq L%=_finish_bit\n" //(1/2) jump to exit

    			"L%=_wait_for_line_high:\n"
    			"sbic %[port], 2\n" //(1/2) reg 9 bit 2 is PIND2, or arduino I/O 2
    			"rjmp L%=_wait_for_signal\n" //(2) line is high
    			"rjmp L%=_wait_for_line_high\n" //(2)

				"L%=_finish_bit:\n" //waste 78 cycle to wait for high end of finish bit
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (8)

    			"L%=_exit:\n" // waste 2us
//				"mov %[retval], r23\n" //(1)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\n" // (2)

				";END OF MANUAL ASSEMBLY BLOCK\n"
    			// ----------
    			// outputs:
    			:
    			[retval] "+a" (retval)
    			:
    			[port] "I" (_SFR_IO_ADDR(PIND))
    			: "r25","24","23"
    	);

    	return retval;
    }

//	void probe_suffix()
//	{
//		// listen for the expected 3 bytes of data back from the controller and
//		// and pack it into the gc_status struct.
//		asm volatile (";Starting to listen");
//
//		asm volatile (
//				// r25 is our bit counter. We read 8 bits and increment the byte
//				// pointer every 8 bits
//				"ldi r25, lo8(0)\n"
//
//				"L%=_wait_for_signal:\n"
//				"sbic %[port], 2\n" //(2) reg 9 bit 2 is PIND2, or arduino I/O 2
//				"rjmp L%=_wait_for_signal\n" // loop if the counter isn't 0
//
//				"L%=_when_line_low:\n"
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\n" // (2)
//
//				"inc r25\n" //(1) increment bitcount
//				"cpi r25,lo8(16)\n" //(1) == 8?
//				"breq L%=_finish_bit\n" //(1/2) jump to exit
//
//				"L%=_wait_for_line_high:\n"
//				"sbic %[port], 2\n" //(1/2) reg 9 bit 2 is PIND2, or arduino I/O 2
//				"rjmp L%=_wait_for_signal\n" //(2) line is high
//				"rjmp L%=_wait_for_line_high\n" //(2)
//
//				"L%=_finish_bit:\n" //waste 78 cycle to wait for high end of finish bit
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (8)
//
//				"L%=_exit:\n" // waste 2us
////				"mov %[retval], r23\n" //(1)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//				"nop\nnop\n" // (2)
//
//				";END OF MANUAL ASSEMBLY BLOCK\n"
//				// ----------
//				// outputs:
//				:
//				:
//				[port] "I" (_SFR_IO_ADDR(PIND))
//				: "r25","23"
//		);
//	}

    int send(unsigned char *buffer, char length)
    {
    	unsigned int retval = 0;
        asm volatile (
                "; Start of gc_send assembly\n"

                // passed in to this block are:
                // the Z register (r31:r30) is the buffer pointer
                // %[length] is the register holding the length of the buffer in bytes

                // Instruction cycles are noted in parentheses
                // branch instructions have two values, one if the branch isn't
                // taken and one if it is

                // r25 will be the current buffer byte loaded from memory
                // r26 will be the bit counter for the current byte. when this
                // reaches 0, we need to decrement the length counter, load
                // the next buffer byte, and loop. (if the length counter becomes
                // 0, that's our exit condition)

                "ld r25, Z\n" // load the first byte

                // This label starts the outer loop, which sends a single byte
                ".L%=_byte_loop:\n"
                "ldi r26, lo8(8)\n" // (1)

                // This label starts the inner loop, which sends a single bit
                ".L%=_bit_loop:\n"
                "sbi %[port], 2\n" // (2) pull the line low

                // line needs to stay low for 1¦Ìs for a 1 bit, 3¦Ìs for a 0 bit
                // this block figures out if the next bit is a 0 or a 1
                // the strategy here is to shift the register left, then test and
                // branch on the carry flag
                "lsl r25\n" // (1) shift left. MSB goes into carry bit of status reg
                "brcc .L%=_send_zero_bit\n" // (1/2) branch if carry is cleared


        		".L%=_send_one_bit:\n"
                // this block is the timing for a 1 bit (1¦Ìs low, 3¦Ìs high)
                // Stay low for 16 - 2 (above lsl,brcc) - 2 (below cbi) = 12 cycles
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
                "nop\nnop\n" // (2)
                "cbi %[port], 2\n" // (2) set the line high again
                // Now stay high for 2¦Ìs of the 3¦Ìs to sync up with the branch below
                // 2*16 - 2 (for the rjmp) = 30 cycles
                "nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
                "rjmp .L%=_finish_bit\n" // (2)


                // this block is the timing for a 0 bit (3¦Ìs low, 1¦Ìs high)
                // Need to go high in 3*16 - 3 (above lsl,brcc) - 2 (below cbi) = 43 cycles
                ".L%=_send_zero_bit:\n"
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
                "nop\nnop\nnop\n" // (3)
                "cbi %[port], 2\n" // (2) set the line high again


                // The two branches meet up here.
                // We are now *exactly* 3¦Ìs into the sending of a bit, and the line
                // is high again. We have 1¦Ìs to do the looping and iteration
                // logic.
                ".L%=_finish_bit:\n"
                "subi r26, 1\n" // (1) subtract 1 from our bit counter
                "breq .L%=_load_next_byte\n" // (1/2) branch if we've sent all the bits of this byte

                // At this point, we have more bits to send in this byte, but the
                // line must remain high for another 1¦Ìs (minus the above
                // instructions and the jump below and the sbi instruction at the
                // top of the loop)
                // 16 - 2(above) - 2 (rjmp below) - 2 (sbi after jump) = 10
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
                "rjmp .L%=_bit_loop\n"


                // This block starts 3 cycles into the last 1¦Ìs of the line being high
                // We need to decrement the byte counter. If it's 0, that's our exit condition.
                // If not we need to load the next byte and go to the top of the byte loop
                ".L%=_load_next_byte:\n"
                "subi %[length], 1\n" // (1)
                "breq .L%=_loop_exit\n" // (1/2) if the byte counter is 0, exit
                "adiw r30, 1\n" // (2) increment byte pointer
                "ld r25, Z\n" // (2) load the next byte
                // delay block:
                // needs to go high after 1¦Ìs or 16 cycles
                // 16 - 9 (above) - 2 (the jump itself) - 3 (after jump) = 2
                "nop\nnop\n" // (2)
                "rjmp .L%=_byte_loop\n" // (2)


                // Loop exit
                ".L%=_loop_exit:\n"

                // final task: send the stop bit, which is a 1 (1¦Ìs low 3¦Ìs high)
                // the line goes low in:
                // 16 - 6 (above since line went high) - 2 (sbi instruction below) = 8 cycles
                "nop\nnop\nnop\nnop\nnop\n" // (5)
                "nop\nnop\nnop\n" // (3)
                "sbi %[port],2\n" // (2) pull the line low
                // stay low for 1¦Ìs
                // 16 - 2 (below cbi) = 14
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
//        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
                "nop\nnop\nnop\nnop\n" // (4)
                "cbi %[port], 2\n" // (2) set the line high again
        		"nop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\nnop\n" // (10)
				"nop\nnop\nnop\nnop\n" // (6)
                // just stay high. no need to wait 3¦Ìs before returning

                :
                // outputs:
        		[retval] "+a" (retval),
                "+z" (buffer) // (read and write)
                :
                // inputs:
                [length] "a" (length),
                [port] "I" (_SFR_IO_ADDR(DDRD))
                :
                // clobbers:
                "r25", "r26"
		);

        return retval;
    }

//    int wait_and_send(unsigned char *buffer, char length)
//    {
//		wait_finish_bit();
//		return send(buffer, length);
//    }
};

#endif

