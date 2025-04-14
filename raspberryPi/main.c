#include "com.h"
#include <locale.h>
#include <signal.h>
#include <unistd.h>

// signal processing function
void sigint_handler(int signum) {
    printf("Capture Ctrl+C interrupt signal (SIGINT), program is about to exit.\n");

    fill_screen(0x0000);
    tft_clear();
    pcf8591_clear();
    // You can add code to clean up resources here
    _exit(0);
}

#define BUFFER_SIZE 20

WaterQuality g_water_quality;

void *debug_info(void* arg)
{
    while (1)
    {
        printf("debug info\n");
        printf("AIN0 value -> turbidity: %.2f\n", g_water_quality.turbidity);
        printf("DS18B20 value -> temperature: %.2f℃\n", g_water_quality.ds18b20);
        printf("pH value -> pH: %.2f\n", g_water_quality.pH);
        sleep(1);
    }
}

void *tft_info(void* arg)
{
    wchar_t turb[BUFFER_SIZE] = {0}; // Turbidity value
    while (1)
    {
        // draw_string(70, 20, L"                            ",  0x0000);
        memset(&turb, ' ', BUFFER_SIZE);
        // draw_string(70, 20, turb,  0xFFFF);
        swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"turbidity: %.2f", g_water_quality.turbidity);
        draw_string(5, 20, turb,  0xFFFF);
        swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"temp: %.2f℃", g_water_quality.ds18b20);
        draw_string(5, 50, turb,  0xFFFF);
        swprintf(turb, sizeof(turb) / sizeof(wchar_t), L"pH: %.2f", g_water_quality.pH);
        draw_string(5, 80, turb,  0xFFFF);
        sleep(1);
        fill_screen(0x0000);
    }
    
    tft_clear();
}

void *sock_info(void *arg)
{

}

int main()
{
    int sock;
    
    char serv_ip[] = "192.168.137.109";
    int port = 8888;
    serv_connect(&sock, serv_ip, port);
    printf("Connection successful: %s:%d", serv_ip, port);

    tft_init();
    printf("TFT init success!\n");
    pcf8591_init();
    printf("PCF8591 init success!\n");
    memset(&g_water_quality, 0, sizeof(g_water_quality));


    struct sigaction sa;
    // Initialize signal processing function
    sa.sa_handler = sigint_handler;
    // Clear signal mask
    sigemptyset(&sa.sa_mask);
    // Do not set special signs
    sa.sa_flags = 0;

    // Register signal processing function
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("The program is running, press Ctrl+C to exit\n");


    float value = 0;
    char buf[128] = {0};

    draw_string(5, 20, L"turbidity:",  0xFFFF);
    draw_string(5, 50, L"temp:", 0xFFFF);
    pthread_t debug_id, tft_id, sock_id;
    pthread_init(&debug_id, debug_info);
    pthread_init(&tft_id, tft_info);
    pthread_init(&sock_id, sock_info);

    while(1)
    {

        g_water_quality.turbidity = 100 - pcf8591_read(PCF8591_AIN0) * 100.0 / 255;
        g_water_quality.ds18b20 = read_temperature();
        g_water_quality.pH = pcf8591_read(PCF8591_AIN1) / 255.0 * 14.0;
        // wprintf(L"turb value: %ls", turb);

        sprintf(buf, "{\"tur\":\"%.2f\", \"tmp\":\"%.2f\", \"pH\":\"%.2f\"}", g_water_quality.turbidity, g_water_quality.ds18b20, g_water_quality.pH);
        printf("%s\n", buf);
        
        send(sock, buf, strlen(buf), 0);

        sleep(1);
    }




    tft_clear();
    pcf8591_clear();
}
