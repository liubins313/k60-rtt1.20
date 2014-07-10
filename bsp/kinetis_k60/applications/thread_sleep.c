#include <rtthread.h>
#include "thread_sleep.h"


static void thread_sleep_entry(void *parameter)
{
    rt_uint32_t cnt = 0;
    while(1)
    {
        cnt++;
        rt_thread_delay(1000);
        rt_kprintf("%d\n", cnt);
    }
}


void thread_sleep_init(void)
{
    rt_thread_t sleep_thread;

    sleep_thread = rt_thread_create("sleep", thread_sleep_entry, RT_NULL,
                                    512, 3, 20);

    if(sleep_thread != RT_NULL)
    {
        rt_thread_startup(sleep_thread);
    }
}

