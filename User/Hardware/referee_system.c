/**
 * @file referee_system.c
 * @author Siri (lixirui2017@outlook.com)
 * @brief ����ϵͳ��ش���
 * @version 0.1
 * @date 2025-03-01
 *
 * @copyright Copyright (c) 2025
 *
 */


#include "referee_system.h"
#include "CRC8_CRC16.h"

#define REFEREE_FIFO_BUF_LENGTH 1024

// �ⲿ���ò���ϵͳ����
REFEREE_DATA_t Referee_data;

// ���ݻ�����
fifo_s_t referee_fifo;       // ����ϵͳ����fifo
fifo_s_t referee_image_fifo; // ͼ����·����fifo
uint8_t referee_fifo_buf[REFEREE_FIFO_BUF_LENGTH];
uint8_t referee_image_fifo_buf[REFEREE_FIFO_BUF_LENGTH];
unpack_data_t referee_unpack_obj;
unpack_data_t referee_image_unpack_obj;

// �ڲ�����
/* ����ϵͳͨ��֡ͷ */
frame_header_struct_t referee_receive_header; // ��������֡ͷ
frame_header_struct_t referee_send_header;    // ��������֡ͷ
/*----------- ����������� -----------*/
ext_game_status_t game_status;         // ����״̬����(1Hz)
ext_game_result_t game_result;         // �����������(������������)
ext_game_robot_HP_t game_robot_HP;     // ������Ѫ������
ext_referee_warning_t referee_warning; // ���о�����Ϣ
/*----------- ������״̬���� -----------*/
ext_robot_status_t robot_status;                 // ��������������(10Hz�������̡���Դ��״̬)
ext_power_heat_data_t power_heat_data;           // ʵʱ������������(50Hz�������̹��ʡ�ǹ��������)
ext_hurt_data_t hurt_data;                       // �˺�״̬����(�ܵ���������)
ext_shoot_data_t shoot_data;                     // ʵʱ�������(�ӵ�����/Ƶ�ʵ�)
ext_projectile_allowance_t projectile_allowance; // ����ʣ�෢�����(��������)
ext_rfid_status_t rfid_status;                   // ������RFID״̬(����λ״̬)
/*----------- �������� -----------*/
ext_robot_interaction_data_t robot_interaction_data; // �����˽�������(�Զ���ͨ������)
ext_custom_info_t exchange_data;                     // �Զ���UI��ʾ����(ѡ�ֶ��ض�λ���ַ���ʾ)
/*----------- �������� -----------*/
ext_remote_control_t remote_control; // ͼ����·��������(Զ�̿����ź�)

static void referee_struct_init(void)
{
    // ����֡
    memset(&referee_receive_header, 0, sizeof(frame_header_struct_t));
    memset(&referee_send_header, 0, sizeof(frame_header_struct_t));
    // �������
    memset(&game_status, 0, sizeof(ext_game_status_t));
    memset(&game_result, 0, sizeof(ext_game_result_t));
    memset(&game_robot_HP, 0, sizeof(ext_game_robot_HP_t));
    memset(&referee_warning, 0, sizeof(ext_referee_warning_t));
    // �������������� 10hz ��attention please��
    memset(&robot_status, 0, sizeof(ext_robot_status_t));
    // ʵʱ������������ 50hz !attention please!
    memset(&power_heat_data, 0, sizeof(ext_power_heat_data_t));
    // �˺�״̬����
    memset(&hurt_data, 0, sizeof(ext_hurt_data_t));
    // ʵʱ�������
    memset(&shoot_data, 0, sizeof(ext_shoot_data_t));
    // ��������
    memset(&projectile_allowance, 0, sizeof(ext_projectile_allowance_t));
    // ������RFID״̬
    memset(&rfid_status, 0, sizeof(ext_rfid_status_t));
    // �����˽�������
    memset(&robot_interaction_data, 0, sizeof(ext_robot_interaction_data_t));
    // ѡ�ֶ��ض�λ����ʾ�ַ�
    memset(&exchange_data, 0, sizeof(ext_custom_info_t));
    // ͼ����·��������
    memset(&remote_control, 0, sizeof(ext_remote_control_t));
}
static void required_Data() //�����ɸ�����Ҫ�������
{
		Referee_data.Initial_SPEED = shoot_data.initial_speed; 
		Referee_data.Launching_Frequency = shoot_data.launching_frequency;
		Referee_data.projectile_allowance_17mm =projectile_allowance.projectile_allowance_17mm;
    Referee_data.robot_id=robot_status.robot_id;
		Referee_data.Barrel_Heat = power_heat_data.shooter_17mm_1_barrel_heat;  		
		Referee_data.CooLing_Value = robot_status.shooter_barrel_cooling_value;		
		Referee_data.Heat_Limit =  robot_status.shooter_barrel_heat_limit;	
		Referee_data.Buffer_Energy = power_heat_data.buffer_energy;
		Referee_data.Chassis_Power_Limit = robot_status.chassis_power_limit; 
		Referee_data.rest_time =	game_status.stage_remain_time;
}
static void referee_data_solve(uint8_t *frame)
{
    uint16_t cmd_id = 0;
    uint8_t index = 0;

    memcpy(&referee_receive_header, frame, sizeof(frame_header_struct_t));

    index += sizeof(frame_header_struct_t);

    memcpy(&cmd_id, frame + index, sizeof(uint16_t));
    index += sizeof(uint16_t);

    switch (cmd_id)
    {
        case GAME_STATE_CMD_ID:
        {
            memcpy(&game_status, frame + index, sizeof(ext_game_status_t));
        }
        break;
        case GAME_RESULT_CMD_ID:
        {
            memcpy(&game_result, frame + index, sizeof(ext_game_result_t));
        }
        break;
        case GAME_ROBOT_HP_CMD_ID:
        {
            memcpy(&game_robot_HP, frame + index, sizeof(ext_game_robot_HP_t));
        }
        break;
        case FIELD_EVENTS_CMD_ID:
        {
            //memcpy(&event_data, frame + index, sizeof(ext_event_data_t));
        }
        break;
        case SUPPLY_PROJECTILE_ACTION_CMD_ID:
        {
            //memcpy(&supply_projectile_action, frame + index, sizeof(ext_supply_projectile_action_t));
        }
        break;
        case REFEREE_WARNING_CMD_ID:
        {
            memcpy(&referee_warning, frame + index, sizeof(ext_referee_warning_t));
        }
        break;
        case DART_LAUNCH_TIME_CMD_ID:
        {
            //memcpy(&dart_remaining_time, frame + index, sizeof(ext_dart_remaining_time_t));
        }
        break;
        case ROBOT_STATE_CMD_ID:
        {
            memcpy(&robot_status, frame + index, sizeof(ext_robot_status_t));
        }
        break;
        case POWER_HEAT_DATA_CMD_ID:
        {
            memcpy(&power_heat_data, frame + index, sizeof(ext_power_heat_data_t));
        }
        break;
        case ROBOT_POS_CMD_ID:
        {
            //memcpy(&robot_pos, frame + index, sizeof(ext_robot_pos_t));
        }
        break;
        case BUFF_MUSK_CMD_ID:
        {
            //memcpy(&robot_buff, frame + index, sizeof(ext_buff_t));
        }
        break;
        case AERIAL_ROBOT_ENERGY_CMD_ID:
        {
            //memcpy(&air_support_data, frame + index, sizeof(ext_air_support_data_t));
        }
        break;
        case ROBOT_HURT_CMD_ID:
        {
            memcpy(&hurt_data, frame + index, sizeof(ext_hurt_data_t));
        }
        break;
        case SHOOT_DATA_CMD_ID:
        {
            memcpy(&shoot_data, frame + index, sizeof(ext_shoot_data_t));
        }
        break;
        case BULLET_REMAINING_CMD_ID:
        {
            memcpy(&projectile_allowance, frame + index, sizeof(ext_projectile_allowance_t));
        }
        break;
        case ROBOT_RFID_STATE_CMD_ID:
        {
            memcpy(&rfid_status, frame + index, sizeof(ext_rfid_status_t));
        }
        break;
        case DART_PLAYER_COMMAND_CMD_ID:
        {
            //memcpy(&dart_client_cmd, frame + index, sizeof(ext_dart_client_cmd_t));
        }
        break;
        case GROUND_ROBOT_POSITION_CMD_ID:
        {
            //memcpy(&ground_robot_position, frame + index, sizeof(ext_ground_robot_position_t));
        }
        break;
        case RADAR_MARKS_PROGRESS_CMD_ID:
        {
            //memcpy(&radar_mark_data, frame + index, sizeof(ext_radar_mark_data_t));
        }
        break;
        case ROBOT_INTERACTION_CMD_ID:
        {
            memcpy(&robot_interaction_data, frame + index, sizeof(ext_robot_interaction_data_t));
        }
        break;
        case CUSTOM_CONTROLLER_AND_BOT_INTERACTION_CMD_ID:
        {
            //memcpy(&custom_robot_data, frame + index, sizeof(ext_custom_robot_data_t));
        }
        break;
        case PLAYER_SIDE_MINIMAP_INTERACTION_CMD_ID:
        {
            //memcpy(&map_command, frame + index, sizeof(ext_map_command_t));
        }
        break;
        case KEYBOARD_AND_MOUSE_REMOTE_CONTROL_CMD_ID:
        {
            memcpy(&remote_control, frame + index, sizeof(ext_remote_control_t));
        }
        break;
        case PLAYER_MINIMAP_RECEIVES_RADAR_CMD_ID:
        {
            //memcpy(&map_robot_data, frame + index, sizeof(ext_map_robot_data_t));
        }
        break;
        case INTERACTION_DATA_BETWEEN_CONTROLLER_AND_PLAYER_CMD_ID:
        {
            //memcpy(&custom_client_data, frame + index, sizeof(ext_custom_client_data_t));
        }
        break;
        case PLAYER_MINIMAP_RECEIVES_SENTRY_CMD_ID:
        {
            //memcpy(&map_sentry_data, frame + index, sizeof(ext_map_sentry_data_t));
        }
        break;
		case SENTRY_DATA_CMD_ID:
        {
            //memcpy(&sentry_data, frame + index, sizeof(ext_sentry_info_t));
        }
        break;
		case RADAR_DATA_CMD_ID:
        {
            //memcpy(&radar_data, frame + index, sizeof(ext_radar_info_t));
        }
        break;		
		case EXCHANGE_DATA_CMD_ID:
        {
            memcpy(&exchange_data, frame + index, sizeof(ext_custom_info_t));
        }
        break;		

        default:
        {
            break;
        }
    }
}
/**
 * @brief ��ʼ������ϵͳ
 *
 */
void Refree_system_init(void)
{
    referee_struct_init();
    fifo_s_init(&referee_fifo, referee_fifo_buf, REFEREE_FIFO_BUF_LENGTH);
    fifo_s_init(&referee_image_fifo, referee_image_fifo_buf, REFEREE_FIFO_BUF_LENGTH);
}

/**
 * @brief ����ϵͳ���
 * 
 * @param p_fifo ����fifo
 * @param p_obj �������
 */
void Referee_unpack_fifo_data(fifo_s_t* p_fifo,unpack_data_t* p_obj)
{
  uint8_t byte = 0;
  uint8_t sof = HEADER_SOF;

  while (fifo_s_used(p_fifo))
  {
    byte = fifo_s_get(p_fifo);
    switch (p_obj->unpack_step)
    {
    case STEP_HEADER_SOF:
    {
      if (byte == sof)
      {
        p_obj->unpack_step = STEP_LENGTH_LOW;
        p_obj->protocol_packet[p_obj->index++] = byte;
      }
      else
      {
        p_obj->index = 0;
      }
    }
    break;

    case STEP_LENGTH_LOW:
    {
      p_obj->data_len = byte;
      p_obj->protocol_packet[p_obj->index++] = byte;
      p_obj->unpack_step = STEP_LENGTH_HIGH;
    }
    break;

    case STEP_LENGTH_HIGH:
    {
      p_obj->data_len |= (byte << 8);
      p_obj->protocol_packet[p_obj->index++] = byte;

      if (p_obj->data_len < (REF_PROTOCOL_FRAME_MAX_SIZE - REF_HEADER_CRC_CMDID_LEN))
      {
        p_obj->unpack_step = STEP_FRAME_SEQ;
      }
      else
      {
        p_obj->unpack_step = STEP_HEADER_SOF;
        p_obj->index = 0;
      }
    }
    break;
    case STEP_FRAME_SEQ:
    {
      p_obj->protocol_packet[p_obj->index++] = byte;
      p_obj->unpack_step = STEP_HEADER_CRC8;
    }
    break;

    case STEP_HEADER_CRC8:
    {
      p_obj->protocol_packet[p_obj->index++] = byte;

      if (p_obj->index == REF_PROTOCOL_HEADER_SIZE)
      {
        if (verify_CRC8_check_sum(p_obj->protocol_packet, REF_PROTOCOL_HEADER_SIZE))
        {
          p_obj->unpack_step = STEP_DATA_CRC16;
        }
        else
        {
          p_obj->unpack_step = STEP_HEADER_SOF;
          p_obj->index = 0;
        }
      }
    }
    break;

    case STEP_DATA_CRC16:
    {
      if (p_obj->index < (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len))
      {
        p_obj->protocol_packet[p_obj->index++] = byte;
      }
      if (p_obj->index >= (REF_HEADER_CRC_CMDID_LEN + p_obj->data_len))
      {
        p_obj->unpack_step = STEP_HEADER_SOF;
        p_obj->index = 0;

        if (verify_CRC16_check_sum(p_obj->protocol_packet, REF_HEADER_CRC_CMDID_LEN + p_obj->data_len))
        {
          referee_data_solve(p_obj->protocol_packet);
        }
      }
    }
    break;

    default:
    {
      p_obj->unpack_step = STEP_HEADER_SOF;
      p_obj->index = 0;
    }
    break;
    }
  }
  required_Data();
}
