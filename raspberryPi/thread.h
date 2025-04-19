#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// int pthread_init(pthread_t *id, void *(*fun)(void* arg));


class Thread {
    public:
        static int init(pthread_t *id, void *(*fun)(void* arg));
    };
#endif 