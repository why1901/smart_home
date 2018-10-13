#include <server.h>

void send_msg_to_phone(list_ptr head_node, char *buf)
{
	list_ptr pos;

	/*判断当前是否有手机连接了服务器*/
	if (list_is_empty(head_node))
	{
		printf("no phone connection\n");
		return ;
	}
	else
	{	
		/*遍历手机链表中所有的socket、给所有已连接的用户发送家庭终端发来的数据*/
		list_for_each_entry(pos, &(head_node->list), list)
		{
			send(pos->socket_fd, buf, sizeof(buf), 0);
		}
	}

	return ;
}


int server(list_ptr head_node, int port)
{
	int retval, server_fd, length, new_fd, home_fd = 0;
	char buf[BUF_SIZE];					//用于接收客户端连接发送来的连接信息
	char time_buf[TIME_SIZE];			//用于获取当前的系统时间
	struct sockaddr_in server, from;
	struct tm *t;
	time_t tt;	
	
	list_ptr new_node, pos;

	int i;							//定义i用于遍历监听集合
	fd_set rfds, fds;				//定义监听集合


	/*创建服务器socket*/
	server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		printf("server socket create failure\n");
		return -1;
	}

	/*绑定服务器socket信息*/
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = INADDR_ANY;
	length = sizeof(struct sockaddr_in);

	retval = bind(server_fd, (struct sockaddr *)&server, length);
	if (retval == -1)
	{
		printf("server socket bind failure\n");
		return -1;
	}

	/*设置最大监听数量*/
	listen(server_fd, MAX_LISTEN);

	//设置监听集合的最大值
	int max = server_fd;

	FD_ZERO(&rfds);
	
	/*把服务器socket加入监听集合*/
	FD_SET(server_fd, &rfds);

	while(1)
	{	

		//保留原始监听集合状态
		fds = rfds;		

		/*监听socket集合*/
		select(max+1, &fds, NULL, NULL, 0);

		memset(buf, 0, sizeof(buf));

		for(i=0; i<max+1; i++)
		{
			/*当服务器有新用户连接时把用户fd按以下操作处理*/
			if (i == server_fd && (FD_ISSET(server_fd, &fds)))
			{
				/*连接新用户*/
				new_fd = accept(server_fd, (struct sockaddr *)&from, &length);

				printf("[%d] , from:%s:%hu  \n", new_fd, inet_ntoa(from.sin_addr),
				 	ntohs(from.sin_port));

				/*非阻塞接收数据、用于判断是否为家庭控制终端连接*/
				retval = recv(new_fd, buf,sizeof(buf), MSG_DONTWAIT);

				/*把新连接的fd添加进入监听集合*/
				FD_SET(new_fd, &rfds);

				//更新监听的最大集合
				max = max > new_fd? max:new_fd;

				/*判断是否为家庭控制终端连接、并把其socket单独记录为home_fd*/
				if (strcmp(buf, "home") == 0)
				{
					home_fd = new_fd;
					printf("home has connection\n");

					//获取当前的系统时间并发送到家庭终端
					time(&tt);
					t = localtime(&tt);
					sprintf(time_buf,"%s:%4d/%02d/%02d %02d:%02d:%02d","time",
					 t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	
					send(home_fd, time_buf, strlen(time_buf), 0);
				}
				/*若不为家庭控制终端连接、则把新连接的socket添加到链表中*/
				else
				{
					/*申请新的链表节点*/
					new_node = request_new_list_node();
					if (new_node == NULL)
						return -1;

					/*记录new_fd到链表中*/
					new_node->socket_fd = new_fd;

					/*将新节点插入链表*/
					insert_new_node_to_list(head_node, new_node);
				}

				memset(buf, 0, sizeof(buf));
			}

			/*集合其它非服务器socket发生动作响应时、判断其为家庭终端或是手机终端会，并接收数据*/
			else if (i != server_fd && (FD_ISSET(i, &fds)))
			{
				if (i == home_fd)
				{
					/*当接收数据小于0时、表示此时家庭终端断开连接，关闭socket、并且删除监听集合对应的fd*/
					retval = recv(i, buf, sizeof(buf), 0);
					if (retval <= 0)	
					{
						home_fd = 0;
						printf("home disconnection\n");
						close(i);
						FD_CLR(i, &rfds); 
						continue;
					}

					/*正常接收到数据时、则把数据转发给所有连接的手机终端*/
					else
					{
						printf("from home = %s\n", buf);
						send_msg_to_phone(head_node, buf);
					}

					memset(buf, 0, sizeof(buf));	
				}

				/*手机终端传输数据到服务器*/
				else
				{
					/*若接收数据返回值小于0时、则表示手机终端断开连接、删除集合对应fd、并且删除链表中记录的fd*/
					retval = recv(i, buf, sizeof(buf), 0);
					if (retval <= 0)
					{
						printf("%d disconnection\n", i);
						close(i);
						FD_CLR(i, &rfds);
						list_for_each_entry(pos, &(head_node->list), list);
						{
							if (pos->socket_fd == i)
							{
								list_del(&(pos->list));
							}
							continue;
						} 
					}
					/*正常接收到数据则转发到家庭控制终端*/
					else
					{
						printf("from phone : %s\n", buf);
						send(home_fd, buf, sizeof(buf), 0);
					}
					memset(buf, 0, sizeof(buf));
				}
			}
		}
	}
	return 0;
}
