#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define MAP_SIZE    800*480*4


/*打开lcd屏幕并映射屏幕内存空间, 失败返回-1，成功返回fd*/
int open_lcd_drive(unsigned int **lcd_buf_point)
{
    int lcd_fd;

    lcd_fd = open("/dev/fb0",O_RDWR);
    if(lcd_fd == -1)
    {
        perror("open lcd drive error\n");
        return -1;
    }

    *lcd_buf_point = mmap(NULL,MAP_SIZE,PROT_READ|PROT_WRITE,
            MAP_SHARED,lcd_fd,0);

    if(lcd_buf_point == NULL)
    {
        perror("mmap error\n");
        return -1;
    }

    return lcd_fd;
}

//打开led文件，失败返回-1，成功返回fd
int open_led_drive(void)
{
    int led_fd, retval;

    led_fd = open("/dev/Led", O_RDWR);
    if (led_fd == -1)
    {
        printf("open led failure\n");
        return -1;
    }

    return led_fd;

}

/*打开蜂鸣器文件，失败返回-1，成功返回fd*/
int open_bee_drive(void)
{
    int bee_fd;

    bee_fd = open("/dev/beep", O_RDWR);
    if (bee_fd == -1)
    {
        printf("beep open failure\n");
        return -1;
    }
    return bee_fd;
}

//打开lcd屏幕文件，失败返回-1，成功返回fd
int open_screen_drive(void)
{
    int ts_fd;
    /*打开触摸屏文件*/
    ts_fd = open("/dev/input/event0", O_RDONLY);
    if (ts_fd == -1)
    {
        printf("open screen error = %s\n");
        return ;
    }
    return ts_fd;
}

/*关闭lcd屏幕，并取消内存映射*/
int close_lcd_drive(int lcd_fd, int led_fd, int ts_fd, unsigned int *lcd_buf_point)
{
    munmap(lcd_buf_point,MAP_SIZE);

    close(lcd_fd);
    close(led_fd);
    close(ts_fd);
    return 0;
}


/*在屏幕上绘点*/
void draw_a_point_to_lcd(int x, int y, unsigned int color, unsigned int *lcd_buf)
{

    *(lcd_buf+800*y+x) = color;

    return ;
}

