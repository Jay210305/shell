#ifndef SYNC_H
#define SYNC_H

#include <pthread.h>
#include <semaphore.h>

void init_sync();
void destroy_sync();

extern pthread_mutex_t vfs_mutex;
extern sem_t vfs_sem;

#endif