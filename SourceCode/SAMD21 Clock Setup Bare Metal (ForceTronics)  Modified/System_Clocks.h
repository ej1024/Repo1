/******************************************************************************
@file System_Clocks.h

Created: 2023-05-01 2:45:06 PM
Author: Eric Minnaert

@brief This is the header for the system clocks classes.  These classes are 
written to to provide object oriented code organization for the processor and 
likely the application layer using the processor. 
******************************************************************************/

#ifndef SYSTEM_CLOCKS_H_
#define SYSTEM_CLOCKS_H_

int * const SYSCTRL_REGISTER_BLOCK				= (int*)0x40000800;
enum Ext_Oscillator_Startup_Time { S31us, S61us, S122us, S244us, S488us, S977us, S1953us, S3906us, S7813us, S15625us, S31250us, S62500us, S125000us, S250000us, S500000us, S1000000us };
enum Generic_Clock_Source { EXT_OSCILLATOR_400kHz_TO_32MHz, GENERIC_CLOCK_GENERATOR_INPUT_PAD, GEN_CLOCK_GENERATOR_1, INT_OSCILATOR_32kHz_ULTRA_LOW_POWER, INT_OSCILATOR_32kHz_HIGH_ACCURACY, EXT_32kHz_CRYSTAL_OSCILLATOR, INT_8MHz_OSCILLATOR, DIG_FREQUENCY_LOCKED_LOOP,	FRACT_DIG_FREQUENCY_LOCKED_LOOP };

/******************************************************************************
@External_Oscillator
This is the class definition for the External Oscillator. Each oscillator type 
in the processor occupies 1 32 bit word.  Some of this bit space is unused.
******************************************************************************/
class  External_Oscillator
{
	public:
		enum Oscillator_Gain { G2MHz, G4MHz, G8MHz, G16MHz, G30MHz };
		
		bool Get_Enable(void);
		void Set_Enable(bool Set_State);
		
		bool Get_On_Demand(void);
		void Set_On_Demand(bool Set_State);
		
		bool Get_Run_In_Standby(void);
		void Set_Run_In_Standby(bool Set_State);
		
		bool Get_Enable_XTAL(void);
		void Set_Enable_XTAL(bool Set_State);

		bool Get_Automatic_Amplitude_Control_Enable(void);
		void Set_Automatic_Amplitude_Control_Enable(bool Set_State);		
		
		Oscillator_Gain Get_Oscillator_Gain(void);
		void Set_Oscillator_Gain(Oscillator_Gain Set_Oscillator_Gain);		
		
		Ext_Oscillator_Startup_Time Get_Oscillator_Startup_Time(void);
		void Set_Oscillator_Startup_Time(Ext_Oscillator_Startup_Time Set_Oscillator_Startup_Time);		
		
		void Setup_Clock(void);
		int Get_Clock_Frequency_Hz(void);	
			
	protected:
	
	private:
		const int	External_Clock_Frequency_Hz			= 16000000;

		//declare constant pointer to int
		//int * const variable_name_here = (int*)address;
		
		int * const XOSC_REG							= (int*)(0x00000010 + SYSCTRL_REGISTER_BLOCK);
		const int SYSCTRL_XOSC_STARTUP_BIT_SHIFT		= 12;
		const int SYSCTRL_XOSC_STARTUP_SIZE_BITS		= 4;
		
		const int XOSC_ONDEMAND_BIT_LOCATION			= 7;
		const int XOSC_RUN_IN_STANDBY_BIT_LOCATION		= 6;
};

class  External_Crystal_32kHz_Oscillator
{
	public:
		enum Oscillator_Startup_Time { S122us, S1068us, S62592us, S1125092us, S500092us, S1000092us, S2000092us, S4000092us };
	
		bool Get_Enable(void);
		void Set_Enable(bool Set_State);
			
		bool Get_On_Demand(void);
		void Set_On_Demand(bool Set_State);
			
		bool Get_Run_In_Standby(void);
		void Set_Run_In_Standby(bool Set_State);
		
		bool Get_Enable_1k(void);
		void Set_Enable_1k(bool Set_State);
		
		bool Get_Enable_32k_Output(void);
		void Set_Enable_32k_Output(bool Set_State);
		
		bool Get_Enable_XTAL(void);
		void Set_Enable_XTAL(bool Set_State);

		bool Get_Write_Lock(void);
		void Set_Write_Lock(bool Set_State);	
	
		bool Get_Automatic_Amplitude_Control_Enable(void);
		void Set_Automatic_Amplitude_Control_Enable(bool Set_State);
		
		Oscillator_Startup_Time Get_Oscillator_Startup_Time(void);
		void Set_Oscillator_Startup_Time(Oscillator_Startup_Time Set_Oscillator_Startup_Time);
	
		void Setup_Clock(void);
		int Get_Clock_Frequency_Hz(void);
		
	protected:
	private:
		int * const X_32K_OSC_REG						= (int*)(0x00000014 + SYSCTRL_REGISTER_BLOCK);
	
};

class  Internal_Oscillator_32kHz_High_Accuracy
{
	public:
		enum Oscillator_Startup_Time { S92us, S122us, S183us, S305us, S549us, S1038us, S2014us, S3967us };
		
		bool Get_Enable(void);
		void Set_Enable(bool Set_State);
		
		bool Get_On_Demand(void);
		void Set_On_Demand(bool Set_State);
		
		bool Get_Run_In_Standby(void);
		void Set_Run_In_Standby(bool Set_State);
		
		bool Get_Enable_32k_Output(void);
		void Set_Enable_32k_Output(bool Set_State);

		bool Get_Enable_1k(void);
		void Set_Enable_1k(bool Set_State);

		bool Get_Write_Lock(void);
		void Set_Write_Lock(bool Set_State);
		
		Oscillator_Startup_Time Get_Oscillator_Startup_Time(void);
		void Set_Oscillator_Startup_Time(Oscillator_Startup_Time Set_Oscillator_Startup_Time);
		
		//bool Get_Overwrite_Default_Oscillator_Calibration(void);
		//void Set_Overwrite_Default_Oscillator_Calibration(bool Set_State);
		
		//int Get_Oscillator_Calibration_Value(void);
		//void Set_Overwrite_Default_Oscillator_Calibration(int Calibration_Set_Value);
		
		void Setup_Clock(void);
		int Get_Clock_Frequency_Hz(void);
		
	protected:
	private:
		int * const Int_HA_32K_OSC_REG					= (int*)(0x00000018 + SYSCTRL_REGISTER_BLOCK);	
};

class  Internal_8MHz_Oscillator
{
	public:
		enum Oscillator_Prescaler { P1, P2, P4, P8 };
		
		bool Get_Enable(void);
		void Set_Enable(bool Set_State);
		
		bool Get_On_Demand(void);
		void Set_On_Demand(bool Set_State);
		
		bool Get_Run_In_Standby(void);
		void Set_Run_In_Standby(bool Set_State);
		
		Oscillator_Prescaler Get_Oscillator_Prescaler(void);
		void Set_Oscillator_Prescaler(Oscillator_Prescaler Set_Oscillator_Prescaler);
		
		//bool Get_Overwrite_Default_Oscillator_Calibration(void);
		//void Set_Overwrite_Default_Oscillator_Calibration(bool Set_State);
				
		//int Get_Oscillator_Calibration_Value(void);
		//void Set_Overwrite_Default_Oscillator_Calibration(int Calibration_Set_Value);
		
		void Setup_Clock(void);
		int Get_Clock_Frequency_Hz(void);
		
	protected:
	private:	
		int * const Int_8M_OSC_REG						= (int*)(0x00000020 + SYSCTRL_REGISTER_BLOCK);	
};

class  Digital_Frequency_Locked_Loop
{
		// DFLL Configuration in Closed Loop mode, cf product data sheet chapter
		// 17.6.7.1 - Closed-Loop Operation
		// Enable the DFLL48M in open loop mode. Without this step, attempts to go into closed loop mode at 48 MHz will
		// result in Processor Reset (you'll be at the in the Reset_Handler in startup_samd21.c).
	
	
	enum Oscillator_Reference_Source { GEN_CLOCK_GENERATOR_0, GEN_CLOCK_GENERATOR_1, GEN_CLOCK_GENERATOR_2, GEN_CLOCK_GENERATOR_3, GEN_CLOCK_GENERATOR_4, GEN_CLOCK_GENERATOR_5, GEN_CLOCK_GENERATOR_6, GEN_CLOCK_GENERATOR_7 };
	enum Operating_Mode { OPEN_LOOP_MODE, CLOSED_LOOP_MODE };
	
	public:
		bool Get_Enable(void);
		void Set_Enable(bool Set_State);
		
		bool Get_On_Demand(void);
		void Set_On_Demand(bool Set_State);
		
		bool Get_Run_In_Standby(void);
		void Set_Run_In_Standby(bool Set_State);
		
		bool Get_Wait_Lock(void);
		void Set_Wait_Lock(bool Set_State);
		
		bool Get_Bypass_Coarse_Lock(void);
		void Set_Bypass_Coarse_Lock(bool Set_State);
		
		bool Get_Quick_Lock_Disable(void);
		void Set_Quick_Lock_Disable(bool Set_State);
		
		bool Get_Chill_Cycle_Disable(void);
		void Set_Chill_Cycle_Disable(bool Set_State);
		
		bool Get_USB_Clock_Recovery_Mode(void);
		void Set_USB_Clock_Recovery_Mode(bool Set_State);
		
		bool Get_Lose_Lock_After_Wake(void);
		void Set_Lose_Lock_After_Wake(bool Set_State);
		
		bool Get_Stable_DFLL_Frequency(void);
		void Set_Stable_DFLL_Frequency(bool Set_State);
		
		Operating_Mode Get_Operating_Mode(void);
		void Set_Operating_Mode(Operating_Mode Set_Operating_Mode);
				
		int Get_Course_Maximum_Step(void);
		void Set_Course_Maximum_Step(int Course_Maximum_Step_Set_Value);
		
		int Get_Fine_Maximum_Step(void);
		void Set_Fine_Maximum_Step(int Fine_Maximum_Step_Set_Value);
		
		int Get_DFLL_Multiply_Factor(void);
		void Set_DFLL_Multiply_Factor(int DFLL_Multiply_Factor_Set_Value);
		
		//bool Get_Overwrite_Default_Oscillator_Calibration(void);
		//void Set_Overwrite_Default_Oscillator_Calibration(bool Set_State);
				
		//int Get_Oscillator_Calibration_Value(void);
		//void Set_Overwrite_Default_Oscillator_Calibration(int Calibration_Set_Value);
		
		void Setup_Clock(void);
		int Get_Clock_Frequency_Hz(void);		
		
	protected:
	private:	
		int * const Int_DFLL_REG						= (int*)(0x00000024 + SYSCTRL_REGISTER_BLOCK);	
};

class  Fractional_Digital_Phase_Locked_Loop
{
	public:
		enum Oscillator_Reference_Source { EXTERNAL_32kHz_CRYSTAL_OSCILLATOR, EXTERNAL_OSCILLATOR_400kHz_TO_32MHz, GEN_CLOCK_GENERATOR_0, GEN_CLOCK_GENERATOR_1, GEN_CLOCK_GENERATOR_2, GEN_CLOCK_GENERATOR_3, GEN_CLOCK_GENERATOR_4, GEN_CLOCK_GENERATOR_5, GEN_CLOCK_GENERATOR_6, GEN_CLOCK_GENERATOR_7 };

		bool Get_Enable(void);
		void Set_Enable(bool Set_State);

		bool Get_On_Demand(void);
		void Set_On_Demand(bool Set_State);

		bool Get_Run_In_Standby(void);
		void Set_Run_In_Standby(bool Set_State);

		bool Get_Lock_ByPass(void);
		void Set_Lock_ByPass(bool Set_State);

		int Get_Clock_Divider(void);
		void Set_Clock_Divider(int Set_Clock_Divider);	
		
		int Get_DPLL_LDFRAC(void);
		void Set_DPLL_LDFRAC(int Set_DPLL_LDFRAC);
		
		int Get_DPLL_LDR(void);
		void Set_DPLL_LDR(int Set_DPLL_LDR);
	
		Oscillator_Reference_Source Get_Oscillator_Reference_Source(void);
		void Set_Oscillator_Reference_Source(Oscillator_Reference_Source Set_Oscillator_Reference_Source);
	
		void Setup_Clock(void);
		int Get_Clock_Frequency_Hz(void);
		
	protected:
	private:
		int * const Int_FDFLL_OSC_REG					= (int*)(0x00000048 + SYSCTRL_REGISTER_BLOCK);		
};

class  Internal_Oscillator_32kHz_Ultra_Low_Power
{
	public:
		bool Get_Write_Lock(void);
		void Set_Write_Lock(bool Set_State);
		
		//bool Get_Overwrite_Default_Oscillator_Calibration(void);
		//void Set_Overwrite_Default_Oscillator_Calibration(bool Set_State);
		
		//int Get_Oscillator_Calibration_Value(void);
		//void Set_Overwrite_Default_Oscillator_Calibration(int Calibration_Set_Value);
		
		void Setup_Clock(void);
		int Get_Clock_Frequency_Hz(void);
		
	protected:
	private:
		int * const Int_FDFLL_OSC_REG					= (int*)(0x0000001C + SYSCTRL_REGISTER_BLOCK);		
};

class GENERIC_CLOCK_GENERATOR
{	
	public:
		//from me
		//enum Clock_Source { EXT_OSCILLATOR_400kHz_TO_32MHz, GENERIC_CLOCK_GENERATOR_INPUT_PAD, GEN_CLOCK_GENERATOR_1, INT_OSCILATOR_32kHz_ULTRA_LOW_POWER, INT_OSCILATOR_32kHz_HIGH_ACCURACY, EXT_32kHz_CRYSTAL_OSCILLATOR, INT_8MHz_OSCILLATOR, DIG_FREQUENCY_LOCKED_LOOP };
		
		//from datasheet
		//this is 1 byte wide
			
		enum Clock_Generator { GEN_0, GEN_1, GEN_2, GEN_3, GEN_4, GEN_5, GEN_6, GEN_7 };

		//Bit 21 – RUNSTDBY: Run in Standby
		bool Get_Run_In_Standby(void);
		void Set_Run_In_Standby(bool Set_State);
		
		//Bit 20 – DIVSEL: Divide Selection
		bool Get_Divide_Selection(void);
		void Set_Divide_Selection(bool Set_State);
		
		//Bit 19 – OE: Output Enable
		bool Get_Output_Enable(void);
		void Set_Output_Enable(bool Set_State);
		
		//Bit 18 – OOV: Output Off Value
		bool Get_Output_Off_Value(void);
		void Set_Output_Off_Value(bool Set_State);
		
		//Bit 17 – IDC: Improve Duty Cycle	
		bool Get_Improve_Duty_Cycle(void);
		void Set_Improve_Duty_Cycle(bool Set_State);
		
		//Bit 16 – GENEN: Generic Clock Generator Enable
		bool Get_Enable(void);
		void Set_Enable(bool Set_State);
		
		//clock div offset 0x8
		//reserved bits 31-24
		//DIV[15:8] are bit 23-16
		//DIV[7:0] are bits 15-8
		//GCLK_ID       bits 3-0
		int Get_Division(void);
		void Set_Division(int Set_Division);
		
		//Bits 12:8 – SRC[4:0]: Source Select
		Generic_Clock_Source Get_Clock_Source(void);
		void Set_Clock_Source(Generic_Clock_Source Set_Clk_Source);
		
		//Clock_Generator
		Clock_Generator Get_Clock_Generator(void);
		void Set_Clock_Generator(Clock_Generator Set_Clock_Generator);
			
		void Setup_Clock(void);
		int Get_Clock_Frequency_Hz(void);
		
	protected:
	private:
		void Synchronize_Clock(void);
		
		//const int G_CLOCK_REGISTER_BLOCK_OFFSET		= 0x40000C00;  //addressing the whole G_Clock section
		//const int SYNCH_BUSY_REGISTER_OFFSET		= 0;  //32 bit addressing
		//const int SYNCH_BUSY_BIT					= 15;  //32 bit addressing
		
		const int SYNCH_BUSY_BIT_SHIFT					= 15;
		int * const GCLK_REGISTER_BLOCK					= (int*)0x40000C00;
};
	


class CPU_CLOCK
{
	public:
		enum CPU_Clock_Prescaler { P1, P2, P4, P8, P16, P32, P64, P128 };
		enum NVM_Wait_States { W1, W2, W3, W4, W5, W6, W7, W8, W9, W10, W11, W12, W13, W14, W15 };
	
		CPU_Clock_Prescaler Get_CPU_Clock_Prescaler(void);
		void Set_CPU_Clock_Prescaler(CPU_Clock_Prescaler Set_CPU_Clock_Prescaler);

		NVM_Wait_States Get_NVM_Wait_States(void);
		void Set_NVM_Wait_States(NVM_Wait_States Set_NVM_Wait_States);
	
		CPU_Clock_Prescaler Get_APBA_Clock_Prescaler(void);
		void Set_APBA_Clock_Prescaler(CPU_Clock_Prescaler Set_APBA_Clock_Prescaler);
	
		CPU_Clock_Prescaler Get_APBB_Clock_Prescaler(void);
		void Set_APBB_Clock_Prescaler(CPU_Clock_Prescaler Set_APBB_Clock_Prescaler);
	
		CPU_Clock_Prescaler Get_APBC_Clock_Prescaler(void);
		void Set_APBC_Clock_Prescaler(CPU_Clock_Prescaler Set_APBC_Clock_Prescaler);
	
		void Setup_Clock(void);
		void Setup_Clock2(void);
		int Get_Clock_Frequency_Hz(void);
			
	protected:
	private:
};

class Components_CLOCK
{
	public:
	enum Clock_Source { GEN_CLOCK_GENERATOR_0, GEN_CLOCK_GENERATOR_1, GEN_CLOCK_GENERATOR_2, GEN_CLOCK_GENERATOR_3, GEN_CLOCK_GENERATOR_4, GEN_CLOCK_GENERATOR_5, GEN_CLOCK_GENERATOR_6, GEN_CLOCK_GENERATOR_7 };
	
	Clock_Source Get_Core_Clock_Source(void);
	void Set_Core_Clock_Source(int Clock_Source);

	Clock_Source Get_Slow_Clock_Source(void);
	void Set_Slow_Clock_Source(int Clock_Source);
	
	void Setup_Clock(void);
	int Get_Clock_Frequency_Hz(void);	
	
	protected:
	private:
};


class System_Clocks
{
	public:
		int Get_CPU_Clock_Frequency_Hz(void);	
		int Get_Core_Clock_Frequency_Hz(void);	
		int Get_Slow_Clock_Frequency_Hz(void);	
		void Setup_System_Clocks();
	
	protected:
	
	private:	
		External_Oscillator Ext_osc;
		External_Crystal_32kHz_Oscillator Ext_32kHz;
		Internal_Oscillator_32kHz_High_Accuracy Int_32k_ha;
		Internal_8MHz_Oscillator Int_8MHz;
		Digital_Frequency_Locked_Loop Dfll;
		Fractional_Digital_Phase_Locked_Loop Fdpll;
		Internal_Oscillator_32kHz_Ultra_Low_Power Int_32k_lp;
			
		GENERIC_CLOCK_GENERATOR G_Clock_Gen;
		//GCLOCK_GENERATOR_1 Gcg1;
		//GCLOCK_GENERATOR_2 Gcg2;
		//GCLOCK_GENERATOR_3 Gcg3;
		//GCLOCK_GENERATOR_4 Gcg4;
		//GCLOCK_GENERATOR_5 Gcg5;
		//GCLOCK_GENERATOR_6 Gcg6;
		//GCLOCK_GENERATOR_7 Gcg7;
			
		Components_CLOCK Comp_clk;
		CPU_CLOCK Cpu_clk;
		
		//#define GCLK   (0x40000C00) /**< \brief (GCLK) APB Base Address */
		//const int GCLK_STATUS_ADDRESS_OFFSET = 1;
		//
		//const int GCLK_CTRL     = 0;     /**< \brief Offset: 0x0 (R/W  8) Control */
		//const int GCLK_STATUS   = 1;     /**< \brief Offset: 0x1 (R/   8) Status */
		//const int GCLK_CLKCTRL  = 2;     /**< \brief Offset: 0x2 (R/W 16) Generic Clock Control */
		//const int GCLK_GENCTRL  = 4;     /**< \brief Offset: 0x4 (R/W 32) Generic Clock Generator Control */
		//const int GCLK_GENDIV   = 8;     /**< \brief Offset: 0x8 (R/W 32) Generic Clock Generator Division */
		
		//values for multiplexing Clock_Generator_0,1,2...
		//Generic clock subsystem setting GENDIV register to set the divide factor for Generic clock 1
		//GCLK->GENDIV.reg |= GCLK_GENDIV_DIV(1) | GCLK_GENDIV_ID(GENERIC_CLOCK_GENERATOR_1); //set divide factor for gen clock 1
		
		bool Get_SYNCBUSY();
		void Synchronize(void);
		
		const int SYNCH_BUSY_BIT_SHIFT					= 15;
		int * const GCLK_REGISTER_BLOCK					= (int*)0x40000C00;
};

#endif /* SYSTEM_CLOCKS_H_ */