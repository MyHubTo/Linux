在Linux/Unix网络编程中，使用read()和write()进行Socket数据传输是基础操作，但需结合协议设计、缓冲管理和错误处理才能实现可靠通信。以下是关键步骤和代码示例：
📦 ​一、基础概念​
​**write()函数**​
​作用​：将数据从应用层缓冲区写入内核发送缓冲区
。
​原型​：ssize_t write(int fd, const void *buf, size_t count);
​返回值​：
成功：实际写入的字节数（可能小于count，需循环处理）。
失败：-1（需检查errno，如EINTR中断需重试）
。
​**read()函数**​
​作用​：从内核接收缓冲区读取数据到应用层缓冲区
。
​原型​：ssize_t read(int fd, void *buf, size_t count);
​返回值​：
>0：实际读取的字节数。
0：连接关闭（EOF）。
-1：错误（如EINTR需重试）
。
⚙️ ​二、数据发送：可靠写入完整数据​
由于write()可能部分写入，需循环发送确保数据完整：

```c
// 自定义安全写入函数
int safe_write(int fd, const void *buf, size_t len) {
    size_t bytes_left = len;
    const char *ptr = (const char *)buf;
    while (bytes_left > 0) {
        ssize_t written = write(fd, ptr, bytes_left);
        if (written <= 0) {
            if (errno == EINTR) continue; // 中断重试
            return -1; // 其他错误（如EPIPE连接断开）
        }
        bytes_left -= written;
        ptr += written; // 移动指针继续写剩余数据
    }
    return len; // 全部写入成功
}
```
📥 ​三、数据接收：处理粘包与半包​
TCP是流式协议，需解决数据边界问题​（如多次write()可能被一次read()合并）：

​方案1：定长协议​
```c
// 接收固定长度数据
int safe_read_fixed(int fd, void *buf, size_t len) {
    size_t bytes_left = len;
    char *ptr = (char *)buf;
    while (bytes_left > 0) {
        ssize_t read_bytes = read(fd, ptr, bytes_left);
        if (read_bytes == 0) return 0; // EOF
        if (read_bytes < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        bytes_left -= read_bytes;
        ptr += read_bytes;
    }
    return len;
}
```
​方案2：长度前缀协议（推荐）​​
​发送端​：先发4字节长度头，再发实际数据：
```c
uint32_t net_len = htonl(data_len); // 转网络字节序
safe_write(fd, &net_len, 4);        // 写长度头
safe_write(fd, data_buf, data_len); // 写实际数据
```
​接收端​：先读长度头，再按长度读数据体：
```c
uint32_t net_len;
if (safe_read_fixed(fd, &net_len, 4) <= 0) return -1;
uint32_t data_len = ntohl(net_len); // 转主机字节序

char *data_buf = malloc(data_len);
safe_read_fixed(fd, data_buf, data_len); // 读取数据体
```
​方案3：分隔符协议（如\n）​​
适用于文本协议（如HTTP）：

```c
// 读取直到遇到分隔符
ssize_t read_until(int fd, char *buf, char delim) {
    size_t total = 0;
    while (1) {
        ssize_t n = read(fd, buf + total, 1);
        if (n <= 0) return n;
        if (buf[total] == delim) break;
        total++;
    }
    buf[total] = '\0'; // 替换分隔符为字符串结束符
    return total;
}
```
🔍 ​四、数据解析：字节流到结构化数据​
接收到的数据是字节流，需反序列化为应用层对象：

​1. 手动解析（简单结构）​​
```c
// 假设协议: [4字节int][变长字符串]
uint32_t id;
char name[100];

safe_read_fixed(fd, &id, 4);
id = ntohl(id); // 字节序转换

uint32_t name_len;
safe_read_fixed(fd, &name_len, 4);
name_len = ntohl(name_len);
safe_read_fixed(fd, name, name_len);
name[name_len] = '\0'; // 字符串终止
```
​2. 序列化框架（复杂数据）​​
​JSON​：{"id":123, "name":"Alice"} → 用cJSON解析。
​Protobuf​：跨语言高效二进制协议（需预定义.proto）