#include "sync.h"
#include <stdio.h>

pthread_mutex_t vfs_mutex;
sem_t vfs_sem;

void init_sync()
{
    pthread_mutex_init(&vfs_mutex, NULL);
    sem_init(&vfs_sem, 0, 1);
}

void destroy_sync()
{
    pthread_mutex_destroy(&vfs_mutex);
    sem_destroy(&vfs_sem);
}