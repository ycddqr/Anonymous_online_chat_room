#ifndef NETWORK_H
#define NETWORK_H

#include <stdbool.h>
#include <netinet/in.h>
#include <stdint.h>

typedef struct sockaddr* SS;

typedef struct NetWork
{
	int type;			//	通信协议 TCP UDP
	int sockfd;		//	socket对象
	struct sockaddr_in addr;	//	通信地址
	socklen_t addrlen;	//	通信地址结构体字节
	bool is_svr;		//  是否为服务端
}NetWork;

//	分配内存，初始化通信地址，创建socket，绑定，监听，链接
NetWork *init_nw(int type,uint16_t port,const char *ip,bool is_svr);

//	等待链接 只能tcp的服务端执行
NetWork *accept_nw(NetWork *svr_nw);

//	发送函数
int send_nw(NetWork *nw,const void *buf,size_t len);

//	接收函数
int recv_nw(NetWork *nw,void *buf,size_t len);

//	关闭socket，释放内存
void close_nw(NetWork *nw);

#endif
