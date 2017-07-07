#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

//服务器端


int main()
{
    //创建服务器socket
    int server = socket(AF_UNIX, SOCK_STREAM, 0);

    //bind
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "socket_file2");
    bind(server, (struct sockaddr*)&addr, sizeof(addr));
    
    //listen
    listen(server, 250);
    //accept
    int client = accept(server, NULL, NULL);
    if(-1 == client)
    {
        perror("accept");
        return -1;
    }

    //这里 -->成功接收客户端连接；可以进行数据交互了
    char recv_buf[1024];
    char send_buf[1024];
    while(1)
    {
        int ret = recv(client, recv_buf, sizeof(recv_buf), 0);
        if(ret <= 0)
        {
            if(errno == EINTR)  //说明接收数据阻塞被中断了；可忽略此错误码，重新读数据
            {
                continue;
            }

            break;
        }

        recv_buf[ret] = '\0';
        printf("recv data is: %s\n", recv_buf);
    }

    return 0;
}

