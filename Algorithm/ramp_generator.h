#ifndef RAMP_GENERATOR_H
#define RAMP_GENERATOR_H

#ifdef __cplusplus
extern "C"
{
#endif

    /**
     * @brief б�·�����ģ��
     *
     * �ṩ�����ٶȿ��ƺ�ƽ�����ɵ���ֵ���ɹ��ܣ�֧�ֶ�̬�����������޷�����
     * ����Ӧ�ã�������ơ��¶ȵ��ڡ��˶����Ƶ���Ҫƽ�����ɵĳ���
     */

    typedef struct
    {
        float current_value;            // ��ǰ���ֵ����λ���û����嵥λ��
        float target_value;             // Ŀ��ֵ
        unsigned int interval_ms;       // ���Ƽ������λ�����룩
        float accel;                    // ���ٶȣ���λ����ֵ/��?��
        float decel;                    // ���ٶȣ���λ����ֵ/��?��
        float max_limit;                // ����ֵ���ֵ����
        unsigned long last_update_time; // �ϴθ���ʱ��������룩
    } RampGenerator;

    /**
     * @brief ��ʼ��б�·�����
     * @param ramp ʵ��ָ��
     * @param interval_ms ���Ƽ������1ms��
     * @param accel ���ٶȣ���0��
     * @param decel ���ٶȣ���0��
     * @param max_limit ������ֵ����
     */
    void RampGenerator_Init(RampGenerator *ramp,
                            unsigned int interval_ms,
                            float accel,
                            float decel,
                            float max_limit);

    /**
     * @brief ����Ŀ��ֵ�����������Ʒ�Χ��
     */
    void RampGenerator_SetTarget(RampGenerator *ramp, float target);

    /**
     * @brief ��ȡ��ǰ���ֵ�����޷���
     */
    float RampGenerator_GetCurrent(const RampGenerator *ramp);

    /**
     * @brief ���·�����״̬���趨�ڵ��ã�
     * @param current_time_ms ���������ĵ�ǰʱ���
     */
    void RampGenerator_Update(RampGenerator *ramp, unsigned long current_time_ms);

    /**
     * @brief �޸Ŀ��Ƽ����������Ч��
     * @param interval_ms �¼����1-1000ms��
     */
    void RampGenerator_SetInterval(RampGenerator *ramp, unsigned int interval_ms);

    /**
     * @brief �޸ļ��ٶȣ��´θ�����Ч��
     * @param accel �¼��ٶȣ���0��
     */
    void RampGenerator_SetAccel(RampGenerator *ramp, float accel);

    /**
     * @brief �޸ļ��ٶȣ��´θ�����Ч��
     * @param decel �¼��ٶȣ���0��
     */
    void RampGenerator_SetDecel(RampGenerator *ramp, float decel);

    /**
     * @brief �޸����ֵ���ƣ�������Ч��ǯλ��ǰֵ��
     */
    void RampGenerator_SetMaxLimit(RampGenerator *ramp, float max_limit);

#ifdef __cplusplus
}
#endif

#endif // RAMP_GENERATOR_H