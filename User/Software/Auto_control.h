#ifndef __AUTO_CONTROL_H__
#define __AUTO_CONTROL_H__

#include "stdint.h"

#pragma pack(1)

typedef struct
{
    struct
    {
        uint8_t sof;
        uint8_t crc8;
    } FrameHeader; // 2
    struct
    {
        float curr_yaw;
        float curr_pitch;
        float curr_omega;
        uint8_t state;// state 0�Ǵ� 1�Ǵ�ǰ��վ 2�Ǵ�С�� 3�Ǵ��
        uint8_t autoaim; // autoaim�Ǹ�0�ǲ������� 1�ǿ�����
        uint8_t enemy_color;// 0Ϊ��ɫ��1Ϊ��ɫ
    } To_minipc_data; // 15
    struct
    {
        uint16_t crc16;
    } FrameTailer;//2
    uint8_t enter;//1
} STM32_data_t;

typedef struct
{
    struct
    {
        uint8_t sof;
        uint8_t crc8;
    } FrameHeader; // 2
    struct
    {
        float shoot_yaw;
        float shoot_pitch;
        uint8_t fire;      // �����ź�
        uint8_t target_id; // Ŀ��ID,UI��ʾ��
    } from_minipc_data;    // 15
    struct
    {
        uint16_t crc16;
    } FrameTailer;
} MINIPC_data_t;

#pragma pack(4)

void STM32_to_MINIPC();
void decodeMINIPCdata(MINIPC_data_t *target, unsigned char buff[], unsigned int len);
void Auto_control();
void MINIPC_to_STM32();

extern MINIPC_data_t fromMINIPC;
extern STM32_data_t toMINIPC;

#endif