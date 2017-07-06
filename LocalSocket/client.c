#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

//客户端


int main()
{
    //1.创建socket
    int client = socket(AF_UNIX, SOCK_STREAM, 0);


    //2.初始化sockaddr_un结构体信息，并且连接服务器
    struct sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, "socket_file");   //与服务器一致
    if(connect(client, (struct sockaddr*)&addr, sizeof(addr)))
    {
        //返回非0说明连接失败
        perror("connect");
        return -1;
    }

    
    //3.到这里说明成功连接到服务器；可以和服务器进行数据交互了
    write(client, "hello world", 11);

    return 0;
}

