#include "ds18b20.h"

#define DS18B20_DEVICE_PATH "/sys/bus/w1/devices/28-000000579aa1/w1_slave" // 需要修改为实际设备ID

// 读取温度数据的函数
float read_temperature() {
    FILE *fp;
    char buf[256];
    float temp = -1;

    // 打开设备文件
    fp = fopen(DS18B20_DEVICE_PATH, "r");
    if (fp == NULL) {
        perror("Failed to open DS18B20 device");
        return -1;
    }

    // 读取文件内容
    if (fread(buf, sizeof(char), sizeof(buf) - 1, fp) > 0) {
        buf[sizeof(buf) - 1] = '\0'; // 确保字符串终止
        char *ptr = strstr(buf, "t="); // 查找温度数据
        if (ptr) {
            temp = atof(ptr + 2) / 1000.0; // 转换为摄氏度
        }
    }

    fclose(fp);
    return temp;
}