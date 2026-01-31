#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>

// Configuration
#define THREAD_POOL_SIZE 20
#define QUEUE_SIZE 100

// Job structure
typedef struct {
    int client_socket;
} job_t;

// Function Prototypes
void init_thread_pool();
void push_job(int client_socket);
void *worker_thread(void *arg);

#endif