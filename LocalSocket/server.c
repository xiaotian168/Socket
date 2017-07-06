#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <sys/un.h> //struct sockaddr_un 头文件

//用于系统内部通信的本地套接字
//此为服务器端

int main()
{
    //1.创建通信的服务器socket; 用于接收客户端连接
    int server = socket(AF_UNIX, SOCK_STREAM, 0);
    if(-1 == server)
    {
        //说明创建本地通信socket服务器失败
        perror("socket");   //打印错误信息
        return -1;
    }


    //2.bind地址结构体信息给服务器通信
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "socket_file");   //addr.sun_path是一个数组；不能直接用等号赋值；（这里相当于指定通信的管道文件）
    bind(server, (struct sockaddr*)&addr, sizeof(addr));


    //3.监听
    listen(server, 250);    //并且指定连接的最大数量

    
    //4.接收客户端连接；产生通信的socket（阻塞函数）
    int client = accept(server, NULL, NULL);
    if(-1 == client)
    {
        perror("accept");
        return -2;
    }


    //5.到这里说明客户端和服务器已经建立好了通信；可以进行数据交互了
    char databuf[1024];
    while(1)
    {
        int ret = read(client, databuf, sizeof(databuf));
        if(ret <= 0)
        {
            break;
        }
        
        databuf[ret] = '\0';  //添加字符串结束标志'\0'
        printf("read data is: %s\n", databuf);
    }

    close(client);
    return 0;
}
