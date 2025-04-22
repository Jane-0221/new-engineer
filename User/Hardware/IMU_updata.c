/**
 * @file IMU_updata.c
 * @author sethome
 * @brief IMU updata info
 * @version 0.1
 * @date 2022-11-19
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "pid.h"
#include "IMU_updata.h"
#include "BMI088Middleware.h"
#include "BMI088driver.h"

// #include "ist8310driver.h"
#include "fdacoefs.h"

#include "User_math.h"
#include "MahonyAHRS.h"
#include "madgwick.h"
#include "AHRS.h"
#include "bsxlite_interface.h"

#include "tim.h"
// #include "iwdg.h"
#include "Stm32_time.h"

// made by sethome
// ʹ��TIM14��Ƶ����

// �������¶�PID
pid_t IMU_tempure_pid;

// ���ٶȼƵ�ͨ�˲�
static fp32 accel_fliter_1[3] = {0.0f, 0.0f, 0.0f};
static fp32 accel_fliter_2[3] = {0.0f, 0.0f, 0.0f};
static fp32 accel_fliter_3[3] = {0.0f, 0.0f, 0.0f};
static const fp32 fliter_num[3] = {1.929454039488895f, -0.93178349823448126f, 0.002329458745586203f};
static uint8_t first_temperate;

unsigned long ulTdleCycleCount = 0UL;

// ��ʱ������
extern TIM_HandleTypeDef htim3;

struct IMU_t IMU_data; // IMU���ݽṹ��

// �ڲ�����

void IMU_heat_set(uint16_t ccr); // ���ȵ���PWMռ�ձ�
void shiftArray(float arr[], int size);
static void imu_temp_control(fp32 temp);

// ��Ԫ��תΪŷ���� �ǵ���lib�汾����ɾ
void Get_angle(fp32 q[4], fp32 *yaw, fp32 *pitch, fp32 *roll)
{
	*yaw = atan2f(2.0f * (q[0] * q[3] + q[1] * q[2]), 2.0f * (q[0] * q[0] + q[1] * q[1]) - 1.0f);
	*pitch = asinf(-2.0f * (q[1] * q[3] - q[0] * q[2]));
	*roll = atan2f(2.0f * (q[0] * q[1] + q[2] * q[3]), 2.0f * (q[0] * q[0] + q[3] * q[3]) - 1.0f);
}

// ��ʼ��
void IMU_init()
{
	BMI088_init(); // ������

	IMU_data.Mahony.q[0] = 1.0f;
	IMU_data.Mahony.q[1] = 0.0f;
	IMU_data.Mahony.q[2] = 0.0f;
	IMU_data.Mahony.q[3] = 0.0f;

	IMU_data.madgwick.q[0] = 1.0f;
	IMU_data.madgwick.q[1] = 0.0f;
	IMU_data.madgwick.q[2] = 0.0f;
	IMU_data.madgwick.q[3] = 0.0f;

	AHRS_init(IMU_data.AHRS.q, IMU_data.accel, IMU_data.mag); // AHRS�˲�����

	pid_set(&IMU_tempure_pid, 2000, 0.2, 0, 4500, 4400);
	
	HAL_TIM_Base_Start(&htim3); // ���ȵ���PWM
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
	HAL_TIM_Base_Start_IT(&htim13); // ʹ�ܸ����жϣ�1000HZ

	// У׼���ٶȣ�δ��ɣ��ͽ��ٶȼ�
	// for (int i = 0; i < 1000;)
	// {
	// 	float x_ = IMU_data.gyro[0];
	// 	float y_ = IMU_data.gyro[1];
	// 	float z_ = IMU_data.gyro[2];
	// 	if (fabs(x_) > 1.0f && fabs(y_) > 1.0f && fabs(z_) > 1.0f)
	// 		continue;
	// 	IMU_data.calibration[0] += x_ / 1000.0f;
	// 	IMU_data.calibration[1] += y_ / 1000.0f;
	// 	IMU_data.calibration[2] += z_ / 1000.0f;
	// 	i++;
	// 	HAL_Delay(1);
	// }
}

void process_IMU_data()
{
	// ������Ȧ��
	if (IMU_data.AHRS.last_yaw > 3.0f && IMU_data.AHRS.yaw < -3.0f)
		IMU_data.AHRS.yaw_rad_cnt += ((PI - IMU_data.AHRS.last_yaw) + (IMU_data.AHRS.yaw + PI));
	else if (IMU_data.AHRS.last_yaw < -3.0f && IMU_data.AHRS.yaw > 3.0f)
		IMU_data.AHRS.yaw_rad_cnt -= ((PI + IMU_data.AHRS.last_yaw) + (PI - IMU_data.AHRS.yaw));
	else
		IMU_data.AHRS.yaw_rad_cnt += (IMU_data.AHRS.yaw - IMU_data.AHRS.last_yaw);
}
// IMU���º���
void IMU_updata() // 1000HZ
{
	//  ��ȡ�����Ǻ͵شż���Ϣ
	BMI088_read(IMU_data.gyro, IMU_data.accel, &IMU_data.temp);
	// ��Ư�Ʋ���
	//	IMU_data.gyro[0] -= 0.00103224139;
	//	IMU_data.gyro[1] += 0.0050994223;
	//	IMU_data.gyro[2] -= 0.000232788681 ;

	// IMU_data.gyro[0] += 0.00169377134;
	// IMU_data.gyro[1] += 0.00817589462;
	// IMU_data.gyro[2] -= 0.000124635975;

	// ������PID����
	imu_temp_control(IMU_data.temp);

	// ���ٶȼƵ�ͨ�˲�
	// accel low-pass filter
	accel_fliter_1[0] = accel_fliter_2[0];
	accel_fliter_2[0] = accel_fliter_3[0];

	accel_fliter_3[0] = accel_fliter_2[0] * fliter_num[0] + accel_fliter_1[0] * fliter_num[1] + IMU_data.accel[0] * fliter_num[2];

	accel_fliter_1[1] = accel_fliter_2[1];
	accel_fliter_2[1] = accel_fliter_3[1];

	accel_fliter_3[1] = accel_fliter_2[1] * fliter_num[0] + accel_fliter_1[1] * fliter_num[1] + IMU_data.accel[1] * fliter_num[2];

	accel_fliter_1[2] = accel_fliter_2[2];
	accel_fliter_2[2] = accel_fliter_3[2];

	accel_fliter_3[2] = accel_fliter_2[2] * fliter_num[0] + accel_fliter_1[2] * fliter_num[1] + IMU_data.accel[2] * fliter_num[2];

	// AHRS.lib
	// ���ʧ�ܣ�����0
	static uint32_t last_time = 0;
	IMU_data.AHRS.err_code = AHRS_update(IMU_data.AHRS.q,0.001f,
										 IMU_data.gyro,
										 accel_fliter_3,
										 IMU_data.mag);
	last_time = Get_sys_time_ms();
	get_angle(IMU_data.AHRS.q, &IMU_data.AHRS.yaw, &IMU_data.AHRS.roll, &IMU_data.AHRS.pitch);

	// process_IMU_data();
	if (IMU_data.AHRS.last_yaw > 3.0f && IMU_data.AHRS.yaw < -3.0f)
		IMU_data.AHRS.yaw_rad_cnt += ((PI - IMU_data.AHRS.last_yaw) + (IMU_data.AHRS.yaw + PI));
	else if (IMU_data.AHRS.last_yaw < -3.0f && IMU_data.AHRS.yaw > 3.0f)
		IMU_data.AHRS.yaw_rad_cnt -= ((PI + IMU_data.AHRS.last_yaw) + (PI - IMU_data.AHRS.yaw));
	else
		IMU_data.AHRS.yaw_rad_cnt += (IMU_data.AHRS.yaw - IMU_data.AHRS.last_yaw);
	IMU_data.AHRS.pitch*=-1;
	IMU_data.AHRS.last_yaw = IMU_data.AHRS.yaw;
}

float rad2degree(float a)
{
	return a / PI * 180.0f;
}
float degree2rad(float a)
{
	return a / 180.0f * PI;
}

void MagUpdate() // ���µشż�
{
	// ist8310_read_mag(IMU_data.mag);
}
void MagZero() // ����شż�
{
	IMU_data.mag[0] = 0;
	IMU_data.mag[1] = 0;
	IMU_data.mag[2] = 0;
}
// �趨���ȵ�PWMռ�ձ�
void IMU_heat_set(uint16_t ccr)
{
	if (ccr < 0)
	{
		ccr = 0;
	}
	__HAL_TIM_SetCompare(&htim3, TIM_CHANNEL_4, ccr); // HAL��PWM��CCR�趨
													  // TIM10->CCR1 = (pwm);
}

/**
 * @brief          ����bmi088���¶�
 * @param[in]      temp:bmi088���¶�
 * @retval         none
 */
static void imu_temp_control(fp32 temp)
{
	uint16_t tempPWM;
	static uint8_t temp_constant_time = 0;
	if(first_temperate)
	{
		pid_cal(&IMU_tempure_pid, temp, 45);

		if (IMU_tempure_pid.total_out < 0.0f)
		{
			IMU_tempure_pid.total_out = 0.0f;
		}
		tempPWM = (uint16_t)IMU_tempure_pid.total_out;
		IMU_heat_set(tempPWM);
	}
	
}

void imu_cail_program(void) // ������У׼���򣬷��������������棬��Ҫ��ʱ��У׼һ�����ƽ����ֵ���ϼ���
{
	IMU_data.calibration[0] = 0;
	IMU_data.calibration[1] = 0;
	IMU_data.calibration[2] = 0;
	for (int i = 0; i < 1000;)
	{
		float x_ = IMU_data.gyro[0];
		float y_ = IMU_data.gyro[1];
		float z_ = IMU_data.gyro[2];
		if (fabs(x_) > 1.0f && fabs(y_) > 1.0f && fabs(z_) > 1.0f)
			continue;
		IMU_data.calibration[0] += x_ / 1000.0f;
		IMU_data.calibration[1] += y_ / 1000.0f;
		IMU_data.calibration[2] += z_ / 1000.0f;
		i++;
	}
	if (IMU_data.calibration[0] != 0 && IMU_data.calibration[1] != 0 && IMU_data.calibration[2] != 0)
	{
		IMU_data.cali_end[0] = IMU_data.calibration[0];
		IMU_data.cali_end[1] = IMU_data.calibration[1];
		IMU_data.cali_end[2] = IMU_data.calibration[2];
	}
}

void IMU_offest()
{
	if (fabs(IMU_data.gyro[0]) > 1.0f && fabs(IMU_data.gyro[1]) > 1.0f && fabs(IMU_data.gyro[2]) > 1.0f)
	{
	}
	else
	{
		for (int j = 0; j < 1000;)
		{
			float x_ = IMU_data.gyro[0];
			float y_ = IMU_data.gyro[1];
			float z_ = IMU_data.gyro[2];

			if (fabs(IMU_data.gyro[0]) > 1.0f && fabs(IMU_data.gyro[1]) > 1.0f && fabs(IMU_data.gyro[2]) > 1.0f)
			{
				IMU_data.gyro[0] -= 0.007288707553;
				IMU_data.gyro[1] -= -0.00267180009;
				IMU_data.gyro[2] -= 0.90244110504;
			}

			IMU_data.calibration[0] += x_ / 1000.0f;
			IMU_data.calibration[1] += y_ / 1000.0f;
			IMU_data.calibration[2] += z_ / 1000.0f;

			j++;
		}
	}

	IMU_data.gyro[0] -= IMU_data.calibration[0];
	IMU_data.gyro[1] -= IMU_data.calibration[1];
	IMU_data.gyro[2] -= IMU_data.calibration[2];

	IMU_data.calibration[0] = 0; //-0.005116222685;
	IMU_data.calibration[1] = 0;
	IMU_data.calibration[2] = 0;
}

// end of file
