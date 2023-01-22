/*
 * File:   USART3.c
 * Author: Armstrong Subero
 * Processor: SAMD21G18A @ 48MHz, 3.3v
 * Program: Source file for application
 * Compiler: ARM-GCC (v6.3.1, Atmel Studio 7.0)
 * Program Version 1.0
 * Program Description: This file contains source code for 
 *                      using the I2C module
 *
 * Modified From: None
 * 
 * Change History:
 *
 * Author                Rev     Date          Description
 * Armstrong Subero      1.0     02/06/2020    Initial Release
 *
 * Updated on June 02nd, 2020, 05:07 PM
 */ 


//////////////////////////////////////////////////////////////////////////
// Include and defines
//////////////////////////////////////////////////////////////////////////
#include "app.h"
#include "I2C1.h"


/*******************************************************************************
 * Function:        void I2C1_init(void)
 *
 * PreCondition:    None
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function initializes the I2C Module
 *                  
 *
 * Note:            Defaults at Standard Speed
 *
 ******************************************************************************/
void I2C1_init(void)
{
	   /* -------------------------------------------------
	   * 1) Enable bus clock to APBC mask
	   */
	   PM->APBCMASK.reg |= PM_APBCMASK_SERCOM0;								
	   while (SERCOM0->I2CM.SYNCBUSY.bit.SYSOP); // Wait for synchronization.
	  
	  
	   /* -------------------------------------------------
	   * 2) select I2C clock
	   */
	   GCLK->CLKCTRL.reg = 
	   GCLK_CLKCTRL_ID(SERCOM0_GCLK_ID_CORE) |				
	   GCLK_CLKCTRL_CLKEN | 
	   GCLK_CLKCTRL_GEN(3);  // we use the 8 MHz clock 
	 
		
	   /* -------------------------------------------------
	   * 3) Setup Pins
	   */
	   // Set PA08 as SDA and PA09 as SCL
	   PORT->Group[0].PINCFG[PIN_PA08].reg = 1;
	   PORT->Group[0].PINCFG[PIN_PA09].reg = 1;
		
		// Enable pull down resistor
		PORT->Group[0].PINCFG[8].reg |= PORT_PINCFG_PULLEN;
		PORT->Group[0].PINCFG[9].reg |= PORT_PINCFG_PULLEN;	
		
	   PORT->Group[0].PMUX[PIN_PA08>>1].bit.PMUXE = 0x02;
	   PORT->Group[0].PMUX[PIN_PA09>>1].bit.PMUXO = 0x02;
	
	   /* -------------------------------------------------
	   * 4) Configure I2C Module Control A
	   */ 
	   SERCOM_I2CM_CTRLA_Type SERCOM0_CTRLA =
	   {
		     .bit.LOWTOUTEN = 0, // Time-out disabled.
		     .bit.INACTOUT  = 0, // Inactive bus time-out disabled.
		     .bit.SPEED     = 0, // Standard speed 100 kHz
		     .bit.SEXTTOEN  = 0, // Slave SCL Low Extend Time-Out disabled.
		     .bit.SDAHOLD   = 0x2, // SDA hold time 600ns.
		     .bit.PINOUT    = 0, // 4-wire operation disabled.
		     .bit.RUNSTDBY  = 1, // Enabled in all modes.
		     .bit.MODE      = 0x5, // Mode = master.
		     .bit.ENABLE    = 0, // Don't enable SERCOM yet.
		     .bit.SWRST     = 0 // No SW resetting.
	     };
	 
	     // write our settings
	     // wait for synchronization
	     SERCOM0->I2CM.CTRLA.reg = SERCOM0_CTRLA.reg; 
	     while (SERCOM0->I2CM.SYNCBUSY.reg & SERCOM_I2CM_SYNCBUSY_ENABLE); 

      /* -------------------------------------------------
	  * 5) Configure I2C Module Control B
	  */ 
	  
	  // Setup SERCOM CTRLB.
	  SERCOM_I2CM_CTRLB_Type SERCOM0_CTRLB = 
	  {
		.bit.SMEN = 1 // Enable 'smart mode', sends acknowledge when Data.Data is read.
	  };
	  
	  // write our settings
	  // wait for synchronization
	  SERCOM0->I2CM.CTRLB.reg = SERCOM0_CTRLB.reg;
	  while (SERCOM0->I2CM.SYNCBUSY.bit.SYSOP);
	  
	  /* -------------------------------------------------
	  * 6) Set the baud rate
	  */ 
	  // Set the Baud rate.
	  SERCOM0->I2CM.BAUD.reg = 
	   SERCOM_I2CM_BAUD_BAUD(30) | 
	   SERCOM_I2CM_BAUD_BAUDLOW(30);
	   
	 // Wait for synchronization.  
 	 while (SERCOM0->I2CM.SYNCBUSY.bit.SYSOP); 


      /* -------------------------------------------------
	  * 7) enable the module and force bus idle
	  */ 
	   // Enable SERCOM I2C 
	   SERCOM0->I2CM.CTRLA.bit.ENABLE = 1;

	  // Bus is forced into idle state.
	  SERCOM0->I2CM.STATUS.bit.BUSSTATE = 0x1;
	  while (SERCOM0->I2CM.SYNCBUSY.bit.SYSOP); // Wait for synchronization.
} // I2C1_init()


/*******************************************************************************
 * Function:        void I2C1_write_byte(unsigned char device_addr, 
 *                      unsigned char mem_addr, unsigned char data)
 *
 * PreCondition:    The I2C bus must have been initialized
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function writes a byte to the I2C bus
 *                  
 *
 * Note:            
 *
 ******************************************************************************/
void I2C1_write_byte(unsigned char device_addr, unsigned char mem_addr, unsigned char data) 
{ 
	// wait sync
	while(SERCOM0->I2CM.SYNCBUSY.bit.SYSOP == 1);
	
	// send device address
	// wait until device address sent
	SERCOM0->I2CM.ADDR.reg = (36<<1)  | 0;  
	while (0 == (SERCOM0->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)); 
    
	// send memory address
	// wait until memory address sent
	SERCOM0->I2CM.DATA.reg = 0x0F;    
	while (0 == (SERCOM0->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));
	
	SERCOM0->I2CM.DATA.reg = 0x10;    
	while (0 == (SERCOM0->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB));

    // send data
	// wait until data sent
	SERCOM0->I2CM.DATA.reg = data;            
    while (0 == (SERCOM0->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_MB)); 

    // issue stop command
	SERCOM0->I2CM.CTRLB.reg = SERCOM_I2CM_CTRLB_CMD(3); 
} // I2C1_write_byte()



/*******************************************************************************
 * Function:        void I2C1_read_byte(unsigned char device_addr, 
 *                     unsigned char mem_addr, unsigned char* data) 
 *
 * PreCondition:    The I2C bus must have been initialized
 *
 * Input:           None
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function reads a byte from the I2C bus
 *                  
 *
 * Note:            
 *
 ******************************************************************************/
void I2C1_read_byte(unsigned char device_addr, unsigned char mem_addr, unsigned char* data) 
{
	// wait sync
	while(SERCOM0->I2CM.SYNCBUSY.bit.SYSOP == 1);
	
	// send device address
	// wait until device address sent
	SERCOM0->I2CM.ADDR.reg = 0x36 << 1;       
	while (0 == (SERCOM0->I2CM.INTFLAG.reg &  SERCOM_I2CM_INTFLAG_MB));
	
	// send memory address
	// wait until memory address sent
	SERCOM0->I2CM.DATA.reg = 0x0F;              
	while(0 == (SERCOM0->I2CM.INTFLAG.reg &  SERCOM_I2CM_INTFLAG_MB)); 

	// send memory address
	// wait until memory address sent
	SERCOM0->I2CM.DATA.reg = 0x10;              
	while(0 == (SERCOM0->I2CM.INTFLAG.reg &  SERCOM_I2CM_INTFLAG_MB)); 
	
	// send restart to slave
	// and wait until data is received with the SB (stop received) condition
	SERCOM0->I2CM.CTRLB.reg &= ~SERCOM_I2CM_CTRLB_ACKACT;
    SERCOM0->I2CM.ADDR.reg = (0x36 << 1) | 1;   
	while(0 == (SERCOM0->I2CM.INTFLAG.reg &  SERCOM_I2CM_INTFLAG_SB));
	*data = SERCOM0->I2CM.DATA.reg; 
    
	// read data from slave
	// and wait until finish reading with the SB (stop received) condition
	while (0 == (SERCOM0->I2CM.INTFLAG.reg & SERCOM_I2CM_INTFLAG_SB));  
	
	SERCOM0->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_ACKACT;
	SERCOM0->I2CM.CTRLB.reg |= SERCOM_I2CM_CTRLB_CMD(3);
	
	data++;
	*data = SERCOM0->I2CM.DATA.reg;  

	// and issue stop command
}// I2C1_read_byte()
