// socket_info_updater.cpp
#include "socket_info_updater.h"
#include <cstdio>
#include <iostream>
#include <sys/socket.h>

SocketInfoUpdater::SocketInfoUpdater(int s) : sock(s) {}

void SocketInfoUpdater::update() {
    char buf[128] = {0};
    std::sprintf(buf, "{\"tur\":\"%.2f\", \"tmp\":\"%.2f\", \"pH\":\"%.2f\"}", 
                 WaterQuality::getInstance().getTurbidity(), 
                 WaterQuality::getInstance().getDS18B20(), 
                 WaterQuality::getInstance().getpH());
    std::cout << buf << std::endl;

    send(sock, buf, strlen(buf), 0);
}