#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

//TCP Socket客户端

int main()
{
    //创建socket通信
    int client = socket(AF_INET, SOCK_STREAM, 0);

    //connect服务器
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9527);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    connect(client, (struct sockaddr*)&addr, sizeof(addr));

    char send_buf[] = "hello world";
    char recv_buf[1024];
    int ret = recv(client, recv_buf, sizeof(recv_buf), 0);
    if(ret > 0)
    {
        recv_buf[ret] = 0;
        printf("recv data is: %s\n", recv_buf);
    }
    

    send(client, send_buf, strlen(send_buf), 0);

    return 0;
}
