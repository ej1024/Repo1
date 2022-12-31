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

/*  @TODO make core pin class
 *   @TODO make app gpio class, extends core pin
 *   @TODO make app pwm class, extends core pin
 *   @TODO make app uart class, extends core pin
 */

#include "../PIN_Definitions.h"

#ifndef CORE_PIN_H_
#define CORE_PIN_H_

enum IO_Config_Direction_Enum
{
    Input,
    Output
};

enum IO_Config_Driver_Strength_Enum
{
    Normal,
    Strong
};

enum IO_Config_Pull_Type_Enum
{
    Pull_Down,
    Pull_Up
};

enum IO_Config_Input_Enable_Enum
{
    Input_Sampling_Disable,
    Input_Sampling_Enable
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
    Peripheral_Mode_H
};

struct Basic_IO_Config_Struct
{
    IO_Config_Direction_Enum IO_Direction;
    IO_Config_Pull_Type_Enum Pull_Type;
};

// struct Adv_IO_Config_Struct
// {
//     IO_Config_Direction_Enum IO_Direction;

//     IO_Config_Driver_Strength_Enum Driver_Strength;
//     IO_Config_Pull_Type_Enum Pull_Type;
//     IO_Config_Input_Enable_Enum Input_Enable;
//     IO_Config_Peripheral_Enable_Enum P_Mux_Enable;

//     IO_Config_Peripheral_Mode_Enum P_Mux_Mode;
// };

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
};

#endif /* CORE_PIN_H_ */