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

#include "PIN_Definitions.h"

#ifndef CORE_PIN_H_
#define CORE_PIN_H_

typedef enum IO_Config_Direction_Enum
{
    Input,
    Output
};

typedef enum IO_Config_Driver_Strength_Enum
{
    Normal,
    Strong
};

typedef enum IO_Config_Pull_Type_Enum
{
    Pull_Down,
    Pull_Up
};

typedef enum IO_Config_Input_Enable_Enum
{
    Disable,
    Enable
};

typedef enum IO_Config_Peripheral_Enable_Enum
{
    Disable,
    Enable
};

typedef enum IO_Config_Peripheral_Mode_Enum
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

typedef struct IO_Config_Struct
{
    IO_Config_Direction_Enum IO_Direction;

    IO_Config_Driver_Strength_Enum Driver_Strength;
    IO_Config_Pull_Type_Enum Pull_Type;
    IO_Config_Input_Enable_Enum Input_Enable;
    IO_Config_Peripheral_Enable_Enum P_Mux_Enable;

    IO_Config_Peripheral_Mode_Enum P_Mux_Mode;
};

class Pin
{
private:
    uint8_t Port;
    uint8_t Pin;
    IO_Config_Struct IO_Config;

public:
    Pin(Port_Pin_Type PortPin, IO_Config_Struct IO_Config)
        : IO_Config(IO_Config)
    {
        Port = PortPin.Port;
        Pin = PortPin.Pin;

        PORT->Group[Port].PINCFG[Pin].reg = IO_Config.Driver_Strength * PORT_PINCFG_PULLEN +
                                            IO_Config.Pull_Type * PORT_PINCFG_PULLEN +
                                            IO_Config.Input_Enable * PORT_PINCFG_INEN +
                                            IO_Config.P_Mux_Enable * PORT_PINCFG_PMUXEN;
    }
};

#endif /* CORE_PIN_H_ */