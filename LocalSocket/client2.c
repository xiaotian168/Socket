#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

//客户端

int main()
{
    //创建通信socket客户端
    int client = socket(AF_UNIX, SOCK_STREAM, 0);

    //connect;
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "socket_file2");
    connect(client, (struct sockaddr*)&addr, sizeof(addr));

    char send_buf[1024] = "hello world";
    send(client, send_buf, strlen(send_buf), 0);

    return 0;

}
