/**
 * @file music.c
 * @author Siri (lixirui2017@outlook.com)
 * @brief ���ֲ���ģ��
 * @version 0.1
 * @date 2025-04-07
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "music.h"

// ===== ����ֻ�ϻ����������� =====
Note melody[] = {
    // ��һ�Σ���������
    {MID_DO, 500},
    {MID_RE, 500},
    {MID_MI, 500},
    {MID_DO, 500}, // 1 2 3 1
    {MID_DO, 500},
    {MID_RE, 500},
    {MID_MI, 500},
    {MID_DO, 500}, // 1 2 3 1
    {MID_MI, 500},
    {MID_FA, 500},
    {MID_SOL, 1000}, // 3 4 5 -
    {MID_MI, 500},
    {MID_FA, 500},
    {MID_SOL, 1000}, // 3 4 5 -

    // �ڶ��Σ������� + ��������
    {MID_SOL, 250},
    {MID_LA, 250},
    {MID_SOL, 250},
    {MID_FA, 250},
    {MID_MI, 500},
    {MID_DO, 500}, // 5 6 5 4 3 1
    {MID_SOL, 250},
    {MID_LA, 250},
    {MID_SOL, 250},
    {MID_FA, 250},
    {MID_MI, 500},
    {MID_DO, 500}, // 5 6 5 4 3 1

    // ��β���֣�����Do + ����Sol��
    {MID_DO, 500},
    {HIGH_SOL, 500},
    {MID_DO, 1000}, // 1 5 1 -
    {MID_DO, 500},
    {HIGH_SOL, 500},
    {MID_DO, 1000}, // 1 5 1 -
    {MUSIC_FINISH,500},
};

void Music_init()
{
    Buzzer_init();
}

void Music_play(Note *note)
{
    for (int i = 0; note[i].fre!=MUSIC_FINISH; i++)
    {
        MUSIC_SET_FREQUENCY(note[i].fre);
        MUSIC_ON();
        MUSIC_DELAY(note[i].time);
    }
    MUSIC_OFF();
}
