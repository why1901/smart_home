#include <stdio.h>

#include "server.h"


int main(int argc, const char *argv[])
{
	list_ptr head_node;			//定义链表头

	if (argc != 2)
	{
		printf("please input the port\n");
		return -1;
	}

	//申请并初始化链表头
	head_node = request_and_init_phone_list();
	if (head_node == NULL)
		return -1;

	//开启服务器,argv[1]为服务器端口号
	server(head_node, atoi(argv[1]));

	return 0;
}