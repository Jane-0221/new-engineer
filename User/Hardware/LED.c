#include "LED.h"
#include "ws2812.h"

void hexToRGB(uint32_t hex, uint8_t *r, uint8_t *g, uint8_t *b)
{
    *r = (hex >> 16) & 0xFF; // ��ȡ��ɫ����
    *g = (hex >> 8) & 0xFF;  // ��ȡ��ɫ����
    *b = hex & 0xFF;         // ��ȡ��ɫ����
}

/**
 * @brief LED����ʾ��ɫ
 *
 * @param color ��ɫ��rgb����
 */
void LEDshowcolor(uint32_t color)
{
    uint8_t r, g, b;
    hexToRGB(color, &r, &g, &b);
    WS2812_Ctrl(r, g, b);
}

//void LEDchangecolor(uint32_t)