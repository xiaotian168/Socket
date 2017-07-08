#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

//本地套接字服务器
//主进程负责接受客户端连接，子进程处理已连接客户端和服务器之间信息交互

int main()
{
    //1.创建服务器socket
    int server = socket(AF_UNIX, SOCK_STREAM, 0);

    //2.bind通信方式和通信文件
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "Socket_file3");
    bind(server, (struct sockaddr*)&addr, sizeof(addr));

    //3.监听listen
    listen(server, 250);

    char recv_buf[1024];
    char send_buf[] = "数据接收完成";
    int client;

    //4.接收客户端连接
    while(1)
    {
        client = accept(server, NULL, NULL);
        if(client > 0)
        {
            pid_t pid = fork();
            if(pid == 0)    //子进程
                break;
            else
            {
                close(client);  //父进程关闭不用的socket通信资源
            }

        }
        else
        {
            //这里处理接收客户端连接异常
        }
    }

    //子进程与客户端交互信息
    //接收数据
    while(1)
    {
        int ret = recv(client, recv_buf, sizeof(recv_buf), 0);
        if(ret <= 0)
        {
            if(errno == EINTR)  //操作被中断错误
            {
                continue;
            }
            break;
        }

        recv_buf[ret] = 0;
        printf("recv data is: %s\n", recv_buf);
        send(client, send_buf, sizeof(send_buf), 0);
    }


    close(client);
    close(server);
}
