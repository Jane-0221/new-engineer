#ifndef __DM4310_DRV_H__
#define __DM4310_DRV_H__
#include "main.h"
#include "fdcan.h"
#include "can_receive_send.h"
#define MIT_MODE 			0x000
#define POS_MODE			0x100
#define SPEED_MODE		0x200
//4310
#define P_MIN -12.5f
#define P_MAX 12.5f
#define V_MIN -30.0f
#define V_MAX 30.0f
#define KP_MIN 0.0f
#define KP_MAX 500.0f
#define KD_MIN 0.0f
#define KD_MAX 5.0f
#define T_MIN -10.0f
#define T_MAX 10.0f
//8009
#define P_MIN2 -12.5f
#define P_MAX2 12.5f
#define V_MIN2 -45.0f
#define V_MAX2 45.0f
#define KP_MIN2 0.0f
#define KP_MAX2 500.0f
#define KD_MIN2 0.0f
#define KD_MAX2 5.0f
#define T_MIN2 -54.0f
#define T_MAX2 54.0f
//10010l
#define P_MIN3 -12.5f
#define P_MAX3 12.5f
#define V_MIN3 -25.0f
#define V_MAX3 25.0f
#define KP_MIN3 0.0f
#define KP_MAX3 500.0f
#define KD_MIN3 0.0f
#define KD_MAX3 5.0f
#define T_MIN3 -200.0f
#define T_MAX3 200.0f
//���������ң������ת��
#define SBUS_POS 3.1415926f/732.0000000f
typedef struct 
{
	uint16_t id;
	uint16_t state;
	int p_int;
	int v_int;
	int t_int;
	int kp_int;
	int kd_int;
	float pos;//���λ��
	float vel;//����ٶ�
	float tor;//Ť��
	float Kp;
	float Kd;
	float Tmos;//Mos��ƽ���¶�
	float Tcoil;
}motor_fbpara_t;
typedef struct
{
    uint16_t set_mode;
    float pos_set;
    float vel_set;
    float tor_set;
	  float cur_set;
    float kp_set;
    float kd_set;
} motor_ctrl_t;



typedef struct
{
	uint8_t read_flag;
	uint8_t write_flag;
	uint8_t save_flag;
	
    float UV_Value;		// ��ѹ����ֵ
    float KT_Value;		// Ť��ϵ��
    float OT_Value;		// ���±���ֵ
    float OC_Value;		// ��������ֵ
    float ACC;			// ���ٶ�
    float DEC;			// ���ٶ�
    float MAX_SPD;		// ����ٶ�
    uint32_t MST_ID;	// ����ID
    uint32_t ESC_ID;	// ����ID
    uint32_t TIMEOUT;	// ��ʱ����ʱ��
    uint32_t cmode;		// ����ģʽ
    float  	 Damp;		// ���ճ��ϵ��
    float    Inertia;	// ���ת������
    uint32_t hw_ver;	// ����
    uint32_t sw_ver;	// ����汾��
    uint32_t SN;		// ����
    uint32_t NPP;		// ���������
    float    Rs;		// ����
    float    Ls;		// ���
    float    Flux;		// ����
    float    Gr;		// ���ּ��ٱ�
    float    PMAX;		// λ��ӳ�䷶Χ
    float    VMAX;		// �ٶ�ӳ�䷶Χ
    float    TMAX;		// Ť��ӳ�䷶Χ
    float    I_BW;		// ���������ƴ���
    float    KP_ASR;	// �ٶȻ�Kp
    float    KI_ASR;	// �ٶȻ�Ki
    float    KP_APR;	// λ�û�Kp
    float    KI_APR;	// λ�û�Ki
    float    OV_Value;	// ��ѹ����ֵ
    float    GREF;		// ��������Ч��
    float    Deta;		// �ٶȻ�����ϵ��
    float 	 V_BW;		// �ٶȻ��˲�����
    float 	 IQ_cl;		// ��������ǿϵ��
    float    VL_cl;		// �ٶȻ���ǿϵ��
    uint32_t can_br;	// CAN�����ʴ���
    uint32_t sub_ver;	// �Ӱ汾��
	float 	 u_off;		// u��ƫ��
	float	 v_off;		// v��ƫ��
	float	 k1;		// ��������1
	float 	 k2;		// ��������2
	float 	 m_off;		// �Ƕ�ƫ��
	float  	 dir;		// ����
	float	 p_m;		// ���λ��
	float	 x_out;		// �����λ��
} esc_inf_t;


typedef enum
{
  Motor1,
  Motor2,
  Motor3,
  Motor4,
  Motor5,
  Motor6,
	Motor7,
	Motor8,
	Motor9,
	Motor10,
    num
} motor_num;

typedef enum
{
	mit_mode = 1,
	pos_mode = 2,
	spd_mode = 3,
	psi_mode = 4
} mode_e;

typedef struct
{
	uint16_t mode;
	motor_fbpara_t para;//����������
  motor_ctrl_t ctrl;//���͵�����
	
	uint16_t mst_id;
	esc_inf_t tmp;
}Arm_Motor_t ;

typedef struct
{
	float pos_rootpitch;
	float pos_rootyaw;
	float pos_middleyaw;
	float pos_middlerow;
	float pos_endyaw;
	float pos_endrow;
}POS_Motor;

typedef struct
{
	float vel_rootpitch;
	float vel_rootyaw;
	float vel_middleyaw;
	float vel_middlerow;
	float vel_endyaw;
	float vel_endrow;
}VEL_Motor;

typedef struct
{
	float tor_rootpitch;
	float tor_rootyaw;
	float tor_middleyaw;
	float tor_middlerow;
	float tor_endyaw;
	float tor_endrow;
}TOR_Motor;
extern Arm_Motor_t arm_motor[num];

extern float Hex_To_Float(uint32_t *Byte,int num);//ʮ�����Ƶ�������
extern uint32_t FloatTohex(float HEX);//��������ʮ������ת��

extern float uint_to_float(int x_int, float x_min, float x_max, int bits);
extern int float_to_uint(float x_float, float x_min, float x_max, int bits);

extern void arm_motor_init(Arm_Motor_t *motor,uint16_t id,uint16_t mode);
extern void enable_motor_mode(hcan_t* hcan, uint16_t motor_id, uint16_t mode_id);
extern void disable_motor_mode(hcan_t* hcan, uint16_t motor_id, uint16_t mode_id);

extern void dm4310_fbdata(Arm_Motor_t *motor, uint8_t *rx_data,uint32_t data_len);
extern void dm8009_fbdata(Arm_Motor_t *motor, uint8_t *rx_data,uint32_t data_len);
extern void dm10010l_fbdata(Arm_Motor_t *motor, uint8_t *rx_data,uint32_t data_len);

void set_DM_pos_vel(float pos, float vel,uint16_t motor_id);
void set_DM_vel(float vel,uint16_t motor_id);
void set_DM_mit(float pos, float vel, float torq,float kp, float kd,uint16_t motor_id);
void set_DM_mode(uint16_t motor_id, uint16_t mode_id);

//�ؽڵ��
extern void mit_ctrl(hcan_t* hcan, uint16_t motor_id, float pos, float vel,float kp, float kd, float torq);
extern void pos_speed_ctrl(hcan_t* hcan,uint16_t motor_id, float pos, float vel);
extern  void speed_ctrl(hcan_t* hcan,uint16_t motor_id, float _vel);


//���ʹ�����
extern void CAN_Send_Enter(hcan_t* hcan,uint16_t motor_id);
extern void CAN_Send_Exit(hcan_t* hcan,uint16_t motor_id);
extern void CAN_Send_Clear_Error(hcan_t* hcan,uint16_t motor_id);
extern void CAN_Send_Save_Zero(hcan_t* hcan,uint16_t motor_id);


#endif /* __DM4310_DRV_H__ */



