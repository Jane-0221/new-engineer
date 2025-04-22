#include "Shoot.h"
#include "Global_status.h"
#include "Gimbal.h"
#include "pid.h"
#include "referee_system.h"

enum shoot_speed_e shoot_speed;

pid_t trigger_speed_pid, trigger_location_pid;
pid_t shoot_L_speed_pid, shoot_R_speed_pid;
void Shoot_init()
{
	// Ħ���ֵ����ʼ��
	SHOOTMotor_init(M3508, ShootMotor_L);
	SHOOTMotor_init(M3508, ShootMotor_R);
	// ���������ʼ��
	TriggerMotor_init(M2006, TRIGGER_MOTOR);

	// Ħ���ֵ��
	pid_set(&shoot_L_speed_pid, 1, 0, 0, SHOOTMOTOR_MAX_CURRENT, 0);
	pid_set(&shoot_R_speed_pid, 1, 0, 0, SHOOTMOTOR_MAX_CURRENT, 0);

	// �������
	pid_set(&trigger_speed_pid, 5, 0, 3, SHOOTMOTOR_MAX_CURRENT, 0);
	pid_set(&trigger_location_pid, 5, 0, 0, SHOOTMOTOR_MAX_CURRENT, 0);
}
/**
 * @brief Ħ���ֿ���
 *
 */
void Shoot_control()
{
	static enum shoot_mode_e pre_shoot_mode = CLOSE;
	// ����ģʽѡ��Ħ�����ٶ�
	if (Global.Shoot.shoot_mode == CLOSE) // Ħ���ֹر�
	{
		shoot_speed = SHOOT_SPEED_CLOSE;
		Global.Shoot.shoot_status=NOK;
	}
	else if (pre_shoot_mode != Global.Shoot.shoot_mode &&
			 pre_shoot_mode == CLOSE) // Ħ������������
	{
		shoot_speed = SHOOT_SPEED_BEGIN;
		// ��������ʱ��
		pid_set(&shoot_L_speed_pid, 5, 0, 0, SHOOTMOTOR_MAX_CURRENT, 0);
		pid_set(&shoot_R_speed_pid, 5, 0, 0, SHOOTMOTOR_MAX_CURRENT, 0);
		if ((abs(SHOOTMotor_get_data(ShootMotor_L).speed_rpm) <= SHOOT_SPEED_BEGIN + 30) &&
			(abs(SHOOTMotor_get_data(ShootMotor_L).speed_rpm) >= SHOOT_SPEED_BEGIN - 30) &&
			(abs(SHOOTMotor_get_data(ShootMotor_R).speed_rpm) <= SHOOT_SPEED_BEGIN + 30) &&
			(abs(SHOOTMotor_get_data(ShootMotor_R).speed_rpm) >= SHOOT_SPEED_BEGIN - 30))
		{
			pre_shoot_mode = Global.Shoot.shoot_mode;
			pid_set(&shoot_L_speed_pid, 10, 0, 0, SHOOTMOTOR_MAX_CURRENT, 0);
			pid_set(&shoot_R_speed_pid, 10, 0, 0, SHOOTMOTOR_MAX_CURRENT, 0);
			Global.Shoot.shoot_status = OK;
		} // ˫�ִ��ٺ�
		else
		{
			Global.Shoot.shoot_status = NOK;
		}
	}
	else if (pre_shoot_mode == Global.Shoot.shoot_mode &&
			 Global.Shoot.shoot_mode != CLOSE) // Ħ������������
	{
		if (Global.Shoot.shoot_mode == READY)
			shoot_speed = SHOOT_SPEED_READY;
		else if (Global.Shoot.shoot_mode == DEBUG_SHOOT)
			shoot_speed = SHOOT_SPEED_BEGIN;
	}
	SHOOTMotor_set(pid_cal(&shoot_L_speed_pid, SHOOTMotor_get_data(ShootMotor_L).speed_rpm, shoot_speed), ShootMotor_L);
	SHOOTMotor_set(pid_cal(&shoot_R_speed_pid, SHOOTMotor_get_data(ShootMotor_R).speed_rpm, -shoot_speed), ShootMotor_R);

	if (pre_shoot_mode != CLOSE)
		pre_shoot_mode = Global.Shoot.shoot_mode;
}

/**
 * @brief �����������
 *
 */
void Trigger_control(void)
{
	static int trigger_kill_cnt; // �������˴���
	float trigger_speed;
	/*����ģʽѡ���ٶ�*/
	switch (Global.Shoot.tigger_mode)
	{
	case TRIGGER_CLOSE:
		trigger_speed = TRIGGER_SPEED_CLOSE;
		break;
	case HIGH:
		trigger_speed = TRIGGER_SPEED_HIGH;
		break;
	case MID:
		trigger_speed = TRIGGER_SPEED_HIGH;
		break;
	case LOW:
		trigger_speed = TRIGGER_SPEED_HIGH;
		break;
	case DEBUG_TRIGGER:
		trigger_speed = TRIGGER_SPEED_HIGH;
		break;
	case SINGLE:
		break;
	}
	/*��������*/
	if (Referee_data.Barrel_Heat < (Referee_data.Heat_Limit - 20)) // 180
	{
		if (Referee_data.Barrel_Heat < (Referee_data.Heat_Limit - 100)) // 100
			trigger_speed = trigger_speed / 1.2f;
		else if ((Referee_data.Barrel_Heat > (Referee_data.Heat_Limit - 100)) && (Referee_data.Barrel_Heat < (Referee_data.Heat_Limit - 90))) // 80--110
			trigger_speed = trigger_speed / 2.0f;
		else
			trigger_speed = trigger_speed / 3.0f; // 150-180
	}
	else
		trigger_speed = TRIGGER_SPEED_CLOSE;
	/*��������*/
	if (TriggerMotor_get_data(TRIGGER_MOTOR).given_current > 10000)
		trigger_kill_cnt = 50; // ���Ĵ�ֵ���Ըı���˷���
	if (trigger_kill_cnt > 1)
	{
		trigger_speed = -10000.0f;
		trigger_kill_cnt--;
	}

	TriggerMotor_set(pid_cal(&trigger_speed_pid, TriggerMotor_get_data(TRIGGER_MOTOR).speed_rpm, trigger_speed), TRIGGER_MOTOR);
}

/**
 * @brief �����������
 *
 */
void Shoot_task(void)
{
	/*Ħ���ֿ���*/
	Shoot_control();
	/*�����������*/
	Trigger_control();
}