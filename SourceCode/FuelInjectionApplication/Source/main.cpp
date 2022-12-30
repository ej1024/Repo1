/*
 * File:   main.c
 * Author: Armstrong Subero
 * Processor: SAMD21J18A @ 48MHz, 3.3v
 * Program: Source file for application
 * Compiler: ARM-GCC (v6.3.1, Atmel Studio 7.0)
 * Program Version 1.0
 * Program Description: This is the main file
 * Hardware Description: A CP2014 USB-UART module is connected through a logic
 *                       level converter as follows:
 *
 *                       GND -> GND
 *                       TX -> PIN32 (PA23)   
 *                       RX -> PIN31 (PA22)
 *
 * Modified From: None
 * 
 * Change History:
 *
 * Author                Rev     Date          Description
 * Armstrong Subero      1.1     26/05/2020    Initial Release
 *
 * Updated on 2022-12-30 00:36:16
 */


//////////////////////////////////////////////////////////////////////////
// Include and Defines
//////////////////////////////////////////////////////////////////////////
#include "CORE_PIN.h"

/**
 * @brief Application Main 
 * 
 * @return int 
 */
int main(void)
{
	/*	Clock initialization (CPU, AHB, APBx, Asynchronous Peripheral Clocks)
		The System RC Oscillator (RCSYS) provides the source for the main clock
		at chip startup. It is set to 1MHz.
	*/
	ClocksInit();
	UART3_Init(115200);
	
	PORT->Group[PORTB].PINCFG[15].reg = PORT_PINCFG_PULLEN;
	PORT->Group[PORTB].DIRSET.reg = 1 << 15;

	// Assign LED0 as OUTPUT
	PORT->Group[PORTB].PINCFG[LED0_PIN_NUMBER].reg = PORT_PINCFG_PULLEN;
	PORT->Group[PORTB].DIRSET.reg = 1 << LED0_PIN_NUMBER;
	PORT->Group[PORTB].OUTSET.reg = 1 << LED0_PIN_NUMBER;

	char c[] = "Juice Box C++ UART V0.1\r\n";
	UART3_Write_Text((char*)&c);

	//check for conditions to continue looping
	//scheduler implemnetation tbd
	//MAIN Loop
	while(1)
	{
		//Toggles at 750kHz or 673ns per iter
		//661kHz 756ns

		// Check to see if we have data
		if (UART3_Has_Data()) 
		{
			// Read Data from UART3
			char c = UART3_Read();
			
			// Write Back the Data We Read
			UART3_Write(c);
		}
	}

}




