//////////////////////////////////////////////////////////////////////////
// Include and defines
//////////////////////////////////////////////////////////////////////////
#include "app.h"
#include "UART_3.h"

void UART_3_Class::Init_UART(uint32_t baud, uint32_t F_CPU_Hz)
{
	
	/* -------------------------------------------------
	* 1) Enable bus clock to APBC mask
	*/
	REG_PM_APBCMASK |=  PM_APBCMASK_SERCOM3;
	
    /* -------------------------------------------------
	* 2) select UART clock
	*/
	GCLK->CLKCTRL.reg =  
	GCLK_CLKCTRL_ID(SERCOM3_GCLK_ID_CORE) |  // select the UART clock
	GCLK_CLKCTRL_CLKEN |                     // enable the clock
    GCLK_CLKCTRL_GEN(0);                     // set GCLK GEN(0)
	
	/* --------------------------------------------------
	* 3) Configure PA22 as TX Pin
	*/
	// Set pin direction to output
	PORT->Group[0].DIRSET.reg = (1 << 22);     
	
    // Enable PMUX
	PORT->Group[0].PINCFG[22].reg |= PORT_PINCFG_PMUXEN;   
	
	// Enable PMUX and set the PMUX bit, since pin is even we use PMUXE
	PORT->Group[0].PMUX[22>>1].bit.PMUXE = PORT_PMUX_PMUXE_C_Val;
	
	
	/* ---------------------------------------------------
	* 4) Configure PA23 as RX Pin
	*/
	// Set pin direction to input
	PORT->Group[0].DIRCLR.reg = (1 << 23);       

	// Enable pull down resistor
	PORT->Group[0].PINCFG[23].reg &= ~PORT_PINCFG_PULLEN;   
	
	// Enable PMUX and set the PMUX bit, since pin is odd we use PMUX0
	PORT->Group[0].PINCFG[23].reg |= PORT_PINCFG_PMUXEN; 
	PORT->Group[0].PMUX[23>>1].bit.PMUXO = PORT_PMUX_PMUXO_C_Val; 
	
	
	/* ----------------------------------------------------
	* 5) Configure USART via Control A and Control B 
	*/
	SERCOM3->USART.CTRLA.reg =                  // USART is ASYNCHRONOUS
	   SERCOM_USART_CTRLA_DORD |                // Transmit LSB First
	   SERCOM_USART_CTRLA_MODE_USART_INT_CLK |  // Set Internal Clock 
	   SERCOM_USART_CTRLA_RXPO(1) |             // Use SERCOM pad 1 for data reception
	   SERCOM_USART_CTRLA_TXPO(0/*PAD0*/);      // Set SERCOM pad 0 for data transmission
	
	
	SERCOM3->USART.CTRLB.reg =        // We don't use PARITY
	    SERCOM_USART_CTRLB_RXEN |     // Enable receive when USART in enabled
		SERCOM_USART_CTRLB_TXEN |     // Enable transmit when USART is enabled
		SERCOM_USART_CTRLB_CHSIZE(0); // Set character size to 8 bits
	
	/* -----------------------------------------------------
	* 6) Set USART Baud Rate
	*/
	// Baud rate is (65536) * (CPU_CLock - 16 * wanted baud) / CPU_Clock
	uint64_t baudRate = (uint64_t)65536 * (F_CPU_Hz - 16 * baud) / F_CPU_Hz;
	
	// Set Baud Rate
	SERCOM3->USART.BAUD.reg = (uint32_t)baudRate;
	

    /* ------------------------------------------------------
	* 7) Enable the USART
	*/
	// SERCOM3 peripheral enabled
	SERCOM3->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
}    

void UART_3_Class::Clear_Screen()
{
        Write_String("\x1b[2J");//clear screen
        Write_String("\x1b[f");//move to top left
}

/*******************************************************************************
 * Function:        void UART3_Write(char data)
 *
 * PreCondition:    The UART must be initialized
 *
 * Input:           The character we want to send
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function writes a character to the UART module
 *                  
 *
 * Note:            
 *
 ******************************************************************************/
void UART_3_Class::Write_Char(char data)
{
	// Wait on interrupt flag and Write some data
	while(!(REG_SERCOM3_USART_INTFLAG) & 1)
	{
		
	}
	
	REG_SERCOM3_USART_DATA = data;
} //UART3_Write()


/*******************************************************************************
 * Function:        void Write_String(char *text)
 *
 * PreCondition:    The UART must be initialized
 *
 * Input:           This text we want to send
 *
 * Output:          None
 *
 * Side Effects:    None
 *
 * Overview:        This function writes a string to the UART module
 *                  
 *
 * Note:            Must fix ISO forbids converting a string constant to char warning
 *
 ******************************************************************************/
void UART_3_Class::Write_String(char *text)
{
	// we write text until we reach EOL
	for(int i=0;text[i]!='\0';i++)
	{
		Write_Char(text[i]);
	}
	
} // Write_String()
