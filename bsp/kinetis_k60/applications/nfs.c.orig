#include <rtthread.h>
#include <lwip/netdb.h> /* 为了解析主机名，需要包含netdb.h头文件 */
#include <lwip/sockets.h> /* 使用BSD socket，需要包含sockets.h头文件 */


void net_demo_nfs(void)
{
    if (dfs_mount(RT_NULL, "/", "nfs", 0, RT_NFS_HOST_EXPORT) == 0)
        rt_kprintf("NFSv3 File System initialized!\n");
    else
        rt_kprintf("NFSv3 File System initialzation failed!\n");

    return ;
}

void nfs_thread_entry(void)
{
    rt_thread_delay(100);
    if (dfs_mount(RT_NULL, "/", "nfs", 0, RT_NFS_HOST_EXPORT) == 0)
        rt_kprintf("NFSv3 File System initialized!\n");
    else
        rt_kprintf("NFSv3 File System initialzation failed!\n");

    return ;
}

#ifdef RT_USING_FINSH
#include <finsh.h>
/* 输出dns_test函数到finsh shell中 */
FINSH_FUNCTION_EXPORT(net_demo_nfs, dns test eg:nfs());
#endif

#ifdef FINSH_USING_MSH
#include <msh.h>
MSH_CMD_EXPORT(net_demo_nfs,start nfs);
#endif
