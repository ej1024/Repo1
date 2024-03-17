/*
 * @file System_Clocks.cpp
 *
 * Created: 5/1/2023 2:44:50 PM
 *  Author: a1035160
 */ 

#include "System_Clocks.h"


#define ERROR_CODE 0

#if (ERROR_CODE == 1)
#error "Hey user! Incorrect clock frequency."
#endif


bool External_Oscillator::Get_On_Demand(void)
{
	return (*XOSC_REG & (1 << XOSC_ONDEMAND_BIT_LOCATION)) == (1 << XOSC_ONDEMAND_BIT_LOCATION);
}

void External_Oscillator::Set_On_Demand(bool Set_State)
{
	if(Set_State)
		*XOSC_REG = *XOSC_REG |   (1 << XOSC_ONDEMAND_BIT_LOCATION);
	else
		*XOSC_REG = *XOSC_REG & (!(1 << XOSC_ONDEMAND_BIT_LOCATION));
}

bool External_Oscillator::Get_Run_In_Standby(void)
{
	return (*XOSC_REG & (1 << XOSC_RUN_IN_STANDBY_BIT_LOCATION))== (1 << XOSC_RUN_IN_STANDBY_BIT_LOCATION);
}

void External_Oscillator::Set_Run_In_Standby(bool Set_State)
{
	if(Set_State)
	*XOSC_REG = *XOSC_REG |   (1 << XOSC_RUN_IN_STANDBY_BIT_LOCATION);
	else
	*XOSC_REG = *XOSC_REG & (!(1 << XOSC_RUN_IN_STANDBY_BIT_LOCATION));
}

void External_Oscillator::Set_Oscillator_Startup_Time(Ext_Oscillator_Startup_Time Set_Oscillator_Startup_Time)
{
	//page 49 in the datasheet 11. Peripherals Configuration Summary
	//11. Peripherals Configuration Summary
	//#define SYSCTRL                       (0x40000800) /**< \brief (SYSCTRL) APB Base Address */
	
	//page 165 of the datasheet
	//16.7 Register Summary (which is the SYSCTRL section but that description in the datasheet is missing this)
	//__IO SYSCTRL_XOSC_Type         XOSC;        /**< \brief Offset: 0x10 (R/W 16) External Multipurpose Crystal Oscillator (XOSC) Control */
	
	//Set_Oscillator_Startup_Time;
	
	const int mask = !((2^SYSCTRL_XOSC_STARTUP_SIZE_BITS) << SYSCTRL_XOSC_STARTUP_BIT_SHIFT);
	
	*XOSC_REG = (*XOSC_REG & mask) | (Set_Oscillator_Startup_Time << SYSCTRL_XOSC_STARTUP_BIT_SHIFT);
}

Ext_Oscillator_Startup_Time External_Oscillator::Get_Oscillator_Startup_Time(void)
{
	const int mask = !((2^SYSCTRL_XOSC_STARTUP_SIZE_BITS) << SYSCTRL_XOSC_STARTUP_BIT_SHIFT);
	
	return (enum Ext_Oscillator_Startup_Time)((*XOSC_REG & mask) >> SYSCTRL_XOSC_STARTUP_BIT_SHIFT);
}

int External_Crystal_32kHz_Oscillator::Get_Clock_Frequency_Hz(void)
{
	return 32768;
}

int Internal_Oscillator_32kHz_High_Accuracy::Get_Clock_Frequency_Hz(void)
{
	return 32768;
}

int Internal_8MHz_Oscillator::Get_Clock_Frequency_Hz(void)
{
	return 8000000;
}

int Internal_Oscillator_32kHz_Ultra_Low_Power::Get_Clock_Frequency_Hz(void)
{
	return 32768;
}

void GENERIC_CLOCK_GENERATOR::Set_Clock_Source(Generic_Clock_Source Set_Clk_Source)
{
	//GENCTRL 0x4, bits 8-12
	const int mask = !(0x0000001f);
	
	int * const GEN_CLK_CTRL_REG = SYSCTRL_REGISTER_BLOCK + 0x00000004;
	 
	*GEN_CLK_CTRL_REG = (*GEN_CLK_CTRL_REG & mask) | (Set_Clk_Source << 8);
}

Generic_Clock_Source GENERIC_CLOCK_GENERATOR::Get_Clock_Source(void)
{
	//GENCTRL 0x4, bits 8-12
	const int mask = !(0x0000001f << 8);
	
	int * const GEN_CLK_CTRL_REG = SYSCTRL_REGISTER_BLOCK + 0x00000004;
	
	return (Generic_Clock_Source)((*GEN_CLK_CTRL_REG & mask) >> 8);
}

//void Synchronize_Clock::GCLOCK_GENERATOR(void)
//{
	//
//}

/******************************************************************************
@Get_SYNCBUSY
This function returns a boolean which is true if the clock system is busy 
in the synchronization process.  The code needs to wait until this 
synchronization completes before continuing.
******************************************************************************/
bool System_Clocks::Get_SYNCBUSY()
{
	//SYNCBUSY bit 15 on GCLK register at
	//while(GCLK->STATUS.bit.SYNCBUSY);
		
	return ((*GCLK_REGISTER_BLOCK >> SYNCH_BUSY_BIT_SHIFT) & 1) == 1;
}

void Digital_Frequency_Locked_Loop::Setup_Clock()
{
			// DFLL Configuration in Closed Loop mode, cf product data sheet chapter
			// 17.6.7.1 - Closed-Loop Operation
			// Enable the DFLL48M in open loop mode. Without this step, attempts to go into closed loop mode at 48 MHz will
			// result in Processor Reset (you'll be at the in the Reset_Handler in startup_samd21.c).
		//while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
		//SYSCTRL->DFLLCTRL.reg = (uint16_t)(SYSCTRL_DFLLCTRL_ENABLE);
		//while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
		
		
}

void CPU_CLOCK::Setup_Clock()
{
	//in power management system do not divide system clock down
	//PM->CPUSEL.reg  = PM_CPUSEL_CPUDIV_DIV1;
	//PM->APBASEL.reg = PM_APBASEL_APBADIV_DIV1_Val;
	//PM->APBBSEL.reg = PM_APBBSEL_APBBDIV_DIV1_Val;
	//PM->APBCSEL.reg = PM_APBCSEL_APBCDIV_DIV1_Val;
}

void CPU_CLOCK::Setup_Clock2()
{
	//// Various bits in the INTFLAG register can be set to one at startup.  This will ensure that these bits are cleared
	//SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET | SYSCTRL_INTFLAG_DFLLRDY;
		//
	//// set number of wait states regarding NVM characteristics at maximum operating frequency
	//NVMCTRL->CTRLB.bit.RWS = CONF_CLOCK_FLASH_WAIT_STATES;
		//
	//// switch all peripheral clocks
	//set_gclk_generator_default();
		//
	//// initialize external crystal oscillator
	//if( CONF_CLOCK_XOSC_ENABLE ){
		//init_xosc();
	//}
		//
	//// initialize internal oscillator
	//init_osc8m();
		//
	//// initialize all internal gclk generators, except main generator which is initialized after all other clock system are set up
	//init_gclk();
		//
	//// initialize internal dfll reference
	//if( CONF_CLOCK_DFLL_ENABLE ){
		//// configure and enable dfll gclk channel
		//configure_gclk_chan(SYSCTRL_GCLK_ID_DFLL48, CONF_CLOCK_DFLL_SOURCE_GCLK_GENERATOR);
		//enable_gclk_chan(SYSCTRL_GCLK_ID_DFLL48);
			//
		//// init dfll clock source
		//init_dfll();
	//}
		//
	//// enable cpu and bus clocks
	//cpu_clock_set_divider(CONF_CLOCK_CPU_DIVIDER);
	//bus_clock_set_divider(CLOCK_APB_APBA, CONF_CLOCK_APBA_DIVIDER);
	//bus_clock_set_divider(CLOCK_APB_APBB, CONF_CLOCK_APBB_DIVIDER);
	//bus_clock_set_divider(CLOCK_APB_APBC, CONF_CLOCK_APBC_DIVIDER);
		//
	//// configure main gclk generator
	//init_main_gclk_generator();
		//
	//// configure ARM systick timer
	//SysTick_Config(lld_sys_gclk_gen_get_hz(GCLK_CLKCTRL_GEN_GCLK0_Val) / TICK_EVERY_1MS);
}

//*****************end System Clocks.cpp*****************