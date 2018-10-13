#ifndef FD_LIST_H
#define FD_LIST_H

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>


#include "list.h"

typedef struct socket_list{
	int socket_fd;

	struct list_head list;
}fd_list, *list_ptr;


extern list_ptr request_and_init_fd_list();

extern list_ptr request_new_list_node();

extern bool list_is_empty();

extern void insert_new_node_to_list();

#endif