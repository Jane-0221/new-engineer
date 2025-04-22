#include "stdint.h"

typedef struct
{
    /*����״̬*/
    struct
    {
        enum chassis_mode_e
        {
            FLOW,   // ���̸���
            SPIN_P, // ��С����
            SPIN_N, // ��С����
        } mode;
        struct
        {
            float x; // x���ƶ��ٶ�
            float y; // y���ƶ��ٶ�
            float r; // ת�� ˳ʱ��������ʱ�븺
            uint8_t reset;
        } input;
    } Chssis;
    /*��̨״̬*/
    struct
    {
        enum gimbal_mode_e
        {
            NORMAL, // ��ͨģʽ
            AUTO,   // ����ģʽ
        } mode;
        struct
        {
            float pitch;
            float yaw;
        } input;

    } Gimbal;
    /*�������״̬*/
    struct
    {
        enum tigger_mode_e
        {
            TRIGGER_CLOSE,
            HIGH,
            MID,
            LOW,
            SINGLE,
            DEBUG_TRIGGER, // �˵�ʹ�ã������ٸ���Ƶ��
        } tigger_mode;
        enum shoot_mode_e
        {
            CLOSE,
            READY,
            DEBUG_SHOOT, // �˵�ʹ��
        } shoot_mode;
        enum shoot_status_e
        {
            NOK,
            OK,
        } shoot_status;
    } Shoot;
    /*����״̬*/
    struct
    {
        enum auto_mode_e
        {
            NONE,       // ����������
            CAR,        // ��
            OUTPOST,    // ǰ��վ
            LOWTARGET,  // С��
            HIGHTARGET, // ���
        } mode;
        struct
        {
            float shoot_yaw;
            float shoot_pitch;
            int Auto_control_online; // ��������
            uint8_t fire;                // �Ƿ���������
            uint8_t target_id;           // Ŀ��ID
        } input;
    } Auto;
    /*����״̬*/
    struct
    {
        enum cap_mode_e
        {
            Not_FULL, // ��ʱ��֪����ɶ
            FULL,
        } mode;
    } Cap;
    /**/
    struct
    {
        enum control_mode_e
        {
            RC,   // ң����
            KEY,  // ����
            LOCK, // ����
        } mode;
    } Control;
} GlobalStatus_t;

extern GlobalStatus_t Global;