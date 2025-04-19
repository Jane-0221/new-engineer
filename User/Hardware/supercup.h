#ifndef __SUPERCUP_H__
#define __SUPERCUO_H__

#include "stdint.h"
#include "fdcan.h"
#include "CAN_receive_send.h"

typedef PACKED_STRUCT()
{
    uint8_t set_max_power;     // �����
    uint8_t cache_energy;      // �������
    uint16_t cacheEnergylimit; // ��������ʣ������
    uint8_t state;             // ״̬
    float remain_vol;          // ʣ���ѹ
    float prediect_energy;     // Ԥ������ 0 - 100%
} Supercap_t;


typedef PACKED_STRUCT()
{
    uint8_t setPower;          // ���õ��̹���
    uint16_t cacheEnergy;      // ��������
    uint16_t cacheEnergylimit; // ��������ʣ������
} Supercap_send_t;

typedef enum
{
    Supercap_send_id = 0x306,
    Supercap_receive_id = 0x307,
} Supercup_canid;

extern Supercap_t cap;
extern Supercap_send_t send_data_cap;

void Supercup_decode_candata(FDCAN_HandleTypeDef *hfdcan, uint8_t *data);
void Supercup_send_data(void);
int Supercap_set_power(uint8_t set);
float Supercap_get_remain_vol(void);
float Supercap_get_predict_energy(void);
#endif // !__SUPERCUP_H__
