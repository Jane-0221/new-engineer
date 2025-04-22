#include "remote_control.h"
#include "Global_status.h"
#include "Chassis.h"
#include "Gimbal.h"
#include "ui.h"
#include "ramp_generator.h"
#include "dm_arm.h"
#include "DT7.h"

#include "Stm32_time.h"

RC_ctrl_t RC_data;

int16_t wait_time[SIZE_OF_WAIT] = {0}; // 键盘消抖用时间

#define MOVE_SENSITIVITY 10.0f   // 移动灵敏度，
#define PITCH_SENSITIVITY 0.008f // pitch轴灵敏度
#define YAW_SENSITIVITY 0.005f   // yaw轴灵敏度

/**
 * @brief 统一消抖
 *
 * @param key 按键宏
 * @return uint8_t 0未到时间，1到时间
 */
uint8_t Wait(uint8_t key)
{
    if (wait_time[key] > 0)
    {
        wait_time[key]--;
        return 0;
    }
    else
    {
        wait_time[key] = 200;
        return 1;
    }
}
/**
 * @brief 遥控数据来源于DT7遥控器
 *
 */
void DT7toRCdata()
{
    /*遥控器数据*/
    RC_data.rc.ch[0] = DT7_data.rc.ch[0];
    RC_data.rc.ch[1] = DT7_data.rc.ch[1];
    RC_data.rc.ch[2] = DT7_data.rc.ch[2];
    RC_data.rc.ch[3] = DT7_data.rc.ch[3];
    RC_data.rc.ch[4] = DT7_data.rc.ch[4];
    RC_data.rc.s[0] = DT7_data.rc.s[0];
    RC_data.rc.s[1] = DT7_data.rc.s[1];
    /*键鼠数据 */
    RC_data.key.v = DT7_data.key.v;
    RC_data.mouse.x = DT7_data.mouse.x;
    RC_data.mouse.y = DT7_data.mouse.y;
    RC_data.mouse.z = DT7_data.mouse.z;
    RC_data.mouse.press_l = DT7_data.mouse.press_l;
    RC_data.mouse.press_r = DT7_data.mouse.press_r;
}








/**
 * @brief 遥控器控制
 *
 */
void RC_control()
{
    /*控制模式选择*/
    if (switch_is_down(RC_R_SW) && switch_is_down(RC_L_SW)) // 左下右下，锁死
        Global.Control.mode = LOCK;
    else if (switch_is_up(RC_R_SW) && switch_is_up(RC_L_SW)) // 左上右上，键盘控制
        Global.Control.mode = KEY;
    else
        Global.Control.mode = RC;
    if (Global.Control.mode != RC)
        return;
    /*底盘控制*/
    if (switch_is_up(RC_R_SW) && (switch_is_mid(RC_L_SW) || switch_is_up(RC_L_SW))) // 右上,左中||左上，正小陀螺
        Global.Chssis.mode = SPIN_P;
    else if (switch_is_down(RC_L_SW) && (switch_is_mid(RC_R_SW) || switch_is_up(RC_R_SW))) // 左下,右中||右上，正小陀螺
        Global.Chssis.mode = SPIN_N;
    else
        Global.Chssis.mode = FLOW;
    Chassis_set_x(RC_data.rc.ch[0] / 40.0f);
    Chassis_set_y(RC_data.rc.ch[1] / 40.0f);


    if(SBUS_CH.CH5==353&&SBUS_CH.CH6==1024)
{

		
    if(SBUS_CH.CH4>1050&&SBUS_CH.CH4<=1695)
    {
        pos_motor.pos_endyaw += 0.005;
    }
    else if( SBUS_CH.CH4>=353&&SBUS_CH.CH4<1000)
    {
        pos_motor.pos_endyaw -= 0.005;
    }
    else if(SBUS_CH.CH4>1000&&SBUS_CH.CH4<1050)
    {	
        pos_motor.pos_middlerow += 0;
    }	


}
 
}

void Keyboard_mouse_control(void)
{
    if (Global.Control.mode != KEY)
        return;
    /*底盘控制*/
    if (IF_KEY_PRESSED_W)
        Chassis_set_y(5.0);
    if (IF_KEY_PRESSED_S)
        Chassis_set_y(-5.0);
    if (IF_KEY_PRESSED_D)
        Chassis_set_x(5.0);
    if (IF_KEY_PRESSED_A)
        Chassis_set_x(-5.0);
    if (!IF_KEY_PRESSED_W && !IF_KEY_PRESSED_S)
        Chassis_set_y(0);
    if (!IF_KEY_PRESSED_A && !IF_KEY_PRESSED_D)
        Chassis_set_x(0);
    if (IF_KEY_PRESSED_SHIFT)
    {
        Global.Cap.mode = FULL;
        Chassis_set_accel(8.0);
    }
    else
    {
        Global.Cap.mode = Not_FULL;
        Chassis_set_accel(4.0);
    }
    if (IF_KEY_PRESSED_Q) // 小陀螺开关
    {
        if (Wait(WAIT_Q)) // 消抖
        {
            if (Global.Chssis.mode != SPIN_P)
                Global.Chssis.mode = SPIN_P;
            else
                Global.Chssis.mode = FLOW;
        }
    }
    if (IF_KEY_PRESSED_Z) // UI开关
    {
        ui_init();
    }
    if (IF_KEY_PRESSED_CTRL)
        Global.Chssis.input.reset = 1;
    else
        Global.Chssis.input.reset = 0;
    /*云台控制*/
    Gimbal_set_pitch_angle(Global.Gimbal.input.pitch -= MOUSE_Y_MOVE_SPEED * PITCH_SENSITIVITY);
    Gimbal_set_yaw_angle(Global.Gimbal.input.yaw -= MOUSE_X_MOVE_SPEED * YAW_SENSITIVITY);
    /*自瞄选择*/
    if (IF_MOUSE_PRESSED_RIGH)
        Global.Auto.mode = CAR;
    else
        Global.Auto.mode = NONE;
    /*发射机构控制*/
    if (IF_KEY_PRESSED_R) // 摩擦轮开关
    {
        if (Wait(WAIT_R)) // 消抖
            Global.Shoot.shoot_mode = !Global.Shoot.shoot_mode;
    }
    if (IF_MOUSE_PRESSED_LEFT &&
        Global.Shoot.shoot_mode != CLOSE &&
        (Global.Auto.mode == NONE ||
         Global.Auto.input.fire == 1)) // 拨弹电机控制
        Global.Shoot.tigger_mode = HIGH;
    else
        Global.Shoot.tigger_mode = TRIGGER_CLOSE;
}
