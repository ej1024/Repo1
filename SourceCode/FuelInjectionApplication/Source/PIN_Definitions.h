/**
 * @file PIN_Definitions.h
 * @author psikora
 * @brief 
 * @version 0.1
 * @date 2022-12-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef PIN_DEFINITIONS_H_
#define PIN_DEFINITIONS_H_

//////////////////////////////////////////////////////////////////////////
// Include and defines
//////////////////////////////////////////////////////////////////////////
#include "EFI_Types.h"

typedef struct  {
  uint8_t Port; 
  uint8_t Pin;
} Port_Pin_Type;

// I/O Port Definitions
#define PORTA     (0ul)
#define PORTB     (1ul)

// Port A I/O Pin Defintions
#define PA00        {PORTA, 0}
#define PA01        {PORTA, 1}
#define PA02        {PORTA, 2}
#define PA03        {PORTA, 3}
#define PA04        {PORTA, 4}
#define PA05        {PORTA, 5}
#define PA06        {PORTA, 6}
#define PA07        {PORTA, 7}
#define PA08        {PORTA, 8}
#define PA09        {PORTA, 9}
#define PA10        {PORTA, 10}
#define PA11        {PORTA, 11}
#define PA12        {PORTA, 12}
#define PA13        {PORTA, 13}
#define PA14        {PORTA, 14}
#define PA15        {PORTA, 15}
#define PA16        {PORTA, 16}
#define PA17        {PORTA, 17}
#define PA18        {PORTA, 18}
#define PA19        {PORTA, 19}
#define PA20        {PORTA, 20}
#define PA21        {PORTA, 21}
#define PA22        {PORTA, 22}
#define PA23        {PORTA, 23}
#define PA24        {PORTA, 24}
#define PA25        {PORTA, 25}
#define PA27        {PORTA, 27}
#define PA28        {PORTA, 28}
#define PA30        {PORTA, 30}
#define PA31        {PORTA, 31}

// Port B I/O Pin Defintions        
#define PB00        {PORTB, 0}
#define PB01        {PORTB, 1}
#define PB02        {PORTB, 2}
#define PB03        {PORTB, 3}
#define PB04        {PORTB, 4}
#define PB05        {PORTB, 5}
#define PB06        {PORTB, 6}
#define PB07        {PORTB, 7}
#define PB08        {PORTB, 8}
#define PB09        {PORTB, 9}
#define PB10        {PORTB, 10}
#define PB11        {PORTB, 11}
#define PB12        {PORTB, 12}
#define PB13        {PORTB, 13}
#define PB14        {PORTB, 14}
#define PB15        {PORTB, 15}
#define PB16        {PORTB, 16}
#define PB17        {PORTB, 17}
#define PB22        {PORTB, 22}
#define PB23        {PORTB, 23}
#define PB30        {PORTB, 30}
#define PB31        {PORTB, 31}

#endif /* PIN_DEFINITIONS_H_ */