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
#include "dm4310_drv.h"
#include "string.h"
/**/
FDCAN_RxHeaderTypeDef RxHeader1;
uint8_t g_Can1RxData[64];
float daibaoshan[3];
motor_measure_t motor_data[33];
uint32_t Flag_damiao[6] ={ 0};
//ǰһʱ�̵ĵ������flag
uint32_t Pre_Flag_damiao [6] ={ 0};
/**/
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
//damiao
void get_motor_measure(motor_measure_t *ptr, uint8_t data[])
{
  (ptr)->last_ecd = (ptr)->ecd;
  (ptr)->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);
  (ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);
  (ptr)->given_current = (uint16_t)((data)[4] << 8 | (data)[5]);
  (ptr)->temperate = (data)[6];
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
//ԭ����û���ã����и���
uint8_t canx_send_data(FDCAN_HandleTypeDef *hcan, uint16_t id, uint8_t *data, uint32_t len)
{
	FDCAN_TxHeaderTypeDef TxHeader;

	TxHeader.Identifier = id;                 // CAN ID
  TxHeader.IdType =  FDCAN_STANDARD_ID ;        
  TxHeader.TxFrameType = FDCAN_DATA_FRAME;  
  if(len<=8)	
	{
	  TxHeader.DataLength =FDCAN_DLC_BYTES_8;     // ���ͳ��ȣ�8byte
	}
	else  if(len==12)	
	{
	   TxHeader.DataLength =FDCAN_DLC_BYTES_12;
	}
	else  if(len==16)	
	{
	  TxHeader.DataLength =FDCAN_DLC_BYTES_16;
	
	}
  else  if(len==20)
	{
		TxHeader.DataLength =FDCAN_DLC_BYTES_20;
	}		
	else  if(len==24)	
	{
	 TxHeader.DataLength =FDCAN_DLC_BYTES_24;	
	}else  if(len==48)
	{
	 TxHeader.DataLength =FDCAN_DLC_BYTES_48;
	}else  if(len==64)
   {
		 TxHeader.DataLength =FDCAN_DLC_BYTES_64;
	 }
											
	TxHeader.ErrorStateIndicator =  FDCAN_ESI_ACTIVE;
  TxHeader.BitRateSwitch = FDCAN_BRS_OFF;//�������л��رգ��������ھ���CAN
  TxHeader.FDFormat =  FDCAN_CLASSIC_CAN;            // CANFD
  TxHeader.TxEventFifoControl =  FDCAN_NO_TX_EVENTS;  
  TxHeader.MessageMarker = 0x00;//��Ϣ���

   // ����CANָ��
  if(HAL_FDCAN_AddMessageToTxFifoQ(hcan, &TxHeader, data) != HAL_OK)
  {
        // ����ʧ�ܴ���
       Error_Handler();      
  }
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
//��ΪԴ����û���������������ֱ��ȫ����
int a;
uint8_t fdcanx_receive(FDCAN_HandleTypeDef *hfdcan,uint32_t RXFIFO,FDCAN_RxHeaderTypeDef *fdcan_RxHeader,uint8_t *buf)
{	
  if(HAL_FDCAN_GetRxMessage(hfdcan,RXFIFO,fdcan_RxHeader,buf)!=HAL_OK)
		a = 1;
		//return 0;//��������
  return fdcan_RxHeader->DataLength>>16;	
}

/**
 * @brief CAN���ܻص�����
 * 
 * @param hfdcan 
 * @param RxFifo0ITs 
 */
/*�������ۼӼ���*/
void process_motor_data(motor_measure_t *motor_data)
{
  // ������Ȧ��
  if (motor_data->last_ecd > 7000 && motor_data->ecd < 1000)
    motor_data->ecd_cnt += ((ECD_MAX - motor_data->last_ecd) + motor_data->ecd);
  else if (motor_data->last_ecd < 1000 && motor_data->ecd > 7000)
    motor_data->ecd_cnt -= ((ECD_MAX - motor_data->ecd) + motor_data->last_ecd);
  else
    motor_data->ecd_cnt += (motor_data->ecd - motor_data->last_ecd);
}


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
//����
if(hfdcan->Instance == FDCAN1)
		{			
			memset(g_Can1RxData, 0, sizeof(g_Can1RxData));
			fdcanx_receive(hfdcan,FDCAN_RX_FIFO0,&RxHeader1,g_Can1RxData);
       	switch(RxHeader1.Identifier)
				{
					case 1 :dm8009_fbdata(&arm_motor[Motor1], g_Can1RxData,RxHeader1.DataLength);break;
					case CAN_ID1 :get_motor_measure(&motor_data[RxHeader1.Identifier - CAN_ID1], g_Can1RxData);
										process_motor_data(&motor_data[RxHeader1.Identifier - CAN_ID1]);Flag_damiao[1] += 1;break;
          case 0x202 :get_motor_measure(&motor_data[RxHeader1.Identifier - CAN_ID1], g_Can1RxData);
						        process_motor_data(&motor_data[RxHeader1.Identifier - CAN_ID1]);break;
          case 0x203 :get_motor_measure(&motor_data[RxHeader1.Identifier - CAN_ID1], g_Can1RxData);
						        process_motor_data(&motor_data[RxHeader1.Identifier - CAN_ID1]);break;
          case 0x204 :get_motor_measure(&motor_data[RxHeader1.Identifier - CAN_ID1], g_Can1RxData);
						        process_motor_data(&motor_data[RxHeader1.Identifier - CAN_ID1]);break;
	        case 0x205 :get_motor_measure(&motor_data[RxHeader1.Identifier - CAN_ID1], g_Can1RxData);
										process_motor_data(&motor_data[RxHeader1.Identifier - CAN_ID1]);break;
					 case 0x206 :get_motor_measure(&motor_data[RxHeader1.Identifier - CAN_ID1], g_Can1RxData);
										process_motor_data(&motor_data[RxHeader1.Identifier - CAN_ID1]);break;
					 case 0x207 :get_motor_measure(&motor_data[RxHeader1.Identifier - CAN_ID1], g_Can1RxData);
										process_motor_data(&motor_data[RxHeader1.Identifier - CAN_ID1]);break;
					default:break;
				}				
   	}
		else if(hfdcan->Instance == FDCAN2)
		{
			memset(g_Can1RxData, 0, sizeof(g_Can1RxData));
			fdcanx_receive(hfdcan,FDCAN_RX_FIFO0,&RxHeader1,g_Can1RxData);							
			
    	switch(RxHeader1.Identifier)
				{
					case 2 :dm10010l_fbdata(&arm_motor[Motor2], g_Can1RxData,RxHeader1.DataLength); Flag_damiao[2] += 1;break;     
					case 3 :dm4310_fbdata(&arm_motor[Motor3], g_Can1RxData,RxHeader1.DataLength);   Flag_damiao[3] += 1;break;
					case 4 :dm4310_fbdata(&arm_motor[Motor4], g_Can1RxData,RxHeader1.DataLength);   Flag_damiao[4] += 1;break;
					case 5 :dm4310_fbdata(&arm_motor[Motor5], g_Can1RxData,RxHeader1.DataLength);   Flag_damiao[5] += 1;break;
					case 0x206 :get_motor_measure(&motor_data[11+RxHeader1.Identifier - CAN_ID1], g_Can1RxData);
										process_motor_data(&motor_data[11+RxHeader1.Identifier - CAN_ID1]);break;
					default:break;
				}				
		}
		else if(hfdcan->Instance == FDCAN3)
		{
			memset(g_Can1RxData, 0, sizeof(g_Can1RxData));
			fdcanx_receive(hfdcan,FDCAN_RX_FIFO0,&RxHeader1,g_Can1RxData);							
			get_motor_measure(&motor_data[22+RxHeader1.Identifier - CAN_ID1], g_Can1RxData);
			process_motor_data(&motor_data[22+RxHeader1.Identifier - CAN_ID1]);
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
