#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "../include/threadpool.h"
#include "../include/http.h"

// Internal Queue Structure
typedef struct {
    job_t *jobs;
    int size;
    int count;
    int head;
    int tail;
    pthread_mutex_t lock;
    pthread_cond_t notify;
    int shutdown;
} job_queue_t;

// Static global variable (only visible in this file)
static job_queue_t queue;
static pthread_t thread_pool[THREAD_POOL_SIZE];

void init_thread_pool() {
    // Initialize Queue
    queue.jobs = (job_t *)malloc(sizeof(job_t) * QUEUE_SIZE);
    queue.size = QUEUE_SIZE;
    queue.count = 0;
    queue.head = 0;
    queue.tail = 0;
    queue.shutdown = 0;
    pthread_mutex_init(&queue.lock, NULL);
    pthread_cond_init(&queue.notify, NULL);

    printf("[INIT] Starting Thread Pool with %d workers...\n", THREAD_POOL_SIZE);
    
    // Create Threads
    for (int i = 0; i < THREAD_POOL_SIZE; i++) {
        if (pthread_create(&thread_pool[i], NULL, worker_thread, NULL) != 0) {
            perror("Failed to create thread");
            exit(1);
        }
    }
}

void push_job(int client_socket) {
    pthread_mutex_lock(&queue.lock);
    
    if (queue.count < queue.size) {
        queue.jobs[queue.tail].client_socket = client_socket;
        queue.tail = (queue.tail + 1) % queue.size;
        queue.count++;
        pthread_cond_signal(&queue.notify);
    } else {
        printf("[WARN] Server overloaded, dropping connection\n");
        close(client_socket);
    }
    
    pthread_mutex_unlock(&queue.lock);
}

job_t pop_job() {
    job_t job = {-1};
    
    pthread_mutex_lock(&queue.lock);
    
    while (queue.count == 0 && !queue.shutdown) {
        pthread_cond_wait(&queue.notify, &queue.lock);
    }
    
    if (queue.count > 0) {
        job = queue.jobs[queue.head];
        queue.head = (queue.head + 1) % queue.size;
        queue.count--;
    }
    
    pthread_mutex_unlock(&queue.lock);
    return job;
}

void *worker_thread(void *arg) {
    while (1) {
        job_t job = pop_job();
        if (job.client_socket != -1) {
            // Forward processing to HTTP module
            handle_client(job.client_socket);
        }
    }
    return NULL;
}