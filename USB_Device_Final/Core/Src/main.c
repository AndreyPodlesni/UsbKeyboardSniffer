/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usbd_hid.h"
#include <stdint.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint8_t state;
  uint8_t lctrl;
  uint8_t lshift;
  uint8_t lalt;
  uint8_t lgui;
  uint8_t rctrl;
  uint8_t rshift;
  uint8_t ralt;
  uint8_t rgui;
  uint8_t keys[6];
}
HID_KEYBD_Info_TypeDef;

typedef struct
{
	uint8_t MODIFIER;
	uint8_t RESERVED;
	uint8_t KEYCODE1;
	uint8_t KEYCODE2;
	uint8_t KEYCODE3;
	uint8_t KEYCODE4;
	uint8_t KEYCODE5;
	uint8_t KEYCODE6;
} keyboardHID;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
HID_KEYBD_Info_TypeDef keyboardInfo = {0,0,0,0,0,0,0,0,0,0};
keyboardHID keyboardhid = {0,0,0,0,0,0,0,0};
uint8_t uartBuffer[sizeof(HID_KEYBD_Info_TypeDef)];
uint8_t KeyInjectionUartBuffer[sizeof(keyboardHID)];
keyboardHID KeyInjectionHID;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void ConvertToKeyboardHID(const HID_KEYBD_Info_TypeDef *hostData, keyboardHID *deviceData);
void ReadUARTtoHIDInfo(UART_HandleTypeDef *huart);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
extern USBD_HandleTypeDef hUsbDeviceFS;
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
  MX_USB_DEVICE_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart3, uartBuffer, sizeof(uartBuffer));
  HAL_UART_Receive_IT(&huart2, KeyInjectionUartBuffer, sizeof(KeyInjectionUartBuffer));
  printf("Started");
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void ReadUARTtoHIDInfo(UART_HandleTypeDef *huart) {
   // HAL_UART_Receive(huart, uartBuffer, sizeof(uartBuffer), HAL_MAX_DELAY);
    memcpy(&keyboardInfo, uartBuffer, sizeof(HID_KEYBD_Info_TypeDef));
    // Convert the data
    ConvertToKeyboardHID(&keyboardInfo, &keyboardhid);
    // Send the converted data
    USBD_HID_SendReport(&hUsbDeviceFS, &keyboardhid, sizeof (keyboardhid));
   char massage[] = "Key recived";
   HAL_UART_Transmit(&huart3, massage, sizeof(massage), HAL_MAX_DELAY);

    //HAL_UART_Transmit(&huart3, uartBuffer, sizeof(uartBuffer), HAL_MAX_DELAY);
}




void ConvertToKeyboardHID(const HID_KEYBD_Info_TypeDef *hostData, keyboardHID *deviceData) {
    // Clear the deviceData struct
    *deviceData = (keyboardHID){0};

    // Set the MODIFIER byte
    deviceData->MODIFIER = (hostData->lctrl ? 0x01 : 0) |
                           (hostData->lshift ? 0x02 : 0) |
                           (hostData->lalt ? 0x04 : 0) |
                           (hostData->lgui ? 0x08 : 0) |
                           (hostData->rctrl ? 0x10 : 0) |
                           (hostData->rshift ? 0x20 : 0) |
                           (hostData->ralt ? 0x40 : 0) |
                           (hostData->rgui ? 0x80 : 0);

    // RESERVED byte is typically set to 0
    deviceData->RESERVED = 0;

    // Copy the keys
    if (hostData->keys[0]) deviceData->KEYCODE1 = hostData->keys[0];
    if (hostData->keys[1]) deviceData->KEYCODE2 = hostData->keys[1];
    if (hostData->keys[2]) deviceData->KEYCODE3 = hostData->keys[2];
    if (hostData->keys[3]) deviceData->KEYCODE4 = hostData->keys[3];
    if (hostData->keys[4]) deviceData->KEYCODE5 = hostData->keys[4];
    if (hostData->keys[5]) deviceData->KEYCODE6 = hostData->keys[5];
}


//
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // Check if the callback comes from UART3
    if(huart->Instance == USART3)
    {
        memcpy(&keyboardInfo, uartBuffer, sizeof(HID_KEYBD_Info_TypeDef));
        ConvertToKeyboardHID(&keyboardInfo, &keyboardhid);
        USBD_HID_SendReport(&hUsbDeviceFS, &keyboardhid, sizeof(keyboardhid));
        HAL_UART_Transmit(&huart3, uartBuffer, sizeof(uartBuffer), HAL_MAX_DELAY);
        HAL_UART_Receive_IT(&huart3, uartBuffer, sizeof(uartBuffer));
    }
    if(huart->Instance == USART2)
    {
        memcpy(&KeyInjectionHID, KeyInjectionUartBuffer, sizeof(keyboardHID));
    	USBD_HID_SendReport(&hUsbDeviceFS, &KeyInjectionHID, sizeof (uartBuffer));
    	HAL_UART_Receive_IT(&huart2, KeyInjectionUartBuffer, sizeof(KeyInjectionUartBuffer));
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

#ifdef  USE_FULL_ASSERT
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
