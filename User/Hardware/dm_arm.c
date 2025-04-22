#include "dm_arm.h"

Motor_DM_Status DM_Status[6] = {Motor_DM_Status_DISABLE};
void Arm_Init()
{
  arm_motor_init(&arm_motor[Motor1], 0x01, POS_MODE); // ����ID�Ϳ���ģʽ
  arm_motor_init(&arm_motor[Motor2], 0x02, POS_MODE); // ����ID�Ϳ���ģʽ
  arm_motor_init(&arm_motor[Motor3], 0x03, POS_MODE); // ����ID�Ϳ���ģʽ
  arm_motor_init(&arm_motor[Motor4], 0x04, POS_MODE); // ����ID�Ϳ���ģʽ
  arm_motor_init(&arm_motor[Motor5], 0x05, POS_MODE); // ����ID�Ϳ���ģʽ

  enable_motor_mode(&hfdcan1, 0x01, POS_MODE);
  enable_motor_mode(&hfdcan2, 0x02, POS_MODE);
  enable_motor_mode(&hfdcan2, 0x03, POS_MODE);
  enable_motor_mode(&hfdcan2, 0x04, POS_MODE);
  enable_motor_mode(&hfdcan2, 0x05, POS_MODE);

  // �ٶ�
  vel_motor.vel_rootpitch = 1.0;
  vel_motor.vel_rootyaw = 1.0;
  vel_motor.vel_middleyaw = 1.0;
  vel_motor.vel_middlerow = 1.0;
  vel_motor.vel_endyaw = 3.0;
  //   vel_motor.vel_endrow = 1.0;
  // λ��
  // pos_motor.pos_endrow =0;
  pos_motor.pos_endyaw = 0;
  pos_motor.pos_middlerow = 0;
  pos_motor.pos_middleyaw = 0;
  pos_motor.pos_rootpitch = 0;
  pos_motor.pos_rootyaw = 0;
}
void Arm_motor4()
{

  // �жϸ�ʱ������Ƿ���չ��������
  if (Flag_damiao[4] == Pre_Flag_damiao[4])
  {
    // ����Ͽ�����
    DM_Status[4] = Motor_DM_Status_DISABLE;
    // �������������ʹ��
    // CAN_Send_Clear_Error(&hfdcan2,0x04);
    CAN_Send_Enter(&hfdcan2, 0x04);
  }
  else
  {
    // �����������
    DM_Status[4] = Motor_DM_Status_ENABLE;
  }

  // �������ݷ���
  set_DM_mode(Motor4, POS_MODE);
  set_DM_pos_vel(pos_motor.pos_middlerow, vel_motor.vel_middlerow, Motor4);
  pos_speed_ctrl(&hfdcan2, 0x04, arm_motor[Motor4].ctrl.pos_set, arm_motor[Motor4].ctrl.vel_set);

  Pre_Flag_damiao[4] = Flag_damiao[4];
}
void Arm_motor5()
{	 

      //�жϸ�ʱ������Ƿ���չ��������
   if (Flag_damiao[5]== Pre_Flag_damiao[5])
   {
     // ����Ͽ�����
      DM_Status[5] = Motor_DM_Status_DISABLE;
//�������������ʹ��
   //	CAN_Send_Clear_Error(&hfdcan2,0x05);
     CAN_Send_Enter(&hfdcan2,0x05);
   }
   else
   {
   }
             //�����������
            DM_Status[5] = Motor_DM_Status_ENABLE;
            //�������ݷ���
   set_DM_mode(Motor5,POS_MODE);
  set_DM_pos_vel(pos_motor.pos_endyaw,vel_motor.vel_endyaw,Motor5);	
  pos_speed_ctrl(&hfdcan2,0x05,arm_motor[Motor5].ctrl.pos_set,arm_motor[Motor5].ctrl.vel_set);
   
   
   
Pre_Flag_damiao[5]=Flag_damiao[5];

 }