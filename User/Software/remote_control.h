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

#define SIZE_OF_WAIT 20 //���������С��Ҫ����������Щ��


typedef PACKED_STRUCT()
{
    struct
    {
        int16_t ch[5]; // ҡ������
        char s[2];     // ��������
    } rc;
    struct
    {
        int16_t x;       // x���ƶ��ٶ�
        int16_t y;       // y���ƶ��ٶ�
        int16_t z;       // z���ƶ��ٶ�
        uint8_t press_l; // ����Ƿ���
        uint8_t press_r; // �Ҽ��Ƿ���
    } mouse;
    struct
    {
        uint16_t v; // ���̰�������
    } key;
    struct
    {
        uint8_t custom_data[30]; // �Զ������������
    } custom_robot;
} RC_ctrl_t;


/*�ⲿ��������*/
void DT7toRCdata();
void RC_control();
void Keyboard_mouse_control();

#endif // !__REMOTE_CONTROL__
