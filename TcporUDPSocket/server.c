#include <stdio.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>


//TCP Socket服务器

int main()
{
    //1.创建服务器server
    int server = socket(AF_INET, SOCK_STREAM, 0);

    //2.bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9527);
    addr.sin_addr.s_addr = 0; //inet_addr("ip地址");
    bind(server, (struct sockaddr*)&addr, sizeof(addr));

    //3.listen
    listen(server, 250);

    //4.accept接收客户端连接
    int client = accept(server, NULL, NULL);
    if(client < 0)
    {
        if(errno != EINTR)  //说明发生了严重错误，直接退出程序
            return -1;
    }
    
    char send_buf[] = "已接受连接，可以通信了";
    char recv_buf[1024];
    send(client, send_buf, strlen(send_buf), 0);
    int ret = recv(client, recv_buf, sizeof(recv_buf), 0);
    if(ret > 0)
    {
        recv_buf[ret] = 0;
        printf("recv data is: %s\n", recv_buf);
    }

    return 0;
}
