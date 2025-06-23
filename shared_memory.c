#include "shared_memory.h"
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>

static int shmid;
static void *shm_ptr;

void init_shared_memory()
{
    key_t key = ftok("/tmp", 'S');
    shmid = shmget(key, 1024, IPC_CREAT | 0666);
    if (shmid < 0)
    {
        perror("shmget");
        return;
    }
    shm_ptr = shmat(shmid, NULL, 0);
    if (shm_ptr == (void *)-1)
        perror("shmat");
}

void destroy_shared_memory()
{
    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);
}