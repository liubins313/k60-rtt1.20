/*
 * File      : serial.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2012, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2006-03-13     bernard      first version
 * 2012-05-15     lgnq         modified according bernard's implementation.
 * 2012-05-28     bernard      code cleanup
 * 2012-11-23     bernard      fix compiler warning.
 * 2013-02-20     bernard      use RT_SERIAL_RB_BUFSZ to define
 *                             the size of ring buffer.
 */

#include <rthw.h>
#include <rtthread.h>
#include "uart.h"
#include "gpio.h"
#include "rtt_serial.h"
#include <rtdevice.h>
#include "uart3_4.h"

static struct rt_serial_device serial3;
static struct serial_ringbuffer uart3_int_rx;

static struct rt_serial_device serial4;
static struct serial_ringbuffer uart4_int_rx;

static uint16_t *gRevCh3 = RT_NULL;
static uint16_t *gRevCh4 = RT_NULL;

static void UART3_ISR(uint16_t byteReceived)
{
    static uint16_t ch;
    /* enter interrupt */
    rt_interrupt_enter();
    ch = byteReceived;
    gRevCh3 = &byteReceived;
    rt_hw_serial_isr(&serial3);

    /* leave interrupt */
    rt_interrupt_leave();
}

static void UART4_ISR(uint16_t byteReceived)
{
    static uint16_t ch;
    /* enter interrupt */
    rt_interrupt_enter();
    ch = byteReceived;
    gRevCh4 = &byteReceived;
    rt_hw_serial_isr(&serial4);

    /* leave interrupt */
    rt_interrupt_leave();
}


static rt_err_t uart3_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
		uint32_t baudrate;
    UART_InitTypeDef UART_InitStruct1;
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);
    UART_InitStruct1.instance = serial->config.reserved;
    
    switch(cfg->baud_rate)
    {
    case BAUD_RATE_9600:
        baudrate = 9600;
        break;
    case BAUD_RATE_115200:
        baudrate = 115200;
        break;
    }
    UART_QuickInit(UART0_RX_PD06_TX_PD07, baudrate);

    /* enable Tx hardware FIFO to enhance proformence */
    UART_EnableTxFIFO(UART_InitStruct1.instance, true);
    UART_SetTxFIFOWatermark(UART_InitStruct1.instance, UART_GetTxFIFOSize(HW_UART0));

    /* enable Rx IT */
    UART_CallbackRxInstall(UART_InitStruct1.instance, UART3_ISR);
    UART_ITDMAConfig(UART_InitStruct1.instance, kUART_IT_Rx, true);
    return RT_EOK;
}

static rt_err_t uart4_configure(struct rt_serial_device *serial, struct serial_configure *cfg)
{
    uint32_t baudrate;
		UART_InitTypeDef UART_InitStruct1;
    RT_ASSERT(serial != RT_NULL);
    RT_ASSERT(cfg != RT_NULL);
    UART_InitStruct1.instance = serial->config.reserved;
    
    switch(cfg->baud_rate)
    {
    case BAUD_RATE_9600:
        baudrate = 9600;
        break;
    case BAUD_RATE_115200:
        baudrate = 115200;
        break;
    }
    UART_QuickInit(UART0_RX_PD06_TX_PD07, baudrate);

    /* enable Tx hardware FIFO to enhance proformence */
    UART_EnableTxFIFO(UART_InitStruct1.instance, true);
    UART_SetTxFIFOWatermark(UART_InitStruct1.instance, UART_GetTxFIFOSize(HW_UART0));

    /* enable Rx IT */
    UART_CallbackRxInstall(UART_InitStruct1.instance, UART4_ISR);
    UART_ITDMAConfig(UART_InitStruct1.instance, kUART_IT_Rx, true);
    return RT_EOK;
}

static rt_err_t uart3_control(struct rt_serial_device *serial, int cmd, void *arg)
{

    RT_ASSERT(serial != RT_NULL);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        UART_ITDMAConfig(serial->config.reserved, kUART_IT_Rx, false);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        UART_ITDMAConfig(serial->config.reserved, kUART_IT_Rx, true);
        break;
    }
    return RT_EOK;
}

static rt_err_t uart4_control(struct rt_serial_device *serial, int cmd, void *arg)
{

    RT_ASSERT(serial != RT_NULL);

    switch (cmd)
    {
    case RT_DEVICE_CTRL_CLR_INT:
        UART_ITDMAConfig(serial->config.reserved, kUART_IT_Rx, false);
        break;
    case RT_DEVICE_CTRL_SET_INT:
        UART_ITDMAConfig(serial->config.reserved, kUART_IT_Rx, true);
        break;
    }
    return RT_EOK;
}

static int uart3_putc(struct rt_serial_device *serial, char c)
{
    UART_WriteByte(serial->config.reserved, c);
    return 1;
}

static int uart4_putc(struct rt_serial_device *serial, char c)
{
    UART_WriteByte(serial->config.reserved, c);
    return 1;
}

static int uart3_getc(struct rt_serial_device *serial)
{
		uint8_t temp = 0;
		int c;
    c = -1;
    
    if(gRevCh3 != RT_NULL)
    {
        c = *gRevCh3;
        gRevCh3 = RT_NULL;
    }

    return c;
}

static int uart4_getc(struct rt_serial_device *serial)
{
    uint8_t temp = 0;
		int c;
    c = -1;
    
    if(gRevCh4 != RT_NULL)
    {
        c = *gRevCh4;
        gRevCh4 = RT_NULL;
    }

    return c;
}

static const struct rt_uart_ops uart3_ops =
{
    uart3_configure,
    uart3_control,
    uart3_putc,
    uart3_getc,
};

static const struct rt_uart_ops uart4_ops =
{
    uart4_configure,
    uart4_control,
    uart4_putc,
    uart4_getc,
};


int rt_hw_usart3_init(void)
{
    struct serial_configure config;
    
    UART_QuickInit(UART3_RX_PB10_TX_PB11, 115200);

    config.baud_rate = BAUD_RATE_115200;
    config.bit_order = BIT_ORDER_LSB;
    config.data_bits = DATA_BITS_8;
    config.parity    = PARITY_NONE;
    config.stop_bits = STOP_BITS_1;
    config.invert    = NRZ_NORMAL;
    config.reserved  = HW_UART3;
    serial3.ops    = &uart3_ops;
    serial3.int_rx = &uart3_int_rx;
    serial3.config = config;

    return rt_hw_serial_register(&serial3, "uart3",
                                 RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM,
                                 RT_NULL);
    
}

int rt_hw_usart4_init(void)
{
	  struct serial_configure config;
    
    UART_QuickInit(UART4_RX_PC14_TX_PC15, 115200);

    config.baud_rate = BAUD_RATE_115200;
    config.bit_order = BIT_ORDER_LSB;
    config.data_bits = DATA_BITS_8;
    config.parity    = PARITY_NONE;
    config.stop_bits = STOP_BITS_1;
    config.invert    = NRZ_NORMAL;
    config.reserved  = HW_UART4;
    serial4.ops    = &uart4_ops;
    serial4.int_rx = &uart4_int_rx;
    serial4.config = config;

    return rt_hw_serial_register(&serial4, "uart4",
                                 RT_DEVICE_FLAG_RDWR | RT_DEVICE_FLAG_INT_RX | RT_DEVICE_FLAG_STREAM,
                                 RT_NULL);
}
