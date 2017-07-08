#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

//客户端


int main()
{
    //1.创建连接服务器的socket
    int server = socket(AF_UNIX, SOCK_STREAM, 0);

    //2.bind
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "Socket_file3");  //Socket_file3和服务器绑定的文件名应该一致

    //3.连接服务器
    connect(server, (struct sockaddr*)&addr, sizeof(addr));

    //4.与服务器进行数据交互
    char send_buf[] = "客户端请求连接";
    char recv_buf[1024];
    send(server, send_buf, strlen(send_buf), 0);
    while(1)
    {
        int ret = recv(server, recv_buf, sizeof(recv_buf), 0);
        if(ret <=0)
        {
            if(errno == EINTR)  //recv操作被中断打断了
                continue;
            else
                break;
        }

        recv_buf[ret] = 0;
        printf("%s\n", recv_buf);
    }

    return 0;
}
