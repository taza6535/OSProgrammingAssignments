#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include<unistd.h>

#include "queue.h"
#include "multi-lookup.h"

int queue_init(queue*q, int ms){
    q->count=0;
    q->front=0;
    q->back=0;
    q->maxSize = ms;
    q->queue = malloc(q->maxSize * sizeof(char*));
    for(int i=0; i<q->maxSize; i++){
        q->queue[i] = (char*)malloc(MAX_NAME_LENGTH*sizeof(char));
    }
    //initializing access mutex
    pthread_mutex_init(&q->access,0);
    sem_init(&q->empty, 0, ms);
    sem_init(&q->full, 0,0);
    return 0;
}

int isFull(queue*q){
    return (q->count == q->maxSize);
}

int isEmpty(queue*q){
    return (q->count==0);
}

//producers (requesters) use this to add hostnames to the queue for. 
int enqueue(queue*q, char * host){
    //while(isFull(q));
    sem_wait(&q->empty);
    if(strlen(host)>MAX_NAME_LENGTH){
        fprintf(stderr, "This hostname is too long. Could not add");
    }
    pthread_mutex_lock(&q->access);
    strcpy(q->queue[q->back], host);
    //printf("the item just got added at index: %d\n", q->back);
    q->back = (q->back+1) % q->maxSize;
    //q->queue[q->back] = host;
    q->count++;
    pthread_mutex_unlock(&q->access);
    sem_post(&q->full);
    return 0;
}

char * dequeue(queue*q){
    //while(isEmpty(q));
    sem_wait(&q->full);
    pthread_mutex_lock(&q->access);
    char * host = (char *) malloc(MAX_NAME_LENGTH * sizeof(char*));
    //char host[MAX_NAME_LENGTH];
    strcpy(host, q->queue[q->front]);
    //printf("Dequeueing: %s\n", q->queue[q->front]);
    q->front = (q->front +1) % q->maxSize;
    q->count--;
    pthread_mutex_unlock(&q->access);
    sem_post(&q->empty);
    return host;

}

int freeQueue(queue *q){
    for(int i=0; i<q->maxSize; i++){
        free(q->queue[i]);
    }
    pthread_mutex_destroy(&q->access);
    sem_destroy(&q->empty);
    sem_destroy(&q->full);
    //free(q->queue);
    free(q->queue);
    free(q);
    return 1;
}

