/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdbool.h>
#include <stdio.h>
#include "st7735.h"
#include "fonts.h"
#include "testimg.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
CAN_FilterTypeDef sFilterConfig;
CAN_TxHeaderTypeDef TxHeader;
HAL_StatusTypeDef status;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
volatile uint8_t flag=0;
uint8_t TxData[1] = {0xA1};
uint32_t TxMailbox;
uint32_t error_code;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_CAN_Init();
  /* USER CODE BEGIN 2 */
  TFT_Init();
  delay_ms_custom(3000);
  TFT_FillDisplay(ST7735_BLACK);
  TFT_WriteString(5, 81, "App Started", Font_11x18, ST7735_WHITE, ST7735_BLACK);

  // choose CAN Filter
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdHigh= 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;

  HAL_CAN_ConfigFilter(&hcan, &sFilterConfig);
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

  TxHeader.StdId = 0x101;
  TxHeader.RTR = CAN_RTR_DATA;
  TxHeader.IDE = CAN_ID_STD;
  TxHeader.DLC = 1;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  TFT_FillDisplay(ST7735_BLACK);
	      TFT_WriteString(5, 40, "Sending...", Font_7x10, ST7735_WHITE, ST7735_BLACK);

	      status = HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);

	      if (status != HAL_OK)
	      {
	          TFT_WriteString(5, 60, "TX Error!", Font_7x10, ST7735_RED, ST7735_BLACK);
	          // هنا الميكرو بيقولك ليه مش عارف يبعت (مثلاً الصناديق مليانة)
	      }

	      HAL_Delay(2000); // بنستنى الرد

	      if (flag == 1)
	      {
	          TFT_FillDisplay(ST7735_BLACK);
	          TFT_WriteString(5, 81, "Data Received", Font_7x10, ST7735_GREEN, ST7735_BLACK);
	          flag = 0;
	      }
	      else
	      {
	          // لو مفيش استقبال، اسأل الكان: إيه المشكلة؟
	          error_code = HAL_CAN_GetError(&hcan);

	          if (error_code == HAL_CAN_ERROR_NONE) {
	              TFT_WriteString(5, 100, "Bus Status: OK", Font_7x10, ST7735_GREEN, ST7735_BLACK);
	          } else {
	              // تشخيص نوع الخطأ بناءً على المسميات الصحيحة للميكرو بتاعك
	              if (error_code & HAL_CAN_ERROR_ACK) {
	                  TFT_WriteString(5, 50, "ERROR: NO ACK", Font_7x10, ST7735_YELLOW, ST7735_BLACK);
	              }
	              // هنا استخدمنا المسمى اللي ظهر لك في الاقتراحات
	              else if (error_code & HAL_CAN_ERROR_BD) {
	                  TFT_WriteString(5, 50, "ERROR: BIT DOMINANT", Font_7x10, ST7735_RED, ST7735_BLACK);
	              }
	              else if (error_code & HAL_CAN_ERROR_BR) {
	                  TFT_WriteString(5, 50, "ERROR: BIT RECESSIVE", Font_7x10, ST7735_RED, ST7735_BLACK);
	              }
	              else if (error_code & HAL_CAN_ERROR_BOF) {
	                  TFT_WriteString(5, 50, "ERROR: BUS OFF", Font_7x10, ST7735_RED, ST7735_BLACK);
	              }
	          }
	      }
	      HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[1];

    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
    {
        // هنا بنتشيك هل الرد هو اللي إحنا مستنيينه (0xB2)؟
        if (RxData[0] == 0xB2)
        {
            // نور ليد تانية خالص (مثلاً PB1) عشان تعرف إن الـ Handshake تم بنجاح
            flag =1;
        }
    }
}

void delay_us(uint16_t us) {
    __HAL_TIM_SET_COUNTER(&htim2, 0); // تصفير العداد
    HAL_TIM_Base_Start(&htim2);       // تشغيل التايمر
    while (__HAL_TIM_GET_COUNTER(&htim2) < us); // انتظار الميكرو ثواني
    HAL_TIM_Base_Stop(&htim2);        // إيقاف التايمر
}

// دالة المللي ثانية (تعتمد على دالة الميكرو)

void delay_ms_custom(uint16_t ms) {
    for(uint16_t i=0; i<ms; i++) {
        delay_us(1000); // تكرار 1000 ميكرو يعطيك 1 مللي
    }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
