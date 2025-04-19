#ifndef __LED_H__
#define __LED_H__
#include "stdint.h"


typedef enum {
    RED     = 0xFF0000,  // ��ɫ
    GREEN   = 0x00FF00,  // ��ɫ
    BLUE    = 0x0000FF,  // ��ɫ
    YELLOW  = 0xFFFF00,  // ��ɫ
    CYAN    = 0x00FFFF,  // ��ɫ
    MAGENTA = 0xFF00FF,  // Ʒ��
    BLACK   = 0x000000,  // ��ɫ
    WHITE   = 0xFFFFFF   // ��ɫ
} Color;


void LEDshowcolor(uint32_t color);

#endif // !__LED_H__
