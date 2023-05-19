#ifndef MULTI_LOOKUP_HEADER
#define MULTI_LOOKUP_HEADER
#include<stdio.h> 
#include<string.h>
#include<pthread.h>
#include <stdlib.h>
#include "util.h"
#include "queue.h"

#define ARRAY_SIZE 8
#define MAX_INPUT_FILES 100
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOVLER_THREADS 10
#define MAX_NAME_LENGTH 1025
#define MAX_IP_LENGTH INET6_ADDRSTRLEN

typedef struct requesterThings {
    queue * sharedBuffer; 
    queue * files;
    FILE * requesterLog;
    pthread_mutex_t requesterLogAccess;
    pthread_mutex_t filesAccess;
} requesterThings;

typedef struct resolverThings {
    queue * sharedBuffer;
    FILE* resolverLog;
    pthread_mutex_t resolverLogAccess;
} resolverThings;

void * requesterRoutine(void * data);
void *  resolverRoutine(void * data);
#endif