#include "thread.h"

int pthread_init(pthread_t *id, void *(*fun)(void* arg))
{
    if (pthread_create(id, NULL, fun, NULL))
    {
        perror("create thread failed\n");
        return -1;
    }

    // thread detach when created
    if (pthread_detach(*id) != 0) {
        perror("pthread_detach 失败");
        return -1;
    }
    return 0;
}
