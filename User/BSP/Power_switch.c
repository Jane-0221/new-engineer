/**
 * @file Power_switch.c
 * @author Siri (lixirui2017@outlook.com)
 * @brief 达秒开发板电源开关功能
 * @version 0.1
 * @date 2024-12-20
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "main.h"
#include "Power_switch.h"

/**
 * @brief 设置电源状态
 *
 * @param power which power you want to set
 * @param status the status you want the power be
 */
void Power_set(powertyp power, GPIO_PinState status)
{
    if (power == power1)
        HAL_GPIO_WritePin(Power_OUT1_GPIO_Port, Power_OUT1_Pin, status);
    else if (power == power2)
        HAL_GPIO_WritePin(Power_OUT2_GPIO_Port, Power_OUT2_Pin, status);
}

/**
 * @brief 关闭电源
 * 
 * @param power the power you want to turn_off
 */
void Power_Turn_off(powertyp power)
{
    if (power == power1)
        HAL_GPIO_WritePin(Power_OUT1_GPIO_Port, Power_OUT1_Pin, RESET);
    else if (power == power2)
        HAL_GPIO_WritePin(Power_OUT2_GPIO_Port, Power_OUT2_Pin, RESET);
}

/**
 * @brief 打开电源
 * 
 * @param power the power you want to turn-on
 */
void Power_Turn_on(powertyp power)
{
    if (power == power1)
        HAL_GPIO_WritePin(Power_OUT1_GPIO_Port, Power_OUT1_Pin, SET);
    else if (power == power2)
        HAL_GPIO_WritePin(Power_OUT2_GPIO_Port, Power_OUT2_Pin, SET);
}

