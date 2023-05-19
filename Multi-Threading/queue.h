#ifndef QUEUE_H
#define QUEUE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include<unistd.h>

#define ARRAY_SIZE 8
#define MAX_INPUT_FILES 100

typedef struct {
    char ** queue;
    int front; //front element of queue
    int back; //last element of queue
    int count; //number of elements in the queue
    int maxSize; //max size of array 
    pthread_mutex_t access;
    sem_t full;
    sem_t empty;
} queue;

int queue_init(queue * q, int maxSize);
int enqueue(queue*q, char * host);
char * dequeue(queue*q);
int isFull(queue*q);
int isEmpty(queue*q);
int freeQueue(queue * q);

#endif
