/*
 * File      : application.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-01-05     Bernard      the first version
 */

/**
 * @addtogroup Kinetis_k60
 */
/*@{*/

#include <stdio.h>
#include <board.h>
#include <rtthread.h>
#include "thread_sleep.h"
#include "components.h"
#include "rtt_ksz8041.h"

rt_sem_t sem = RT_NULL;

extern void uart_thread_entry(void* parameter);
extern void uart3_deal_entry(void* parameter);

void rt_init_thread_entry(void* parameter)
{
    rt_thread_t thread;
#ifdef RT_USING_MODULE
    rt_system_module_init();
#endif

#ifdef RT_USING_LWIP
    eth_system_device_init();
    rt_hw_ksz8041_init();
    lwip_system_init();
    rt_kprintf("TCP/IP initialized!\n");
#endif

#ifdef RT_USING_DFS
    /* initialize the device file system */
    dfs_init();

#ifdef RT_USING_DFS_ELMFAT
    /* initialize the elm chan FatFS file system*/
    elm_init();
#endif

#if defined(RT_USING_DFS_NFS) && defined(RT_USING_LWIP)
    /* initialize NFSv3 client file system */
    nfs_init();
#endif
#endif

		sem = rt_sem_create("sem", 1, RT_IPC_FLAG_FIFO);
    thread = rt_thread_create("uart3", uart_thread_entry, RT_NULL, 1024*2, 0x22, 20);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
		thread = rt_thread_create("uart3_d", uart3_deal_entry, RT_NULL, 1024*2, 0x21, 20);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
		
}


int rt_application_init()
{
    rt_thread_t init_thread;

    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 0x20, 20);

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

//    thread_sleep_init();

    return 0;
}

/*@}*/
