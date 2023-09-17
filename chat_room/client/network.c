#include "network.h"
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

//	分配内存，初始化通信地址，创建socket，绑定，监听，链接
NetWork *init_nw(int type,uint16_t port,const char *ip,bool is_svr)
{
	NetWork *nw = malloc(sizeof(NetWork));
	nw->type = type;
	nw->sockfd = socket(AF_INET,type,0);
	if(0 > nw->sockfd)
	{
		perror("socket");
		free(nw);
		return NULL;
	}
	//	初始化通信地址
	nw->addr.sin_family = AF_INET;
	nw->addr.sin_port = htons(port);
	nw->addr.sin_addr.s_addr = inet_addr(ip);
	nw->addrlen = sizeof(nw->addr);
	nw->is_svr = is_svr;
	if(true == is_svr)
	{
		//	TCP,UDP服务器绑定
		if(bind(nw->sockfd,(SS)&nw->addr,nw->addrlen))
		{
			perror("bind");
			free(nw);
			return NULL;
		}
	}
	//	TCP服务器监听
	if(true == is_svr && SOCK_STREAM == type)
	{
		if(listen(nw->sockfd,12))
		{
			perror("listen");
			free(nw);
			return NULL;
		}
	}
	//	TCP客户端链接
	if(false == is_svr && SOCK_STREAM == type)
	{
		if(connect(nw->sockfd,(SS)&nw->addr,nw->addrlen))
		{
			perror("connect");
			free(nw);
			return NULL;
		}
	}
	return nw;
}

//	等待链接 只能tcp的服务端执行
NetWork *accept_nw(NetWork *svr_nw)
{
	if(false == svr_nw->is_svr || SOCK_DGRAM == svr_nw->type)
		return NULL;
	NetWork *nw = malloc(sizeof(NetWork));
	memcpy(nw,svr_nw,sizeof(NetWork));
	nw->sockfd = accept(svr_nw->sockfd,(SS)&(nw->addr),&(nw->addrlen));
	if(0 > nw->sockfd)
	{
		perror("accept");
		free(nw);
		return NULL;
	}
	return nw;
}

//	发送函数
int send_nw(NetWork *nw,const void *buf,size_t len)
{
	int ret = 0;
	if(SOCK_STREAM == nw->type)
	{
		ret = send(nw->sockfd,buf,len,0);
		if(0 >= ret)
		{
			perror("send");
		}
	}
	else
	{
		ret = sendto(nw->sockfd,buf,len,0,(SS)&nw->addr,nw->addrlen);
		if(0 >= ret)
		{
			perror("sendto");
		}
	}
	return ret;
}

//	接收函数
int recv_nw(NetWork *nw,void *buf,size_t len)
{
	int ret = 0;
	if(SOCK_STREAM == nw->type)
	{
		ret = recv(nw->sockfd,buf,len,0);
		if(0 >= ret)
		{
			perror("recv");
		}
	}
	else
	{
		ret = recvfrom(nw->sockfd,buf,len,0,(SS)&nw->addr,&nw->addrlen);
		if(0 >= ret)
		{
			perror("recvfrom");
		}
	}
	return ret;
}

//	关闭socket，释放内存
void close_nw(NetWork *nw)
{
	close(nw->sockfd);
	free(nw);
}

