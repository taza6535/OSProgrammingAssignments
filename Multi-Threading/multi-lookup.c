#include<stdio.h> 
#include<string.h>
#include<pthread.h>
#include <stdlib.h>
#include<semaphore.h>
#include<time.h>
#include "multi-lookup.h"
#include "queue.h"

int main(int argc, char * argv[]){
    //start time for program
    clock_t start, end;
    double timeTaken;
    start = clock();
    if(argc<5){
        printf("Wrong number of arguments.\n");
        printf("Usage:./multi-lookup <# requesters> <# resolvers> <requester log> <resolver log> [<data files> ...]\n");
        exit(-1);
    }
    if(argc-5 > MAX_INPUT_FILES){
        fprintf(stderr, "Number of input files is larger than maximum of %d\n", MAX_INPUT_FILES);
        return -1;
    }
    for(int i=5; i<argc; i++){
        if(access(argv[i], F_OK)!=0){
            fprintf(stderr, "invalid file %s", argv[i]);
            return -1;
        }
    }
    queue * files = (queue *)malloc(sizeof(queue));
    queue_init(files, MAX_INPUT_FILES);
    for(int i=5; i<(argc); i++){
        char * fw = strndup(argv[i], strlen(argv[i]));
        enqueue(files, fw);
        free(fw);
    }
    int requesterNum = atoi(argv[1]);
    int resolverNum = atoi(argv[2]);
    if(requesterNum > MAX_REQUESTER_THREADS){
        printf("Number of requester threads is greater than the maximum of %d\n", MAX_REQUESTER_THREADS);
        return -1;
    }
    if(resolverNum > MAX_RESOVLER_THREADS){
        printf("Number of resolver threads is greater than the maximum of %d\n", MAX_RESOVLER_THREADS);
        return -1;
    }
    //printf("adding things\n");
    pthread_t requesters[requesterNum];
    pthread_t resolvers[resolverNum];
    queue * buffer = (queue *)malloc(sizeof(queue));
    queue_init(buffer, ARRAY_SIZE);
    requesterThings reqThings;
    resolverThings resThings;
    reqThings.files = files;
    reqThings.requesterLog = fopen(argv[3],"w");
    reqThings.sharedBuffer = buffer;
    pthread_mutex_init(&reqThings.filesAccess,0);
    pthread_mutex_init(&reqThings.requesterLogAccess,0);
    resThings.resolverLog = fopen(argv[4],"w");
    resThings.sharedBuffer = buffer;
    pthread_mutex_init(&resThings.resolverLogAccess,0);
    for(int i=0; i<requesterNum; i++){
        pthread_create(&requesters[i], NULL, requesterRoutine, &reqThings);
    }
    for(int i=0; i<resolverNum; i++){
        pthread_create(&resolvers[i], NULL, resolverRoutine, &resThings);
    }
    
    for(int i=0; i<requesterNum; i++){
        pthread_join(requesters[i], NULL);
    }

   
    for(int i=0; i<resolverNum; i++){
        enqueue(reqThings.sharedBuffer, "POISON PILL");
    }


    for(int i=0; i<resolverNum; i++){
        pthread_join(resolvers[i], NULL);
    }

    fclose(reqThings.requesterLog);
    fclose(resThings.resolverLog);
    freeQueue(buffer);
    freeQueue(files);
    pthread_mutex_destroy(&reqThings.filesAccess);
    pthread_mutex_destroy(&reqThings.requesterLogAccess);
    pthread_mutex_destroy(&resThings.resolverLogAccess);
    end = clock();
    timeTaken = ((double) (end-start)) / CLOCKS_PER_SEC;
    printf("./multi-lookup: total time is %f seconds\n", timeTaken);
    return 0;
}

void * requesterRoutine(void * req){
    requesterThings * d = (requesterThings *) req;
    int threadID = pthread_self();
    int numFilesServiced=0;
    while(1){
        while(d->files->count>0){
        char * fileName = dequeue(d->files);
        FILE * file = fopen(fileName, "r");
        numFilesServiced++;
        pthread_mutex_unlock(&d->filesAccess);
        char * hostname = malloc(MAX_NAME_LENGTH * sizeof(char)); 
        while(!feof(file)){
            size_t hostSize = MAX_NAME_LENGTH;
            if(getline(&hostname, &hostSize, file)>=0){
            fprintf(d->requesterLog,"%s", hostname);
            char * e = strndup(hostname, strlen(hostname));
            enqueue(d->sharedBuffer, e);
            free(e);
            }
        }
        fclose(file);
        free(fileName);
        free(hostname);
    }
    break;
}
    pthread_mutex_lock(&d->requesterLogAccess);
    printf("Thread %d serviced %d files\n", threadID, numFilesServiced);
    pthread_mutex_unlock(&d->requesterLogAccess);
    return 0;
}

void * resolverRoutine(void * res){
    resolverThings* d = (resolverThings *) res;
    int threadId = pthread_self();
    int numFilesServiced = 0;
    char * ipAddr = (char*)malloc(MAX_IP_LENGTH+1 * sizeof(char));
    while(1){
        while(d->sharedBuffer->count == 0);
            char * hostName = dequeue(d->sharedBuffer);
            if(strcmp("POISON PILL", hostName)==0){
                printf("Thread %d resolved %d hostnames\n", threadId, numFilesServiced);
                free(hostName);
                free(ipAddr);
                break;
            }
            else{
            char * lookup = strndup(hostName, strlen(hostName)-1);
            int ret = dnslookup(lookup, ipAddr, MAX_IP_LENGTH);
            if(ret!=0){
                pthread_mutex_lock(&d->resolverLogAccess);
                char * h = strndup(hostName, strlen(hostName)-1);
                fprintf(d->resolverLog, "%s, NOT_RESOLVED\n", h);
                pthread_mutex_unlock(&d->resolverLogAccess);
                numFilesServiced++;
                free(h);
                free(lookup);
                free(hostName);
            }
            else if(ret==0){
                pthread_mutex_lock(&d->resolverLogAccess);
                char * hw = strndup(hostName, strlen(hostName)-1);
                fprintf(d->resolverLog, "%s, %s\n", hw, ipAddr);
                pthread_mutex_unlock(&d->resolverLogAccess);
                numFilesServiced++;
                free(hw);
                free(lookup);
                free(hostName);
            }
            }
    }
    return 0;
}
