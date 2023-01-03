/**
 * @file CORE_PIN.cpp
 * @author psikora
 * @brief
 * @version 0.1
 * @date 2022-12-29
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "CORE_PIN.h"

//////////////////////////////////////////////////////////////////////////
// Pin Class Functions
//////////////////////////////////////////////////////////////////////////

/**
 * @brief 
 * 
 * @param Direction 
 */
void Pin::Set_Direction(IO_Config_Direction_Enum Direction) {
    if (Direction == Input) {
        PORT->Group[Port_Num].DIRCLR.reg |= Bit_Set(Pin_Num);
    }

    else { //(Direction == Output)
        PORT->Group[Port_Num].DIRSET.reg |= Bit_Set(Pin_Num);
    }
}

/**
 * @brief 
 * 
 * @param Control_Mode 
 */
void Pin::Set_Control_Mode(IO_Config_Control_Mode_Enum Control_Mode) {
    if (Control_Mode == On_Demand_Sampling) {
        PORT->Group[Port_Num].CTRL.reg &= Bit_Mask(Pin_Num);
    }

    else { //(Control_Mode == Continuos_Sampling)
        PORT->Group[Port_Num].CTRL.reg |= Bit_Set(Pin_Num);
    }
}

/**
 * @brief 
 * 
 * @param PMux_Mode 
 */
void Pin::Set_PMux_Mode(IO_Config_Peripheral_Mode_Enum PMux_Mode) {
    if (PMux_Mode < Peripheral_Mode_Off) {
        PORT->Group[Port_Num].PINCFG[Pin_Num].bit.PMUXEN = Pmux_Mode_Enable;

        if (Pin_Num%2 == 1) { // Odd Pin Number
            PORT->Group[Port_Num].PMUX[Pin_Num>>1].bit.PMUXO = PMux_Mode;
        }

        else if (Pin_Num%2 == 0) { // Even Pin Number
            PORT->Group[Port_Num].PMUX[Pin_Num>>1].bit.PMUXE = PMux_Mode;
        }
    }

    else { //(Pmux Mode == Turn Off)
        PORT->Group[Port_Num].PINCFG[Pin_Num].bit.PMUXEN = Pmux_Mode_Disable;
    }
}

/**
 * @brief 
 * 
 * @param Input_Enable 
 */
void Pin::Set_Input_Enable(IO_Config_Input_Enable_Enum Input_Enable) {
    PORT->Group[Port_Num].PINCFG[Pin_Num].bit.INEN = Input_Enable;
}

/**
 * @brief 
 * 
 * @param Pull_Type 
 */
void Pin::Set_Pull_Up_Mode(IO_Config_Pull_Type_Enum Pull_Type) {
    PORT->Group[Port_Num].PINCFG[Pin_Num].bit.PULLEN = Pull_Type;
}

/**
 * @brief 
 * 
 * @param Driver_Strength 
 */
void Pin::Set_Driver_Strength(IO_Config_Driver_Strength_Enum Driver_Strength) {
    PORT->Group[Port_Num].PINCFG[Pin_Num].bit.DRVSTR = Driver_Strength;
}
