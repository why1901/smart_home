#ifndef __PHONE_LIST_H
#define __PHONE_LIST_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


#include "list.h"

typedef struct socket_list{

	int socket_fd;			//手机连接到服务器的socket描述符

	struct list_head list;
}fd_list, *list_ptr;


extern list_ptr request_and_init_phone_list();

extern list_ptr request_new_list_node();

extern bool list_is_empty();

extern void insert_new_node_to_list();

#endif