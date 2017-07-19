#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <netpacket/packet.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <unistd.h>


//原始套接字
//1.抓包分析


unsigned short eth_type;

//解析打印以太网帧头的信息
//以太网帧头部长度为14个字节
void print_eth_header(const unsigned char *eth_frame)
{
    /*打印格式:
     * Get a packet
     * DMAC: 11:22:33:44:55:66
     * SMAC: 77:88:99:00:11:22
     * TYPE: 0x0800
     */

    const unsigned char *pEth = eth_frame;
    //6字节 -->目的MAC地址
    //6字节 -->源MAC地址
    //2字节 -->类型
    printf("Get a packet \nDMAC:%02x:%02x:%02x:%02x:%02x:%02x\n", pEth[0], pEth[1], pEth[2], pEth[3], pEth[4], pEth[5]);

    printf("SMAC:%02x:%02x:%02x:%02x:%02x:%02x\n", pEth[6], pEth[7], pEth[8], pEth[9], pEth[10], pEth[11]);

    //获取协议类型并且转为主机序
    eth_type = ntohs(*(unsigned short*)(pEth+12));
    printf("协议号:0x%04X\n", eth_type);
    printf("以太网帧头解析结束\n");

}

//处理ip报文信息
void handle_ip(const unsigned char *ip_frame)
{
    /*打印格式:
     * (占4bit)ip版本号: ipV4或ipV6
     * (占4bit)首部长度: xxx
     * (占8bit)服务类型：
     * (占16bit)总长度
     * ...
     * 源IP: 11:22:33:44:55:66
     * 目的IP: 77:88:99:00:11:22
     * TYPE: TCP或者UDP
     */
    
    //ip头偏移12字节到源IP地址
    printf("源IP: %d.%d.%d.%d\n", 
            *(ip_frame+12),
            *(ip_frame+13),
            *(ip_frame+14),
            *(ip_frame+15));
    
    
    printf("目的IP: %d.%d.%d.%d\n", 
            *(ip_frame+16),
            *(ip_frame+17),
            *(ip_frame+18),
            *(ip_frame+19));

    printf("协议: %d\n", *(ip_frame+9));

    
}


int main()
{
    //1.创建原始套接字socket
    //SOCK_RAW: 包括以太网frame的头
    //SOCK_DGRAM:以太网frame的数据
    int raw_socket = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));


    //2.保存raw_socket的一些信息到ifstruct结构体里
    struct ifreq ifstruct;
    strcpy(ifstruct.ifr_ifrn.ifrn_name, "eth1"); //初始化网卡名(后面抓包用到的网卡)；参数二为网卡名
    if(ioctl(raw_socket, SIOCGIFINDEX, &ifstruct) == -1)
    {
        printf("1\n");
        printf("ioctl error! errno = %d\n", errno);
        return -1;
    }


    //3.初始化地址结构体信息
    struct sockaddr_ll addr;    //low level地址
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = ifstruct.ifr_ifindex;
    addr.sll_protocol = htons(ETH_P_ALL);
    addr.sll_hatype = ARPHRD_ETHER; //ha == hardware; 网卡
    addr.sll_pkttype = PACKET_OTHERHOST;    //抓包 外面进来的数据包
    addr.sll_halen = ETH_ALEN;  //MAC地址长度
    addr.sll_addr[6] = 0;
    addr.sll_addr[7] = 0;   //MAC地址长度6位；后面两位不用的置为0


    //4.获取MAC地址
    if(ioctl(raw_socket, SIOCGIFHWADDR, &ifstruct) == -1)
    {
        printf("获取网卡信息出错，errno = %d\n", errno);
        return -2;
    }


    //5.获取属性(flags)，设置混杂模式，抓包必须要设置的
    if(ioctl(raw_socket, SIOCGIFFLAGS, &ifstruct) < 0)
    {
        printf("获取元素套接字flags出错，errno = %d\n", errno);
        return -3;
    }

    ifstruct.ifr_ifru.ifru_flags |= IFF_PROMISC;   //set promisc(混杂模式)
    if(ioctl(raw_socket, SIOCGIFFLAGS, &ifstruct) == -1)
    {
        printf("设置混杂模式出错，errno = %d\n", errno);
        return -4;
    }


    //6.bind
    if(bind(raw_socket, (struct sockaddr*)&addr, sizeof(struct sockaddr_ll)) == -1)
    {
        printf("bind出错, errno = %d\n", errno);
        return -5;
    }
    ioctl(raw_socket, SIOCGIFHWADDR, &ifstruct);


    //7.完成socket的建立工作；可以抓包了
    char recv_buf[1518];    //以太网帧的最大长度
    while(1)
    {
        int ret = read(raw_socket, recv_buf, sizeof(recv_buf));
        printf("开始解析报文\n");
        printf("recv data: len = %d\t%s\n", ret, recv_buf);
        if(ret > 0)
        {
            //打印以太网头部信息, 并且获得数据报文协议类型eth_type
            print_eth_header((unsigned char*)recv_buf);

            if(eth_type == 0x0800) //ip报文
            {
                printf("ip报文\n");
                handle_ip((unsigned char*)recv_buf+14); 
            }
            else if(eth_type == 0x0806)
            {
                printf("arp报文\n");
                //handle_arp((unsigned char*)recv_buf+14);
            }
            else if(eth_type == 0x8863)
            {
                printf("PPPoE发现阶段报文\n");
            }
            else if(eth_type == 0x8864)
            {
                printf("PPPoE会话阶段报文\n");
            }
            else
            {
                printf("其它类型报文; 协议号:0x%04X\n", eth_type);
            }

        }
        sleep(3);   //停3秒再接收

        printf("解析报文结束\n");
        //来个分割线
        printf("-----------------------------\n\n\n");
    }

    return 0;
}

