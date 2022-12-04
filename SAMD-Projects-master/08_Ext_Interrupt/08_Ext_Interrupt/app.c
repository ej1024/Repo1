/*
 * File:   app.c
 * Author: Armstrong Subero
 * Processor: SAMD21G18A @ 48MHz, 3.3v
 * Program: Source file for application
 * Compiler: ARM-GCC (v6.3.1, Atmel Studio 7.0)
 * Program Version 1.0
 * Program Description: This file contains your application source
 *
 * Modified From: None
 * 
 * Change History:
 *
 * Author             Rev     Date          Description
 * Armstrong Subero   1.0     31/05/2020    Initial Release.
 * 
 * Updated on May 31, 2020, 11:22 AM
 */

//////////////////////////////////////////////////////////////////////////
// Include and defines
//////////////////////////////////////////////////////////////////////////
#include "app.h"
#include "clock.h"

#define F_CPU 48000000UL
#include "delay.h"

/*******************************************************************************
 * Function:        void AppInit(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This routine takes care of all of the 1-time hardware/software
 *                  initialization that is required.
 *
 * Note:            This was taken from the _00_LED_ON provided by
 *                  Microchip Technology Inc. 
 *
 ******************************************************************************/
void AppInit(void)
{
	/*	Clock initialization (CPU, AHB, APBx, Asynchronous Peripheral Clocks)
		The System RC Oscillator (RCSYS) provides the source for the main clock
		at chip startup. It is set to 1MHz.
	*/
	ClocksInit();
	
	//LED Configuration
	PORT->Group[PORTB].PINCFG[LED0_PIN_NUMBER].reg = PORT_PINCFG_PULLEN;
	PORT->Group[PORTB].DIRSET.reg = 1 << LED0_PIN_NUMBER;
	PORT->Group[PORTB].OUTSET.reg = 1 << LED0_PIN_NUMBER;

	//SW Configuration
	PORT->Group[PORTA].PINCFG[EXT0_PIN_NUMBER].reg = PORT_PINCFG_PULLEN | PORT_PINCFG_INEN | PORT_PINCFG_PMUXEN;
	PORT->Group[PORTA].PMUX[7].bit.PMUXO = MUX_PA15A_EIC_EXTINT15;
	PORT->Group[PORTA].DIRCLR.reg = 1 << EXT0_PIN_NUMBER;	// make input
	PORT->Group[PORTA].OUTSET.reg = 1 << EXT0_PIN_NUMBER;	// needs to set as high because switch is switch to gnd

} // AppInit()




/*******************************************************************************
 * Function:        void AppRun(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function contains your main application
 *                  
 *
 * Note:
 *
 ******************************************************************************/

void AppRun(void)
{
    /* -------------------------------------------------
	* 1) Disable Interrupt and Set Priority
	*/
	// disable external interrupt controller
	NVIC_DisableIRQ(EIC_IRQn);
	
	// clear device specific interrupt from pending
	NVIC_ClearPendingIRQ(EIC_IRQn);
	
	// set interrupt priority
	NVIC_SetPriority(EIC_IRQn, 0);
	
  
    /* -------------------------------------------------
	* 2) Setup Interrupt Pin
	*/
    // PORT->Group[EXT0_PORT].PINCFG[EXT0_PIN_NUMBER].reg = PORT_PINCFG_PMUXEN | PORT_PINCFG_PULLEN; // Pin pullup and enable MUX'ing
    // PORT->Group[EXT0_PORT].PMUX[15].bit.PMUXE = MUX_PA15A_EIC_EXTINT15; // Set up IRQ-pin (PA7) to special function A (External Interrupt 7)
	/* -------------------------------------------------
	* 3) select EIC clock
	*/
	GCLK->CLKCTRL.reg = 
	GCLK_CLKCTRL_ID(EIC_GCLK_ID) |  // select EIC clock
	GCLK_CLKCTRL_CLKEN |            // enable the clock
	GCLK_CLKCTRL_GEN(0);            //  select GCLK GEN0
	
	
	/* -------------------------------------------------
	* 4) Set to Vanilla State
	*/
	// software reset on EIC
	EIC->CTRL.bit.SWRST = 1;
	
	// wait for synchronization
	while ((EIC->CTRL.bit.SWRST == 1) && (EIC->STATUS.bit.SYNCBUSY == 1))
	{
		
	}
	
	
	/* -------------------------------------------------
	* 5) Configure and Enable Interrupt
	*/
	
	// falling edge detection
	// filter 7 enable
	// config 0 for EXTINT 0-7 and config 1 for EXTINT 8-15
	EIC->CONFIG[1].reg |= EIC_CONFIG_SENSE7_FALL | EIC_CONFIG_FILTEN7;
	
    
    // enable the interrupt
	EIC->INTENSET.bit.EXTINT15 = 1;
	EIC->CTRL.bit.ENABLE = 1;
	
	// wait for synchronization	
	while(EIC->STATUS.bit.SYNCBUSY == 1)
	{
		
	}
	
	// enable the interrupt
	NVIC_EnableIRQ(EIC_IRQn);

	
	while(1)
	{
        // regular state toggle LED
		REG_PORT_OUTSET1 = LED0_PIN_MASK;
		delay_ms(100);
	}

} // Apprun()



/*******************************************************************************
 * Function:        void EIC_Handler(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function is our external interrupt handler
 *                  
 *
 * Note:
 *
 ******************************************************************************/
void EIC_Handler(void)
{
	// if external interrupt detected
	if ( (EIC->INTFLAG.reg & (EXT0_PIN_MASK) ) != 0 )
	{
		// Turn the LED on PA17 ON
		REG_PORT_OUTCLR1 = LED0_PIN_MASK;
		
		// delay 3000 ms
		delay_ms(3000);

		// clear interrupt flag
		EIC->INTFLAG.reg = (EXT0_PIN_MASK);
	}
}