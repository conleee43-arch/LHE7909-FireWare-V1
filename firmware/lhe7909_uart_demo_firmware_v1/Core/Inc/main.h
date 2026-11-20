/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

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
#define MCU_LED_RUN_Pin GPIO_PIN_3
#define MCU_LED_RUN_GPIO_Port GPIOE
#define LHE7909_GPIO2_Pin GPIO_PIN_5
#define LHE7909_GPIO2_GPIO_Port GPIOA
#define LHE7909_DAISY_IN_Pin GPIO_PIN_6
#define LHE7909_DAISY_IN_GPIO_Port GPIOA
#define LHE7909_GPIO4_Pin GPIO_PIN_7
#define LHE7909_GPIO4_GPIO_Port GPIOA
#define LHE7909_START_Pin GPIO_PIN_10
#define LHE7909_START_GPIO_Port GPIOB
#define LHE7909_GPIO3_Pin GPIO_PIN_6
#define LHE7909_GPIO3_GPIO_Port GPIOC
#define LHE7909_GPIO1_Pin GPIO_PIN_7
#define LHE7909_GPIO1_GPIO_Port GPIOC
#define LHE7909_SPI2_CS_Pin GPIO_PIN_8
#define LHE7909_SPI2_CS_GPIO_Port GPIOC
#define LHE7909_nPWDN_Pin GPIO_PIN_6
#define LHE7909_nPWDN_GPIO_Port GPIOB
#define LHE7909_nRESET_Pin GPIO_PIN_7
#define LHE7909_nRESET_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
