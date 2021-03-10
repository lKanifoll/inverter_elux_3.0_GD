/*!
    \file  main.c
    \brief GPIO keyboard polling demo
    
    \version 2017-06-28, V1.0.0, demo for GD32F3x0
    \version 2019-06-01, V2.0.0, demo for GD32F3x0
*/

/*
    Copyright (c) 2019, GigaDevice Semiconductor Inc.

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


#include "main.h"
#include "inverter.h"
#include <stdio.h>
#include <string.h>
#include "crc32.h"

void gpio_config(void);
void rcu_config(void);
void uart_init(void);
void adc_config(void);
void spi0_init(void);
void pwm_config_lcd_bl(void);
void pwm_config_buzzer(void);
void heat_timer_config(void);
void i2c_config(void);
void RTC_config(void);
uint32_t ntohl(uint32_t const net);
uint32_t calc_crc32(uint8_t * data, uint32_t size);
uint32_t sck;
uint32_t valcrc0 = 0, valcrc1 = 0;
uint8_t arr8[15] = {0xab, 0xcd, 0x12, 0x34, 0x34, 0x56, 0xcd, 0xef, 0x78, 0x9a, 0x15, 0x24, 0x12, 0x34, 0x12};
//uint8_t arr8[16] = {0x34, 0x12, 0xcd, 0xab, 0xef, 0xcd, 0x56, 0x34, 0x24, 0x15, 0x9a, 0x78, 0x34, 0x12, 0x34, 0x12};
uint32_t arr32[4] = {0xabcd1234, 0x3456cdef, 0x789a1524, 0x12341200};

int main(void)
{
	systick_config();
	
	rcu_config();
	
	gpio_config();
	i2c_config();
	TUYA_OFF;
	uart_init();
	spi0_init();
	adc_config();
	
	pwm_config_buzzer();
	pwm_config_lcd_bl();
	heat_timer_config();
	pmu_backup_write_enable();
	exti_init(EXTI_17,EXTI_INTERRUPT,EXTI_TRIG_RISING);
  nvic_irq_enable(RTC_IRQn,0,0);
	
	RTC_config();
	delay_1ms(500);
	
	
  loop();
}






void rcu_config(void)
{
		rcu_periph_clock_enable(RCU_I2C0);
    // enable the GPIO clock 
		rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
	
	  // enable the PERIPH clock 
    rcu_periph_clock_enable(RCU_SPI1);
		rcu_periph_clock_enable(RCU_USART1);
		rcu_periph_clock_enable(RCU_TIMER16);
	  rcu_periph_clock_enable(RCU_TIMER1);
	  rcu_periph_clock_enable(RCU_ADC);
	  
	  rcu_periph_clock_enable(RCU_PMU);
	  rcu_periph_clock_enable(RCU_CRC);
    // ADCCLK = PCLK2/6 
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
		rcu_periph_clock_enable(RCU_WWDGT);
}




void gpio_config(void)
{
		// TUYA Enable\disable pin
    gpio_mode_set(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_1);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_1);	
	
		//DO_HIGH DO_LOW DO_IR DO_INV
	  gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLDOWN, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8);	
	
		// ADC pin
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_5);

	  // LCD PWM Backlight pin
	  gpio_af_set(GPIOA, GPIO_AF_2, GPIO_PIN_0);
	  gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_0);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,GPIO_PIN_0);
	
		// Buzzer PWM pin
	  gpio_af_set(GPIOB, GPIO_AF_2, GPIO_PIN_9);
	  gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLDOWN, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,GPIO_PIN_9);
    
		// I2C pins
    gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_9);
    gpio_af_set(GPIOA, GPIO_AF_4, GPIO_PIN_10);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_9);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE,GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_10);
	
	  // LCD -------------
		// SPI CLK & MOSI pins
	  gpio_af_set(GPIOB, GPIO_AF_0,GPIO_PIN_13 | GPIO_PIN_15);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_13 | GPIO_PIN_15);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_13 | GPIO_PIN_15);
		
		// LCD CS, RS, RESET pins
    gpio_mode_set(GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_10);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_10);
		// -----------------
		
    // USART pins
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_2);
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_3);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_3);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_3);		

}

void i2c_config(void)
{
    /* enable I2C clock */
    
    /* configure I2C clock */
    i2c_clock_config(I2C0,400000, I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C0,I2C_I2CMODE_ENABLE,I2C_ADDFORMAT_7BITS,0x81);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    //i2c_ack_config(I2C0,I2C_ACK_ENABLE);
		//i2c_ack_config(I2C0,I2C_ACK_DISABLE);
}

void adc_config(void)
{
	  //adc_discontinuous_mode_config(ADC_REGULAR_CHANNEL, 1);
	  adc_special_function_config(ADC_SCAN_MODE, DISABLE);

    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1);
	  adc_regular_channel_config(0, ADC_CHANNEL_5, ADC_SAMPLETIME_239POINT5);
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    adc_resolution_config(ADC_RESOLUTION_12B);
    /* ADC external trigger enable */
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);
    /* ADC external trigger source config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
	  
    /* enable ADC interface */
    
	  adc_enable();
	  delay_1ms(10);
    /* ADC calibration and reset calibration */
    adc_calibration_enable();
    /* ADC SCAN function enable */
		adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
    /* ADC DMA function enable */
    //adc_dma_mode_enable();
}

void uart_init(void)
{
    /* USART configure */
	  
	  nvic_irq_enable(USART1_IRQn, 0, 0);
    usart_deinit(USART1);
		usart_parity_config(USART1, USART_PM_NONE);
		usart_word_length_set(USART1, USART_WL_8BIT);
		usart_stop_bit_set(USART1,USART_STB_1BIT);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);
    usart_dma_receive_config(USART1, USART_DENR_ENABLE);
		usart_baudrate_set(USART1, 9600U);
	  //usart_halfduplex_enable(USART1);
    usart_enable(USART1);
	
    
	
    while (RESET == usart_flag_get(USART1, USART_FLAG_TC));
		usart_interrupt_enable(USART1, USART_INT_RBNE);
		usart_interrupt_enable(USART1, USART_INT_IDLE);
		delay_1ms(100);

}

void spi0_init(void)
{
    spi_parameter_struct spi_init_struct;

    /* SPI1 parameter config */
    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
    spi_init_struct.device_mode          = SPI_MASTER;
    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
    spi_init_struct.nss                  = SPI_NSS_SOFT;
    spi_init_struct.prescale             = SPI_PSC_2;
    spi_init_struct.endian               = SPI_ENDIAN_MSB;
    spi_init(SPI1, &spi_init_struct);
    spi_dma_enable(SPI1, SPI_DMA_TRANSMIT);
    /* set crc polynomial */
    //spi_crc_polynomial_set(SPI1, 7);
    spi_enable(SPI1);
		
}


uint8_t spi_write_byte(uint32_t spi_periph,uint8_t byte)
{
	//LCD_CS_CLR;
    while(RESET == (SPI_STAT(spi_periph)&SPI_FLAG_TBE));
    SPI_DATA(spi_periph) = byte;

    while(RESET == (SPI_STAT(spi_periph)&SPI_FLAG_RBNE));
    return(SPI_DATA(spi_periph));
	//LCD_CS_SET;
	
	
} 
void pwm_config_lcd_bl(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER1);

    /* TIMER1 configuration */
    timer_initpara.prescaler         = 84-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 500;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER1,&timer_initpara);

     /* CH1 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER1,TIMER_CH_0,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER1,TIMER_CH_0,0);
    timer_channel_output_mode_config(TIMER1,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER1,TIMER_CH_0,TIMER_OC_SHADOW_ENABLE);

    timer_primary_output_config(TIMER1,ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER1);
    timer_enable(TIMER1);
}

void pwm_config_buzzer(void)
{
    timer_oc_parameter_struct timer_ocintpara;
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER16);
		
    /* TIMER16 configuration */
    timer_initpara.prescaler         = 84-1;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 175;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER16,&timer_initpara);

     /* CH0 configuration in PWM mode */
    timer_ocintpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
    timer_ocintpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocintpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER16,TIMER_CH_0,&timer_ocintpara);

    timer_channel_output_pulse_value_config(TIMER16,TIMER_CH_0,0);
    timer_channel_output_mode_config(TIMER16,TIMER_CH_0,TIMER_OC_MODE_PWM0);
    timer_channel_output_shadow_config(TIMER16,TIMER_CH_0,TIMER_OC_SHADOW_DISABLE);

    timer_primary_output_config(TIMER16,ENABLE);
    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER16);
    timer_enable(TIMER16);
		//delay_1ms(10);
    //timer_disable(TIMER16);
		
}

void heat_timer_config(void)
{
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER13);
    timer_deinit(TIMER13);
    timer_struct_para_init(&timer_initpara);

    timer_initpara.prescaler         = 83999; // 84MHz
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 999; // 200ms
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_init(TIMER13, &timer_initpara);

    /* clear channel 0 interrupt bit */
    timer_interrupt_flag_clear(TIMER13, TIMER_INT_FLAG_UP);
    /* enable the TIMER interrupt */
    timer_interrupt_enable(TIMER13, TIMER_INT_UP);
    /* enable a TIMER */
    timer_enable(TIMER13);
	  nvic_irq_enable(TIMER13_IRQn, 0,0);
}



void RTC_config(void)
{
	  rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();		
	
	  rcu_osci_on(RCU_LXTAL);
	  rcu_osci_stab_wait(RCU_LXTAL);
	  rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);

    rcu_periph_clock_enable(RCU_RTC);
    rtc_register_sync_wait();
}



