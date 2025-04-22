#include "Chassis.h"
#include "Global_status.h"
#include "Gimbal.h"
#include "ui.h"
#include "IMU_updata.h"
#include "referee_system.h"
#include "supercup.h"
#include "stm32_time.h"
#include "remote_control.h"
#include "pid.h"
#include "ramp_generator.h"
#include "User_math.h"
#include "DT7.h"
pid_t chassis_speed_pid_FL, chassis_speed_pid_FR, chassis_speed_pid_BL, chassis_speed_pid_BR; // 轮子速度控制Pid
RampGenerator Vx_ramp, Vy_ramp;
extern pid_t yaw_auto_location_pid;

float AHRS_yaw_set = 0;
pid_t yaw_pid;

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

    /*轮组电机PID速度环初始化*/
    pid_set(&chassis_speed_pid_FR, 9000, 0, 120, CHASSISMOTOR_MAX_CURRENT, 5000); // 16000 1000
    pid_set(&chassis_speed_pid_FL, 9000, 0, 120, CHASSISMOTOR_MAX_CURRENT, 5000);
    pid_set(&chassis_speed_pid_BL, 9000, 0, 120, CHASSISMOTOR_MAX_CURRENT, 5000);
    pid_set(&chassis_speed_pid_BR, 9000, 0, 120, CHASSISMOTOR_MAX_CURRENT, 5000);
    //
    pid_set(&yaw_pid, 3.0, 0.0, 0.5, 100.0, 2.0);
}

/**
 * @brief 设置底盘移动速度
 *
 */
void Chassis_set_x(float x)
{
    // RampGenerator_SetTarget(&Vx_ramp, x);
    Global.Chssis.input.x = x;
}
void Chassis_set_y(float y)
{
    // RampGenerator_SetTarget(&Vy_ramp, y);
    Global.Chssis.input.y = y;
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

    float CHASSIS_WZ_SET_SCALE = 0.0f; // 底盘转角速度设置比例

    /*逆解轮速*/
    FR_speed = +Vx_speed - Vy_speed + (1 + CHASSIS_WZ_SET_SCALE) * R_speed;
    FL_speed = +Vx_speed + Vy_speed + (1 + CHASSIS_WZ_SET_SCALE) * R_speed;
    BL_speed = -Vx_speed + Vy_speed + (1 - CHASSIS_WZ_SET_SCALE) * R_speed;
    BR_speed = -Vx_speed - Vy_speed + (1 - CHASSIS_WZ_SET_SCALE) * R_speed;

    /*正解车速*/
    Vx_now = CHASSISMotor_get_data(WHEEL_FL).speed_rpm / 2.0f - CHASSISMotor_get_data(WHEEL_BL).speed_rpm / 2.0f;
    Vy_now = CHASSISMotor_get_data(WHEEL_FL).speed_rpm / 2.0f - CHASSISMotor_get_data(WHEEL_FR).speed_rpm / 2.0f;
    W_now = CHASSISMotor_get_data(WHEEL_FR).speed_rpm / 2.0f + CHASSISMotor_get_data(WHEEL_BL).speed_rpm / 2.0f;

    /*电流计算*/
    float current[4] = {0};
    current[1] = pid_cal(&chassis_speed_pid_FR, (CHASSISMotor_get_data(WHEEL_FR).speed_rpm) / 19.0f * 0.104719755 * r_wheel, FR_speed); // 右前
    current[3] = pid_cal(&chassis_speed_pid_BR, (CHASSISMotor_get_data(WHEEL_BR).speed_rpm) / 19.0f * 0.104719755 * r_wheel, BR_speed);
    current[0] = pid_cal(&chassis_speed_pid_FL, (CHASSISMotor_get_data(WHEEL_FL).speed_rpm) / 19.0f * 0.104719755 * r_wheel, FL_speed);
    current[2] = pid_cal(&chassis_speed_pid_BL, (CHASSISMotor_get_data(WHEEL_BL).speed_rpm) / 19.0f * 0.104719755 * r_wheel, BL_speed);
    for (int i = 0; i < 4; i++)
    {
        if (current[i] >= (CHASSISMOTOR_MAX_CURRENT - 1))
            current[i] = CHASSISMOTOR_MAX_CURRENT - 1;
        if (current[i] <= -(CHASSISMOTOR_MAX_CURRENT - 1))
            current[i] = -(CHASSISMOTOR_MAX_CURRENT - 1);
    }

    /*电流设置*/
    CHASSISMotor_set(current[0], WHEEL_FL);
    CHASSISMotor_set(current[1], WHEEL_FR);
    CHASSISMotor_set(current[2], WHEEL_BL);
    CHASSISMotor_set(current[3], WHEEL_BR);
}

/**
 * @brief 底盘移动解算
 *
 */
void Chassis_move()
{
    float X_speed, Y_speed, R_speed;

    /*运动分解*/
    X_speed = Global.Chssis.input.x;
    Y_speed = Global.Chssis.input.y;
    R_speed = Global.Chssis.input.r;

    // Chassisui_change(relative_angle);//UI

    /*模式選擇*/
    switch (Global.Chssis.mode)
    {
    case FLOW:
        if (SBUS_CH.CH4 != 1024)
        {
            AHRS_yaw_set = IMU_data.AHRS.yaw - (SBUS_CH.CH4 - 1024) / 1000.0f;
        }
        // pid计算
        R_speed = -pid_cal(&yaw_pid, IMU_data.AHRS.yaw, AHRS_yaw_set);
        break;
    }
  
    /*底盘电机控制*/
    ChassisMotor_Control(X_speed, Y_speed, R_speed);
}