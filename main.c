#include <stdio.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "cJSON.h"  // 包含cJSON头文件

// PCF8591的I2C地址
#define PCF8591_ADDRESS 0x48

// DS18B20的文件路径
#define DS18B20_PATH "/sys/bus/w1/devices/28-*/w1_slave"

// Qt服务器的IP和端口
#define SERVER_IP "192.168.1.100"  // 替换为Qt服务器的IP地址
#define SERVER_PORT 8888

// 读取PCF8591的模拟输入
int read_pcf8591(int fd, int channel) {
    wiringPiI2CWrite(fd, 0x40 | channel);  // 选择通道
    wiringPiI2CRead(fd);  // 丢弃第一次读取的值
    return wiringPiI2CRead(fd);
}

// 读取DS18B20温度传感器的数据
float read_ds18b20() {
    FILE *fp;
    char path[100];
    char buffer[256];
    float temperature = 0.0;

    // 查找DS18B20设备文件
    snprintf(path, sizeof(path), "/sys/bus/w1/devices/28-*/w1_slave");
    fp = popen("cat " DS18B20_PATH, "r");
    if (fp == NULL) {
        printf("Failed to read DS18B20 sensor.\n");
        return -1;
    }

    // 读取文件内容
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        if (strstr(buffer, "t=")) {
            sscanf(strstr(buffer, "t=") + 2, "%f", &temperature);
            temperature /= 1000;  // 转换为摄氏度
        }
    }

    pclose(fp);
    return temperature;
}

int main() {
    // 初始化WiringPi
    if (wiringPiSetup() == -1) {
        printf("WiringPi setup failed.\n");
        return 1;
    }

    // 初始化I2C
    int pcf8591_fd = wiringPiI2CSetup(PCF8591_ADDRESS);
    if (pcf8591_fd == -1) {
        printf("I2C setup failed.\n");
        return 1;
    }

    // 创建Socket
    int client_socket;
    struct sockaddr_in server_addr;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        printf("Socket creation failed.\n");
        return 1;
    }

    // 设置服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    // 连接到服务器
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        printf("Connection failed.\n");
        close(client_socket);
        return 1;
    }

    printf("Connected to Qt server.\n");

    while (1) {
        // 读取传感器数据
        int ph_value = read_pcf8591(pcf8591_fd, 0);
        float ph_voltage = ph_value * 3.3 / 255;  // 转换为电压值
        int turbidity_value = read_pcf8591(pcf8591_fd, 1);
        float turbidity_voltage = turbidity_value * 3.3 / 255;  // 转换为电压值
        float temperature = read_ds18b20();

        // 创建JSON对象
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, "pH", ph_voltage);
        cJSON_AddNumberToObject(root, "turbidity", turbidity_voltage);
        cJSON_AddNumberToObject(root, "temperature", temperature);

        // 将JSON对象转换为字符串
        char *json_str = cJSON_Print(root);
        printf("JSON Data: %s\n", json_str);

        // 发送JSON数据到服务器
        if (send(client_socket, json_str, strlen(json_str), 0) == -1) {
            printf("Failed to send data.\n");
            break;
        }

        // 释放JSON对象和字符串
        cJSON_Delete(root);
        free(json_str);

        // 间隔1秒
        sleep(1);
    }

    // 关闭Socket
    close(client_socket);
    return 0;
}
