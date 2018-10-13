#ifndef __SERVER_H
#define __SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <sys/wait.h>
#include <time.h>

#include "phone_list.h"

#define MAX_LISTEN 		10		//定义服务器最大监听数量
#define BUF_SIZE 		1024	//定义服务器读取内存大小
#define TIME_SIZE		256		//定义时间的大小

extern int server();

#endif