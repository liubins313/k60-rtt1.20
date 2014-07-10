#include <rtthread.h>
#include <rthw.h>

void uart_thread_entry(void* parameter)
{
		char *test1={"this is test1\r\n"};
		char *test2={"this is test2\r\n"};
    rt_device_t device;
    char ch;

    device = rt_device_find("uart3");
    if(device != RT_NULL)
    {
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR);

        rt_kprintf("open device uart3 succeed!\r\n");
        while(1)
        {
            while(rt_device_read(device, 0, &ch, 1) == 1)
            {
                switch(ch)
                {
                case 0x30:
                    rt_device_write(device,0,test1,strlen(test1));
                    break;
                case 0x31:
                    rt_device_write(device,0,test2,strlen(test2));
                    break;

                }
            }
        }
    }
}
