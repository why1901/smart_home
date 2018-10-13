#ifndef __HOME_H
#define __HOME_H

#include <stdio.h>
#include <linux/input.h>
#include <pthread.h>

#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <freetype.h>
#include <wctype.h>
#include <locale.h>
#include <wchar.h>

#include "lcd.h"

#define TEST_MAGIC	'x'

#define LED1 _IO(TEST_MAGIC, 0)
#define LED2 _IO(TEST_MAGIC, 1)
#define LED3 _IO(TEST_MAGIC, 2)
#define LED4 _IO(TEST_MAGIC, 3)

#define BEEP_ON		0
#define BEEP_OFF 	1

#define LED_ON		0
#define LED_OFF		1

#define PTH_SIZE 	128
#define BUF_SIZE	1024



#define LED_1(a);	if(a)\
						ioctl(led_fd, LED1, LED_ON);\
					else\
						ioctl(led_fd, LED1, LED_OFF);

#define LED_2(a);	if(a)\
						ioctl(led_fd, LED2, LED_ON);\
					else\
						ioctl(led_fd, LED2, LED_OFF);

#define LED_3(a);	if(a)\
						ioctl(led_fd, LED3, LED_ON);\
					else\
						ioctl(led_fd, LED3, LED_OFF);

#define LED_4(a);	if(a)\
						ioctl(led_fd, LED4, LED_ON);\
					else\
						ioctl(led_fd, LED4, LED_OFF);

#endif