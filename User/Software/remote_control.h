#ifndef __REMOTE_CONTROL__
#define __REMOTE_CONTROL__

#include "stdint.h"
#include "main.h"

#define    WAIT_W 0
#define    WAIT_S 1
#define    WAIT_A 2
#define    WAIT_D 3
#define    WAIT_Q 4
#define    WAIT_E 5
#define    WAIT_G 6
#define    WAIT_X 7
#define    WAIT_Z 8
#define    WAIT_C 9
#define    WAIT_B 10
#define    WAIT_V 11
#define    WAIT_F 12
#define    WAIT_R 13
#define    WAIT_CTRL 14
#define    WAIT_SHIFT 15

#define SIZE_OF_WAIT 20 //消抖数组大小，要大于上面那些数


typedef PACKED_STRUCT()
{
    struct
    {
        uint16_t ch[5]; // 摇杆数据
        int16_t s[9];     // 拨杆数据
    } rc;
    struct
    {
        int16_t x;       // x轴移动速度
        int16_t y;       // y轴移动速度
        int16_t z;       // z轴移动速度
        uint8_t press_l; // 左键是否按下
        uint8_t press_r; // 右键是否按下
    } mouse;
    struct
    {
        uint16_t v; // 键盘按键数据
    } key;
    struct
    {
        uint8_t custom_data[30]; // 自定义控制器数据
    } custom_robot;
} RC_ctrl_t;
 extern  RC_ctrl_t RC_data;

/*外部函数调用*/
void DT7toRCdata();
void RC_control();
void Keyboard_mouse_control();

#endif // !__REMOTE_CONTROL__
