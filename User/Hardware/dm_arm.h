#ifndef __DM_ARM_H__
#define __DM_ARM__H__

#include "can_receive_send.h"
#include "dm4310_drv.h"
typedef enum  Enum_Motor_DM_Status
{
    Motor_DM_Status_DISABLE = 0,
    Motor_DM_Status_ENABLE,
}Motor_DM_Status;
extern Motor_DM_Status DM_Status[6];

void Arm_Init(void);
void Arm_motor4(void);
void Arm_motor5(void);
#endif