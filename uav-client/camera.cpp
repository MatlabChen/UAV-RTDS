#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

//---------GPS---------
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
//#include <unistd.h>
#include <termios.h>

//#include <stdlib.h>
//#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <time.h>
//---------GPS---------

using namespace cv;
using namespace std;

//---------GPS---------
string operator+(string &content, int number) {
	string temp = "";
	char t = 0;
	while (true) {
		t = number % 10 + '0';
		temp = t + temp;
		number /= 10;
		if (number == 0) {
			return content + temp;
		}
	}
}

//由于+=会调用+号，所以 += 必须写在 + 号重载后面
string& operator+=(string &content, int number) {
	return content = content + number;
}

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


string getGPS()
{
	int fd = 0;
	int n = 0;
	char buff[1024];
	char* dev_name = "/dev/ttyTHS2";

	if ((fd = open(dev_name, O_RDWR | O_NOCTTY | O_NDELAY)) < 0)
	{
		perror("Can't Open the ttyUSB0 Serial Port");
		return "nogps";
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

	string rst = "";
	rst += gpsdata[1];
	rst += "_";
	rst += gpsdata[2];
	close(fd);
	cout << rst << endl;
	return rst;
}
//--------------------------GPS-----------------------------

void getTimeString(char* timeStr){
    time_t times = time(NULL);
    struct tm* utcTime = gmtime(&times);
    sprintf(timeStr, "%04d%02d%02d%02d%02d%02d", utcTime->tm_year+1900, utcTime->tm_mon+1, utcTime->tm_mday, utcTime->tm_hour, utcTime->tm_min, utcTime->tm_sec);
}

string getImageName(string path) {
	string gps = getGPS();
	char time[20];
	getTimeString(time);
	//string timeStr = time;
	string imgName = "auto_";
	imgName.append(time);
	imgName += "_";
	imgName.append(gps);
	//imgName += index;
	imgName.append(".jpg");
	
	string absoultePath = path + "/";
	absoultePath += imgName;
	cout << absoultePath << endl;
	return absoultePath;
}

int main(int argc, char** argv) {
	
	string path;
	if (argc == 1) {
		cout << "Usage: ./camera -p 文件路径，'/' 结尾" << endl;
		return - 1;
	}
	
	// 解析程序参数
	for (int i = 0; i < argc; i++) {
		if (strcmp("-p", argv[i]) == 0) {
			// -p 后面的参数是保存照片的路径
			path.append(argv[++i]);
			cout << "文件路径为：" << path << endl;
		}
	}
	
	// 切勿注意：只能用yjchen在平台上运行代码，不要用nvidia去运行，否则无法保存图片
	// cout << cv::getBuildInformation();
    VideoCapture cap("v4l2src device=/dev/video1 ! videoconvert ! video/x-raw ! appsink drop=true");
	
    if (!cap.isOpened()) {
        cout << "Failed to open camera." << endl;
        return -1;
    } else {
		cout << "--------------open camera success--------------" << endl;
	}
  
	int index = 0;
    while(1)
    {
        Mat frame;
        cap >> frame;
		index++;
        //imshow("original", frame);
		
		string absoultePath = getImageName(path);
		//string absoultePath = path + "/";
		//absoultePath += imgName;
		//cout << absoultePath << endl;
		
		// 拍照：保存图片
		imwrite(absoultePath, frame);
		//imgName.at(0)++;
		frame.release();
		// 等待30秒
		sleep(6);
		
		/* 以下代码用于按键操作
		char keyCode = waitKey(30);
		if (keyCode == 's') {
			// 把图片保存起来
			//Mat dst = frame.clone();
			imwrite(imgName, frame);
			imgName.at(0)++;
			frame.release();
		} else if (keyCode == 27) {
			break;
		}
		*/

    }
    return 0;
}  