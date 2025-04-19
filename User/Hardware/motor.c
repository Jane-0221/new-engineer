/**
 * @file motor.c
 * @author Siri (lixirui2017@outlook.com)
 * @brief ����������������뷴��
 * @version 0.1
 * @date 2024-10-20
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "motor.h"
#include "math.h"

#ifdef USE_DJI_MOTOR
#define abs(a) a > 0 ? a : -a
// ������ݶ���
DJI_motor_data_s DJIMotor_data[QUANTITY_OF_CAN][QUANTITY_OF_DJIMOTOR];

/**
 * @brief �󽮵����ʼ������ʱֻ������ʼ������
 *
 * @param motor_type ������ࣺGM6020,M3508,M2006
 * @param motor_id ���canͨ����ID
 */
void DJIMotor_init(Motor_Type_e motor_type, DJIcan_id motor_id)
{
    uint8_t cantype = (motor_id + 1) / 11; // ��õ������can·
    uint8_t canid = motor_id % 11;         // �õ����IDֵ��

    DJIMotor_data[cantype][canid].Motor_type = motor_type; // ��ʼ����Ӧ���
}

/**
 * @brief �������
 *
 * @param zero_angle ���Ƕ�
 * @param motor_id ���ID
 */
void DJIMotor_setzero(double zero_angle, DJIcan_id motor_id)
{
    uint8_t cantype = (motor_id + 1) / 11; // ��õ������can·
    uint8_t canid = motor_id % 11;         // �õ����IDֵ��

    DJIMotor_data[cantype][canid].angle_zero = zero_angle; // ��ʼ����Ӧ���
}

/**
 * @brief ���ô󽮵������
 *
 * @param val ����ֵ
 * @param motor_id ���canͨ����ID
 */
void DJIMotor_set(int16_t val, DJIcan_id motor_id)
{
    DJIMotor_data[(motor_id + 1) / 11][motor_id % 11].set = val; // ���õ���
}

/**
 * @brief ��ȡ�󽮵������
 *
 * @param motor_id ���canͨ����ID
 * @return DJI_motor_data_s ������ݽṹ�塣
 */
DJI_motor_data_s DJIMotor_get_data(DJIcan_id motor_id) // ��ȡ�������
{
    return DJIMotor_data[(motor_id + 1) / 11][motor_id % 11];
}

/**
 * @brief �󽮵��CAN���ݽ����Լ�����
 *
 * @param ptr �������
 * @param data can����
 */
void DJIMotor_get_process_motor_data(DJI_motor_data_s *ptr, uint8_t data[])
{
    // get raw data
    (ptr)->last_ecd = (ptr)->ecd;
    (ptr)->ecd = (uint16_t)((data)[0] << 8 | (data)[1]);
    (ptr)->speed_rpm = (uint16_t)((data)[2] << 8 | (data)[3]);
    (ptr)->given_current = (uint16_t)((data)[4] << 8 | (data)[5]);
    (ptr)->temperate = (data)[6];

    // process the data
    // count cnt
    if ((ptr)->last_ecd > 7000 && (ptr)->ecd < 1000)
        (ptr)->ecd_cnt += ((ECD_MAX - (ptr)->last_ecd) + (ptr)->ecd);
    else if ((ptr)->last_ecd < 1000 && (ptr)->ecd > 7000)
        (ptr)->ecd_cnt -= ((ECD_MAX - (ptr)->ecd) + (ptr)->last_ecd);
    else
        (ptr)->ecd_cnt += ((ptr)->ecd - (ptr)->last_ecd);
    // process data
    (ptr)->angle_cnt = (ptr)->ecd_cnt * ECD_TO_ANGEL_DJI;
    // ���ݲ�ͬ������в�ͬ����
    if ((ptr)->Motor_type == GM6020)
    {
        // �������ת��
        (ptr)->round_speed = (ptr)->speed_rpm;

        // ������ԽǶ� -180~180 �������ȶ�ʧ �ܽǶȹ���ʱ
        float angle = (ptr)->angle_cnt - (ptr)->angle_zero;
        uint32_t mul = abs((int)angle) / 180;
        if (angle > 180.0f)
        {
            if (mul % 2 == 1) // ����-180��
                angle -= (mul + 1) * 180;
            else // ����180��
                angle -= mul * 180;
        }
        if (angle < -180.0f)
        {
            if (mul % 2 == 1) // ����180��
                angle += (mul + 1) * 180;
            else // ����-180��
                angle += mul * 180;
        }
        (ptr)->angle = angle;
    }
    else if ((ptr)->Motor_type == M3508)
    {
        (ptr)->round_speed = (ptr)->speed_rpm / 19.0f;
    }
    else if ((ptr)->Motor_type == M2006)
    {
        (ptr)->round_speed = (ptr)->speed_rpm / 36.0f;
    }
}

/**
 * @brief �󽮵��can���ݴ���
 *
 * @param hfdcan CANͨ��
 * @param id can��ʶ��
 * @param data can����
 */
void DJIMotor_decode_candata(FDCAN_HandleTypeDef *hfdcan, uint32_t id, uint8_t *data)
{
    if (id - CAN_ID1 <= 11) // ��ֹ�������
    {
        if (hfdcan == &hfdcan1)
        {
            DJIMotor_get_process_motor_data(&DJIMotor_data[0][id - CAN_ID1], data);
        }
        else if (hfdcan == &hfdcan2)
        {
            DJIMotor_get_process_motor_data(&DJIMotor_data[1][id - CAN_ID1], data);
        }
        else if (hfdcan == &hfdcan3)
        {
            DJIMotor_get_process_motor_data(&DJIMotor_data[2][id - CAN_ID1], data);
        }
    }
}

/**
 * @brief �󽮵������ֵ���ͣ������freertos�ﶨ�ڷ��ͣ���������ô�Ӧ�ò�һ�£�
 *
 */
void DJIMotor_send_current(void)
{
    uint8_t can_send_data[3][8];
    uint8_t canid;
    static FDCAN_TxHeaderTypeDef tx_message;
    if (hfdcan1.ErrorCode)
        HAL_FDCAN_ErrorCallback(&hfdcan1);
    if (hfdcan2.ErrorCode)
        HAL_FDCAN_ErrorCallback(&hfdcan2);
    if (hfdcan3.ErrorCode)
        HAL_FDCAN_ErrorCallback(&hfdcan3);
    // ����3508/2006ǰ4��
    tx_message.Identifier = CAN_20063508_1_4_send_ID;
    tx_message.IdType = FDCAN_STANDARD_ID;              // ��׼ID
    tx_message.TxFrameType = FDCAN_DATA_FRAME;          // ����֡
    tx_message.DataLength = FDCAN_DLC_BYTES_8;          // �������ݳ���
    tx_message.ErrorStateIndicator = FDCAN_ESI_ACTIVE;  // ���ô���״ָ̬ʾ
    tx_message.BitRateSwitch = FDCAN_BRS_OFF;           // �������ɱ䲨����
    tx_message.FDFormat = FDCAN_CLASSIC_CAN;            // ��ͨCAN��ʽ
    tx_message.TxEventFifoControl = FDCAN_NO_TX_EVENTS; // ���ڷ����¼�FIFO����, ���洢
    tx_message.MessageMarker = 0x00;                    // ���ڸ��Ƶ�TX EVENT FIFO����ϢMaker��ʶ����Ϣ״̬����Χ0��0xFF
    // ��·can���θ�ֵ
    for (int i = 0; i < 3; i++)
    {
        canid = 0;
        for (int k = 0; k < 8; k += 2)
        {
            can_send_data[i][k] = (DJIMotor_data[i][canid].set >> 8);
            can_send_data[i][k + 1] = DJIMotor_data[i][canid].set;
            canid++;
        }
    }
    // ��·canͬʱ����
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_message, can_send_data[0]);
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_message, can_send_data[1]);
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &tx_message, can_send_data[2]);

    // ����3508/2006��4��
    tx_message.Identifier = CAN_20063508_5_8_send_ID;
    for (int i = 0; i < 3; i++)
    {
        canid = 4;
        for (int k = 0; k < 8; k += 2)
        {
            can_send_data[i][k] = (DJIMotor_data[i][canid].set >> 8);
            can_send_data[i][k + 1] = DJIMotor_data[i][canid].set;
            canid++;
        }
    }
    // ��·canͬʱ����
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_message, can_send_data[0]);
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_message, can_send_data[1]);
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &tx_message, can_send_data[2]);

    // ����6020ǰ4��
    tx_message.Identifier = CAN_6020_1_4_send_ID;
    for (int i = 0; i < 3; i++)
    {
        canid = 4;
        for (int k = 0; k < 8; k += 2)
        {
            can_send_data[i][k] = (DJIMotor_data[i][canid].set >> 8);
            can_send_data[i][k + 1] = DJIMotor_data[i][canid].set;
            canid++;
        }
    }
    // ��·canͬʱ����
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_message, can_send_data[0]);
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_message, can_send_data[1]);
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &tx_message, can_send_data[2]);

    // ����6020��3��
    tx_message.Identifier = CAN_6020_5_7_send_ID;
    for (int i = 0; i < 3; i++)
    {
        canid = 8;
        for (int k = 0; k < 6; k += 2)
        {
            can_send_data[i][k] = (DJIMotor_data[i][canid].set >> 8);
            can_send_data[i][k + 1] = DJIMotor_data[i][canid].set;
            canid++;
        }
    }
    // ��·canͬʱ����
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &tx_message, can_send_data[0]);
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &tx_message, can_send_data[1]);
    HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan3, &tx_message, can_send_data[2]);
}

#endif // USE_DJI_MOTOR