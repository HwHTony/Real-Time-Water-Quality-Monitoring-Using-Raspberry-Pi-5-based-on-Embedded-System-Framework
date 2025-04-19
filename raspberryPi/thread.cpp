#include "thread.h"
#include <iostream>

int Thread::init(pthread_t *id, void *(*fun)(void* arg)) {
    if (pthread_create(id, NULL, fun, NULL)) {
        std::cerr << "create thread failed" << std::endl;
        return -1;
    }
    // 立即分离线程
    if (pthread_detach(*id) != 0) {
        std::cerr << "pthread_detach 失败" << std::endl;
        return -1;
    }
    return 0;
}