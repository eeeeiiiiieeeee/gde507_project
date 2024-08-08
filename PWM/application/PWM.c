#ifndef PWM_C
#define PWM_C

#include "gd32e50x_libopt.h"
#include "PWM.h"


void PWM_Init(void)
{
    timer_parameter_struct Timer_ParamType;
    timer_oc_parameter_struct Timer_OC_ParamType;

    // 1. 时钟和GPIO配置（省略具体代码）

    // 2. 定时器基本配置
    timer_deinit(TIMER0); // 定时器去初始化
    timer_struct_para_init(&Timer_ParamType);
    Timer_ParamType.alignedmode = TIMER_COUNTER_EDGE;
    Timer_ParamType.counterdirection = TIMER_COUNTER_UP;
    Timer_ParamType.prescaler = 71; // 预分频值
    Timer_ParamType.period = 999; // 定时器周期
    timer_init(TIMER0, &Timer_ParamType);

    // 3. PWM通道配置
    timer_channel_output_struct_para_init(&Timer_OC_ParamType);
    Timer_OC_ParamType.ocpolarity = TIMER_OC_POLARITY_HIGH;
    Timer_OC_ParamType.outputstate = TIMER_CCX_ENABLE;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &Timer_OC_ParamType);

    // 4. 设置PWM脉冲值
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 500);

    // 5. 配置PWM模式
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);

    // 6. 使能定时器
    timer_enable(TIMER0);
}




#endif