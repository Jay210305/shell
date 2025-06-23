#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include "threading.h"

static pthread_t monitor_thread;
static int running = 0;

void *monitor_func(void *arg)
{
    while (running)
    {
        // Placeholder: monitoring logic
        sleep(1);
    }
    return NULL;
}

void start_monitor_thread()
{
    running = 1;
    if (pthread_create(&monitor_thread, NULL, monitor_func, NULL))
    {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
}

void stop_monitor_thread()
{
    running = 0;
    pthread_join(monitor_thread, NULL);
}