#include "home.h"


enum hh{ext = 0, led, bee};
enum LED{led1=1,led2,led3,led4};

int led_fd, bee_fd, socket_fd;
unsigned int *lcd_buf;
char time_buf[1024];
int lcd_fd,ts_fd;



int open_drive(void)
{
	/*获取lcd屏幕fd*/
	lcd_fd = open_lcd_drive(&lcd_buf);
	if (lcd_fd == -1)
		return -1;

	/*获取led灯的fd*/
	led_fd = open_led_drive();
	if (led_fd == -1)
		return -1;

	/*获取屏幕输入fd*/
	ts_fd = open_screen_drive();
	if (ts_fd == -1)
		return -1;

	/*获取蜂鸣器fd*/
	bee_fd = open_bee_drive();
	if (bee_fd == -1)
		return -1;

	return 0;
}


void *time_thread(void *arg)
{
	char *pt;
	wchar_t wtext_time[1024];
	
	pt = time_buf + 5;


	int ret = mbstowcs(wtext_time,pt,strlen(pt));

	while(1)
	{
		Lcd_Show_FreeType(wtext_time,20,0xffffff,600,470, lcd_buf);
	}
	
	

	return ;
}



/*模拟温度上传*/
void *tem_thread(void *arg)
{
	char tem[16] ;

	memcpy(tem, "wendu:35", 10);

	while(1)
	{
		send(socket_fd, tem, strlen(tem), 0);

		sleep(10);
	}
}


/*客户端连接服务器*/
int client(char *port)
{
	int retval, length;
	struct sockaddr_in server;
	char buf[BUF_SIZE];
	char *ptr;
	pthread_t tim;

	/*创建socket套接字*/
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd == -1)
	{
		printf("socket create failure\n");
		return -1;
	}

	/*绑定服务器地址*/
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(port));
	server.sin_addr.s_addr = inet_addr("112.74.37.132");

	length = sizeof(struct sockaddr_in);

	/*连接服务器*/
	retval = connect(socket_fd, (struct sockaddr *)&server, length);
	if (retval == -1)
	{
		printf("connet server failure\n");
		return -1;
	}

	/*向服务器发送home表明身份*/
	memcpy(buf, "home", 4);
	send(socket_fd, buf, strlen(buf), 0);
	printf("buf = %s\n", buf);

	while(1)
	{
		/*从服务器接收数据*/
		memset(buf, 0, sizeof(buf));
		recv(socket_fd, buf, sizeof(buf), 0);
		if (strlen(buf) == 0)
			break;

		/*判断接收数据*/
		if (strncmp(buf, "LED", 3) == 0)
		{
			//printf("LED\n");

			/*控制灯亮灯灭*/
			ptr = buf + 3;
			if(strncmp(ptr, "1", 1) == 0)
				ioctl(led_fd, LED1, LED_ON);
			else
				ioctl(led_fd, LED1, LED_OFF);

			ptr = buf+4;
			if(strncmp(ptr, "1", 1) == 0)
				ioctl(led_fd, LED2, LED_ON);
			else
				ioctl(led_fd, LED2, LED_OFF);

			ptr = buf+5;
			if(strncmp(ptr, "1", 1) == 0)
				ioctl(led_fd, LED3, LED_ON);
			else
				ioctl(led_fd, LED3, LED_OFF);

			ptr = buf+6;
			if(strncmp(ptr, "1", 1) == 0)
				ioctl(led_fd, LED4, LED_ON);
			else
				ioctl(led_fd, LED4, LED_OFF);

		}
		/*操作蜂鸣器*/
		else if (strncmp(buf, "BEE", 3) == 0)
		{
			ioctl (bee_fd, BEEP_ON, 1);
			sleep(1);
			ioctl (bee_fd, BEEP_OFF, 1);
		}
		/*其他数据获取*/
		else
		{
			if (strncmp(buf, "time", 4) == 0)
			{
				printf("buf = %s\n", buf);
				memcpy(time_buf, buf, sizeof(buf));
				pthread_create(&tim, NULL, time_thread, NULL);
				pthread_detach(tim);
			}

			
		}
		memset(buf, 0, sizeof(buf));
	}
	return 0;
}


void *client_thread(void *arg)
{
	char *prot = (char *)arg;

	/*调用客户端连接服务器*/
	client(prot);

	return ;
}

/*监控主界面触摸屏*/
int screen()
{
	int x, y, pressure, retval, opt;
	struct input_event ts_ev;

	while (1)
	{
		read(ts_fd, &ts_ev, sizeof(ts_ev));

		/*记录触摸屏x,y的坐标位置*/
		if(ts_ev.type == EV_ABS)
		{
			switch(ts_ev.code)
			{
				case ABS_X:
					x = ts_ev.value;
					break;
				case ABS_Y:
					y = ts_ev.value;
					break;
			}
		}
		else
		{
			/**判断松手时x,y的坐标*/
			if ((ts_ev.type == EV_KEY) && (ts_ev.code == BTN_TOUCH))
			{
				pressure = ts_ev.value;

				if (pressure == 0)
				{
					if (x>130 && x<305 && y>340 && y<420)
					{
						opt = led;
						break;
					}
					if (x>56 && x<658 && y>340 && y<420)
					{
						opt = bee;
						break;
					}
					if (x>735 && x<790 && y>10 && y<60)
					{
						opt = ext;
						break;
					}
				}
			}
		}
	}
	return opt;
}

/*监控led界面触摸屏*/
int led_screen(int ts_fd)
{
	int x, y, pressure, opt;
	struct input_event ts_ev;

	while (1)
	{
		read(ts_fd, &ts_ev, sizeof(ts_ev));

		/*记录触摸的x,y坐标*/
		if(ts_ev.type == EV_ABS)
		{
			switch(ts_ev.code)
			{
				case ABS_X:
					x = ts_ev.value;
					break;
				case ABS_Y:
					y = ts_ev.value;
					break;
			}
		}
		else
		{
			/*判断触摸手松开时、x/y的坐标范围*/
			if ((ts_ev.type == EV_KEY) && (ts_ev.code == BTN_TOUCH))
			{
				pressure = ts_ev.value;

				//printf("x=%d, y=%d\n", x, y);
				if (pressure == 0)
				{
					if (x>60 && x<135 && y>370 && y<430)
					{
						opt = led1;
						break;
					}
					if (x>240 && x<310 && y>370 && y<430)
					{
						opt = led2;
						break;
					}
					if (x>430 && x<505 && y>370 && y<430)
					{
						opt = led3;
						break;
					}
					if (x>610 && x<685 && y>370 && y<430)
					{
						opt = led4;
						break;
					}
					if (x>730 && x<785 && y>10 && y<55)
					{
						opt = ext;
						break;
					}
				}
				else
					continue;
			}
		}
	}
	return opt;
}



/*根据触摸屏返回值操作led灯，并把相应的操作发送到服务器以转发到手机*/
int opt_led()
{

	char pth[PTH_SIZE];
	char msg[20] = {'\0'};
	int led_opt;

	char flag = 0;

	/*输入led界面照片路径*/
	memcpy(pth, "../../picture/led.jpg", 22);
	display_format_jpeg(pth, lcd_buf);

	while(1)
	{
		memset(msg, 0, sizeof(msg));
		/*监控触摸屏幕，led_opt接收其返回*/
		led_opt = led_screen(ts_fd);
		switch (led_opt)
		{
			/*灯1亮, 并把灯的相关操作定稿msg并发送到服务器*/
			case led1:
				if (((flag >> 0) & 1) == 0)
				{
					ioctl(led_fd, LED1, LED_ON);
					flag |= 1;
					memcpy(msg, "LED1 ON ", 8);
					send(socket_fd, msg, strlen(msg), 0);
				}
				else
				{
					ioctl(led_fd, LED1, LED_OFF);
					flag &= ~(1 << 0);
					memcpy(msg, "LED1 OFF", 8);
					send(socket_fd, msg, strlen(msg), 0);
				}
				break;

			/*灯2亮*/
			case led2:
				if (((flag >> 2) & 1) == 0)
				{
					ioctl(led_fd, LED2, LED_ON);
					flag |= 0x04;
					memcpy(msg, "LED2 ON ", 8);
					send(socket_fd, msg, strlen(msg), 0);
				}
				else
				{
					ioctl(led_fd, LED2, LED_OFF);
					flag &= ~(1<<2); 
					memcpy(msg, "LED2 OFF", 8);
					send(socket_fd, msg, strlen(msg), 0);
				}
				break;

			/*灯3亮*/
			case led3:
				if (((flag >> 4) & 1) == 0)
				{
					ioctl(led_fd, LED3, LED_ON);
					flag |= 0x16;
					memcpy(msg, "LED3 ON ", 8);
					send(socket_fd, msg, strlen(msg), 0);
				}
				else
				{
					ioctl(led_fd, LED3, LED_OFF);
					flag &= ~(1<<4);
					memcpy(msg, "LED3 OFF", 8);
					send(socket_fd, msg, strlen(msg), 0);
				}
				break;

			/*灯4亮*/
			case led4:
				if (((flag >> 6) & 1) == 0)
				{
					ioctl(led_fd, LED4, LED_ON);
					flag |= (1<<6);
					memcpy(msg, "LED4 ON ", 7);-
					send(socket_fd, msg, strlen(msg), 0);
				}
				else
				{
					ioctl(led_fd, LED4, LED_OFF);
					flag &= ~(1<<6);
					memcpy(msg, "LED4 OFF", 7);
					send(socket_fd, msg, strlen(msg), 0);
				}
				break;

			/*退出*/
			case ext:
				return -1;
				break;	
		}
	}
}

int main(int argc, const char *argv[])
{
	int retval, flag = 0;
	char pth[PTH_SIZE];
	pthread_t tid, tid_tem;
	int opt, ret_led;

	if (argc != 2)
	{
		printf("please input port\n");
		return -1;
	}

	/*创建一条新的线程、用于连接服务器*/
	pthread_create(&tid, NULL, client_thread, (void *)argv[1]);
	pthread_detach(tid);

	/*输入主界面照片的路径*/
	memcpy(pth, "../../picture/home.jpg", 23);

	//打开相关驱动文件
	open_drive();

	//初始灯灭
	LED_1(flag);
	LED_2(flag);
	LED_3(flag);
	LED_4(flag);

	/*创建线程模拟温度上传*/
	pthread_create(&tid_tem, NULL, tem_thread, NULL);
	pthread_detach(tid_tem);
	while(1)
	{
		/*打开主界面照片*/
		display_format_jpeg(pth, lcd_buf);

		//get_weather(lcd_buf);

		/*调用函数监控屏幕， opt接收其返回值*/
		opt = screen();
		switch (opt)
		{
			/*触摸退出按钮*/
			case ext:
				return -1;
				break;

			/*触摸led按钮*/
			case led:
				opt_led();
				break;

			/*触摸蜂鸣器按钮*/
			case bee:
				ioctl (bee_fd, BEEP_ON, 1);
				sleep(1);
				ioctl (bee_fd, BEEP_OFF, 1);
				
				break;
		}
	}

	/*关闭打开的各种文件描述符*/
	close_lcd_drive(lcd_fd, led_fd, ts_fd, lcd_buf);

	return 0;
}