#include "Chassis.h"
#include "Global_status.h"
#include "Gimbal.h"
#include "ui.h"

#include "referee_system.h"
#include "supercup.h"
#include "stm32_time.h"

#include "pid.h"
#include "ramp_generator.h"
#include "User_math.h"

pid_t chassis_speed_pid_FL, chassis_speed_pid_FR, chassis_speed_pid_BL, chassis_speed_pid_BR; // �����ٶȿ���Pid
pid_t chassis_T_pid_x, chassis_T_pid_y, chassis_T_pid_w;
pid_t chassis_follow_pid; // ���̸���
RampGenerator Vx_ramp, Vy_ramp;
extern pid_t yaw_auto_location_pid;

float power_limt(float FL_current, float FR_current, float BL_current, float BR_current,
                 float FL_speed, float FR_speed, float BL_speed, float BR_speed, float max_p)
{
    float current[4] = {FL_current, FR_current, BL_current, BR_current};
    float speed[4] = {FL_speed, FR_speed, BL_speed, BR_speed};
    float now_p = 0.0f;
    float b = 0.015f;
    const float a = 2.56e-07;   // k1  //2.56e-07
    const float k2 = 1.553e-07; // k2
    const float constant = 4.081f;
    // ���ٱȸ�Ϊ14�ø�ϵ�������ļ��ٱ�Ϊ19Ϊע�ͺ�ϵ��
    const float toque_coefficient = (20.0f / 16384.0f) * (0.22f) * (187.0f / 3591.0f) / 9.55f; // (20/16384)*(0.3)*(187/3591)/9.55=1.99688994e-6f P19
    /*���������*/
    Supercap_set_power(max_p);
    if (cap.remain_vol <= 9)
        max_p -= 2.0f; // 2w����
    else if (cap.remain_vol > 9)
    {
        max_p += cap.remain_vol * 14;
    }
    /*���㵱ǰ����*/
    for (int i = 0; i < 4; i++)
    {
        // ���㹦��
        // ���������֣�https://github.com/MaxwellDemonLin/Motor-modeling-and-power-control/blob/master/chassis_power_control.c#L89
        // ��ǰ���� = ������ * ��������ת��ϵ�� * �ٶ� + K2 * �ٶȵ�ƽ�� + a * ������ƽ���� / Ч��
        // K2 �������ٶ����ޣ����ٳ���������K2
        // a ����������ʱ�����������𲽳���������a
        // Ч��һ�㲻�ö���ԼΪ0.6-0.85
        now_p += fabs(current[i] * toque_coefficient * speed[i] + // ���� * ��������ת��ϵ�� * �ٶ�
                      k2 * speed[i] * speed[i] +                  // K2 * �ٶȵ�ƽ��
                      a * current[i] * current[i] +               // a * ������ƽ��
                      constant) /
                 0.85f;
    }
    float percentage = max_p / now_p;
    if (percentage > 1.0f)
        return 1.0f;
    return percentage - b; // b ��һ����ѧ�Ĳ��������ڲ�������ٶ�ʱ���ʳ��������
}

/**
 * @brief ���̳�ʼ��
 *
 */
void Chassis_init()
{
    /*�����ʼ��*/
    CHASSISMotor_init(M3508_P, WHEEL_FL);
    CHASSISMotor_init(M3508_P, WHEEL_FR);
    CHASSISMotor_init(M3508_P, WHEEL_BL);
    CHASSISMotor_init(M3508_P, WHEEL_BR);
    /*PID�ٶȻ���ʼ��*/
    // pid_set(&chassis_speed_pid_FL, 200.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    // pid_set(&chassis_speed_pid_FR, 200.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    // pid_set(&chassis_speed_pid_BL, 200.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    // pid_set(&chassis_speed_pid_BR, 200.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_speed_pid_FL, 2.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_speed_pid_FR, 2.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_speed_pid_BL, 2.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_speed_pid_BR, 2.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    /*���̸���PID*/
    pid_set(&chassis_follow_pid, 5.0f, 0.0f, 10.0f, 200, 40);
    /*��������PID*/
    pid_set(&chassis_T_pid_x, 20.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_T_pid_y, 20.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_T_pid_w, 10.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    /*�����˶�б��*/
    RampGenerator_Init(&Vx_ramp, CHASSIS_TASK_TIME, 4, 30, 2);
    RampGenerator_Init(&Vy_ramp, CHASSIS_TASK_TIME, 4, 30, 4);
    /*Ĭ�ϵ��̸���ģʽ*/
    Global.Chssis.mode = FLOW;
}

/**
 * @brief ���õ����ƶ��ٶ�
 *
 */
void Chassis_set_x(float x)
{
    RampGenerator_SetTarget(&Vx_ramp, x);
    // Global.Chssis.input.x = x;
}
void Chassis_set_y(float y)
{
    RampGenerator_SetTarget(&Vy_ramp, y);
    // Global.Chssis.input.y = y;
}
void Chassis_set_r(float r)
{
    Global.Chssis.input.r = r;
}

void Chassis_set_accel(float acc)
{
    RampGenerator_SetAccel(&Vx_ramp, acc);
    RampGenerator_SetAccel(&Vy_ramp, acc);
}

/**
 * @brief ���̵������
 *
 * @param FL_speed ��ǰ���ٶ�
 * @param FR_speed ��ǰ���ٶ�
 * @param BL_speed ������ٶ�
 * @param BR_speed �Һ����ٶ�
 */
float Vx_now, Vy_now, W_now;
float T_FR, T_FL, T_BR, T_BL, K;
void ChassisMotor_Control(float Vx_speed, float Vy_speed, float R_speed)
{
    float FL_speed, FR_speed, BL_speed, BR_speed;
    float Fx, Fy, T_w;
    /*�������*/
    FL_speed = RAD_S_TO_RPM * IK_WHEEL_FL(Vx_speed, Vy_speed, R_speed);
    FR_speed = RAD_S_TO_RPM * IK_WHEEL_FR(Vx_speed, Vy_speed, R_speed);
    BL_speed = RAD_S_TO_RPM * IK_WHEEL_BL(Vx_speed, Vy_speed, R_speed);
    BR_speed = RAD_S_TO_RPM * IK_WHEEL_BR(Vx_speed, Vy_speed, R_speed);
    /*���⳵��*/
    Vx_now = FK_VX(CHASSISMotor_get_data(WHEEL_FL).speed_rpm / 14.0f * RPM_TO_RAD_S,
                   CHASSISMotor_get_data(WHEEL_FR).speed_rpm / 14.0f * RPM_TO_RAD_S,
                   CHASSISMotor_get_data(WHEEL_BL).speed_rpm / 14.0f * RPM_TO_RAD_S,
                   CHASSISMotor_get_data(WHEEL_BR).speed_rpm / 14.0f * RPM_TO_RAD_S);
    Vy_now = FK_VY(CHASSISMotor_get_data(WHEEL_FL).speed_rpm / 14.0f * RPM_TO_RAD_S,
                   CHASSISMotor_get_data(WHEEL_FR).speed_rpm / 14.0f * RPM_TO_RAD_S,
                   CHASSISMotor_get_data(WHEEL_BL).speed_rpm / 14.0f * RPM_TO_RAD_S,
                   CHASSISMotor_get_data(WHEEL_BR).speed_rpm / 14.0f * RPM_TO_RAD_S);
    W_now = FK_OMEGA(CHASSISMotor_get_data(WHEEL_FL).speed_rpm / 14.0f * RPM_TO_RAD_S,
                     CHASSISMotor_get_data(WHEEL_FR).speed_rpm / 14.0f * RPM_TO_RAD_S,
                     CHASSISMotor_get_data(WHEEL_BL).speed_rpm / 14.0f * RPM_TO_RAD_S,
                     CHASSISMotor_get_data(WHEEL_BR).speed_rpm / 14.0f * RPM_TO_RAD_S);
    /*PID����ó�ǰ������*/
    Fx = pid_cal(&chassis_T_pid_x, Vx_now, Vx_speed);
    Fy = pid_cal(&chassis_T_pid_y, Vy_now, Vy_speed);
    T_w = pid_cal(&chassis_T_pid_w, W_now, R_speed);
    T_FR = IK_WHEEL_FR_T(Fx, Fy, T_w) / CHASSISMOTOR_T_A * CHASSISMOTOR_MAX_CURRENT / 20.0f;
    T_FL = IK_WHEEL_FL_T(Fx, Fy, T_w) / CHASSISMOTOR_T_A * CHASSISMOTOR_MAX_CURRENT / 20.0f;
    T_BR = IK_WHEEL_BR_T(Fx, Fy, T_w) / CHASSISMOTOR_T_A * CHASSISMOTOR_MAX_CURRENT / 20.0f;
    T_BL = IK_WHEEL_BL_T(Fx, Fy, T_w) / CHASSISMOTOR_T_A * CHASSISMOTOR_MAX_CURRENT / 20.0f;
    float Plimit = 0.0f;

    /*��������*/
    float current[4] = {T_FL + pid_cal(&chassis_speed_pid_FL, CHASSISMotor_get_data(WHEEL_FL).speed_rpm / 14.0f, FL_speed),
                        T_FR + pid_cal(&chassis_speed_pid_FR, CHASSISMotor_get_data(WHEEL_FR).speed_rpm / 14.0f, FR_speed),
                        T_BL + pid_cal(&chassis_speed_pid_BL, CHASSISMotor_get_data(WHEEL_BL).speed_rpm / 14.0f, BL_speed),
                        T_BR + pid_cal(&chassis_speed_pid_BR, CHASSISMotor_get_data(WHEEL_BR).speed_rpm / 14.0f, BR_speed)};
    for (int i = 0; i < 4; i++)
    {
        if (current[i] >= (CHASSISMOTOR_MAX_CURRENT - 1))
            current[i] = CHASSISMOTOR_MAX_CURRENT - 1;
        if (current[i] <= -(CHASSISMOTOR_MAX_CURRENT - 1))
            current[i] = -(CHASSISMOTOR_MAX_CURRENT - 1);
    }
    /*��������*/
    Plimit = power_limt(current[0], current[1], current[2], current[3],
                        CHASSISMotor_get_data(WHEEL_FL).speed_rpm,
                        CHASSISMotor_get_data(WHEEL_FR).speed_rpm,
                        CHASSISMotor_get_data(WHEEL_BL).speed_rpm,
                        CHASSISMotor_get_data(WHEEL_BR).speed_rpm,
                        Referee_data.Chassis_Power_Limit);
    Plimit = 1; // �رչ��ʿ���
    /*��������*/
    CHASSISMotor_set(Plimit * current[0], WHEEL_FL);
    CHASSISMotor_set(Plimit * current[1], WHEEL_FR);
    CHASSISMotor_set(Plimit * current[2], WHEEL_BL);
    CHASSISMotor_set(Plimit * current[3], WHEEL_BR);
}

/**
 * @brief �����ƶ�����
 *
 */

void Chassis_move()
{
    float X_speed, Y_speed, R_speed;
    float relative_angle;
    /*�����ٶȸ���*/
    RampGenerator_Update(&Vx_ramp, Get_sys_time_ms());
    RampGenerator_Update(&Vy_ramp, Get_sys_time_ms());
    Global.Chssis.input.x = RampGenerator_GetCurrent(&Vx_ramp);
    Global.Chssis.input.y = RampGenerator_GetCurrent(&Vy_ramp);
    relative_angle = GIMBALMotor_get_data(YAWMotor).angle_cnt - GIMBALMotor_get_data(YAWMotor).angle_zero;
    // �˶��ֽ�
    Y_speed = Global.Chssis.input.x * sinf(DEG_TO_RAD * GIMBALMotor_get_data(YAWMotor).angle) +
              Global.Chssis.input.y * cosf(DEG_TO_RAD * GIMBALMotor_get_data(YAWMotor).angle);
    X_speed = Global.Chssis.input.x * cosf(DEG_TO_RAD * GIMBALMotor_get_data(YAWMotor).angle) -
              sinf(DEG_TO_RAD * GIMBALMotor_get_data(YAWMotor).angle) * Global.Chssis.input.y;
    R_speed = Global.Chssis.input.r;
    /*�����Ȧ�Ƕ�*/
    uint32_t mul = fabs(relative_angle) / 180.0f;
    if (relative_angle > 180.0f)
    {
        if (mul % 2 == 1) // ����-180��
            relative_angle -= (mul + 1) * 180.0f;
        else // ����180��
            relative_angle -= mul * 180.0f;
    }
    if (relative_angle < -180.0f)
    {
        if (mul % 2 == 1) // ����180��
            relative_angle += (mul + 1) * 180.0f;
        else // ����-180��
            relative_angle += mul * 180.0f;
    }
    Chassisui_change(relative_angle);
    /*ģʽѡ��*/
    switch (Global.Chssis.mode)
    {
    case FLOW:
        if(Global.Auto.input.Auto_control_online <= 0 || Global.Auto.mode == NONE)
        R_speed = pid_cal(&chassis_follow_pid, DEG_TO_RAD * relative_angle, 0.0f) - 0.9 * yaw_location_pid.total_out * DEG_TO_RAD;
        else
        R_speed = pid_cal(&chassis_follow_pid, DEG_TO_RAD * relative_angle, 0.0f) - 0.9 * yaw_auto_location_pid.total_out * DEG_TO_RAD;
        break;
    case SPIN_P:
        R_speed = 60 * RPM_TO_RAD_S;
        break;
    case SPIN_N:
        R_speed = -60 * RPM_TO_RAD_S;
        break;
    }
    ChassisMotor_Control(X_speed, Y_speed, R_speed);
}