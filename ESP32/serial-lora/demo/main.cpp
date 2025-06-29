/**
 * @file main.cpp
 * @author <NAME> (<EMAIL>)
 * https://www.cnblogs.com/muyi23333/articles/13529115.html
 * https://my.oschina.net/emacs_8829458/blog/17365371
 * https://zhuanlan.zhihu.com/p/521283753
 */
#include <atomic>
#include <string>
#include <iostream>

#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/format.h>

//! Serial devices
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>

#define DEVICE "/dev/ttyUSB0"

#define LOGD(...) fmt::print(fg(fmt::color::blue), __VA_ARGS__)
#define LOGI(...) fmt::print(fg(fmt::color::green), __VA_ARGS__)
#define LOGW(...) fmt::print(fg(fmt::color::yellow)|fmt::emphasis::bold, __VA_ARGS__)
#define LOGE(...) fmt::print(fg(fmt::color::red)|fmt::emphasis::bold, __VA_ARGS__)

#define SUCCESS 0
#define FAILURE -1
#define UNUSED(x) (void)(x)
#define MAX_BUFFER_SIZE 512

#define SEND_MODE 
#define SEND_DELAY_SECOND 3
static pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

enum StopBit{
	STOPBITS_1 = 0,
	STOPBITS_1_5,
	STOPBITS_2,
};
enum DataBit{
	DATABITS_5 = 5,
	DATABITS_6,
	DATABITS_7,
	DATABITS_8,
};
struct Message{
	int fd;
	int len;
	char data[MAX_BUFFER_SIZE];
};
int set_serial_baudrate(struct termios *opt,unsigned int baudrate){
	cfsetispeed(opt,baudrate);
	cfsetospeed(opt,baudrate);
	LOGI("set_serial_baudrate success\n");
	return SUCCESS;
}
/**
 * 设置停止位
 * 若停止位为1，对应数据位为7/8。
 *! 若停止位为1.5，对应数据位为5。
 * 若停止位为2，对应数据位为6/7/8。
 */
int set_serial_stopbit(struct termios *opt,StopBit stopbits){
	switch(stopbits){
		case STOPBITS_1:
			opt->c_cflag &= ~CSTOPB;//1位停止位
			break;
		case STOPBITS_1_5:
			opt->c_cflag &= ~CSTOPB;//需关闭2位停止位
			break;
		case STOPBITS_2:			//2位停止位
			opt->c_cflag |= CSTOPB;
			break;
		default:
			LOGE("set_serial_stopbits error\n");
			break;
	}
	LOGI("set_serial_stopbits success\n");
	return SUCCESS;
}

int set_serial_data_bit(struct termios *opt, DataBit databit)
{
    opt->c_cflag &= ~CSIZE;
    switch (databit) {
    	case DATABITS_8:
        	opt->c_cflag |= CS8;
    		break;
    	case DATABITS_7:
        	opt->c_cflag |= CS7;
    		break;
    	case DATABITS_6:
        	opt->c_cflag |= CS6;
    		break;
    	case DATABITS_5:
        	opt->c_cflag |= CS5;
    		break;
    	default:
        	opt->c_cflag |= CS8;
    		break;
    	}
	LOGI("set_data_bit success\n");
	return SUCCESS;
}

static void set_serial_parity(struct termios *opt, char parity) {
    switch (parity) {
        case 'N': 
		case 'n':  // 无校验
            opt->c_cflag &= ~PARENB;
            break;
        case 'E': 
		case 'e':  // 偶校验
            opt->c_cflag |= PARENB;
            opt->c_cflag &= ~PARODD;
            break;

        case 'O': 
		case 'o':  // 奇校验
            opt->c_cflag |= PARENB;
            opt->c_cflag |= PARODD;  // 修正位操作
            break;
        case 'S': 
		case 's':  // Space校验 (校验位恒0)
            opt->c_cflag |= PARENB;
            #ifdef CMSPAR
                opt->c_cflag |= CMSPAR;  // Linux特殊模式
            #endif
            opt->c_cflag &= ~PARODD;
            opt->c_iflag |= INPCK;
            break;
        case 'M': 
		case 'm':  // Mark校验 (校验位恒1)
            opt->c_cflag |= PARENB;
            #ifdef CMSPAR
                opt->c_cflag |= CMSPAR;
            #endif
            opt->c_cflag |= PARODD;
            opt->c_iflag |= INPCK;
            break;
        default:  // 默认无校验
            opt->c_cflag &= ~PARENB;
            break;
    }
	LOGI("set_serial_parity success\n");
}
int set_serial_attr(int fd,int  baudrate, DataBit databit, StopBit stopbit, char parity, int vtime,int vmin){
	//设置串口属性
	struct termios options;
	tcgetattr(fd, &options);
	//设置波特率
	set_serial_baudrate(&options,baudrate);
	//设置数据位
	set_serial_data_bit(&options,databit);
	//设置停止位
	set_serial_stopbit(&options,stopbit);
	//设置校验位
	set_serial_parity(&options,parity);
	//设置最小读取字符和超时时间
	options.c_cc[VTIME] = vtime;
	options.c_cc[VMIN] = vmin;
	//关闭硬件流控
	options.c_cflag &= ~CRTSCTS;
	//丢弃初始垃圾数据
	tcflush(fd, TCIFLUSH);  	
	//应用串口配置
	if(tcsetattr(fd, TCSANOW, &options) != 0){
		LOGE("tcsetattr failed");
		return FAILURE;
	}
	LOGI("set_serial_attr success\n");
	return SUCCESS;
}


int try_send_data(int fd, const char *data, size_t size) {
	if(pthread_mutex_lock(&mutex) == 0){
		ssize_t bytes_written = write(fd, data, size);
		pthread_mutex_unlock(&mutex);
		if (bytes_written < 0) {
			return FAILURE;
		}
		return bytes_written;
	}
	return FAILURE;
}

int try_receive_data(int fd, char *buffer, size_t size) {
	if(pthread_mutex_lock(&mutex) == 0){
		ssize_t bytes_read = read(fd, buffer, size);
		pthread_mutex_unlock(&mutex);
		if (bytes_read < 0) {
			return FAILURE;
		}
		return bytes_read;
	}
	return FAILURE;
}

void* send_data(void* arg) {
	int ret = 0;
	if(arg == NULL){
		LOGE("arg is null");
		return NULL;
	}
	int fd =*(int*)arg;
	int count = 0;
	while(true){
		//需要增加回车换行，否则无法接收到数据
		std::string msg = "hello world "+std::to_string(count++)+"\r\n";
		ret = try_send_data(fd, msg.c_str(), msg.length());
		if(ret> 0){
			LOGI("send data {}\n",msg.c_str());
			char res[MAX_BUFFER_SIZE] = {0};
			LOGI("get response\n");
			while(true){
				ret = try_receive_data(fd,res, MAX_BUFFER_SIZE);
				if(ret > 0){
					break;
					LOGI("respond: {}",res);
				}
			}
		}
		sleep(SEND_DELAY_SECOND);
	}
	return NULL;
};

void* receive_data(void* arg) {
	int ret = 0;
	int fd = *(int*)arg;
	std::string res="{\"state\":\"ok\"}\r\n";
	while(true){
		char msg[MAX_BUFFER_SIZE] = {0};
		ret = try_receive_data(fd, msg, MAX_BUFFER_SIZE);
		if(ret > 0){
			LOGI("receive {}",msg);
			try_send_data(fd, res.c_str(), res.length());
		}
	}	
	return NULL;
}

int main(int argc, char** argv)
{
	int fd = 0;
	int ret = 0;
	pthread_t send_thread;
	pthread_t receive_thread;
	if(access(DEVICE, F_OK) == -1)
	{
		LOGE("Device {} does not exist\n", DEVICE);
		return FAILURE;
	}

	if((access(DEVICE, R_OK | W_OK)) == -1)
	{
		LOGE("Device {} is not readable/writable\n", DEVICE);
		return FAILURE;
	}else{
		LOGI("Device {} is readable/writable\n", DEVICE);
	}
	/**
	 * Open device
	 * O_RDWR:可读可写
	 * O_NOCTTY:忽略控制终端,若未设置此标志，终端信号（如Ctrl+C）可能中断进程
	 * O_NDELAY:启用非阻塞I/O模式
	 */
	fd = open(DEVICE, O_RDWR | O_NOCTTY | O_NDELAY);
	if(fd<0){
		LOGE("Error opening device {} error code {}\n", DEVICE,fd);
		return fd;
	}
	//设置串口属性
	ret = set_serial_attr(fd,B9600,DATABITS_8,STOPBITS_1,'N',10,0);
	if(ret < 0){
		LOGE("set_serial_attr failed");
		close(fd);
		return FAILURE;
	}
#if defined(SEND_MODE)
	//发送数据
	ret = pthread_create(&send_thread, NULL, send_data, &fd);
	if(ret != 0){
		LOGE("pthread_create failed");
		close(fd);
		return FAILURE;
	}
#else
	// receive_thread 传递有效参数
	// Message* recv_msg = new Message();
	// recv_msg->fd = fd;
	// recv_msg->len = MAX_BUFFER_SIZE;
	ret = pthread_create(&receive_thread, NULL, receive_data, &fd);
	if(ret != 0){
		LOGE("pthread_create failed");
		close(fd);
		return FAILURE;
	}

#endif
#if defined(SEND_MODE)
	pthread_join(send_thread, NULL);
#else
	pthread_join(receive_thread, NULL);
#endif
	close(fd);
	return 0;
}


