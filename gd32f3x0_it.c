/*!
    \file  gd32f3x0_it.c
    \brief interrupt service routines
    
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

#include "gd32f3x0_it.h"
#include "systick.h"

extern void TIMER_Heat_callback();
extern void SysTick_Handler_Callback();
//extern void receive_uart_int();
extern uint8_t idle_flag_stat;
extern uint8_t recv_buffer[200];
extern uint8_t rxcount;
extern void rtc_alarm_callback();


/*!
    \brief      this function handles NMI exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void NMI_Handler(void)
{
}

/*!
    \brief      this function handles HardFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HardFault_Handler(void)
{
    /* if Hard Fault exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles MemManage exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void MemManage_Handler(void)
{
    /* if Memory Manage exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles BusFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void BusFault_Handler(void)
{
    /* if Bus Fault exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles UsageFault exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void UsageFault_Handler(void)
{
    /* if Usage Fault exception occurs, go to infinite loop */
    while (1);
}

/*!
    \brief      this function handles SVC exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SVC_Handler(void)
{
}

/*!
    \brief      this function handles DebugMon exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void DebugMon_Handler(void)
{
}

/*!
    \brief      this function handles PendSV exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void PendSV_Handler(void)
{
}

/*!
    \brief      this function handles SysTick exception
    \param[in]  none
    \param[out] none
    \retval     none
*/
void SysTick_Handler(void)
{
		
    delay_decrement();
		SystemTickInc();
		
}


void RTC_IRQHandler(void)
{
    if(RESET != rtc_flag_get(RTC_STAT_ALRM0F)){
        rtc_flag_clear(RTC_STAT_ALRM0F);
				rtc_alarm_callback();
    } 
}

/*!
    \brief      this function handles external lines 0 to 1 interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void TIMER13_IRQHandler(void)
{
    if(SET == timer_interrupt_flag_get(TIMER13, TIMER_INT_UP)){
        /* clear update interrupt bit */
        timer_interrupt_flag_clear(TIMER13, TIMER_INT_UP);
        /* toggle selected led */
        TIMER_Heat_callback();
    }
}

/*!
    \brief      this function handles USART RBNE interrupt request and TBE interrupt request
    \param[in]  none
    \param[out] none
    \retval     none
*/
void USART1_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE)){
        /* receive data */
       // receiver_buffer[rxcount++] = usart_data_receive(USART1);
       //if(rxcount == receivesize){
       //     usart_interrupt_disable(USART1, USART_INT_RBNE);
       //}
			 recv_buffer[rxcount++] = usart_data_receive(USART1);
				if(rxcount==200) rxcount = 0;
			//receive_uart_int();
			//usart_interrupt_enable(USART1, USART_INT_IDLE);
			
    }
    if((RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE)) ){
        /* receive data */
       // receiver_buffer[rxcount++] = usart_data_receive(USART1);
       //if(rxcount == receivesize){
        //usart_interrupt_disable(USART1, USART_INT_RBNE);
				
       //}
			 usart_interrupt_flag_clear(USART1, USART_INT_FLAG_IDLE);
			 //usart_interrupt_disable(USART1, USART_INT_IDLE);
			 //rxcount = 0;
			 idle_flag_stat = 1;
			 //receive_uart_int();

    }

}
