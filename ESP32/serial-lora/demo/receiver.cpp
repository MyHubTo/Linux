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
#include <memory>
#include <system_error>
#include <fmt/core.h>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fmt/chrono.h>
#include <chrono>
#include <iomanip>
#include <mutex>

// 串口设备相关
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <pthread.h>

constexpr const char* DEVICE = "/dev/ttyUSB1";
constexpr bool IS_SENDER = false;           // 默认作为发送方
constexpr int SEND_DELAY_SECONDS = 3;       // 发送延迟秒数
constexpr int MAX_BUFFER_SIZE = 512;        // 最大缓冲区大小
constexpr int RESPONSE_TIMEOUT_MS = 2000;   // 响应超时毫秒数

// 线程安全的时间格式化函数（毫秒精度）
inline std::string get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                now.time_since_epoch()) % 1000;
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm = *std::localtime(&t);

    static std::mutex ts_mutex;
    std::lock_guard<std::mutex> lock(ts_mutex);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
       << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

// 带时间戳的日志宏（线程安全）
#define LOGD(...) fmt::print(fg(fmt::color::cornflower_blue), "[{}] [DEBUG] ", get_timestamp()), fmt::print(__VA_ARGS__), fmt::print("\n")
#define LOGI(...) fmt::print(fg(fmt::color::lime_green), "[{}] [INFO] ", get_timestamp()), fmt::print(__VA_ARGS__), fmt::print("\n")
#define LOGW(...) fmt::print(fg(fmt::color::gold) | fmt::emphasis::bold, "[{}] [WARN] ", get_timestamp()), fmt::print(__VA_ARGS__), fmt::print("\n")
#define LOGE(...) fmt::print(fg(fmt::color::red) | fmt::emphasis::bold, "[{}] [ERROR] ", get_timestamp()), fmt::print(__VA_ARGS__), fmt::print("\n")

enum class Result {
    SUCCESS,
    FAILURE
};  // 操作结果

// 停止位配置
enum class StopBits {
    ONE = 0,          // 1位停止位
    ONE_AND_HALF,     // 1.5位停止位
    TWO               // 2位停止位
};

// 数据位配置
enum class DataBits {
    FIVE = 5,         // 5数据位
    SIX,              // 6数据位
    SEVEN,            // 7数据位
    EIGHT             // 8数据位
};

// 校验位配置
enum class Parity {
    NONE,             // 无校验
    EVEN,             // 偶校验
    ODD,              // 奇校验
    SPACE,            // 空格校验(恒0)
    MARK              // 标记校验(恒1)
};

struct Message{
	int fd;
	int len;
	char data[MAX_BUFFER_SIZE];
};


/**
 * 串口配置
 * VTIME：等待超时时间
 * VMIN：最小读取字符数，只有设置为阻塞时这两个参数才有效，仅针对于读操作
*/
struct SerialConfig {
    int baudrate = B9600;                   // 波特率，默认9600
    DataBits data_bits = DataBits::EIGHT;   // 数据位，默认8位
    StopBits stop_bits = StopBits::ONE;     // 停止位，默认1位
    Parity parity = Parity::NONE;           // 校验位，默认无校验
    int vtime = 10;                         // 超时时间(十分之一秒)
    int vmin = 0;                           // 最小读取字符数
};


// 作用域锁(RAII实现)
class ScopedLock {
public:
    explicit ScopedLock(pthread_mutex_t& mutex) : mutex_(mutex) {
        pthread_mutex_lock(&mutex_);
    }
    ~ScopedLock() {
        pthread_mutex_unlock(&mutex_);
    }
private:
    pthread_mutex_t& mutex_;
};

// 全局互斥锁，用于线程安全的串口访问
static pthread_mutex_t serial_mutex = PTHREAD_MUTEX_INITIALIZER;

// 设置串口波特率
Result set_serial_baudrate(termios& options, int baudrate) {
    cfsetispeed(&options, baudrate);
    cfsetospeed(&options, baudrate);
    return Result::SUCCESS;
}

/**
 * 设置停止位
 * 若停止位为1，对应数据位为7/8。
 *! 若停止位为1.5，对应数据位为5。
 * 若停止位为2，对应数据位为6/7/8。
 */
Result set_serial_stopbits(termios& options, StopBits stopbits, DataBits databits) {
    switch (stopbits) {
        case StopBits::ONE:
            options.c_cflag &= ~CSTOPB;
            break;
        case StopBits::ONE_AND_HALF:
            if (databits != DataBits::FIVE) {
                LOGE("1.5 stop bits only valid with 5 data bits\n");
                return Result::FAILURE;
            }
            options.c_cflag &= ~CSTOPB;
            break;
        case StopBits::TWO:
            if (databits == DataBits::FIVE) {
                LOGE("2 stop bits not valid with 5 data bits\n");
                return Result::FAILURE;
            }
            options.c_cflag |= CSTOPB;
            break;
        default:
            LOGE("invalid stop bits configuration\n");
            return Result::FAILURE;
    }
    return Result::SUCCESS;
}

// 设置数据位并进行验证
Result set_serial_data_bit(termios& options, DataBits databits) {
    options.c_cflag &= ~CSIZE;
    switch (databits) {
        case DataBits::FIVE:
            options.c_cflag |= CS5;
            break;
        case DataBits::SIX:
            options.c_cflag |= CS6;
            break;
        case DataBits::SEVEN:
            options.c_cflag |= CS7;
            break;
        case DataBits::EIGHT:
            options.c_cflag |= CS8;
            break;
        default:
            LOGE("invalid data bits configuration\n");
            return Result::FAILURE;
    }
    return Result::SUCCESS;
}

// 配置校验位
Result set_serial_parity(termios& options, Parity parity) {
    switch (parity) {
        case Parity::NONE:  // 无校验
            options.c_cflag &= ~PARENB;
            break;
        case Parity::EVEN:  // 偶校验
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            break;
        case Parity::ODD:   // 奇校验
            options.c_cflag |= PARENB;
            options.c_cflag |= PARODD;
            break;
        case Parity::SPACE: // 空格校验
        case Parity::MARK:   // 标记校验
            LOGE("space/mark parity not supported in this implementation\n");
            return Result::FAILURE;
        default:
            LOGE("invalid parity configuration\n");
            return Result::FAILURE;
    }
    return Result::SUCCESS;
}

// 配置串口所有参数
Result configure_serial_port(int fd, const SerialConfig& config) {
    termios options;
    if (tcgetattr(fd, &options) != 0) {
        LOGE("tcgetattr failed: {}\n", strerror(errno));
        return Result::FAILURE;
    }

    // 应用配置
    if (set_serial_baudrate(options, config.baudrate) != Result::SUCCESS ||
        set_serial_data_bit(options, config.data_bits) != Result::SUCCESS ||
        set_serial_stopbits(options, config.stop_bits, config.data_bits) != Result::SUCCESS ||
        set_serial_parity(options, config.parity) != Result::SUCCESS) {
        return Result::FAILURE;
    }

    // 额外设置
    options.c_cflag |= CLOCAL | CREAD;                  // 启用接收器
    options.c_cflag &= ~CRTSCTS;                        // 禁用硬件流控
    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // 原始输入模式
    options.c_oflag &= ~OPOST;                          // 原始输出模式
    options.c_cc[VTIME] = config.vtime;                 // 超时设置
    options.c_cc[VMIN] = config.vmin;                   // 最小读取字符数

    // 清除现有数据
    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        LOGE("tcsetattr failed: {}\n", strerror(errno));
        return Result::FAILURE;
    }

    LOGI("serial port configured successfully\n");
    return Result::SUCCESS;
}

// 带超时的线程安全写入
ssize_t safe_write(int fd, const char* data, size_t size) {
    ScopedLock lock(serial_mutex);
    return write(fd, data, size);
}

// 带超时的线程安全读取
ssize_t safe_read(int fd, char* buffer, size_t size, int timeout_ms = RESPONSE_TIMEOUT_MS) {
    ScopedLock lock(serial_mutex);

    fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    timeval timeout = {
        .tv_sec = timeout_ms / 1000,
        .tv_usec = (timeout_ms % 1000) * 1000
    };

    // 带超时等待数据
    int ready = select(fd + 1, &read_fds, nullptr, nullptr, &timeout);
    if (ready == -1) {
        LOGE("select error: {}\n", strerror(errno));
        return -1;
    }
    if (ready == 0) {
        LOGW("read timeout\n");
        return 0;
    }

    return read(fd, buffer, size);
}

// 发送数据的线程函数
void* send_handler(void* arg) {
    int fd = *static_cast<int*>(arg);
    int count = 0;

    while (true) {
        std::string message = fmt::format("hello world {}\r\n", count++);

        ssize_t bytes_written = safe_write(fd, message.c_str(), message.size());
        if (bytes_written < 0) {
            LOGE("write error: {}\n", strerror(errno));
        } else if (bytes_written == static_cast<ssize_t>(message.size())) {
            LOGI("sent: {}", message);

            // 等待响应
            char response[MAX_BUFFER_SIZE] = {0};
            ssize_t bytes_read = safe_read(fd, response, sizeof(response) - 1);

            if (bytes_read > 0) {
                LOGI("response: {}\n", response);
            } else if (bytes_read == 0) {
                LOGW("no response received\n");
            }
        } else {
            LOGW("partial write: {}/{} bytes\n", bytes_written, message.size());
        }
        sleep(SEND_DELAY_SECONDS); // 等待下次发送
    }
    return nullptr;
}

// 接收数据的线程函数
void* receive_handler(void* arg) {
    int fd = *static_cast<int*>(arg);
    char buffer[MAX_BUFFER_SIZE];
    while (true) {
        ssize_t bytes_read = safe_read(fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';  // 确保字符串终止
            LOGI("received: {}\n", buffer);
            // 发送确认
            const char* ack = "{\"status\":\"ACK\"}\r\n";
            safe_write(fd, ack, strlen(ack));
        } else if (bytes_read < 0) {
            break;  // 发生错误时终止
        }
    }
    return nullptr;
}

int main(int argc, char** argv) {
    // 检查设备可访问性
    if (access(DEVICE, F_OK) == -1) {
        LOGE("device {} does not exist\n", DEVICE);
        return EXIT_FAILURE;
    }

    if (access(DEVICE, R_OK | W_OK) == -1) {
        LOGE("device {} is not readable/writable\n", DEVICE);
        return EXIT_FAILURE;
    }

    LOGI("device {} is accessible\n", DEVICE);

    // 打开串口
    int fd = open(DEVICE, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        LOGE("error opening {}: {}\n", DEVICE, strerror(errno));
        return EXIT_FAILURE;
    }
    // 配置串口
    SerialConfig config;
    if (configure_serial_port(fd, config) != Result::SUCCESS) {
        close(fd);
        return EXIT_FAILURE;
    }

    // 启动相应线程
    pthread_t thread;

    int result = pthread_create(
        &thread,
        nullptr,
        IS_SENDER ? send_handler : receive_handler,
        &fd
    );

    if (result != 0) {
        LOGE("thread creation failed: {}\n", strerror(result));
        close(fd);
        return EXIT_FAILURE;
    }

    // 等待线程结束（实际上永不结束）
    pthread_join(thread, nullptr);

    close(fd);
    return EXIT_SUCCESS;
}