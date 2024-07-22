/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    tim.c
  * @brief   This file provides code for the configuration
  *          of the TIM instances.
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
#include "tim.h"

/* USER CODE BEGIN 0 */


volatile uint8_t timer1_flag = 0;
volatile uint8_t ommo_timer_rx_flag = 0;
volatile uint8_t ommo_timer_tx_flag = 0;
volatile uint8_t ommo_timer_led_flag = 0;
volatile uint8_t ommo_timer_err_flag = 0;
/* USER CODE END 0 */

/* TIM1 init function */
void MX_TIM1_Init(void)
{

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  LL_TIM_InitTypeDef TIM_InitStruct = {0};

  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

  /* TIM1 interrupt Init */
  NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn, 0);
  NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  TIM_InitStruct.Prescaler = 47999;
  TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
  TIM_InitStruct.Autoreload = 9;
  TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
  TIM_InitStruct.RepetitionCounter = 0;
  LL_TIM_Init(TIM1, &TIM_InitStruct);
  LL_TIM_DisableARRPreload(TIM1);
  LL_TIM_SetClockSource(TIM1, LL_TIM_CLOCKSOURCE_INTERNAL);
  LL_TIM_SetTriggerOutput(TIM1, LL_TIM_TRGO_RESET);
  LL_TIM_SetTriggerOutput2(TIM1, LL_TIM_TRGO2_RESET);
  LL_TIM_DisableMasterSlaveMode(TIM1);
  /* USER CODE BEGIN TIM1_Init 2 */
  /* Clear the update flag */
  LL_TIM_ClearFlag_UPDATE(TIM1);

  /* Enable the update interrupt */
  LL_TIM_EnableIT_UPDATE(TIM1);

  /* Enable counter */
  LL_TIM_EnableCounter(TIM1);
  /* USER CODE END TIM1_Init 2 */

}

/* USER CODE BEGIN 1 */
/**
  * @brief  Timer update interrupt processing
  * @param  None
  * @retval None
  */
void TimerUpdate_Callback(void)
{
	timer1_flag = 1;
}
/**
 * @brief ommo_timer1_task
 * 
 */
void ommo_timer1_task(void)
{
	static uint16_t tx_cnt = 0;
	static uint16_t rx_cnt = 0;
	static uint16_t led_cnt = 0;
	static uint16_t err_cnt = 0;
	static uint16_t uart_err_wait_cnt = 0;
	static uint8_t uart_error_on = 0;
	if(timer1_flag)
	{
		tx_cnt++;
		rx_cnt++;
		led_cnt++;
		err_cnt++;
	    uart_err_wait_cnt++;
	    timer1_flag = 0;

	}
	if( tx_cnt == (TIMER_TX_INTERVAL/ TIMER_INTERVAL) )
	{
		ommo_timer_tx_flag = 1;
		tx_cnt = 0;
	}
	if( rx_cnt == (TIMER_RX_INTERVAL/ TIMER_INTERVAL) )
	{
		ommo_timer_rx_flag = 1;
		rx_cnt = 0;
	}
	if( led_cnt == (TIMER_LED_INTERVAL/ TIMER_INTERVAL) )
	{
		ommo_timer_led_flag = 1;
		led_cnt = 0;
	}
	if( err_cnt == (TIMER_ERR_INTERVAL/ TIMER_INTERVAL) )
	{
		ommo_timer_err_flag = 1;//no check error at test
		err_cnt = 0;
	}
	if( uart_err_wait_cnt == (TIMER_ERR_INTERVAL*10*6/ TIMER_INTERVAL) )
	{
		if(!uart_error_on)
		{
//			LL_USART_EnableIT_ERROR(USART1);
			uart_error_on = 1;
		}
	}
}
/* USER CODE END 1 */
