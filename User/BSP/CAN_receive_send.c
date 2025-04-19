/**
 * @file CAN_receive_send.c
 * @author Siri (lixirui2017@outlook.com)
 * @brief can bsp�㷢�������
 * @version 0.1
 * @date 2024-10-19
 *
 * @copyright Copyright (c) 2024
 *
 */
// #include "cover_headerfile_h.h"
#include "can_receive_send.h"
#include "motor.h"
#include "supercup.h"

float daibaoshan[3];

// CAN�Ĵ�����������
extern FDCAN_HandleTypeDef hfdcan1;
extern FDCAN_HandleTypeDef hfdcan2;
extern FDCAN_HandleTypeDef hfdcan3; // ����ԭ����fdcan.c�ļ�

/**
 * @brief ��ʼ��can,����������������ʹ��
 *
 */
void can_init(void)
{
  FDCAN_FilterTypeDef fdcan_filter;

  fdcan_filter.IdType = FDCAN_STANDARD_ID;             // ���˱�׼ID
  fdcan_filter.FilterIndex = 0;                        // �˲�������
  fdcan_filter.FilterType = FDCAN_FILTER_MASK;         // ����ģʽ
  fdcan_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // ������0������FIFO0
  fdcan_filter.FilterID1 = 0x00000000;                 // ��ȥ�����κ�ID
  fdcan_filter.FilterID2 = 0x00000000;                 // ͬ��

  HAL_FDCAN_ConfigFilter(&hfdcan1, &fdcan_filter); // ���������õ�CAN1
  HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
  HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  HAL_FDCAN_Start(&hfdcan1);

  HAL_FDCAN_ConfigFilter(&hfdcan2, &fdcan_filter);
  HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
  HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  HAL_FDCAN_Start(&hfdcan2);

  HAL_FDCAN_ConfigFilter(&hfdcan3, &fdcan_filter);
  HAL_FDCAN_ConfigGlobalFilter(&hfdcan3, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
  HAL_FDCAN_ActivateNotification(&hfdcan3, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  HAL_FDCAN_Start(&hfdcan3);
}

/**
************************************************************************
* @brief:      	fdcanx_send_data(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint32_t len)
* @param:       hfdcan��FDCAN���
* @param:       id��CAN�豸ID
* @param:       data�����͵�����
* @param:       len�����͵����ݳ���
* @retval:     	void
* @details:    	��������
************************************************************************
**/
uint8_t fdcanx_send_data(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint32_t len)
{
  FDCAN_TxHeaderTypeDef TxHeader;

  TxHeader.Identifier = id;
  TxHeader.IdType = FDCAN_STANDARD_ID;              // ��׼ID
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;          // ����֡
  TxHeader.DataLength = FDCAN_DLC_BYTES_8;          // �������ݳ���
  TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;  // ���ô���״ָ̬ʾ
  TxHeader.BitRateSwitch = FDCAN_BRS_OFF;           // �������ɱ䲨����
  TxHeader.FDFormat = FDCAN_CLASSIC_CAN;            // ��ͨCAN��ʽ
  TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // ���ڷ����¼�FIFO����, ���洢
  TxHeader.MessageMarker = 0x00;                    // ���ڸ��Ƶ�TX EVENT FIFO����ϢMaker��ʶ����Ϣ״̬����Χ0��0xFF

  if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeader, data) != HAL_OK)
    return 1; // ����
  return 0;
}

/**
************************************************************************
* @brief:      	fdcanx_receive(FDCAN_HandleTypeDef *hfdcan, uint8_t *buf)
* @param:       hfdcan��FDCAN���
* @param:       buf���������ݻ���
* @retval:     	���յ����ݳ���
* @details:    	��������
************************************************************************
**/
uint8_t fdcanx_receive(FDCAN_HandleTypeDef *hfdcan, FDCAN_RxHeaderTypeDef *fdcan_RxHeader, uint8_t *buf)
{
  if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, fdcan_RxHeader, buf) != HAL_OK)
    return 0; // ��������
  return fdcan_RxHeader->DataLength >> 16;
}

/**
 * @brief CAN���ܻص�����
 * 
 * @param hfdcan 
 * @param RxFifo0ITs 
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{

  FDCAN_RxHeaderTypeDef rx_header; // CAN ����ָ��
  uint8_t rx_data[8];              // ��ȡ��������

  if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
  {
    HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &rx_header, rx_data);
    //����֡
    if(rx_header.Identifier==Supercap_receive_id)
      Supercup_decode_candata(hfdcan,rx_data);
    //���֡
    DJIMotor_decode_candata(hfdcan, rx_header.Identifier, rx_data);
  }
}

/**
 * @brief CAN������ص���������������豸
 * 
 * @param hfdcan 
 */
void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan){
  FDCAN_FilterTypeDef fdcan_filter;

  fdcan_filter.IdType = FDCAN_STANDARD_ID;             // ���˱�׼ID
  fdcan_filter.FilterIndex = 0;                        // �˲�������
  fdcan_filter.FilterType = FDCAN_FILTER_MASK;         // ����ģʽ
  fdcan_filter.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // ������0������FIFO0
  fdcan_filter.FilterID1 = 0x00000000;                 // ��ȥ�����κ�ID
  fdcan_filter.FilterID2 = 0x00000000;                 // ͬ��

  HAL_FDCAN_Stop(hfdcan);
  HAL_FDCAN_DeInit(hfdcan);
  HAL_FDCAN_Init(hfdcan);
  HAL_FDCAN_ConfigFilter(hfdcan, &fdcan_filter); // ���������õ�CAN
  HAL_FDCAN_ConfigGlobalFilter(hfdcan, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);
  HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
  HAL_FDCAN_Start(hfdcan);
}
