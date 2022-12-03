#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <time.h>

int Flag = 0;

int* GetGpsData(char* buff, int gpsdata[]);
int GetBuff();
int set_serial(int fd);

int set_serial(int fd)
{
    struct termios newttys1, oldttys1;

    /*保存原有串口配置*/
    if (tcgetattr(fd, &oldttys1) != 0)
    {
        perror("Setupserial 1");
        return -1;
    }
    memset(&newttys1, 0, sizeof(newttys1));/* 先将新串口配置清0 */
    newttys1.c_cflag |= (CLOCAL | CREAD); /* CREAD 开启串行数据接收，CLOCAL并打开本地连接模式 */

    newttys1.c_cflag &= ~CSIZE;/* 设置数据位 */
    /* 数据位选择 */

    newttys1.c_cflag |= CS8;

    /* 设置奇偶校验位 */
    newttys1.c_cflag &= ~PARENB;

    /* 设置波特率 */
 
    cfsetispeed(&newttys1, B9600);
    cfsetospeed(&newttys1, B9600);
      
    /*设置停止位*/

    newttys1.c_cflag &= ~CSTOPB;/*默认为一位停止位； */


    /* 设置最少字符和等待时间，对于接收字符和等待时间没有特别的要求时*/
    newttys1.c_cc[VTIME] = 0;/* 非规范模式读取时的超时时间；*/
    newttys1.c_cc[VMIN] = 0; /* 非规范模式读取时的最小字符数*/
    tcflush(fd, TCIFLUSH);/* tcflush清空终端未完成的输入/输出请求及数据；TCIFLUSH表示清空正收到的数据，且不读取出来 */

     /*激活配置使其生效*/
    if ((tcsetattr(fd, TCSANOW, &newttys1)) != 0)
    {
        perror("com set error");
        exit(1);
    }

    return 0;
}


int* GetGpsData(char* buff, int gpsdata[])
{

	int flag = 9;
	char* result = NULL;
	result = strtok(buff, ",");
	{
		while (flag--) {
			if (flag == 6) {
				if (!strcmp(result, "A")) Flag = 1;
				else Flag = -1;
				gpsdata[0] = Flag;
			}
			if (flag == 5) {

				double tmp = atof(result);
				gpsdata[1] = floor(tmp / 100) * 100000 + (tmp - floor(tmp / 100) * 100) / 60 * 100000;
			}
			if (flag == 3) {
				double tmp = atof(result);
				gpsdata[2] = floor(tmp / 100) * 100000 + (tmp - floor(tmp / 100) * 100) / 60 * 100000;
				break;
			}
			result = strtok(NULL, ",");
		}
	}

//	printf("1:%d\t2:%d\t3:%d\n", Flag, gpsdata[0], gpsdata[1]);

	return gpsdata;
}


int GetBuff()
{
	int fd = 0;
	int n = 0;
	char buff[1024];
	char* dev_name = "/dev/ttyTHS2";

	if ((fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
	{
		perror("Can't Open the ttyUSB0 Serial Port");
		return -1;
	}
	set_serial(fd);

	int* gpsdata = (int*)malloc(3 * sizeof(int)) ;
	while (Flag == 0)
	{
		sleep(1);
		read(fd,buff,sizeof(buff));

		if(buff[0] == '$'&& buff[5] == 'C' )
			gpsdata =GetGpsData(buff, gpsdata);
	}

	printf("1:%d\t2:%d\t3:%d\n",  gpsdata[0], gpsdata[1],gpsdata[2]);
	close(fd);
	return 0;
}

int main(int argc, char** argv)
{

//	 GetBuff();
// /*
	time_t start,end;
	start =clock();//or time(&start);
	GetBuff();
	end =clock();
	printf("time=%f\n",(double)(end-start)/CLOCKS_PER_SEC);
	// */
	return 0;
}







