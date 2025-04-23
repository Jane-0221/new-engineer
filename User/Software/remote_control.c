#include "remote_control.h"
#include "Global_status.h"
#include "Chassis.h"
#include "Gimbal.h"
#include "ui.h"
#include "ramp_generator.h"

#include "DT7.h"

#include "Stm32_time.h"
#include <tim.h>
int b =0;
int c =0;
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
void DT7toRCdata()//
{
    /*遥控器数据*/
    //有问题，直接改成原来的
    RC_data.rc.ch[0] = DT7_data.rc.ch[0];
    RC_data.rc.ch[1] = SBUS_CH.CH1;
    RC_data.rc.ch[2] = SBUS_CH.CH2;
    RC_data.rc.ch[3] = SBUS_CH.CH3;
    RC_data.rc.ch[4] = SBUS_CH.CH4;
    RC_data.rc.s[0] = DT7_data.rc.s[0];
    RC_data.rc.s[1] = SBUS_CH.CH5;
    RC_data.rc.s[2] = SBUS_CH.CH6;
    RC_data.rc.s[3] = SBUS_CH.CH7;
    RC_data.rc.s[4] = SBUS_CH.CH8;
    RC_data.rc.s[5] = SBUS_CH.CH9;
    RC_data.rc.s[6] = SBUS_CH.CH10;
    RC_data.rc.s[7] = SBUS_CH.CH11;
    RC_data.rc.s[8] = SBUS_CH.CH12;
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
    if (RC_data.rc.s[1]==RC_data.rc.s[2]==RC_data.rc.s[3]==RC_data.rc.s[4]==1695) // 左下右下，锁死
        Global.Control.mode = LOCK;
    else if (switch_is_up(RC_R_SW) && switch_is_up(RC_L_SW)) // 左上右上，键盘控制
        Global.Control.mode = KEY;
    else
        Global.Control.mode = RC;
    if (Global.Control.mode != RC)
        return;
    /*底盘控制*/
    Global.Chssis.mode = FLOW;
    
    Chassis_set_x((SBUS_CH.CH1-1024) / 40.0f);
    Chassis_set_y(-(SBUS_CH.CH2-1024)/ 40.0f);

 


}
//
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
void GPIO_init(){
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_14 ,GPIO_PIN_RESET);//PD14
        
    HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15 ,GPIO_PIN_RESET);//PD15
        
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8 ,GPIO_PIN_RESET);//PB8
    
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9 ,GPIO_PIN_RESET);//PB9
    
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_14 ,GPIO_PIN_RESET);//PE14
    
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_0 ,GPIO_PIN_RESET);//PE0	
        
    HAL_GPIO_WritePin(GPIOE,GPIO_PIN_1 ,GPIO_PIN_RESET);//PE1
        
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_10 ,GPIO_PIN_RESET);//PC10
        
    HAL_GPIO_WritePin(GPIOC,GPIO_PIN_11 ,GPIO_PIN_RESET);//PC11
    }
    void PWM_control_init(void)
{
    HAL_TIM_Base_Start(&htim1);
	//HAL_TIM_Base_Start(&htim2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	
	HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);//蜂鸣器PWM
//	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);	//加热电阻PWM,不开启	
}
void switch_servos(){  //舵机控制云台
    if(1){ //具体根据需求确定遥控器通道
    if(SBUS_CH.CH4 > 1024){
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3,b=b+1 );//通道3云台1
            
        }
            if(SBUS_CH.CH4 < 1024){
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_3,b=b-1 );
            
        }
        if(b>=2100){ //给个限位，防止跑飞
            b=2100;
        }
        if(b<=400){
            b=400;
        }
}
if(1){//具体根据需求确定遥控器通道
    if(SBUS_CH.CH3 > 1024){
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1,c=c+1 );  //通道1云台2

        }
            if(SBUS_CH.CH3 < 1024){
        __HAL_TIM_SetCompare(&htim1, TIM_CHANNEL_1,c=c-1 );

        }
}
if(c>=2100){//给个限位，防止跑飞
    c=2100;
}
if(c<=400){
    c=400;
}
}