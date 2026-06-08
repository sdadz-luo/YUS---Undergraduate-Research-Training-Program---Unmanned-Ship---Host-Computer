/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "memorymap.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

uint8_t RData[255] = {0};
uint8_t RData_Tap = 0;
uint8_t RDataBuffer;
uint8_t RData_Flag = 0;

uint8_t RDataBuffer1;

uint8_t RData6[255] = {0};
uint8_t RData_Tap6 = 0;
uint8_t RDataBuffer6;
uint8_t RData_Flag6 = 0;

uint8_t TData[9] = {0};

int dataid_int = 0;
uint8_t strdataid_int[32] = {0};
float dataversion = 0 ;
uint8_t strdataversion[32] = {0};
uint8_t dataid[32] = {0};
float datavalue = 0;
uint8_t strdatavalue[32] = {0};

float imu_x = 0;
float imu_y = 0;
float imu_z = 0;
float dht11 = 0;
float humi = 0;
float temp = 0;
float ph = 0;
float cond = 0;
float turb = 0;
uint8_t timedata[] = "00:00";



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */


int fputc(int ch, FILE *f)
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xffff);
  return ch;
}

void printf_string(char* name, char* showdata){
	printf("%s=\"%s\"\xff\xff\xff", name, showdata);
}

void printf_number(char* name, int num){
	printf("%s=%d\xff\xff\xff", name, num);
}

void printf_float(char* name, float num){
	printf("%s=%d\xff\xff\xff", name, (int)(num * 10000));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
		if(huart->Instance == USART1){
		RData[RData_Tap] = RDataBuffer;
		RData_Tap = RData_Tap + 1;
		if(RData[0] != 0x0A){
			RData_Tap = 0;
		}
		if((RData[RData_Tap-1] == 0x0A)&&(RData[RData_Tap-2] == 0x0D)){
			RData_Tap = 0;
			RData_Flag = 1;
		}
		HAL_UART_Receive_IT(&huart1, &RDataBuffer, sizeof(RDataBuffer));
	}
	if(huart->Instance == USART2){
		if(RDataBuffer1 == 'A'){
			printf_string("password.txt","123456");
		}
		HAL_UART_Receive_IT(&huart2, &RDataBuffer1, sizeof(RDataBuffer1));
	}
	if(huart->Instance == UART4){
		RData6[RData_Tap6] = RDataBuffer6;
		RData_Tap6 = RData_Tap6 + 1;
		if((RData6[RData_Tap6-1] == '}')&&(RData6[RData_Tap6-2] == '}')){
			RData_Flag6 = 1;
			RData_Tap6 = 0;
		}
		HAL_UART_Receive_IT(&huart4, &RDataBuffer6, sizeof(RDataBuffer6));
	}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM1){
		printf_string("t3.txt",(char*)timedata);
		printf_float("x0.val", imu_x);
		printf_float("x1.val", imu_y);
		printf_float("x2.val", imu_z);
		printf_float("x3.val", dht11);
		printf_float("x4.val", humi);
		printf_float("x5.val", temp);
		printf_float("x6.val", ph);
		printf_float("x7.val", cond);
		printf_float("x8.val", turb);
	}
}

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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_UART4_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	HAL_UART_Receive_IT(&huart1, &RDataBuffer, sizeof(RDataBuffer));
	HAL_UART_Receive_IT(&huart2, &RDataBuffer1, sizeof(RDataBuffer1));
	HAL_UART_Receive_IT(&huart4, &RDataBuffer6, sizeof(RDataBuffer6));
	HAL_TIM_Base_Start_IT(&htim1);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {	
		
		if(RData_Flag == 1){
			if(RData[3] == 48){
				printf_string("t1.txt","STOP");
			}else{
				switch(RData[1]){
					case 0x00:
						printf_string("t1.txt","STOP");
						break;
					case 0x01:
						printf_string("t1.txt","AHEAD");
						break;
					case 0x02:
						printf_string("t1.txt","BEHIND");
						break;
					case 0x03:
						printf_string("t1.txt","LEFT");
						break;
					case 0x04:
						printf_string("t1.txt","RIGHT");
						break;
				}
			}

			if(RData[1]==0x0E){
				TData[0] = RData[2];
				TData[1] = RData[3] + 48;
			}else{
				TData[0] = RData[1] + 48;
				TData[1] = RData[2] + 48;
				TData[2] = RData[3] + 48;
				TData[3] = RData[4] + 48;
				TData[4] = RData[5] + 48;
				TData[5] = RData[6] + 48;
				TData[6] = RData[7] + 49;
				TData[7] = RData[8] + 48;
				TData[8] = RData[9] + 48;
			}
			
			if (RData[1]==0x0F){
				TData[0] = RData[1] + 48;
				TData[1] = RData[2] + 48;
				TData[2] = RData[3] - 48;
			}
			printf_number("n0.val",TData[1]-48);
			printf_number("n1.val",TData[3]-48);
			printf_number("n2.val",TData[4]-48);
			HAL_UART_Transmit(&huart3, TData, 9, 100);
			RData_Flag = 0;
		}
		
		if(RData_Flag6 == 1){
			sscanf((char *)RData6,"{\"id\":\"%d\",\"version\":\"%f\",\"params\":{\"%31[^\"]\":\"%[^\"]\"}}", &dataid_int, &dataversion, dataid, strdatavalue);
			
			if(strdatavalue[2] == ':'){
				printf_string("t3.txt",(char*)strdatavalue);
				timedata[0] = strdatavalue[0];
				timedata[1] = strdatavalue[1];
				timedata[2] = strdatavalue[2];
				timedata[3] = strdatavalue[3];
				timedata[4] = strdatavalue[4];
			}
			else{	
				sscanf((char*)strdatavalue,"%f",&datavalue);
				uint8_t buffer[20];
				sprintf((char*)buffer, "%.4f", datavalue);
				sscanf((char*)buffer, "%f", &datavalue);
			}
			
			if(dataid[0] == 'i' && dataid[1] == 'm' && dataid[2] == 'u'){
				if(dataid[4] == 'x'){
					printf_float("x0.val", datavalue);
					imu_x = datavalue;
				}
				else if(dataid[4] == 'y'){
					printf_float("x1.val", datavalue);
					imu_y = datavalue;
				}
				else if(dataid[4] == 'z'){
					printf_float("x2.val", datavalue);
					imu_z = datavalue;
				}
			}
			else if(dataid[0] == 'd' && dataid[1] == 'h' && dataid[2] == 't'){
				printf_float("x3.val", datavalue);
				dht11 = datavalue;
			}
	 		else if(dataid[0] == 'h' && dataid[1] == 'u' && dataid[2] == 'm'){
				printf_float("x4.val", datavalue);
				humi = datavalue;
			}
			else if(dataid[0] == 't' && dataid[1] == 'e' && dataid[2] == 'm'){
				printf_float("x5.val", datavalue);
				temp = datavalue;
			}
			else if(dataid[0] == 'P' && dataid[1] == 'H' ){
				printf_float("x6.val", datavalue);
				ph = datavalue;
			}
			else if(dataid[0] == 'c' && dataid[1] == 'o' && dataid[2] == 'n'){
				printf_float("x7.val", datavalue);
				cond = datavalue;
			}
			else if(dataid[0] == 't' && dataid[1] == 'u' && dataid[2] == 'r'){
				printf_float("x8.val", datavalue);
				turb = datavalue;
			}
			memset(dataid, 0, sizeof(dataid));
			memset(strdatavalue, 0, sizeof(strdatavalue));

			
//			printf("%s\r\n%g\r\n", dataid, datavalue);
			RData_Flag6 = 0;
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

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 60;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
