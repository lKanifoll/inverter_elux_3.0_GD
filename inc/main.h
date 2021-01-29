/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif
	#include "gd32f3x0.h"
#include "systick.h"
#include <stdio.h>
	#include <stdint.h>
/* Includes ------------------------------------------------------------------*/


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
	
#define PIN_SET(GPIO_PORT, GPIO_PIN)    	((uint32_t)(GPIO_BOP(GPIO_PORT) = GPIO_PIN))
#define PIN_RESET(GPIO_PORT, GPIO_PIN)   	((uint32_t)(GPIO_BC(GPIO_PORT)  = GPIO_PIN))

#define LL_GPIO_ResetOutputPin(GPIO_PORT, GPIO_PIN) 		PIN_RESET(GPIO_PORT, GPIO_PIN)
#define LL_GPIO_SetOutputPin(GPIO_PORT, GPIO_PIN)   		PIN_SET(GPIO_PORT, GPIO_PIN)
	
#define TUYA_ON      ((uint32_t)(GPIO_BOP(GPIOA) = GPIO_PIN_1))
#define TUYA_OFF      ((uint32_t)(GPIO_BC(GPIOA) = GPIO_PIN_1))

#define LCD_CS_SET      ((uint32_t)(GPIO_BOP(GPIOB) = GPIO_PIN_11))
#define LCD_CS_CLR      ((uint32_t)(GPIO_BC(GPIOB) = GPIO_PIN_11))

/* PC3 tft rs/dc */
#define LCD_RS_SET      ((uint32_t)(GPIO_BOP(GPIOB) = GPIO_PIN_10))
#define LCD_RS_CLR      ((uint32_t)(GPIO_BC(GPIOB) = GPIO_PIN_10))

/* PC4 tft rst */
#define LCD_RST_SET     ((uint32_t)(GPIO_BOP(GPIOB) = GPIO_PIN_2))
#define LCD_RST_CLR     ((uint32_t)(GPIO_BC(GPIOB) = GPIO_PIN_2))
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */



/* Exported functions prototypes ---------------------------------------------*/


/* USER CODE BEGIN EFP */


uint8_t spi_write_byte(uint32_t spi_periph,uint8_t byte);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

#define LCD_BL_Pin GPIO_PIN_0
#define LCD_BL_GPIO_Port GPIOA

#define LCD_RST_Pin GPIO_PIN_2
#define LCD_RST_GPIO_Port GPIOB

#define LCD_RS_Pin GPIO_PIN_10
#define LCD_RS_GPIO_Port GPIOB

#define LCD_CS_Pin GPIO_PIN_11
#define LCD_CS_GPIO_Port GPIOB
/*
#define CH1_Pin LL_GPIO_PIN_0
#define CH1_GPIO_Port GPIOA
#define CH2_Pin LL_GPIO_PIN_1
#define CH2_GPIO_Port GPIOA
#define THERMO1_Pin LL_GPIO_PIN_4
#define THERMO1_GPIO_Port GPIOA
#define BUZZER_Pin LL_GPIO_PIN_6
#define BUZZER_GPIO_Port GPIOA
#define LCD_WR_Pin LL_GPIO_PIN_7
#define LCD_WR_GPIO_Port GPIOA
#define LCD_RS_Pin LL_GPIO_PIN_5
#define LCD_RS_GPIO_Port GPIOC


#define Key1_Pin LL_GPIO_PIN_6
#define Key1_GPIO_Port GPIOC
#define Key0_Pin LL_GPIO_PIN_7
#define Key0_GPIO_Port GPIOC
#define Key3_Pin LL_GPIO_PIN_8
#define Key3_GPIO_Port GPIOC
#define Key2_Pin LL_GPIO_PIN_9
#define Key2_GPIO_Port GPIOC
#define Key6_Pin LL_GPIO_PIN_8
#define Key6_GPIO_Port GPIOA
#define Key5_Pin LL_GPIO_PIN_9
#define Key5_GPIO_Port GPIOA
#define Key4_Pin LL_GPIO_PIN_10
#define Key4_GPIO_Port GPIOA
*/
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
