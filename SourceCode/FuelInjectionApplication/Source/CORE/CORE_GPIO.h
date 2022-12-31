/**
 * @file CORE_GPIO.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2022-12-30
 * 
 * @copyright Copyright (c) 2022
 * 
 */

/*  @TODO make core pin class
 *   @TODO make app gpio class, extends core pin
 *   @TODO make app pwm class, extends core pin
 *   @TODO make app uart class, extends core pin
 */

#include "CORE_PIN.h"

#ifndef CORE_GPIO_H_
#define CORE_GPIO_H_

//Output Pin class extends the Pin class
class Output_Pin : public Pin {
private:
    IO_Config_Direction_Enum IO_Direction;
    IO_Config_Pull_Type_Enum IO_Pull_Type;
    IO_Config_Driver_Strength_Enum IO_Driver_Strength;
    
public:
	Output_Pin(Port_Pin_Type PortPin) : Pin(PortPin)
    {
        IO_Direction = Output;
        IO_Pull_Type = Pull_Up;
        IO_Driver_Strength = Normal;
        
        PORT->Group[Port_Num].DIRSET.reg |= IO_Direction << Pin_Num;
		PORT->Group[Port_Num].PINCFG[Pin_Num].bit.PULLEN = IO_Pull_Type;
        PORT->Group[Port_Num].PINCFG[Pin_Num].bit.DRVSTR = IO_Driver_Strength;
    };
};

//Input Pin class extends the Pin class
class Input_Pin : public Pin {
private:
    IO_Config_Direction_Enum IO_Direction;
    IO_Config_Pull_Type_Enum IO_Pull_Type;
    IO_Config_Input_Enable_Enum IO_Input_Sampling;

public:
    Input_Pin(Port_Pin_Type PortPin) : Pin(PortPin)
    {
        IO_Direction = Input;
        IO_Pull_Type = Pull_Up;
        IO_Input_Sampling = Input_Sampling_Disable;
        
        PORT->Group[Port_Num].PINCFG[Pin_Num].bit.PULLEN = IO_Pull_Type;
        PORT->Group[Port_Num].DIRSET.reg |= IO_Direction << Pin_Num;
		PORT->Group[Port_Num].PINCFG[Pin_Num].bit.INEN = IO_Input_Sampling;
		
		if (IO_Pull_Type == Pull_Up){
			PORT->Group[Port_Num].OUTSET.reg |= 1 << Pin_Num;
		}
		else { //(IO_Pull_Type == Pull_Up) 
			PORT->Group[Port_Num].OUTSET.reg |= 0 << Pin_Num;
		}
    };
};

#endif /* CORE_GPIO_H_ */