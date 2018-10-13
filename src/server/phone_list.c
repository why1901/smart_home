
#include "phone_list.h"



//申请内核链表的表头,成功返回申请到地址，失败返回空
list_ptr request_and_init_phone_list(void)
{
	list_ptr head_node;

	head_node = malloc(sizeof(fd_list));
	if (head_node == NULL)
	{
		printf("request head node failure\n");
		return NULL;
	}

	INIT_LIST_HEAD(&(head_node->list));

	return head_node;
}

//申请一个新的链表节点以用于存放新连接上的手机客户信息，成功返回申请到时的节点，失败返回空
list_ptr request_new_list_node(void)
{
	list_ptr new_node;

	new_node = malloc(sizeof(fd_list));
	if (new_node == NULL)
	{
		printf("request new node failure \n");
		return NULL;
	}

	new_node->list.prev = NULL;
	new_node->list.next = NULL;

	return new_node;
}

//将新连接的手机插入到手机链表中
void insert_new_node_to_list(list_ptr head_node, list_ptr new_node)
{
	list_add_tail(&(new_node->list), &(head_node->list));

	return ;
}

//判断当前是否无手机连接服务器
bool list_is_empty(list_ptr head_node)
{
	if (list_empty(&(head_node->list)))
		return true;

	return false;
}



