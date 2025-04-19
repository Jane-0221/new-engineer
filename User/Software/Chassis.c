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

pid_t chassis_speed_pid_FL, chassis_speed_pid_FR, chassis_speed_pid_BL, chassis_speed_pid_BR; // 轮子速度控制Pid
pid_t chassis_T_pid_x, chassis_T_pid_y, chassis_T_pid_w;
pid_t chassis_follow_pid; // 底盘跟随
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
    // 减速比改为14用该系数，不改减速比为19为注释后系数
    const float toque_coefficient = (20.0f / 16384.0f) * (0.22f) * (187.0f / 3591.0f) / 9.55f; // (20/16384)*(0.3)*(187/3591)/9.55=1.99688994e-6f P19
    /*最大功率设置*/
    Supercap_set_power(max_p);
    if (cap.remain_vol <= 9)
        max_p -= 2.0f; // 2w余量
    else if (cap.remain_vol > 9)
    {
        max_p += cap.remain_vol * 14;
    }
    /*估算当前功率*/
    for (int i = 0; i < 4; i++)
    {
        // 估算功率
        // 西交利物浦：https://github.com/MaxwellDemonLin/Motor-modeling-and-power-control/blob/master/chassis_power_control.c#L89
        // 当前功率 = （电流 * 电流力矩转换系数 * 速度 + K2 * 速度的平方 + a * 电流的平方） / 效率
        // K2 决定了速度上限，高速超功率升高K2
        // a 决定了启动时的最大电流，起步超功率升高a
        // 效率一般不用动，约为0.6-0.85
        now_p += fabs(current[i] * toque_coefficient * speed[i] + // 电流 * 电流力矩转换系数 * 速度
                      k2 * speed[i] * speed[i] +                  // K2 * 速度的平方
                      a * current[i] * current[i] +               // a * 电流的平方
                      constant) /
                 0.85f;
    }
    float percentage = max_p / now_p;
    if (percentage > 1.0f)
        return 1.0f;
    return percentage - b; // b 做一定玄学的补偿，用于补偿最大速度时功率超出的误差
}

/**
 * @brief 底盘初始化
 *
 */
void Chassis_init()
{
    /*电机初始化*/
    CHASSISMotor_init(M3508_P, WHEEL_FL);
    CHASSISMotor_init(M3508_P, WHEEL_FR);
    CHASSISMotor_init(M3508_P, WHEEL_BL);
    CHASSISMotor_init(M3508_P, WHEEL_BR);
    /*PID速度环初始化*/
    // pid_set(&chassis_speed_pid_FL, 200.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    // pid_set(&chassis_speed_pid_FR, 200.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    // pid_set(&chassis_speed_pid_BL, 200.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    // pid_set(&chassis_speed_pid_BR, 200.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_speed_pid_FL, 2.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_speed_pid_FR, 2.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_speed_pid_BL, 2.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_speed_pid_BR, 2.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    /*底盘跟随PID*/
    pid_set(&chassis_follow_pid, 5.0f, 0.0f, 10.0f, 200, 40);
    /*底盘力控PID*/
    pid_set(&chassis_T_pid_x, 20.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_T_pid_y, 20.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    pid_set(&chassis_T_pid_w, 10.0f, 0.0, 0, CHASSISMOTOR_MAX_CURRENT, 10000);
    /*底盘运动斜坡*/
    RampGenerator_Init(&Vx_ramp, CHASSIS_TASK_TIME, 4, 30, 2);
    RampGenerator_Init(&Vy_ramp, CHASSIS_TASK_TIME, 4, 30, 4);
    /*默认地盘跟随模式*/
    Global.Chssis.mode = FLOW;
}

/**
 * @brief 设置底盘移动速度
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
 * @brief 底盘电机控制
 *
 * @param FL_speed 左前轮速度
 * @param FR_speed 右前轮速度
 * @param BL_speed 左后轮速度
 * @param BR_speed 右后轮速度
 */
float Vx_now, Vy_now, W_now;
float T_FR, T_FL, T_BR, T_BL, K;
void ChassisMotor_Control(float Vx_speed, float Vy_speed, float R_speed)
{
    float FL_speed, FR_speed, BL_speed, BR_speed;
    float Fx, Fy, T_w;
    /*逆解轮速*/
    FL_speed = RAD_S_TO_RPM * IK_WHEEL_FL(Vx_speed, Vy_speed, R_speed);
    FR_speed = RAD_S_TO_RPM * IK_WHEEL_FR(Vx_speed, Vy_speed, R_speed);
    BL_speed = RAD_S_TO_RPM * IK_WHEEL_BL(Vx_speed, Vy_speed, R_speed);
    BR_speed = RAD_S_TO_RPM * IK_WHEEL_BR(Vx_speed, Vy_speed, R_speed);
    /*正解车速*/
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
    /*PID计算得出前馈力矩*/
    Fx = pid_cal(&chassis_T_pid_x, Vx_now, Vx_speed);
    Fy = pid_cal(&chassis_T_pid_y, Vy_now, Vy_speed);
    T_w = pid_cal(&chassis_T_pid_w, W_now, R_speed);
    T_FR = IK_WHEEL_FR_T(Fx, Fy, T_w) / CHASSISMOTOR_T_A * CHASSISMOTOR_MAX_CURRENT / 20.0f;
    T_FL = IK_WHEEL_FL_T(Fx, Fy, T_w) / CHASSISMOTOR_T_A * CHASSISMOTOR_MAX_CURRENT / 20.0f;
    T_BR = IK_WHEEL_BR_T(Fx, Fy, T_w) / CHASSISMOTOR_T_A * CHASSISMOTOR_MAX_CURRENT / 20.0f;
    T_BL = IK_WHEEL_BL_T(Fx, Fy, T_w) / CHASSISMOTOR_T_A * CHASSISMOTOR_MAX_CURRENT / 20.0f;
    float Plimit = 0.0f;

    /*电流计算*/
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
    /*功率限制*/
    Plimit = power_limt(current[0], current[1], current[2], current[3],
                        CHASSISMotor_get_data(WHEEL_FL).speed_rpm,
                        CHASSISMotor_get_data(WHEEL_FR).speed_rpm,
                        CHASSISMotor_get_data(WHEEL_BL).speed_rpm,
                        CHASSISMotor_get_data(WHEEL_BR).speed_rpm,
                        Referee_data.Chassis_Power_Limit);
    Plimit = 1; // 关闭功率控制
    /*电流设置*/
    CHASSISMotor_set(Plimit * current[0], WHEEL_FL);
    CHASSISMotor_set(Plimit * current[1], WHEEL_FR);
    CHASSISMotor_set(Plimit * current[2], WHEEL_BL);
    CHASSISMotor_set(Plimit * current[3], WHEEL_BR);
}

/**
 * @brief 底盘移动解算
 *
 */

void Chassis_move()
{
    float X_speed, Y_speed, R_speed;
    float relative_angle;
    /*底盘速度更新*/
    RampGenerator_Update(&Vx_ramp, Get_sys_time_ms());
    RampGenerator_Update(&Vy_ramp, Get_sys_time_ms());
    Global.Chssis.input.x = RampGenerator_GetCurrent(&Vx_ramp);
    Global.Chssis.input.y = RampGenerator_GetCurrent(&Vy_ramp);
    relative_angle = GIMBALMotor_get_data(YAWMotor).angle_cnt - GIMBALMotor_get_data(YAWMotor).angle_zero;
    // 运动分解
    Y_speed = Global.Chssis.input.x * sinf(DEG_TO_RAD * GIMBALMotor_get_data(YAWMotor).angle) +
              Global.Chssis.input.y * cosf(DEG_TO_RAD * GIMBALMotor_get_data(YAWMotor).angle);
    X_speed = Global.Chssis.input.x * cosf(DEG_TO_RAD * GIMBALMotor_get_data(YAWMotor).angle) -
              sinf(DEG_TO_RAD * GIMBALMotor_get_data(YAWMotor).angle) * Global.Chssis.input.y;
    R_speed = Global.Chssis.input.r;
    /*化简多圈角度*/
    uint32_t mul = fabs(relative_angle) / 180.0f;
    if (relative_angle > 180.0f)
    {
        if (mul % 2 == 1) // 处于-180度
            relative_angle -= (mul + 1) * 180.0f;
        else // 处于180度
            relative_angle -= mul * 180.0f;
    }
    if (relative_angle < -180.0f)
    {
        if (mul % 2 == 1) // 处于180度
            relative_angle += (mul + 1) * 180.0f;
        else // 处于-180度
            relative_angle += mul * 180.0f;
    }
    Chassisui_change(relative_angle);
    /*模式选择*/
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