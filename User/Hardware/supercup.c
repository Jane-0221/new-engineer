/**
 * @file supercup.c
 * @author Siri (lixirui@outlook.com)
 * @brief ������ؿ���
 * @version 0.1
 * @date 2024-10-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "supercup.h"
#include "string.h"
#include "stm32h7xx_hal.h"

Supercap_t cap;
Supercap_send_t send_data_cap;
FDCAN_HandleTypeDef *Supercup_send_hfdcan; // ͨ���˴��Զ�ȷ��������ص�canͨ��

/**
 * @brief ���볬����Ϣ
 * 
 * @param hfdcan �յ�������Ϣ��canͨ��
 * @param data �յ�������
 */
void Supercup_decode_candata(FDCAN_HandleTypeDef *hfdcan, uint8_t *data)
{
    // ȷ�����������ص�canͨ��
    Supercup_send_hfdcan = hfdcan;

    // ��ȡ�����ѹ
    static float rev_vol;
    memcpy(&rev_vol, data, sizeof(rev_vol));
    cap.remain_vol = rev_vol;
}

/**
 * @brief ���ͳ����������
 * 
 */
void Supercup_send_data(void)
{
    static uint8_t can_send_data[8];
    static FDCAN_TxHeaderTypeDef tx_message;

    tx_message.Identifier = Supercap_send_id;
    tx_message.IdType = FDCAN_STANDARD_ID;              // ��׼ID
    tx_message.TxFrameType = FDCAN_DATA_FRAME;          // ����֡
    tx_message.DataLength = FDCAN_DATA_BYTES_8;         // �������ݳ���
    tx_message.ErrorStateIndicator = FDCAN_ESI_ACTIVE;  // ���ô���״ָ̬ʾ
    tx_message.BitRateSwitch = FDCAN_BRS_OFF;           // �������ɱ䲨����
    tx_message.FDFormat = FDCAN_CLASSIC_CAN;            // ��ͨCAN��ʽ
    tx_message.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // ���ڷ����¼�FIFO����, ���洢
    tx_message.MessageMarker = 0x00;                    // ���ڸ��Ƶ�TX EVENT FIFO����ϢMaker��ʶ����Ϣ״̬����Χ0��0xFF

    Supercap_send_t send_data;

    send_data.setPower = cap.set_max_power;
    send_data.cacheEnergy = cap.cache_energy;
    send_data.cacheEnergylimit = cap.cacheEnergylimit;

    memcpy(can_send_data, &send_data, sizeof(Supercap_send_t));

    HAL_FDCAN_AddMessageToTxFifoQ(Supercup_send_hfdcan, &tx_message, can_send_data);
}

/**
 * @brief ���ó���Ĺ���
 * 
 * @param set ����ֵ 50~200
 * @return int �Ƿ����óɹ� 0�ɹ���-1ʧ��
 */
int Supercap_set_power(uint8_t set)
{
	if (set > 200 || set < 50)
		return -1;
	cap.set_max_power = set;
	return 0;
}

/**
 * @brief ��ȡ����ʣ���ѹ
 * 
 * @return float ����ʣ���ѹ
 */
float Supercap_get_remain_vol(void)
{
	return cap.remain_vol;
}

/**
 * @brief ��ȡ����Ԥ��ʣ������
 * 
 * @return float ����ʣ������ �ٷ�ֵ
 */
float Supercap_get_predict_energy(void)
{
	return cap.prediect_energy;
}