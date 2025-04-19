#include "com.h"
#include <locale.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

// 使用命名空间
using namespace std;

// 信号处理函数
void sigint_handler(int signum) {
    cout << "Capture Ctrl+C interrupt signal (SIGINT), program is about to exit." << endl;
    _exit(0);
}

#define BUFFER_SIZE 20

// 水质参数结构体
WaterQuality g_water_quality;

// 调试信息线程函数
void* debug_info(void* arg) {
    while (1) {
        cout << "debug info" << endl;
        cout << "AIN0 value -> turbidity: " << g_water_quality.turbidity << endl;
        cout << "DS18B20 value -> temperature: " << g_water_quality.ds18b20 << "℃" << endl;
        cout << "pH value -> pH: " << g_water_quality.pH << endl;
        sleep(1);
    }
    return nullptr;
}

// TFT 信息线程函数
void* tft_info(void* arg) {
    TFTFreetype tft;
    wchar_t turb[BUFFER_SIZE] = {0}; // Turbidity value
    while (1) {
        memset(turb, ' ', BUFFER_SIZE);
        swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"turbidity: %.2f", g_water_quality.turbidity);
        tft.drawString(5, 20, turb,  0xFFFF);
        swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"temp: %.2f℃", g_water_quality.ds18b20);
        tft.drawString(5, 50, turb,  0xFFFF);
        swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"pH: %.2f", g_water_quality.pH);
        tft.drawString(5, 80, turb,  0xFFFF);
        sleep(1);
        tft.fillScreen(0x0000);
    }
    return nullptr;
}

// 套接字信息线程函数
void* sock_info(void *arg) {
    return nullptr;
}

int main() {
    int sock;
    char serv_ip[] = "192.168.137.109";
    int port = 8888;
    serv_connect(&sock, serv_ip, port);
    cout << "Connection successful: " << serv_ip << ":" << port << endl;

    // 初始化PCF8591
    PCF8591 pcf8591;
    cout << "PCF8591 init success!" << endl;

    // 初始化水质参数
    memset(&g_water_quality, 0, sizeof(g_water_quality));

    // 信号处理设置
    struct sigaction sa;
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    cout << "The program is running, press Ctrl+C to exit" << endl;

    char buf[128] = {0};

    // 创建线程
    pthread_t debug_id, tft_id, sock_id;
    Thread th;
    if (th.pthread_init(&debug_id, debug_info) != 0) {
        return -1;
    }
    if (th.pthread_init(&tft_id, tft_info) != 0) {
        return -1;
    }
    if (th.pthread_init(&sock_id, sock_info) != 0) {
        return -1;
    }
    DS18B20 ds18b20;

    while (1) {
        g_water_quality.turbidity = 100 - pcf8591.read(PCF8591_AIN0) * 100.0 / 255;
        g_water_quality.ds18b20 = ds18b20.readTemperature();
        g_water_quality.pH = pcf8591.read(PCF8591_AIN1) / 255.0 * 14.0;

        sprintf(buf, "{\"tur\":\"%.2f\", \"tmp\":\"%.2f\", \"pH\":\"%.2f\"}", g_water_quality.turbidity, g_water_quality.ds18b20, g_water_quality.pH);
        cout << buf << endl;

        send(sock, buf, strlen(buf), 0);

        sleep(1);
    }

    // 清理资源
    // pcf8591_clear();
    // close(sock);

    return 0;
}