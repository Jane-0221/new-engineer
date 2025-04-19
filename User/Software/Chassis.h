#ifndef __CHASSIS_H__
#define __CHASSIS_H__

#include "CAN_receive_send.h"

#include "User_math.h"

#include "motor.h"
/*���̲���*/
#define WHEEL_FL CAN_1_2
#define WHEEL_FR CAN_1_1
#define WHEEL_BL CAN_1_3
#define WHEEL_BR CAN_1_4
#define R_body 0.273  // ���̰�װ�뾶����λ���ף�����ʵ��ֵ�޸ģ�
#define r_wheel 0.077 // ���Ӱ뾶����λ���ף�����ʵ��ֵ�޸ģ�

//---------------------------
// ���˶�ѧ�궨�壨�ٶ� �� ���٣���M_S �� RAD_S��
//---------------------------
// ���룺�����ٶ� vx, vy, ���ٶ� omega
// ������ĸ����ӵ�ת�٣������ͣ�
#define USE_OMNI_CHASSIS
#ifdef USE_OMNI_CHASSIS

// ǰ���� FL ת�ټ��㵥λ
#define IK_WHEEL_FL(vx, vy, omega) \
    ((SQRT1_2 * (vy) + SQRT1_2 * (vx) + (R_body) * (omega)) / r_wheel)

// ǰ���� FR ת�ټ���
#define IK_WHEEL_FR(vx, vy, omega) \
    ((-SQRT1_2 * (vy) + SQRT1_2 * (vx) + (R_body) * (omega)) / r_wheel)

// ������ BL ת�ټ���
#define IK_WHEEL_BL(vx, vy, omega) \
    ((+SQRT1_2 * (vy) - SQRT1_2 * (vx) + (R_body) * (omega)) / r_wheel)

// ������ BR ת�ټ���
#define IK_WHEEL_BR(vx, vy, omega) \
    ((-SQRT1_2 * (vy) - SQRT1_2 * (vx) + (R_body) * (omega)) / r_wheel)

#endif // USE_OMNI_CHASSIS

//---------------------------
// ���˶�ѧ�궨�壨���� �� �ٶȣ���RAD_S �� M_S��
//---------------------------
// ���룺�ĸ����� wFL, wFR, wBL, wBR
// ����������ٶ� vx, vy, ���ٶ� omega��ͨ��ָ�뷵�أ�

#ifdef USE_OMNI_CHASSIS

// ���� vx
#define FK_VX(wFL, wFR, wBL, wBR) \
    (((-(wBR) + (wFR) + (wFL) - (wBL)) * SQRT2 * r_wheel) / 4.0f)

// ���� vy
#define FK_VY(wFL, wFR, wBL, wBR) \
    ((((wFL) - (wBR) - (wFR) + (wBL)) * SQRT2 * r_wheel) / 4.0f)

// ���� omega�����ٶȣ�
#define FK_OMEGA(wFL, wFR, wBL, wBR) \
    ((((wFL) + (wFR) + (wBL) + (wBR)) * r_wheel) / (4.0f * R_body))

#endif

//---------------------------
// �涯��ѧ�궨�壨���������� �� ������أ���F �� T��
//---------------------------
// ���룺���������� Fx, Fy, ��ת����   T
// ������ĸ����ӵ����أ������ͣ�
#ifdef USE_OMNI_CHASSIS

// ǰ���� FL ���ؼ���
#define IK_WHEEL_FL_T(Fx, Fy, T) \
    ((SQRT2 * (Fy) + SQRT2 * (Fx) + (T) / 4.0f / (R_body)) * r_wheel)

// ǰ���� FR ���ؼ���
#define IK_WHEEL_FR_T(Fx, Fy, T) \
    ((-SQRT2 * (Fy) + SQRT2 * (Fx) + (T) / 4.0f / (R_body)) * r_wheel)

// ������ BL ���ؼ���
#define IK_WHEEL_BL_T(Fx, Fy, T) \
    ((+SQRT2 * (Fy) - SQRT2 * (Fx) + (T) / 4.0f / (R_body)) * r_wheel)

// ������ BR ���ؼ���
#define IK_WHEEL_BR_T(Fx, Fy, T) \
    ((-SQRT2 * (Fy) - SQRT2 * (Fx) + (T) / 4.0f / (R_body)) * r_wheel)

#endif // USE_OMNI_CHASSIS

/*���*/
#define USE_DJIMotor

/*�������*/
#ifdef USE_DJIMotor

#define CHASSISMotor_init DJIMotor_init
#define CHASSISMotor_set DJIMotor_set
#define CHASSISMotor_get_data DJIMotor_get_data
// ����һ��ʡ�ı���
void CHASSISMotor_init(Motor_Type_e motor_type, DJIcan_id motor_id);
void CHASSISMotor_set(int16_t val, DJIcan_id motor_id);
DJI_motor_data_s CHASSISMotor_get_data(DJIcan_id motor_id);
/*�������*/
#define CHASSISMOTOR_MAX_CURRENT DJIMOTOR_MAX_CURRENT
#define CHASSISMOTOR_T_A DJIMOTOR_T_A

#endif // USE_DJIMotor

/*�ڲ���������*/

/*�ⲿ����*/
void Chassis_init();
void Chassis_move();
void Chassis_set_x(float x);
void Chassis_set_y(float y);
void Chassis_set_r(float r);
void Chassis_set_accel(float acc);

#define CHASSIS_TASK_TIME 1 // ��������ˢ�¼��

#endif // !__CHASSIS_H__
