/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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

/* Includes ------------------------------------------------------------------*/
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ALERT_Pin GPIO_PIN_14
#define ALERT_GPIO_Port GPIOC
#define STATUS_Pin GPIO_PIN_15
#define STATUS_GPIO_Port GPIOC
#define RELAY08_Pin GPIO_PIN_1
#define RELAY08_GPIO_Port GPIOA
#define RELAY07_Pin GPIO_PIN_2
#define RELAY07_GPIO_Port GPIOA
#define RELAY06_Pin GPIO_PIN_3
#define RELAY06_GPIO_Port GPIOA
#define RELAY05_Pin GPIO_PIN_4
#define RELAY05_GPIO_Port GPIOA
#define RELAY04_Pin GPIO_PIN_5
#define RELAY04_GPIO_Port GPIOA
#define RELAY03_Pin GPIO_PIN_6
#define RELAY03_GPIO_Port GPIOA
#define RELAY02_Pin GPIO_PIN_7
#define RELAY02_GPIO_Port GPIOA
#define RELAY01_Pin GPIO_PIN_0
#define RELAY01_GPIO_Port GPIOB
#define RELAY09_Pin GPIO_PIN_1
#define RELAY09_GPIO_Port GPIOB
#define RELAY10_Pin GPIO_PIN_8
#define RELAY10_GPIO_Port GPIOA
#define RELAY11_Pin GPIO_PIN_9
#define RELAY11_GPIO_Port GPIOA
#define RELAY12_Pin GPIO_PIN_10
#define RELAY12_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define RELAY16_Pin GPIO_PIN_15
#define RELAY16_GPIO_Port GPIOA
#define RELAY15_Pin GPIO_PIN_3
#define RELAY15_GPIO_Port GPIOB
#define RELAY14_Pin GPIO_PIN_4
#define RELAY14_GPIO_Port GPIOB
#define RELAY13_Pin GPIO_PIN_5
#define RELAY13_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_6
#define SCL_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
