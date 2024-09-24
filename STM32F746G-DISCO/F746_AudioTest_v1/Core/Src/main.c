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
#include "dma.h"
#include "dma2d.h"
#include "ltdc.h"
#include "sai.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"
#include "fmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "wm8994/wm8994.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_audio.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_sdram.h"
#include "distortion.h"
#include "delay.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
  BUFFER_OFFSET_NONE = 0,
  BUFFER_OFFSET_HALF,
  BUFFER_OFFSET_FULL,
} BUFFER_StateTypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
extern SAI_HandleTypeDef haudio_out_sai;
extern SAI_HandleTypeDef haudio_in_sai;
static int16_t sampleBuffer[BUFFER_SIZE_SAMPLES];
static uint8_t saiDMATransmitBuffer[DMA_BUFFER_SIZE_BYTES];
static uint8_t saiDMAReceiveBuffer[DMA_BUFFER_SIZE_BYTES];
volatile uint32_t audio_rec_buffer_state;

// LCD Fonts
extern sFONT Font24;
extern sFONT Font20;
extern sFONT Font16;
extern sFONT Font12;
extern sFONT Font8;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
/* USER CODE BEGIN PFP */
void PrintDMABuffer(uint8_t *buffer, size_t size);
void PrintSampleBuffer(int16_t *buffer, size_t size);
void BSP_Bringup(void);
static void AudioInit(void);
static void FillSquareWave(int16_t *buf, uint32_t num_samples);
static void ConvertSampleToDMA(int16_t *sampleBuffer, uint8_t *dmaBuffer, uint32_t num_samples);
static void ExtractSampleFromDMA(uint8_t *dmaBuffer, int16_t *sampleBuffer, uint32_t num_samples);
static void CopySampleBuffer(int16_t *dest, int16_t *orig, uint32_t num_samples);
static void CopyDMABuffer(uint8_t *dest, uint8_t *orig, uint32_t num_samples);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}
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

  /* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SAI2_Init();
  MX_TIM1_Init();
  MX_USART1_UART_Init();
  MX_DMA2D_Init();
  MX_FMC_Init();
  MX_LTDC_Init();
  /* USER CODE BEGIN 2 */
  // BSP LCD Initialization
  BSP_LCD_Init();
  BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
  BSP_LCD_DisplayOn();
  // Layer 0
  BSP_LCD_SelectLayer(0);
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  // Start
  BSP_LCD_SelectLayer(0);
  BSP_LCD_SetFont(&Font20);
  BSP_LCD_DisplayStringAt(0, 10, (uint8_t *)"Electric Guitar Effect Pedal", CENTER_MODE);

  // Audio Setup
  AudioInit();
  audio_rec_buffer_state = BUFFER_OFFSET_NONE;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (audio_rec_buffer_state != BUFFER_OFFSET_NONE)
    {
      if (audio_rec_buffer_state == BUFFER_OFFSET_HALF)
      {
        // PrintDMABuffer(&saiDMAReceiveBuffer[0], 20);
        // DistortionProcess(&saiDMAReceiveBuffer[0], 1, 600, DMA_BUFFER_SIZE_BYTES / 2);
        DelayProcess(&saiDMAReceiveBuffer[0], 0.7, 5000, DMA_BUFFER_SIZE_BYTES / 2);
        CopyDMABuffer(&saiDMATransmitBuffer[0], &saiDMAReceiveBuffer[0], DMA_BUFFER_SIZE_BYTES / 2);
        // ExtractSampleFromDMA(&saiDMAReceiveBuffer[0], &sampleBuffer[0], BUFFER_SIZE_SAMPLES / 2);
        // ConvertSampleToDMA(&sampleBuffer[0], &saiDMATransmitBuffer[0], BUFFER_SIZE_SAMPLES / 2);
      }
      else if (audio_rec_buffer_state == BUFFER_OFFSET_FULL)
      {
        // PrintDMABuffer(&saiDMAReceiveBuffer[DMA_BUFFER_SIZE_BYTES / 2], 20);
        // DistortionProcess(&saiDMAReceiveBuffer[DMA_BUFFER_SIZE_BYTES / 2], 1, 600, DMA_BUFFER_SIZE_BYTES / 2);
        DelayProcess(&saiDMAReceiveBuffer[DMA_BUFFER_SIZE_BYTES / 2], 0.7, 5000, DMA_BUFFER_SIZE_BYTES / 2);
        CopyDMABuffer(&saiDMATransmitBuffer[DMA_BUFFER_SIZE_BYTES / 2], &saiDMAReceiveBuffer[DMA_BUFFER_SIZE_BYTES / 2], DMA_BUFFER_SIZE_BYTES / 2);
        // ExtractSampleFromDMA(&saiDMAReceiveBuffer[DMA_BUFFER_SIZE_BYTES / 2], &sampleBuffer[DMA_BUFFER_SIZE_BYTES / 2], BUFFER_SIZE_SAMPLES / 2);
        // ConvertSampleToDMA(&sampleBuffer[DMA_BUFFER_SIZE_BYTES / 2], &saiDMATransmitBuffer[DMA_BUFFER_SIZE_BYTES / 2], BUFFER_SIZE_SAMPLES / 2);
      }
      audio_rec_buffer_state = BUFFER_OFFSET_NONE;
    }
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

  /** Configure LSE Drive Capability
   */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
   */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 210;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
   */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief Peripherals Common Clock Configuration
 * @retval None
 */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Initializes the peripherals clock
   */
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC | RCC_PERIPHCLK_SAI2;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 50;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV2;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  PeriphClkInitStruct.Sai2ClockSelection = RCC_SAI2CLKSOURCE_PLLSAI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void PrintDMABuffer(uint8_t *buffer, size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    printf("%u ", buffer[i]);
  }
  printf("\n");
}

void PrintSampleBuffer(int16_t *buffer, size_t size)
{
  for (size_t i = 0; i < size; i++)
  {
    printf("%i ", buffer[i]);
  }
  printf("\n");
}

void BSP_Bringup(void)
{
  FillSquareWave(sampleBuffer, BUFFER_SIZE_SAMPLES);
  ConvertSampleToDMA(sampleBuffer, saiDMATransmitBuffer, BUFFER_SIZE_SAMPLES);
  BSP_AUDIO_OUT_Init(OUTPUT_DEVICE_HEADPHONE, VOLUME, AUDIO_FREQ);
  HAL_SAI_Transmit_DMA(&haudio_out_sai, saiDMATransmitBuffer, DMA_BUFFER_SIZE_MSIZE);

  while (1)
    ;
}

static void AudioInit(void)
{
  BSP_AUDIO_OUT_ClockConfig(&haudio_in_sai, AUDIO_FREQ, NULL);
  BSP_AUDIO_OUT_MspInit(&haudio_out_sai, NULL);
  SAIx_Out_Init(AUDIO_FREQ);
  BSP_AUDIO_IN_MspInit(&haudio_in_sai, NULL);
  SAIx_In_Init(SAI_MODESLAVE_RX, 0xF, AUDIO_FREQ);
  wm8994_Init(AUDIO_I2C_ADDRESS, INPUT_DEVICE_INPUT_LINE_1 | OUTPUT_DEVICE_HEADPHONE, VOLUME, AUDIO_FREQ);

  HAL_SAI_Transmit_DMA(&haudio_out_sai, saiDMATransmitBuffer, DMA_BUFFER_SIZE_MSIZE);
  HAL_SAI_Receive_DMA(&haudio_in_sai, saiDMAReceiveBuffer, DMA_BUFFER_SIZE_MSIZE);
}

static void FillSquareWave(int16_t *buf, uint32_t num_samples)
{
  // fill up a 100 Hz square wave
  // 48 kHz sample rate -> 480 samples in 100 Hz -> toggle every 240 samples
  int toggle_period = 240;
  int count = 0;
  int wave_state = 1;
  int magnitude = 30000;

  for (int i = 0; i < num_samples; i++)
  {
    buf[i] = magnitude * wave_state;
    count++;
    if (count >= toggle_period)
    {
      count = 0;
      wave_state = wave_state * (-1);
    }
  }
}

static void ConvertSampleToDMA(int16_t *sampleBuffer, uint8_t *dmaBuffer, uint32_t num_samples)
{
  for (uint32_t i = 0; i < num_samples; i++)
  {
    int16_t *ptr = (int16_t *)&dmaBuffer[i * 8];
    *ptr = sampleBuffer[i];
    *(ptr + 2) = sampleBuffer[i];
  }
}

static void ExtractSampleFromDMA(uint8_t *dmaBuffer, int16_t *sampleBuffer, uint32_t num_samples)
{
  for (uint32_t i = 0; i < num_samples; i++)
  {
    int16_t *ptr = (int16_t *)&dmaBuffer[i * 8];
    sampleBuffer[i] = *ptr;
  }
}

static void CopySampleBuffer(int16_t *dest, int16_t *orig, uint32_t num_samples)
{
  for (uint32_t i = 0; i < num_samples; i++)
  {
    dest[i] = orig[i];
  }
}

static void CopyDMABuffer(uint8_t *dest, uint8_t *orig, uint32_t num_samples)
{
  for (uint32_t i = 0; i < num_samples; i++)
  {
    dest[i] = orig[i];
  }
}

void BSP_AUDIO_IN_TransferComplete_CallBack(void)
{
  audio_rec_buffer_state = BUFFER_OFFSET_FULL;
}

void BSP_AUDIO_IN_HalfTransfer_CallBack(void)
{
  audio_rec_buffer_state = BUFFER_OFFSET_HALF;
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
