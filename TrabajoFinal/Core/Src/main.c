/**
  ******************************************************************************
  * @file           : main.c
  * @author  		: Lcdo. Mariano Ariel Deville
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include "compatibility.h"
#include "API_delay.h"
#include "API_LCD.h"
#include "API_UART.h"
#include "API_MRF24J40.h"
#include "API_MRF24J40_port.h"
#include "API_debounce.h"

/* Private define ------------------------------------------------------------*/
#define LOW_END_ADDR	(0x1112)
#define INDEX_CERO		0
#define CADENA_SIZE		20

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c3;
SPI_HandleTypeDef hspi3;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart5;
puerto_UART puerto_UART1;
static 	datosFSMUART_t dataFSMUART;
static char mensajeUART[CADENA_SIZE];
static 	debounceData_t boton1;

/* Private function prototypes -----------------------------------------------*/
static void MensajesEntrantesUART(void);
static void CheckBoton(void);
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C3_Init(void);
static void MX_SPI3_Init(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {

	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_I2C3_Init();
	MX_SPI3_Init();

	if(!UARTtInit(&puerto_UART1, &huart5))
		Error_Handler();
	UARTReceiveStringSize(&puerto_UART1, RX_MSG_SIZE);
	UARTFSMInit(&dataFSMUART, &puerto_UART1, mensajeUART, CADENA_SIZE);
	LCDInint();

	if(MRF24J40Init() != INICIALIZACION_OK)
		Error_Handler();
	DebounceFSMInit(&boton1);
	MRF24SetDireccionDestino(LOW_END_ADDR);
	MRF24SetPANIDDestino(MRF24GetMiPANID());
	LCDWrite2String("UART", "WIRELESS");

	while(1) {

		CheckBoton();
		MensajesEntrantesUART();

		if(MRF24IsNewMsg() == MSG_PRESENTE) {

			MRF24ReciboPaquete();
			LCDClearLinea(COMIENZO_2_LINEA);
			LCDGoto(COMIENZO_2_LINEA);
			LCDWriteString((char *)MRF24GetMensajeEntrada());
			UARTSendString(&puerto_UART1, MRF24GetMensajeEntrada());
			UARTSendString(&puerto_UART1, (const uint8_t *)CRLF);

			if(!strcmp((char *)MRF24GetMensajeEntrada(),"CMD:PLV"))
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, ENCENDIDO);
			else if(!strcmp((char *)MRF24GetMensajeEntrada(),"CMD:ALV"))
				HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, APAGADO);
		}
	}
}

/**
  * @brief  Manejo de los mensajes entrantes por la UART
  * @retval
  */
static void MensajesEntrantesUART(void) {

	switch(CheckMsgUART(&dataFSMUART)) {

		case COMIENZO_MSG:

			LCDClearLinea(COMIENZO_1_LINEA);
			LCDGoto(COMIENZO_1_LINEA);
			LCDWriteString(mensajeUART);
			UARTSendString(&puerto_UART1, puerto_UART1.rx_buff);
			break;

		case NUEVO_CARACTER:

			LCDGoto(COMIENZO_1_LINEA);
			LCDWriteString(mensajeUART);
			UARTSendString(&puerto_UART1, puerto_UART1.rx_buff);
			break;

		case FINAL_MSG:

			UARTSendString(&puerto_UART1, (const uint8_t *) CRLF);
			MRF24SetMensajeSalida(mensajeUART);
			MRF24TransmitirDato();
			memset(mensajeUART, VACIO, sizeof(mensajeUART));
			break;

		case SIN_MSG:

			UARTSendString(&puerto_UART1, (const uint8_t *) CRLF);
			LCDClearLinea(COMIENZO_1_LINEA);
			break;

		case BUFFER_LLENO:

			LCDClearLinea(COMIENZO_1_LINEA);
			LCDGoto(COMIENZO_1_LINEA);
			LCDWriteString("Err-Buffer lleno");
			break;

		case ESCUCHANDO:

			break;

		default:

			UARTSendString(&puerto_UART1, (const uint8_t *) "Error inesperado." CRLF);
	}
}

/**
  * @brief  Manejo de las pulsaciones del botón
  * @retval
  */
static void CheckBoton(void){

	switch(DebounceFSMUpdate(&boton1, HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin))) {

		case PRESIONO_BOTON:

			MRF24SetMensajeSalida("CMD:PLA");
			MRF24TransmitirDato();
			break;

		case SUELTO_BOTON:

			MRF24SetMensajeSalida("CMD:ALA");
			MRF24TransmitirDato();
			break;

		case RUIDO:

			UARTSendString(&puerto_UART1, (const uint8_t *) "Ruido detectado en el pulsador." CRLF);
			break;

		case ERROR_ANTI_REBOTE:

			UARTSendString(&puerto_UART1, (const uint8_t *) "Error al procesar el antirrebote." CRLF);
			break;

		case BOTON_SIN_CAMBIOS:

			break;

		default:

			UARTSendString(&puerto_UART1, (const uint8_t *) "Error inesperado." CRLF);
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

/**
  * @brief I2C3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C3_Init(void)
{

  /* USER CODE BEGIN I2C3_Init 0 */

  /* USER CODE END I2C3_Init 0 */

  /* USER CODE BEGIN I2C3_Init 1 */

  /* USER CODE END I2C3_Init 1 */
  hi2c3.Instance = I2C3;
  hi2c3.Init.ClockSpeed = 80000;
  hi2c3.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c3.Init.OwnAddress1 = 0;
  hi2c3.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c3.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c3.Init.OwnAddress2 = 0;
  hi2c3.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c3.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C3_Init 2 */

  /* USER CODE END I2C3_Init 2 */

}

/**
  * @brief SPI3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI3_Init(void)
{

  /* USER CODE BEGIN SPI3_Init 0 */

  /* USER CODE END SPI3_Init 0 */

  /* USER CODE BEGIN SPI3_Init 1 */

  /* USER CODE END SPI3_Init 1 */
  /* SPI3 parameter configuration*/
  hspi3.Instance = SPI3;
  hspi3.Init.Mode = SPI_MODE_MASTER;
  hspi3.Init.Direction = SPI_DIRECTION_2LINES;
  hspi3.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi3.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi3.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi3.Init.NSS = SPI_NSS_SOFT;
  hspi3.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_128;
  hspi3.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi3.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi3.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi3.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI3_Init 2 */

  /* USER CODE END SPI3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, MRF24_CS_Pin|MRF24_WAKE_Pin|MRF24_RESET_Pin|LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : MRF24_CS_Pin MRF24_WAKE_Pin MRF24_RESET_Pin LD2_Pin */
  GPIO_InitStruct.Pin = MRF24_CS_Pin|MRF24_WAKE_Pin|MRF24_RESET_Pin|LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : MRF24_INTERRUP_Pin */
  GPIO_InitStruct.Pin = MRF24_INTERRUP_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(MRF24_INTERRUP_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	UARTReceiveStringSize(&puerto_UART1, RX_MSG_SIZE);
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
	  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, 1);
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
