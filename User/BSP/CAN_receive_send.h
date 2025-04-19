#ifndef __CAN_RECEIVE_SEND_H__
#define __CAN_RECEIVE_SEND_H__

//#include "cover_headerfile_h.h"
#include "fdcan.h"

// ����������
#define ECD_MAX 8192.0f    // ���������ֵ
#define M3508_P 19.0f      //13.7f      // M3508������ٱ�
#define M2006_P 36.0f      // M2006������ٱ�
#define MAX_CURRENT 16384  // M2006+M3508������ 20A / MAX_CURRENT
#define MAX_6020_VOL 30000 // 6020����ѹ 24V / MAX_6020_VOL

// rm motor data
// notice:Ĭ��Ϊ3508���
// 3508/2006�õ��ǵ�����6020�õ��ǵ�ѹ
typedef struct
{
  int16_t set; // �趨�ĵ��� / ��ѹ

  // ԭʼ����
  uint16_t ecd;          // ��������ֵ
  int16_t speed_rpm;     // ת��
  int16_t given_current; // ������ĵ���
  uint8_t temperate;     // �¶ȣ���ȡ������
  int16_t last_ecd;      // ��һ�α���������ֵ

  // ��������
  long long ecd_cnt;  // ������������
  double angle_cnt;   // ת�����ܽǶ� degree
  double angle_zero;  // ������0��Ƕ� degree
  double angle;       // -180~180 degree
  double round_speed; // ������ת�ٶ� degree/s
	
} motor_measure_t;

extern motor_measure_t motor_data[33];

extern void can_init(void);

extern uint8_t fdcanx_send_data(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint32_t len);
extern uint8_t fdcanx_receive(FDCAN_HandleTypeDef *hfdcan,	FDCAN_RxHeaderTypeDef *fdcan_RxHeader, uint8_t *buf);

extern void get_motor_measure(motor_measure_t *ptr, uint8_t data[]);
extern void process_motor_data(motor_measure_t *motor_data);
extern void CAN1_send_current(void); // ���͵�����Ƶ���
extern void CAN2_send_current(void); // ���͵�����Ƶ���
extern void CAN3_send_current(void); // ���͵�����Ƶ���

extern void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan);


#endif /* __CAN_RECEIVE_SEND_H__ */






