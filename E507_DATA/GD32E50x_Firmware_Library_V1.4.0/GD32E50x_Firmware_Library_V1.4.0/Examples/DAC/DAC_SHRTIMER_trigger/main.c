/*!
    \file    main.c
    \brief   DAC SHRTIMER trigger demo

    \version 2023-12-31, V1.4.0, firmware for GD32E50x
*/

/*
    Copyright (c) 2023, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32e50x.h"

#define CONVERT_NUM                     (10)
#define DAC_R8DH_ADDRESS                (0x40007410)
const uint8_t convertarr[CONVERT_NUM] = {0x00, 0x33, 0x66, 0x99, 0xCC, 0xFF, 0xCC, 0x99, 0x66, 0x33};

/* configure RCU peripheral */
void rcu_config(void);
/* configure GPIO peripheral */
void gpio_config(void);
/* configure DMA peripheral */
void dma_config(void);
/* configure DAC peripheral */
void dac_config(void);
/* configure SHRTIMER peripheral */
void shrtimer_config(void);

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    /* configure RCU peripheral */
    rcu_config();
    /* configure GPIO peripheral */
    gpio_config();
    /* configure DMA peripheral */
    dma_config();
    /* configure SHRTIMER peripheral */
    shrtimer_config();
    /* configure DAC peripheral */
    dac_config();

    while(1){
    }
}

/*!
    \brief      configure RCU peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void rcu_config(void)
{
    /* enable GPIOA clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA1);
    /* enable DAC clock */
    rcu_periph_clock_enable(RCU_DAC);
    /* enable SHRTIMER clock */
    rcu_periph_clock_enable(RCU_SHRTIMER);
    /* configure SHRTIMER clock */
    rcu_shrtimer_clock_config(RCU_SHRTIMERSRC_CKSYS);
}

/*!
    \brief      configure GPIO peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* configure PA4 as DAC output */
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, GPIO_PIN_4);
}

/*!
    \brief      configure DMA peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dma_config(void)
{
    dma_parameter_struct dma_struct;

    /* DMA flag clear */
    dma_flag_clear(DMA1, DMA_CH2, DMA_FLAG_G);

    /* DMA configuration */
    dma_struct.periph_addr  = DAC_R8DH_ADDRESS;
    dma_struct.memory_addr  = (uint32_t)convertarr;
    dma_struct.direction    = DMA_MEMORY_TO_PERIPHERAL;
    dma_struct.number       = CONVERT_NUM;
    dma_struct.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_struct.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_struct.priority     = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA1, DMA_CH2, &dma_struct);

    /* DMA circulation enable */
    dma_circulation_enable(DMA1, DMA_CH2);

    /* DMA channel enable */
    dma_channel_enable(DMA1, DMA_CH2);
}

/*!
    \brief      configure SHRTIMER peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void shrtimer_config(void)
{
    shrtimer_baseinit_parameter_struct baseinit_para;
    shrtimer_timerinit_parameter_struct timerinit_para;

    /* periodic DLL calibration */
    shrtimer_dll_calibration_start(SHRTIMER0, SHRTIMER_CALIBRATION_16384_PERIOD);
    while(RESET == shrtimer_common_flag_get(SHRTIMER0, SHRTIMER_FLAG_DLLCAL));

    /* Slave_TIMER0 time base clock config  */
    shrtimer_baseinit_struct_para_init(&baseinit_para);
    baseinit_para.period            = 180;
    baseinit_para.prescaler         = SHRTIMER_PRESCALER_DIV1;
    baseinit_para.repetitioncounter = 0;
    baseinit_para.counter_mode      = SHRTIMER_COUNTER_MODE_CONTINOUS;
    shrtimer_timers_base_init(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, &baseinit_para);

    /* initialize Slave_TIMER0 to work in waveform mode */
    shrtimer_timerinit_struct_para_init(&timerinit_para);
    timerinit_para.cnt_bunch         = SHRTIMER_TIMERBUNCHNMODE_MAINTAINCLOCK;
    timerinit_para.dac_trigger       = SHRTIMER_DAC_TRIGGER_DACTRIG1;
    timerinit_para.half_mode         = SHRTIMER_HALFMODE_DISABLED;
    timerinit_para.repetition_update = SHRTIMER_UPDATEONREPETITION_ENABLED;
    timerinit_para.reset_sync        = SHRTIMER_SYNCRESET_DISABLED;
    timerinit_para.shadow            = SHRTIMER_SHADOW_ENABLED;
    timerinit_para.start_sync        = SHRTIMER_SYNISTART_DISABLED;
    timerinit_para.update_selection  = SHRTIMER_MT_ST_UPDATE_SELECTION_INDEPENDENT;
    shrtimer_timers_waveform_init(SHRTIMER0, SHRTIMER_SLAVE_TIMER0, &timerinit_para);

    /* enable a counter */
    shrtimer_timers_counter_enable(SHRTIMER0, SHRTIMER_ST0_COUNTER);
}

/*!
    \brief      configure DAC peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dac_config(void)
{
    /* initialize DAC */
    dac_deinit(DAC0);
    /* DAC channel trigger enable */
    dac_trigger_enable(DAC0, DAC_OUT0);
    /* DAC trigger source config */
    dac_trigger_source_config(DAC0, DAC_OUT0, DAC_TRIGGER_SHRTIMER_DACTRIG1);

    /* DAC enable */
    dac_enable(DAC0, DAC_OUT0);
    /* DAC dma enable */
    dac_dma_enable(DAC0, DAC_OUT0);
}
