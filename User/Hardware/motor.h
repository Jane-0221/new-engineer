#ifndef __MOTOR_H__
#define __MOTOR_H__
#include "stdint.h"

#include "User_math.h"
#include "CAN_receive_send.h"
// �Զ�����
#define USE_DJI_MOTOR
#define QUANTITY_OF_CAN 3       // �����õ�can���������������õĶ�����ˣ���������3����ˣ�
#define QUANTITY_OF_DJIMOTOR 11 // ���ص������һ·can�����õĵ��

// ������
#define PI 3.1415926f
#define ECD_TO_ANGEL_DJI 0.043945f //(360/8192),��������ֵת��Ϊ�Ƕ���
#define DJIMOTOR_MAX_CURRENT 16384 // M2006+M3508������ 20A / DJI_MAX_CURRENT
#define DJIMOTOR_T_A 0.3 //3508ת�س���

#ifdef USE_DJI_MOTOR
typedef enum
{
    /* data */
    MOTOR_TYPE_NONE = 0,
    GM6020,
    M3508,
    M2006,
} Motor_Type_e;

/* DJImotorCAN send and receive ID */
typedef enum
{
    CAN_20063508_1_4_send_ID = 0x200,

    CAN_20063508_5_8_send_ID = 0x1FF,

    CAN_6020_1_4_send_ID = 0x1FE,

    CAN_6020_5_7_send_ID = 0x2FE,

    CAN_ID1 = 0x201,
} DJIcan_send_id_e;

// 6020�� CAN_1_5(��ӦID1) �� CAN_1_6020_7����ӦID7��
// 3508/2006�� CAN_1_1(��ӦID1) �� CAN_1_8����ӦID8��
// ��Ȼ��6020��3508/2006�����ص�ID,����ʱ��ע��
typedef enum
{
    CAN_1_1 = 0,  // 0
    CAN_1_2,      // 1
    CAN_1_3,      // 2
    CAN_1_4,      // 3
    CAN_1_5,      // 4
    CAN_1_6,      // 5
    CAN_1_7,      // 6
    CAN_1_8,      // 7
    CAN_1_6020_5, // 8
    CAN_1_6020_6, // 9
    CAN_1_6020_7, // 10

    CAN_2_1, // 11
    CAN_2_2, // 12
    CAN_2_3,
    CAN_2_4,
    CAN_2_5, // 15
    CAN_2_6,
    CAN_2_7, // 17
    CAN_2_8,
    CAN_2_6020_5,
    CAN_2_6020_6,
    CAN_2_6020_7, // 21

    CAN_3_1, // 22
    CAN_3_2, // 23
    CAN_3_3,
    CAN_3_4,
    CAN_3_5,
    CAN_3_6,
    CAN_3_7,
    CAN_3_8,
    CAN_3_6020_5,
    CAN_3_6020_6,
    CAN_3_6020_7, // 32

} DJIcan_id;

typedef struct
{
    // ��������
    int16_t set; // �趨�ĵ��� / ��ѹ
    Motor_Type_e Motor_type;

    // ԭʼ����
    uint16_t ecd;          // ��������ֵ
    int16_t speed_rpm;     // ת��RPM
    int16_t given_current; // ʵ��ת�ص���
    uint8_t temperate;     // �¶ȣ���ȡ������
    uint16_t last_ecd;     // ��һ�α���������ֵ

    // ��������
    long long ecd_cnt;  // ������������
    double angle_cnt;   // ת�����ܽǶ� degree
    double angle_zero;  // ������0��Ƕ� degree
    double angle;       // -180~180 degree
    double round_speed; // ����ת�� rpm
} DJI_motor_data_s;

void DJIMotor_init(Motor_Type_e motor_type, DJIcan_id motor_id);
void DJIMotor_setzero(double zero_angle, DJIcan_id motor_id);
void DJIMotor_set(int16_t val, DJIcan_id motor_id);
void DJIMotor_decode_candata(FDCAN_HandleTypeDef *hfdcan, uint32_t id, uint8_t *data);
void DJIMotor_send_current(void);

#endif //! USE_DJI_MOTOR

#ifdef USE_UNTREE_MOTOR

typedef struct
{
    // ���� ���͸�ʽ������

    // �����͵ĸ�������
    unsigned short motor_LR; // ������ң�0��1��
    unsigned short motor_id; // ���ID��0xBB����ȫ�����
    unsigned short mode;     // 0:����, 5:����ת��, 10:�ջ�FOC����
    // ʵ�ʸ�FOC��ָ������Ϊ��
    // K_P*delta_Pos + K_W*delta_W + T
    float T;   // �����ؽڵ�������أ�������ʵ�ʵ����أ���Nm��
    float W;   // �����ؽ��ٶȣ����������ٶȣ�(rad/s)
    float Pos; // �����ؽ�λ�ã�rad��
    float K_P; // �ؽڸն�ϵ��
    float K_W; // �ؽ��ٶ�ϵ��

} MOTOR_send;

#endif // USE_UNTREE_MOTOR

#endif // !__MOTOR_H__