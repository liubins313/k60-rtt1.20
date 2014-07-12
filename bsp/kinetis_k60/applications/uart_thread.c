#include <rtthread.h>
#include <rthw.h>
#include "jcprotocol.h"

#define MAXLEN 1200

enum
{
    UART_IDLE,
    UART_RECV_BEGIN,
    UART_RECV_LENGTH,
    UART_RECV_HEAD,
    UART_RECV_DATA,
    UART_RECV_END,
    UART_RECV_BUSY
};

rt_device_t device;

unsigned char uartcatch[MAXLEN];
unsigned char *uartbuff = uartcatch;
unsigned char *curuartptr = uartcatch;
unsigned char uartstate = UART_IDLE;
unsigned char uartdo = 0;
unsigned char uartbytecnt = 0;
unsigned long uartrecvcnt = 0;
unsigned long recvcnt = 0;
unsigned long sendcnt = 0;

extern rt_sem_t sem;

/********************************************************
函数名称：
函数参数：
函数说明：
返回值：
**********************************************************/
int uartRecvProc (unsigned char curchar)
{
    *curuartptr++ = curchar;
    uartrecvcnt++;

    switch (uartstate)
    {
    case UART_IDLE:
    {
        if (curchar == 0X55)
        {
            uartstate = UART_RECV_BEGIN;
        }
        else
        {
            uartrecvcnt = 0;
            uartbuff = uartcatch;
            curuartptr = uartcatch;
        }
        break;
    }
    case UART_RECV_BEGIN:
    {
        if (curchar == 0XAA)
        {
            uartstate = UART_RECV_LENGTH;
            uartbytecnt = 0;
        }
        else
        {
            uartrecvcnt = 0;
            uartbuff = uartcatch;
            curuartptr = uartcatch;
            uartstate = UART_IDLE;
        }
        break;
    }
    case UART_RECV_LENGTH:
    {
        uartbytecnt++;
        if (uartbytecnt == 2)
        {
            uartstate = UART_RECV_HEAD;
            uartbytecnt = 0;
        }
        break;
    }
    case UART_RECV_HEAD:
    {
        if (curchar == 0X7e)
        {
            uartbytecnt++;
            if (uartbytecnt == 2)
            {
                uartstate = UART_RECV_DATA;
                uartbytecnt = 0;
            }
        }
        else
        {
            uartrecvcnt = 0;
            uartbuff = uartcatch;
            curuartptr = uartcatch;
            uartstate = UART_IDLE;
        }
        break;
    }
    case UART_RECV_DATA:
    {
        if (curchar == 0x7e)
            uartstate = UART_RECV_END;
        break;
    }
    case UART_RECV_END:
    {
        if (curchar == 0x7e)
        {
            uartstate = UART_RECV_BUSY;
            uartdo = 1;
						rt_sem_release(sem);
        }
        else
        {
            uartrecvcnt = 0;
            uartbuff = uartcatch;
            curuartptr = uartcatch;
            uartstate = UART_IDLE;
        }
        break;
    }
    case UART_RECV_BUSY:
    {
        *curuartptr--;
        uartrecvcnt--;
        break;
    }
    }
    if (uartrecvcnt >= MAXLEN)	// 包太长，直接退出
    {
        uartdo = 0;
        uartrecvcnt = 0;
        uartbuff = uartcatch;
        curuartptr = uartcatch;
        uartstate = UART_IDLE;
    }
    return 0;
}

//串口接收数据线程
void uart_thread_entry(void* parameter)
{
    char ch;

    device = rt_device_find("uart3");
    if(device != RT_NULL)
    {
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR);
			
        rt_kprintf("open device uart3 succeed!\r\n");
        while(1)
        {
            if(rt_device_read(device, 0, &ch, 1) == 1)
            {
                uartRecvProc(ch);
            }
				}
    }
}

//串口数据处理线程
void uart3_deal_entry(void* parameter)
{
		rt_uint8_t buf[MAXLEN];
		rt_uint8_t len_send;
		rt_sem_take(sem, RT_WAITING_FOREVER);
    while(1)
    {
			rt_sem_take(sem, RT_WAITING_FOREVER);
			rt_kprintf("received a packet\r\n");
			if((recvcnt=dataunpack(uartbuff,uartrecvcnt,buf)) != 0)//对接收的数据进行解包放到buf中，解包后数据长度recvcnt
			{
				rt_uint8_t i;
				
				for(i=0;i<recvcnt;i++)
				{
						rt_kprintf("%02x ",buf[i]); 
				}
				
				len_send =	datapack(buf,2,uartbuff);//将要发送的buf中的2个数据进行打包后放到uartbuff中，打包后数据长度len_send
				rt_device_write(device,0,uartbuff,len_send);
			}
			uartrecvcnt = 0;
			uartbuff = uartcatch;
			curuartptr = uartcatch;
			uartstate = UART_IDLE;
			
    }
}
