/******************************************************************//**
* @file		lld_sys.c
* @brief	System driver
* @author	Tim Rudolph, Sensata HVOR OST Berlin
*
* JD ICM / lld sys
*
* (c) Sensata Technologies GmbH, Berlin - Germany
*********************************************************************/

#include <samd21j18a.h>
//#include "lld_sys.h"
//#include "conf_clocks.h"


void set_gclk_generator_default(void);
void init_xosc( void );
void init_dfll( void );
void init_osc8m( void );
void init_gclk( void );
void enable_gclk(void);
void init_gclk_generators(void);	
void init_main_gclk_generator(void);
void configure_gclk_gen(uint8_t gen_channel, generic_clk_config_t gclk_config);
void enable_gclk_gen(uint8_t gen_channel);
void disable_gclk_chan(const uint8_t gen_channel);
void configure_gclk_chan(const uint8_t gen_channel, const uint8_t generator);
void enable_gclk_chan(const uint8_t gen_channel);
uint8_t cpu_clock_set_divider(uint32_t divider);
uint8_t bus_clock_set_divider(uint32_t bus, uint32_t divider);
void clock_source_set_config_dfll_errata_9905(void);
uint32_t gclk_chan_get_hz(const uint8_t channel);
uint32_t clock_source_get_hz(const uint8_t clock_source);
uint32_t sysctrl_get_dfllmul(void);

/******************************************************************//**
* @brief		initialization of all clocks dependent on the 
*				conf_clocks.h entries
* @return		-
**********************************************************************/
void lld_sys_clock_init( void ){

	// Various bits in the INTFLAG register can be set to one at startup.  This will ensure that these bits are cleared
	SYSCTRL->INTFLAG.reg = SYSCTRL_INTFLAG_BOD33RDY | SYSCTRL_INTFLAG_BOD33DET | SYSCTRL_INTFLAG_DFLLRDY;
	
	// set number of wait states regarding NVM characteristics at maximum operating frequency
	NVMCTRL->CTRLB.bit.RWS = CONF_CLOCK_FLASH_WAIT_STATES;
		
	// switch all peripheral clocks	
	set_gclk_generator_default();
	
	// initialize external crystal oscillator
	if( CONF_CLOCK_XOSC_ENABLE ){
		init_xosc();	
	}
	
	// initialize internal oscillator	
	init_osc8m();
	
	// initialize all internal gclk generators, except main generator which is initialized after all other clock system are set up
	init_gclk();
	
	// initialize internal dfll reference
	if( CONF_CLOCK_DFLL_ENABLE ){
		// configure and enable dfll gclk channel
		configure_gclk_chan(SYSCTRL_GCLK_ID_DFLL48, CONF_CLOCK_DFLL_SOURCE_GCLK_GENERATOR);
		enable_gclk_chan(SYSCTRL_GCLK_ID_DFLL48);
		
		// init dfll clock source
		init_dfll();
	}
	
	// enable cpu and bus clocks
	cpu_clock_set_divider(CONF_CLOCK_CPU_DIVIDER);
	bus_clock_set_divider(CLOCK_APB_APBA, CONF_CLOCK_APBA_DIVIDER);
	bus_clock_set_divider(CLOCK_APB_APBB, CONF_CLOCK_APBB_DIVIDER);
	bus_clock_set_divider(CLOCK_APB_APBC, CONF_CLOCK_APBC_DIVIDER);
	
	// configure main gclk generator
	init_main_gclk_generator();
	
	// configure ARM systick timer
	SysTick_Config(lld_sys_gclk_gen_get_hz(GCLK_CLKCTRL_GEN_GCLK0_Val) / TICK_EVERY_1MS);
}
/******************************************************************//**
* @brief		set all peripheral clocks to first unused generic clock
* @return		-
**********************************************************************/
void set_gclk_generator_default(void){
	uint32_t generator;	
	#if CONF_CLOCK_GCLK_1_ENABLE == false
		generator = GCLK_CLKCTRL_GEN_GCLK1_Val;
	#elif CONF_CLOCK_GCLK_2_ENABLE == false
		generator = GCLK_CLKCTRL_GEN_GCLK2_Val;
	#elif CONF_CLOCK_GCLK_3_ENABLE == false
		generator = GCLK_CLKCTRL_GEN_GCLK3_Val;
	#elif CONF_CLOCK_GCLK_4_ENABLE == false
		generator = GCLK_CLKCTRL_GEN_GCLK4_Val;
	#elif CONF_CLOCK_GCLK_5_ENABLE == false
		generator = GCLK_CLKCTRL_GEN_GCLK5_Val;
	#elif CONF_CLOCK_GCLK_6_ENABLE == false
		generator = GCLK_CLKCTRL_GEN_GCLK6_Val;
	#elif CONF_CLOCK_GCLK_7_ENABLE == false
		generator = GCLK_CLKCTRL_GEN_GCLK7_Val;
	#else
		generator = GCLK_CLKCTRL_GEN_GCLK8_Val;
	#endif		
		
	// switch all peripheral clocks to first unused generic clock channel
	uint32_t gclk_id;
	for (gclk_id = 0; gclk_id < GCLK_NUM; gclk_id++) {
		// configure generic clocks
		configure_gclk_chan(gclk_id, generator);
	}
}

/******************************************************************//**
* @brief		initialize the external crystal oscillator
* @return		-
**********************************************************************/
void init_xosc( void ){	
	// configure external clock source XOSC
	// start up time 1024 clocks (31250us), crystal oscillator enabled, automatic amplitude gain control disabled, oscillator gain 16MHz, 
	// on demand enabled = oscillator is not always on, it runs when peripheral requested
	SYSCTRL->XOSC.reg = SYSCTRL_XOSC_STARTUP(CONF_CLOCK_XOSC_STARTUP_TIME) | 
						SYSCTRL_XOSC_XTALEN | 
						SYSCTRL_XOSC_GAIN_3 | 
						(CONF_CLOCK_XOSC_ON_DEMAND << SYSCTRL_XOSC_ONDEMAND_Pos) |
						(CONF_CLOCK_XOSC_RUN_IN_STANDBY << SYSCTRL_XOSC_RUNSTDBY_Pos);	
						
	SYSCTRL->XOSC.reg |= SYSCTRL_XOSC_ENABLE;					
}
/******************************************************************//**
* @brief		initialization of the dfll in the system controller
* @return		-
**********************************************************************/
void init_dfll( void ){
			
	clock_source_set_config_dfll_errata_9905();
	
	uint32_t mask = (SYSCTRL_PCLKSR_DFLLRDY | SYSCTRL_PCLKSR_DFLLLCKF | SYSCTRL_PCLKSR_DFLLLCKC);

#ifndef UNIT_TEST
	while(! ((SYSCTRL->PCLKSR.reg & mask) == mask) );
#else
    WAIT_FOR_SYNCH(!((SYSCTRL->PCLKSR.reg & mask) == mask));
#endif	
	if (CONF_CLOCK_DFLL_ON_DEMAND) {
		SYSCTRL->DFLLCTRL.bit.ONDEMAND = 1;
	}
}
/******************************************************************//**
* @brief		set dfll configuration to system controller
* @note			the initialization considers the errata 9905
* @return		-
**********************************************************************/
void clock_source_set_config_dfll_errata_9905(){
		
#ifndef UNIT_TEST		
	uint32_t coarse =( *((uint32_t *)(NVMCTRL_OTP4)	+ (NVM_DFLL_COARSE_POS / 32)) >> (NVM_DFLL_COARSE_POS % 32)) & ((1 << NVM_DFLL_COARSE_SIZE) - 1);
#else
    uint32_t temp = 0;
    uint32_t coarse = GetFlash_u32(temp);
#endif	
	// In some revision chip, the coarse calibration value is not correct.
	if (coarse == 0x3f) {
		coarse = 0x1f;
	}
	
	// ERRATA 9905
	// Disable ONDEMAND mode while writing configurations
	SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
#ifndef UNIT_TEST
	while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY)) {
		// Wait for DFLL sync
		// while loop is allowed due to system clock configuration
	}
#else
    WAIT_FOR_SYNCH(!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY));
#endif

	SYSCTRL->DFLLMUL.reg =	sysctrl_get_dfllmul();
	
	SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(CONF_CLOCK_DFLL_FINE_VALUE);

	// Write full configuration to DFLL control register
	SYSCTRL->DFLLCTRL.reg = 0;
#ifndef UNIT_TEST	
	while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY)) {
		// Wait for DFLL sync
		// while loop is allowed due to system clock configuration
	}
#else
    WAIT_FOR_SYNCH(!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY));
#endif	
	
	//NOTE: some register entries are negated because it's a disable/clear register
	SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE | 
							CONF_CLOCK_DFLL_LOOP_MODE |	
							(!CONF_CLOCK_DFLL_TRACK_AFTER_FINE_LOCK << SYSCTRL_DFLLCTRL_STABLE_Pos) |
							(!CONF_CLOCK_DFLL_KEEP_LOCK_ON_WAKEUP << SYSCTRL_DFLLCTRL_LLAW_Pos) |
							(CONF_CLOCK_DFLL_RUN_IN_STANDBY << SYSCTRL_DFLLCTRL_RUNSTDBY_Pos) |
							(CONF_CLOCK_DFLL_ON_DEMAND << SYSCTRL_DFLLCTRL_ONDEMAND_Pos) |
							(!CONF_CLOCK_DFLL_ENABLE_CHILL_CYCLE << SYSCTRL_DFLLCTRL_CCDIS_Pos) |
							(!CONF_CLOCK_DFLL_QUICK_LOCK << SYSCTRL_DFLLCTRL_QLDIS_Pos);
}
/******************************************************************//**
* @brief		get dfll mul value
* @return		uint32_t
**********************************************************************/
uint32_t sysctrl_get_dfllmul(void){
	return	SYSCTRL_DFLLMUL_CSTEP(CONF_CLOCK_DFLL_MAX_COARSE_STEP_SIZE) |
			SYSCTRL_DFLLMUL_FSTEP(CONF_CLOCK_DFLL_MAX_FINE_STEP_SIZE) |
			SYSCTRL_DFLLMUL_MUL(CONF_CLOCK_DFLL_MULTIPLY_FACTOR);
}
/******************************************************************//**
* @brief		initialize 8MHz internal oscillator
* @return		-
**********************************************************************/
void init_osc8m( void ){
	
	// configuration depends on conf_clocks.h
	// use temporary struct
	SYSCTRL_OSC8M_Type temp = SYSCTRL->OSC8M;
	temp.bit.PRESC    = CONF_CLOCK_OSC8M_PRESCALER;
	temp.bit.ONDEMAND = CONF_CLOCK_OSC8M_ON_DEMAND;
	temp.bit.RUNSTDBY = CONF_CLOCK_OSC8M_RUN_IN_STANDBY;
	// write back of register entries
	SYSCTRL->OSC8M = temp;
	
	// enable OSC8M						
	SYSCTRL->OSC8M.reg |= SYSCTRL_OSC8M_ENABLE;	
}
/******************************************************************//**
* @brief		initialize generic clock module
* @return		-
**********************************************************************/
void init_gclk( void ){
	
	// power on gclk module
	enable_gclk();
	
	// configure each gclk generator except main gclk
	init_gclk_generators();	
	
}
/******************************************************************//**
* @brief		enable generic clock module
* @return		-
**********************************************************************/
void enable_gclk(void){
	// turn on digital interface clock
	PM->APBAMASK.reg |= PM_APBAMASK_GCLK;
	
	// Software reset the module to ensure it is re-initialized correctly
	GCLK->CTRL.reg = GCLK_CTRL_SWRST;
#ifndef UNIT_TEST	
	while (GCLK->CTRL.reg & GCLK_CTRL_SWRST) {
		// Wait for reset to complete
		// while loop is allowed due to system clock configuration
	}	
#else
    WAIT_FOR_SYNCH(GCLK->CTRL.reg & GCLK_CTRL_SWRST);
#endif	
}
/******************************************************************//**
* @brief		initialize generic clock generators except main generator
* @return		-
**********************************************************************/
void init_gclk_generators(void){
	// the initialization of each generic clock will be done step by step
	// the ASF uses several macros which leads to be unreadable
	
	generic_clk_config_t gclk_conf;
	
	#if (CONF_CLOCK_GCLK_1_ENABLE == true)
		// data of gclk #1 from conf_clocks
		gclk_conf.division_factor = CONF_CLOCK_GCLK_1_PRESCALER;
		gclk_conf.output_enable = CONF_CLOCK_GCLK_1_OUTPUT_ENABLE;
		gclk_conf.run_in_standby = CONF_CLOCK_GCLK_1_RUN_IN_STANDBY;
		gclk_conf.source_clock = CONF_CLOCK_GCLK_1_CLOCK_SOURCE;
		// configure gclk #1
		configure_gclk_gen(GCLK_CLKCTRL_GEN_GCLK1_Val, gclk_conf);
		// enable gclk #1
		enable_gclk_gen(GCLK_CLKCTRL_GEN_GCLK1_Val);
	#endif
	
	#if (CONF_CLOCK_GCLK_2_ENABLE == true)
		// data of gclk #2 from conf_clocks
		gclk_conf.division_factor = CONF_CLOCK_GCLK_2_PRESCALER;
		gclk_conf.output_enable = CONF_CLOCK_GCLK_2_OUTPUT_ENABLE;
		gclk_conf.run_in_standby = CONF_CLOCK_GCLK_2_RUN_IN_STANDBY;
		gclk_conf.source_clock = CONF_CLOCK_GCLK_2_CLOCK_SOURCE;
		// configure gclk #2
		configure_gclk_gen(GCLK_CLKCTRL_GEN_GCLK2_Val, gclk_conf);
		// enable gclk #2
		enable_gclk_gen(GCLK_CLKCTRL_GEN_GCLK2_Val);
	#endif	
	
	#if (CONF_CLOCK_GCLK_3_ENABLE == true)
		// data of gclk #3 from conf_clocks
		gclk_conf.division_factor = CONF_CLOCK_GCLK_3_PRESCALER;
		gclk_conf.output_enable = CONF_CLOCK_GCLK_3_OUTPUT_ENABLE;
		gclk_conf.run_in_standby = CONF_CLOCK_GCLK_3_RUN_IN_STANDBY;
		gclk_conf.source_clock = CONF_CLOCK_GCLK_3_CLOCK_SOURCE;
		// configure gclk #3
		configure_gclk_gen(GCLK_CLKCTRL_GEN_GCLK3_Val, gclk_conf);
		// enable gclk #3
		enable_gclk_gen(GCLK_CLKCTRL_GEN_GCLK3_Val);
	#endif

	#if (CONF_CLOCK_GCLK_4_ENABLE == true)
		// data of gclk #4 from conf_clocks
		gclk_conf.division_factor = CONF_CLOCK_GCLK_4_PRESCALER;
		gclk_conf.output_enable = CONF_CLOCK_GCLK_4_OUTPUT_ENABLE;
		gclk_conf.run_in_standby = CONF_CLOCK_GCLK_4_RUN_IN_STANDBY;
		gclk_conf.source_clock = CONF_CLOCK_GCLK_4_CLOCK_SOURCE;
		// configure gclk #4
		configure_gclk_gen(GCLK_CLKCTRL_GEN_GCLK4_Val, gclk_conf);
		// enable gclk #4
		enable_gclk_gen(GCLK_CLKCTRL_GEN_GCLK4_Val);
	#endif
	
	#if (CONF_CLOCK_GCLK_5_ENABLE == true)
		// data of gclk #5 from conf_clocks
		gclk_conf.division_factor = CONF_CLOCK_GCLK_5_PRESCALER;
		gclk_conf.output_enable = CONF_CLOCK_GCLK_5_OUTPUT_ENABLE;
		gclk_conf.run_in_standby = CONF_CLOCK_GCLK_5_RUN_IN_STANDBY;
		gclk_conf.source_clock = CONF_CLOCK_GCLK_5_CLOCK_SOURCE;
		// configure gclk #5
		configure_gclk_gen(GCLK_CLKCTRL_GEN_GCLK5_Val, gclk_conf);
		// enable gclk #5
		enable_gclk_gen(GCLK_CLKCTRL_GEN_GCLK5_Val);
	#endif
	
	#if (CONF_CLOCK_GCLK_6_ENABLE == true)	
		// data of gclk #6 from conf_clocks
		gclk_conf.division_factor = CONF_CLOCK_GCLK_6_PRESCALER;
		gclk_conf.output_enable = CONF_CLOCK_GCLK_6_OUTPUT_ENABLE;
		gclk_conf.run_in_standby = CONF_CLOCK_GCLK_6_RUN_IN_STANDBY;
		gclk_conf.source_clock = CONF_CLOCK_GCLK_6_CLOCK_SOURCE;
		// configure gclk #6
		configure_gclk_gen(GCLK_CLKCTRL_GEN_GCLK6_Val, gclk_conf);
		// enable gclk #6
		enable_gclk_gen(GCLK_CLKCTRL_GEN_GCLK6_Val);
	#endif
	
	#if (CONF_CLOCK_GCLK_7_ENABLE == true)	
		// data of gclk #7 from conf_clocks
		gclk_conf.division_factor = CONF_CLOCK_GCLK_7_PRESCALER;
		gclk_conf.output_enable = CONF_CLOCK_GCLK_7_OUTPUT_ENABLE;
		gclk_conf.run_in_standby = CONF_CLOCK_GCLK_7_RUN_IN_STANDBY;
		gclk_conf.source_clock = CONF_CLOCK_GCLK_7_CLOCK_SOURCE;
		// configure gclk #7
		configure_gclk_gen(GCLK_CLKCTRL_GEN_GCLK7_Val, gclk_conf);
		// enable gclk #7
		enable_gclk_gen(GCLK_CLKCTRL_GEN_GCLK7_Val);
	#endif
	
	#if (CONF_CLOCK_GCLK_8_ENABLE == true)	
		// data of gclk #8 from conf_clocks
		gclk_conf.division_factor = CONF_CLOCK_GCLK_8_PRESCALER;
		gclk_conf.output_enable = CONF_CLOCK_GCLK_8_OUTPUT_ENABLE;
		gclk_conf.run_in_standby = CONF_CLOCK_GCLK_8_RUN_IN_STANDBY;
		gclk_conf.source_clock = CONF_CLOCK_GCLK_8_CLOCK_SOURCE;
		// configure gclk #8
		configure_gclk_gen(GCLK_CLKCTRL_GEN_GCLK8_Val, gclk_conf);
		// enable gclk #8
		enable_gclk_gen(GCLK_CLKCTRL_GEN_GCLK8_Val);	
	#endif
}
/******************************************************************//**
* @brief		initialize main generic clock generator (0)
* @return		-
**********************************************************************/
void init_main_gclk_generator(void){
	generic_clk_config_t gclk_conf;
	
	#if (CONF_CLOCK_GCLK_0_ENABLE == true)
		// data of gclk #1 from conf_clocks
		gclk_conf.division_factor = CONF_CLOCK_GCLK_0_PRESCALER;
		gclk_conf.output_enable = CONF_CLOCK_GCLK_0_ENABLE;
		gclk_conf.run_in_standby = CONF_CLOCK_GCLK_0_RUN_IN_STANDBY;
		gclk_conf.source_clock = CONF_CLOCK_GCLK_0_CLOCK_SOURCE;
		// configure gclk #0
		configure_gclk_gen(GCLK_CLKCTRL_GEN_GCLK0_Val, gclk_conf);
		// enable gclk #0
		enable_gclk_gen(GCLK_CLKCTRL_GEN_GCLK0_Val);	
	#endif
}
/******************************************************************//**
* @brief		configure Generic Clock Generator
* @param[in]	gen_channel Generic Clock Generator to configure
* @param[in]	gclk_config Generic Clock Generator configuration
* @return		-
**********************************************************************/
void configure_gclk_gen(uint8_t gen_channel, generic_clk_config_t gclk_config){
	uint32_t tmp_reg_gclk_genctrl = 0;
	uint32_t tmp_reg_gclk_gendiv = 0;
	
	// Cache new register configurations to minimize sync requirements
	tmp_reg_gclk_genctrl = (gen_channel << GCLK_GENCTRL_ID_Pos);
	tmp_reg_gclk_gendiv  = (gen_channel << GCLK_GENDIV_ID_Pos);
	
	// Select the requested source clock for the generator
	tmp_reg_gclk_genctrl |= (gclk_config.source_clock << GCLK_GENCTRL_SRC_Pos);
	
	// Configure if the clock output to I/O pin should be enabled
	if (gclk_config.output_enable) {
		tmp_reg_gclk_genctrl |= GCLK_GENCTRL_OE;
	}
	
	// Set division factor
	if (gclk_config.division_factor > 1) {
		// Check if division is a power of two
		if (((gclk_config.division_factor & (gclk_config.division_factor - 1)) == 0)) {
			// Determine the index of the highest bit set to get the
			// division factor that must be loaded into the division
			// register 

			uint32_t div2_count = 0;

			uint32_t mask;
			for (mask = (1UL << 1); mask < gclk_config.division_factor;	mask <<= 1) {
				div2_count++;
			}

			// Set binary divider power of 2 division factor
			tmp_reg_gclk_gendiv  |= div2_count << GCLK_GENDIV_DIV_Pos;
			tmp_reg_gclk_genctrl |= GCLK_GENCTRL_DIVSEL;
		} else {
			// Set integer division factor
			tmp_reg_gclk_gendiv  |=	(gclk_config.division_factor) << GCLK_GENDIV_DIV_Pos;

			// Enable non-binary division with increased duty cycle accuracy
			tmp_reg_gclk_genctrl |= GCLK_GENCTRL_IDC;
		}

	}
	
	// Enable or disable the clock in standby mode
	if (gclk_config.run_in_standby) {
		tmp_reg_gclk_genctrl |= GCLK_GENCTRL_RUNSTDBY;
	}

#ifndef UNIT_TEST
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {
		// Wait for synchronization
		// while loop is allowed due to system clock configuration
	};
#else
    WAIT_FOR_SYNCH(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
#endif	
		
	
	// Select the correct generator
	*((uint8_t*)&GCLK->GENDIV.reg) = gen_channel;

	// Write the new generator configuration
#ifndef UNIT_TEST	
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {
		// Wait for synchronization
		// while loop is allowed due to system clock configuration
	};
#else
    WAIT_FOR_SYNCH(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
#endif	
	GCLK->GENDIV.reg  = tmp_reg_gclk_gendiv;

#ifndef UNIT_TEST
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {
		// Wait for synchronization
		// while loop is allowed due to system clock configuration
	};
#else
    WAIT_FOR_SYNCH(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
#endif	
	GCLK->GENCTRL.reg = tmp_reg_gclk_genctrl | (GCLK->GENCTRL.reg & GCLK_GENCTRL_GENEN);
}
/******************************************************************//**
* @brief		enable a Generic Clock generator that was previously configured
* @param[in]	generator Generic Clock generator to enable
* @return		-
**********************************************************************/
void enable_gclk_gen(uint8_t generator){
		
#ifndef UNIT_TEST
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {
		// Wait for synchronization
	};
#else		
	WAIT_FOR_SYNCH(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
#endif	

	// Select the requested generator
	*((uint8_t*)&GCLK->GENCTRL.reg) = generator;
#ifndef UNIT_TEST	
	while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {
		// Wait for synchronization
	};
#else
    WAIT_FOR_SYNCH(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
#endif	

	// Enable generator
	GCLK->GENCTRL.reg |= GCLK_GENCTRL_GENEN;
}
/******************************************************************//**
* @brief		disable a Generic Clock that was previously enabled
* @param[in]	gen_channel Generic Clock channel to disable
* @return		-
**********************************************************************/
void disable_gclk_chan(const uint8_t gen_channel){

	// Select the requested generator channel
	*((uint8_t*)&GCLK->CLKCTRL.reg) = gen_channel;

	// Switch to known-working source so that the channel can be disabled
	uint32_t prev_gen_id = GCLK->CLKCTRL.bit.GEN;
	GCLK->CLKCTRL.bit.GEN = 0;

	// Disable the generic clock
	GCLK->CLKCTRL.reg &= ~GCLK_CLKCTRL_CLKEN;
#ifndef UNIT_TEST
	while (GCLK->CLKCTRL.reg & GCLK_CLKCTRL_CLKEN) {
		// Wait for clock to become disabled
	}
#else
    WAIT_FOR_SYNCH(GCLK->CLKCTRL.reg & GCLK_CLKCTRL_CLKEN);
#endif	

	// Restore previous configured clock generator
	GCLK->CLKCTRL.bit.GEN = prev_gen_id;

}
/******************************************************************//**
* @brief		enable a Generic Clock that was previously configured
* @param[in]	gen_channel Generic Clock channel to enable
* @return		-
**********************************************************************/
void enable_gclk_chan(const uint8_t gen_channel){

	// Select the requested generator channel
	*((uint8_t*)&GCLK->CLKCTRL.reg) = gen_channel;

	// Enable the generic clock
	GCLK->CLKCTRL.reg |= GCLK_CLKCTRL_CLKEN;
}
/******************************************************************//**
* @brief		writes a Generic Clock configuration to hardware module
* @param[in]	gen_channel Generic Clock channel of hardware module
* @param[in]	generator Generic Clock generator
* @return		-
**********************************************************************/	
void configure_gclk_chan(const uint8_t gen_channel, const uint8_t generator){
	// Cache the new config to reduce sync requirements
	uint32_t new_clkctrl_config = (gen_channel << GCLK_CLKCTRL_ID_Pos);

	// Select the desired generic clock generator
	new_clkctrl_config |= generator << GCLK_CLKCTRL_GEN_Pos;

	// Disable generic clock channel
	disable_gclk_chan(gen_channel);

	// Write the new configuration
	GCLK->CLKCTRL.reg = new_clkctrl_config;	
}
/******************************************************************//**
* @brief		set CPU main clock divider
* @param[in]	divider CPU clock divider to set
* @return		-
**********************************************************************/
uint8_t cpu_clock_set_divider(uint32_t divider){
	uint8_t ret_state = EXIT_FAILURE;
	if(((uint32_t)divider & PM_CPUSEL_CPUDIV_Msk) == divider){
		PM->CPUSEL.reg = divider;	
		ret_state = EXIT_SUCCESS;
	}	
	return ret_state;
}
/******************************************************************//**
* @brief		set clock divider used on main clock to provide 
*				clock to the given APBx bus
* @param[in]	bus APBx bus to set divider
* @param[in]	divider APBx bus clock divider to set
* @return		-
**********************************************************************/
uint8_t bus_clock_set_divider(uint32_t bus, uint32_t divider){
	uint8_t ret_state = EXIT_FAILURE;
	switch (bus) {
		case CLOCK_APB_APBA:
			PM->APBASEL.reg = (uint32_t)divider;
			ret_state = EXIT_SUCCESS;
			break;
		case CLOCK_APB_APBB:
			PM->APBBSEL.reg = (uint32_t)divider;
			ret_state = EXIT_SUCCESS;
			break;
		case CLOCK_APB_APBC:
			PM->APBCSEL.reg = (uint32_t)divider;
			ret_state = EXIT_SUCCESS;
			break;
		default:
			ret_state = EXIT_FAILURE;
	}	
	return ret_state;
}
/******************************************************************//**
* @brief		get frequency of chosen generic clock generator
* @param[in]	generator	number of generic clock generator
* @note			in case of wrong generator number the returned hz will be 0
* @return		generator hz
**********************************************************************/
uint32_t lld_sys_gclk_gen_get_hz(const uint8_t generator){
	uint32_t gen_input_hz = 0;

	if( generator < GCLK_SOURCE_NUM ){
		
#ifndef UNIT_TEST		
		while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {
			// Wait for synchronization
		};
#else
        WAIT_FOR_SYNCH(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
#endif		

		// Select the appropriate generator
		*((uint8_t*)&GCLK->GENCTRL.reg) = generator;
#ifndef UNIT_TEST		
		while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {
			// Wait for synchronization
		};
#else
        WAIT_FOR_SYNCH(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY);
#endif		
	
		// Get the frequency of the source connected to the GCLK generator
		gen_input_hz = clock_source_get_hz( (uint8_t)GCLK->GENCTRL.bit.SRC );
	
		*((uint8_t*)&GCLK->GENCTRL.reg) = generator;

		uint8_t divsel = GCLK->GENCTRL.bit.DIVSEL;

		// Select the appropriate generator division register
		*((uint8_t*)&GCLK->GENDIV.reg) = generator;
		while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {
			// Wait for synchronization
		};
	
		uint32_t divider = GCLK->GENDIV.bit.DIV;

		// Check if the generator is using fractional or binary division
		if (!divsel && divider > 1) {
			gen_input_hz /= divider;
		} else if (divsel) {
			gen_input_hz >>= (divider+1);
		}

	}

	return gen_input_hz;	
}
/******************************************************************//**
* @brief		get frequency of chosen clock source
* @param[in]	clock_source	number of clock source
* @return		uint32_t
**********************************************************************/
uint32_t clock_source_get_hz(const uint8_t clock_source){
	uint32_t clock_freq = 0;
	
	switch (clock_source) {
		case GCLK_SOURCE_XOSC:
			clock_freq = CONF_CLOCK_XOSC_EXTERNAL_FREQUENCY;
			break;

		case GCLK_SOURCE_OSC8M:
			clock_freq = 8000000UL >> SYSCTRL->OSC8M.bit.PRESC;
			break;

		case GCLK_SOURCE_DFLL48M:

			// Make sure that the DFLL module is ready
#ifndef UNIT_TEST			
			while (!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY)) {
				// Wait for DFLL sync
			}
#else
            WAIT_FOR_SYNCH(!(SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY));
#endif			

			// Check if operating in closed loop mode
			clock_freq = gclk_chan_get_hz(SYSCTRL_GCLK_ID_DFLL48) * (sysctrl_get_dfllmul() & 0xffff);
			break;

		default:
			break;
	}
	
	return clock_freq;
}
/******************************************************************//**
* @brief		get frequency of chosen generic clock channel
* @param[in]	channel	number of generic clock channel
* @return		uint32_t
**********************************************************************/
uint32_t gclk_chan_get_hz(const uint8_t channel){
	uint8_t gen_id;

	// Select the requested generic clock channel
	*((uint8_t*)&GCLK->CLKCTRL.reg) = channel;
	gen_id = GCLK->CLKCTRL.bit.GEN;

	// Return the clock speed of the associated GCLK generator
	return lld_sys_gclk_gen_get_hz(gen_id);
}
/******************************************************************//**
* @brief		initialize generic clocks for the peripheral modules
* @return		void
**********************************************************************/
void lld_sys_gclk_init_peripheral( void ){
	
	// NVM
	// default GCLK main
	
	// USART
	configure_gclk_chan(GCLK_CLKCTRL_ID_SERCOM2_CORE_Val, GCLK_CLKCTRL_GEN_GCLK0_Val);
	enable_gclk_chan(GCLK_CLKCTRL_ID_SERCOM2_CORE_Val);
	
	// ADC
	//configure_gclk_chan(GCLK_CLKCTRL_ID_ADC_Val, GCLK_CLKCTRL_GEN_GCLK4_Val);
	configure_gclk_chan(GCLK_CLKCTRL_ID_ADC_Val, GCLK_CLKCTRL_GEN_GCLK0_Val);
	enable_gclk_chan(GCLK_CLKCTRL_ID_ADC_Val);	
	
	// TCC0
	configure_gclk_chan(GCLK_CLKCTRL_ID_TCC0_TCC1_Val, GCLK_CLKCTRL_GEN_GCLK0_Val);
	enable_gclk_chan(GCLK_CLKCTRL_ID_TCC0_TCC1_Val);
		
	// TCC2
	configure_gclk_chan(GCLK_CLKCTRL_ID_TCC2_TC3_Val, GCLK_CLKCTRL_GEN_GCLK0_Val);
	enable_gclk_chan(GCLK_CLKCTRL_ID_TCC2_TC3_Val);	
	
	// EVSYS
	configure_gclk_chan(GCLK_CLKCTRL_ID_EVSYS_1_Val, GCLK_CLKCTRL_GEN_GCLK0_Val);
	enable_gclk_chan(GCLK_CLKCTRL_ID_EVSYS_1_Val);	
		
	// EXTINT
	configure_gclk_chan(GCLK_CLKCTRL_ID_EIC_Val, GCLK_CLKCTRL_GEN_GCLK0_Val);
	enable_gclk_chan(GCLK_CLKCTRL_ID_EIC_Val);	
	
	// PTC
	configure_gclk_chan(GCLK_CLKCTRL_ID_PTC_Val, GCLK_CLKCTRL_GEN_GCLK3_Val);
	enable_gclk_chan(GCLK_CLKCTRL_ID_PTC_Val);	
	
	// HIGH SPEED USART
	configure_gclk_chan(GCLK_CLKCTRL_ID_SERCOM4_CORE_Val, GCLK_CLKCTRL_GEN_GCLK0_Val);
	enable_gclk_chan(GCLK_CLKCTRL_ID_SERCOM4_CORE_Val);
}
/******************************************************************//**
* @brief		initialize power management for the peripheral modules
* @return		-
**********************************************************************/
void lld_sys_pm_init_peripheral(void){
	
	// NVM
	PM->AHBMASK.reg |= PM_APBBMASK_NVMCTRL;
	
	// PM
	PM->APBAMASK.reg |= PM_APBAMASK_PM;
	
	// SYSCTRL
	PM->APBAMASK.reg |= PM_APBAMASK_SYSCTRL;	
	
	// GCLK
	PM->APBAMASK.reg |= PM_APBAMASK_GCLK;	
	
	// EXTINT
	PM->APBAMASK.reg |= PM_APBAMASK_EIC;
	
	// WDT
	//PM->APBAMASK.reg |= PM_APBAMASK_WDT;
	
	// USART
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM2;
	
	// ADC
	PM->APBCMASK.reg |= PM_APBCMASK_ADC;
	
	// TCC0
	PM->APBCMASK.reg |= PM_APBCMASK_TCC0;
		
	// TCC2
	PM->APBCMASK.reg |= PM_APBCMASK_TCC2;
	
	// EVSYS
	PM->APBCMASK.reg |= PM_APBCMASK_EVSYS;
	
	// PTC
	PM->APBCMASK.reg |= PM_APBCMASK_PTC;

	// HIGH SPEED USART
	PM->APBCMASK.reg |= PM_APBCMASK_SERCOM4;
}
/*******************************************************************//**
* @brief		set the module interrupts to predefined levels
* @return    	-
**********************************************************************/
void lld_sys_nvic_init_isr_prio( void ){
	
	//EIC_IRQn unused
	//TCC2_IRQn unused
	//PM_IRQn unused
	//SYSCTRL_IRQn unused
	//EVSYS_IRQn unused
	//NVMCTRL_IRQn unused
	//WDT_IRQn unused
	
	// --- highest prio ---
	
	// NonMaskableInt - default 0	
	// HardFault - default 0
	
	// sys tick
	//NVIC_SetPriority(SysTick_IRQn, ISR_PRIO_1);
	NVIC_SetPriority(SysTick_IRQn, ISR_PRIO_1);
	
	// sercom2 for LIN
	//NVIC_SetPriority(SERCOM2_IRQn, ISR_PRIO_1);
	NVIC_SetPriority(SERCOM2_IRQn, ISR_PRIO_1);
	
	// PTC touch
	//NVIC_SetPriority(PTC_IRQn, ISR_PRIO_2);
	NVIC_SetPriority(PTC_IRQn, ISR_PRIO_2);
	
	// ADC for Rocker
	NVIC_SetPriority(ADC_IRQn, ISR_PRIO_3);
	
	// Timer for PWM Reverser
	NVIC_SetPriority(TCC0_IRQn, ISR_PRIO_3);
	
	// sercom4 for highspeed usart
	NVIC_SetPriority(SERCOM4_IRQn, ISR_PRIO_3);
			
	// --- lowest prio ---
}
/*******************************************************************//**
* @brief		get the device/mcu/cpu/chip revision
* @return    	unsigned 4bit value revision number
**********************************************************************/
uint8_t lld_sys_get_device_revision( void ){
	return DSU->DID.bit.REVISION;
}
/*******************************************************************//**
* @brief		get the reset cause
* @return    	unsigned 8bit value reset cause
**********************************************************************/
uint8_t lld_sys_get_reset_cause( void ){
    return PM->RCAUSE.reg;
}