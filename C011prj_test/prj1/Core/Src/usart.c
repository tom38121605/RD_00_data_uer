/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
USART_TypeDef huart1;
uint8_t tx_buff[]={65,66,67,68,69,70,71,72,73,74}; //ABCDEFGHIJ in ASCII code
uint8_t rx_buff[10];
__IO uint8_t ubSend = 0;
const uint8_t aStringToSend[] = "STM32C0xx USART LL API Example : TX in IT mode\r\nConfiguration UART 115200 bps, 8 data bit/1 stop bit/No parity/No HW flow control\r\n";
uint8_t ubSizeToSend = sizeof(aStringToSend);

uint32_t uart_rx_cnt = 0;
uint32_t uart_err = 0;
/* USER CODE END 0 */

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

  LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_PCLK1);

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);

  LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
  /**USART1 GPIO Configuration
  PA9 [PA11]   ------> USART1_TX
  PA10 [PA12]   ------> USART1_RX
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  LL_SYSCFG_EnableFastModePlus(LL_SYSCFG_I2C_FASTMODEPLUS_PA9);

  LL_SYSCFG_EnableFastModePlus(LL_SYSCFG_I2C_FASTMODEPLUS_PA10);

  /* USART1 interrupt Init */
  NVIC_SetPriority(USART1_IRQn, 0);
  NVIC_EnableIRQ(USART1_IRQn);

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  USART_InitStruct.PrescalerValue = LL_USART_PRESCALER_DIV1;
  USART_InitStruct.BaudRate = 1000000;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_SetTXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_SetRXFIFOThreshold(USART1, LL_USART_FIFOTHRESHOLD_1_8);
  LL_USART_DisableFIFO(USART1);
  LL_USART_ConfigAsyncMode(USART1);

  /* USER CODE BEGIN WKUPType USART1 */

  /* USER CODE END WKUPType USART1 */

  LL_USART_Enable(USART1);

  /* Polling USART1 initialisation */
  while((!(LL_USART_IsActiveFlag_TEACK(USART1))) || (!(LL_USART_IsActiveFlag_REACK(USART1))))
  {
  }
  /* USER CODE BEGIN USART1_Init 2 */
//  while((!(LL_USART_IsActiveFlag_TEACK(USART1))) || (!(LL_USART_IsActiveFlag_REACK(USART1))))
//  {
//  }
  LL_USART_ClearFlag_ORE(USART1);
  LL_USART_ClearFlag_NE(USART1);
  LL_USART_ClearFlag_PE(USART1);
  LL_USART_ClearFlag_FE(USART1);

  LL_USART_EnableIT_RXNE(USART1);
  LL_USART_EnableIT_ERROR(USART1);// for some reason cannot start ERR interrupt


  /* USER CODE END USART1_Init 2 */

}

/* USER CODE BEGIN 1 */

/**
 * @brief ommo_uart_tx_task
 * 
 */

void ommo_uart_tx_task(void)
{
  /* Start transfer only if not already ongoing */
  if (ubSend == 0)
  {
	/* Start USART transmission : Will initiate TXE interrupt after TDR register is empty */
	LL_USART_TransmitData8(USART1, aStringToSend[ubSend++]);

	/* Enable TXE interrupt */
	LL_USART_EnableIT_TXE(USART1);
  }
}

/**
  * @brief  Function called for achieving next TX Byte sending
  * @param  None
  * @retval None
  */
void USART_TXEmpty_Callback(void)
{
  if (ubSend == (ubSizeToSend - 1))
  {
    /* Disable TXE interrupt */
    LL_USART_DisableIT_TXE(USART1);

    /* Enable TC interrupt */
    LL_USART_EnableIT_TC(USART1);
  }

  /* Fill TDR with a new char */
  LL_USART_TransmitData8(USART1, aStringToSend[ubSend++]);
}

/**
  * @brief  Function called at completion of last byte transmission
  * @param  None
  * @retval None
  */
void USART_CharTransmitComplete_Callback(void)
{
  if (ubSend == sizeof(aStringToSend))
  {
    ubSend = 0;

    /* Disable TC interrupt */
    LL_USART_DisableIT_TC(USART1);

    /* Turn LED4 On at end of transfer : Tx sequence completed successfully */
//    LED_On();
  }
}

/**
  * @brief  Function called in case of error detected in USART IT Handler
  * @param  None
  * @retval None
  */
void Error_Callback(void)
{
  __IO uint32_t isr_reg;


  /* Disable USARTx_IRQn */
//  NVIC_DisableIRQ(USART1_IRQn);

  /* Error handling example :
    - Read USART ISR register to identify flag that leads to IT raising
    - Perform corresponding error handling treatment according to flag
  */
  isr_reg = LL_USART_ReadReg(USART1, ISR);
  if (isr_reg & LL_USART_ISR_NE)
  {
	  uart_err = 1;
	  LL_USART_ClearFlag_NE(USART1);
  }
  else if (isr_reg & LL_USART_ISR_PE)
  {
	  uart_err = 2;
	  LL_USART_ClearFlag_PE(USART1);
  }
  else if (isr_reg & LL_USART_ISR_ORE)
  {
	  uart_err = 3;
	  LL_USART_ClearFlag_ORE(USART1);
  }
  else if (isr_reg & LL_USART_ISR_FE)
  {
	  uart_err = 4;
	  LL_USART_ClearFlag_FE(USART1);
  }
}

/**
 * @brief ommo_uart_rx_task
 *
 */
/**
  * @brief  Function called from USART IRQ Handler when RXNE flag is set
  *         Function is in charge of reading character received on USART RX line.
  * @param  None
  * @retval None
  */
void USART_CharReception_Callback(void)
{

	uint8_t temp;
	static uint8_t back;
	temp = LL_USART_ReceiveData8(USART1);
//	LL_USART_TransmitData8(USART1 , temp);
	if(back + 1 == temp)
	{
		uart_rx_cnt ++;
	}
	back = temp;
}

/**
 * @brief ommo_uart_tx_string_task
 * 
 */
void ommo_uart_tx_string_task(void)
{
	if(ommo_timer_tx_flag)
	{
		int num = 0;
		LL_USART_TransmitData8(USART1,0);// must add this when the data first byte is not 00
		for( ;num<256;num++)
		{
			while(LL_USART_IsActiveFlag_TXE(USART1) != 1);
			LL_USART_TransmitData8(USART1,num);
		}
		ommo_timer_tx_flag = 0;
	}
}

/**
 * @brief ommo_uart_rx_string_task
 * 
 */
void ommo_uart_rx_string_task(void)
{
	if(ommo_timer_rx_flag)
	{
//		while(LL_USART_IsActiveFlag_TXE(USART1) != 1);
//		LL_USART_TransmitData8(USART1 , 'H');
//		while(LL_USART_IsActiveFlag_TXE(USART1) != 1);
//		LL_USART_TransmitData8(USART1 , 'i');
//		while(LL_USART_IsActiveFlag_TXE(USART1) != 1);
//		LL_USART_TransmitData8(USART1 , '\r');
//		while(LL_USART_IsActiveFlag_TXE(USART1) != 1);
//		LL_USART_TransmitData8(USART1 , '\n');

//		while(LL_USART_IsActiveFlag_TXE(USART1) != 1);
//		LL_USART_TransmitData8(USART1 , uart_rx_cnt/1000%10+'0');
//		while(LL_USART_IsActiveFlag_TXE(USART1) != 1);
//		LL_USART_TransmitData8(USART1 , uart_rx_cnt/100%10+'0');
//		while(LL_USART_IsActiveFlag_TXE(USART1) != 1);
//		LL_USART_TransmitData8(USART1 , uart_rx_cnt%10+'0');

		if(uart_rx_cnt >= (255 * (TIMER_RX_INTERVAL/TIMER_TX_INTERVAL)))
		{
			led_toggle();
			uart_err = 0;
		}
//		if(uart_err)
//		{
//			led_toggle();
//		}
		uart_rx_cnt = 0;
		ommo_timer_rx_flag = 0;
	}
}

/**
 * @brief ommo_uart_task
 *
 */
void ommo_uart_task(void)
{
////    if(ommo_status == STATUS_ON)
////    {
////        if(ommo_mode == TX_MODE)
////        {
////        	ommo_uart_tx_string_task();
////        }
////        else
////        {
////    		ommo_uart_rx_string_task();
////        }

////    }
}

/* USER CODE END 1 */
