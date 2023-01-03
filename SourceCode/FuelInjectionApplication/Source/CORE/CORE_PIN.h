/**
 * @file CORE_PIN.h
 * @author psikora
 * @brief
 * @version 0.1
 * @date 2022-12-29
 *
 * @copyright Copyright (c) 2022
 *
 */

#ifndef CORE_PIN_H_
#define CORE_PIN_H_

//////////////////////////////////////////////////////////////////////////
// Include and defines
//////////////////////////////////////////////////////////////////////////
#include "../PIN_Definitions.h"

enum IO_Config_Direction_Enum
{
    Input,
    Output
};

enum IO_Config_Control_Mode_Enum
{
    On_Demand_Sampling,
    Continuos_Sampling
};

enum IO_Config_Peripheral_Enable_Enum
{
    Pmux_Mode_Disable,
    Pmux_Mode_Enable
};

enum IO_Config_Peripheral_Mode_Enum
{
    Peripheral_Mode_A,
    Peripheral_Mode_B,
    Peripheral_Mode_C,
    Peripheral_Mode_D,
    Peripheral_Mode_E,
    Peripheral_Mode_F,
    Peripheral_Mode_G,
    Peripheral_Mode_H,
    Peripheral_Mode_I,

    Peripheral_Mode_Off
};

enum IO_Config_Input_Enable_Enum
{
    Input_Buffer_Disable,
    Input_Buffer_Enable
};

enum IO_Config_Pull_Type_Enum
{
    Pull_Down,
    Pull_Up
};

enum IO_Config_Driver_Strength_Enum
{
    Normal,
    Strong
};

class Pin
{
protected:
    uint8_t Pin_Num;
    uint8_t Port_Num;

public:
    Pin(Port_Pin_Type PortPin)
    {
        Pin_Num = PortPin.Pin;
        Port_Num = PortPin.Port;
    };

    void Set_Direction(IO_Config_Direction_Enum Direction);
    void Set_Control_Mode(IO_Config_Control_Mode_Enum Control_Mode);
    void Set_PMux_Mode(IO_Config_Peripheral_Mode_Enum PMux_Mode);
    void Set_Input_Enable(IO_Config_Input_Enable_Enum Input_Enable);
    void Set_Pull_Up_Mode(IO_Config_Pull_Type_Enum Pull_Type);
    void Set_Driver_Strength(IO_Config_Driver_Strength_Enum Driver_Strength);
};

#endif /* CORE_PIN_H_ */