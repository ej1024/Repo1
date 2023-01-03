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
#include "./CORE/CORE_PIN.h"
#include "./CORE/clock.h"
#include "./CORE/delay.h"

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

	Pin LED_Pin(PB30);
	
	LED_Pin.Set_Direction(Output);
	LED_Pin.Set_Pull_Up_Mode(Pull_Up);

	while(1){
		delay_s(1);
		PORT->Group[1].OUTTGL.reg = 1 << 30;
	}


}




