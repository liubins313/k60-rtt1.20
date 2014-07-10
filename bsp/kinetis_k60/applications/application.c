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

extern void uart_thread_entry(void* parameter);

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

    thread = rt_thread_create("uart3", uart_thread_entry, RT_NULL, 1024*8, 63, 20);
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }

}



int rt_application_init()
{
    rt_thread_t init_thread;

#if (RT_THREAD_PRIORITY_MAX == 32)
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#else
    init_thread = rt_thread_create("init",
                                   rt_init_thread_entry, RT_NULL,
                                   2048, 8, 20);
#endif

    if (init_thread != RT_NULL)
        rt_thread_startup(init_thread);

//    thread_sleep_init();

    return 0;
}

/*@}*/
