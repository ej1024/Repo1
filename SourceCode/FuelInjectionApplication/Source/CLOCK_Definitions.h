/**
 * @file CLOCK_Definitions.h
 * @author psikora
 * @brief 
 * @version 0.1
 * @date 2023-01-02
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef CLOCK_DEFINITIONS_H_
#define CLOCK_DEFINITIONS_H_

//////////////////////////////////////////////////////////////////////////
// Include and defines
//////////////////////////////////////////////////////////////////////////
#include "EFI_Types.h"

// GCLK_MAIN Clock output IO Pin Definition
#define GCLK_MAIN_OUTPUT_PORT       PORTA
#define GCLK_MAIN_OUTPUT_PIN_NUMBER (28ul)
#define GCLK_MAIN_OUTPUT_PIN_MASK   PORT_PA28

// Constants for Clock Generators
#define GENERIC_CLOCK_GENERATOR_MAIN      (0u)
#define GENERIC_CLOCK_GENERATOR_XOSC32K   (1u)

// initialized at reset for WDT
#define GENERIC_CLOCK_GENERATOR_OSCULP32K (2u) 
#define GENERIC_CLOCK_GENERATOR_OSC8M     (3u)

// Constants for Clock multiplexers
#define GENERIC_CLOCK_MULTIPLEXER_DFLL48M (0u)

// Constants for DFLL48M
#define MAIN_CLK_FREQ (48000000u)

// Assumes 32.768 kHz Crystal is connected
#define EXT_32K_CLK_FREQ (32768u)

#endif /* CLOCK_DEFINITIONS_H_ */