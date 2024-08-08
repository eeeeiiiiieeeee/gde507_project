#ifndef PWM_C
#define PWM_C

#include "gd32e50x_libopt.h"
#include "PWM.h"


void PWM_Init(void)
{
    timer_parameter_struct Timer_ParamType;
    timer_oc_parameter_struct Timer_OC_ParamType;

    // 1. ʱ�Ӻ�GPIO���ã�ʡ�Ծ�����룩

    // 2. ��ʱ����������
    timer_deinit(TIMER0); // ��ʱ��ȥ��ʼ��
    timer_struct_para_init(&Timer_ParamType);
    Timer_ParamType.alignedmode = TIMER_COUNTER_EDGE;
    Timer_ParamType.counterdirection = TIMER_COUNTER_UP;
    Timer_ParamType.prescaler = 71; // Ԥ��Ƶֵ
    Timer_ParamType.period = 999; // ��ʱ������
    timer_init(TIMER0, &Timer_ParamType);

    // 3. PWMͨ������
    timer_channel_output_struct_para_init(&Timer_OC_ParamType);
    Timer_OC_ParamType.ocpolarity = TIMER_OC_POLARITY_HIGH;
    Timer_OC_ParamType.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &Timer_OC_ParamType);

    // 4. ����PWM����ֵ
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 500);

    // 5. ����PWMģʽ
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);

    // 6. ʹ�ܶ�ʱ��
    timer_enable(TIMER0);
}




#endif