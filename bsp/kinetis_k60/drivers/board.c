/*
 * File      : board.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2009 RT-Thread Develop Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      first implementation
 */

#include <rthw.h>
#include <rtthread.h>
#include "board.h"
#include "common.h"
#include <stdint.h>
/**
 * @addtogroup K60
 */

/*@{*/

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
#ifdef  VECT_TAB_RAM
	/* Set the Vector Table base location at 0x1FFF0000 */
    SCB->VTOR = 0x1FFF0000 | (0x0 & (uint32_t)0x1FFFFF80);
#else  /* VECT_TAB_FLASH  */
	/* Set the Vector Table base location at 0x00000000 */
    SCB->VTOR = 0 | (0x0 & (uint32_t)0x1FFFFF80);
#endif
}


/**
 * This is the timer interrupt service routine.
 *
 */
void SysTick_Handler(void)
{
	/* enter interrupt */
	rt_interrupt_enter();

	rt_tick_increase();

	/* leave interrupt */
	rt_interrupt_leave();
}

/**
 * This function will initial STM32 board.
 */
void rt_hw_board_init()
{
	/* NVIC Configuration */
	NVIC_Configuration();

	DelayInit();
	/* Configure the SysTick */
//	SysTick_Config(g_Clocks_Freq.SystemCoreClock/1000);

	rt_hw_usart_init();
	rt_hw_usart3_init();
	rt_hw_usart4_init();
#ifdef RT_USING_CONSOLE
	rt_console_set_device(CONSOLE_DEVICE);
#endif
}

/*@}*/
