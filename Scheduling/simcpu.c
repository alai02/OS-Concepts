#include <stdio.h>
#include <stdlib.h>
#include <sys/timeb.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

#define MAX_THREADS 50
#define MAX_BURST 10
#define MAX 10

void parseData(bool verbose, bool detailed);

typedef enum status{
    new,
    ready,
    running,
    blocking,
    terminated,
} status;

typedef struct Thread{
    int pID;
    int ID;
    int at; //ftime()
    int numBursts;
    int st;
    int tat;
    int ft;
    int ioTime;
    int tTime;
    int tSwitch;
    int totalServiceTime;
    int threadCount;
    int cpuBurst[MAX_BURST];
    int ioBurst[MAX_BURST];
} Thread;

typedef struct schedulerInfo{
    int numProcesses;
    int tSwitch;
    int pSwitch;
} schedulerInfo;

schedulerInfo* initSumulation(){
    schedulerInfo* fileInfo = malloc(sizeof(schedulerInfo));
    fileInfo->pSwitch = 0;
    fileInfo->numProcesses = 0;
    fileInfo->tSwitch = 0;
    return fileInfo;
}

int main(int argc, char* argv[]){

    bool verbose = false;
    bool detailed = false;
    int i = 0;
    struct timeb t_start, t_end;
    int t_diff;
    ftime(&t_start);
    while(argv[i]){
        if(strcmp(argv[i], "-v") == 0){
            verbose = true;
        } else if(strcmp(argv[i], "-d") == 0){
            detailed = true;
        } else if(strcmp(argv[i], "-r") == 0){
            //roundRobin = true;
        }
        i++;
    }
    parseData(verbose, detailed);
    //printf("here\n" );
    ftime(&t_end);
    t_diff = (int) (1000.0 * (t_end.time - t_start.time) + (t_end.millitm - t_start.millitm));
    printf("Total Time required is %d time units\n", t_diff);


    return 0;
}

void fcfs(Thread* threadQueue, schedulerInfo* thisSim, bool verbose, bool detailed){
   int i = 0, j = 0, k = 0;
   int count = 0;
   int tempArrival, tempID, tempPID;
   int tempCPU[MAX_BURST], tempIO[MAX_BURST];
   int totalTime = 0;
   
   for(i = 0; i < threadQueue->threadCount-1; i++){

       for(j = i+1; j < threadQueue->threadCount; j++){

           if(threadQueue[i].at > threadQueue[j].at){
               
               printf("Swapping: t%d of p%d with t%d of p%d\n", threadQueue[i].pID, threadQueue[i].ID, threadQueue[j].pID, threadQueue[j].ID);
               tempArrival = threadQueue[i].at;
               tempID = threadQueue[i].ID;
               tempPID = threadQueue[i].pID;

               for(k = 0; k < threadQueue[i].numBursts; k++){
                   tempCPU[k] = threadQueue[i].cpuBurst[k];
                   tempIO[k] = threadQueue[i].ioBurst[k];
               }

               threadQueue[i].ID = threadQueue[j].ID;
               threadQueue[i].at = threadQueue[j].at;
               threadQueue[i].pID = threadQueue[j].pID;
               
               for(k = 0; k < threadQueue[j].numBursts; k++){
                   threadQueue[i].cpuBurst[k]  = threadQueue[j].cpuBurst[k];
                   threadQueue[i].ioBurst[k] = threadQueue[j].ioBurst[k];
               }
               
               threadQueue[j].ID = tempID;
               threadQueue[j].at = tempArrival;
               threadQueue[j].pID = tempPID;

               for(k = 0; k < threadQueue[i].numBursts; k++){
                   threadQueue[j].cpuBurst[k]  = tempCPU[k];
                   threadQueue[j].ioBurst[k] = tempIO[k];
               }

           }
       }
   }


   for(i = 0; i < count; i++){
       threadQueue[i].tTime = totalTime;
   }
   

   if(verbose == true){

       int threadTime = 0;
       
       for(i = 0; i < threadQueue->threadCount; i++){
       
           if(threadQueue[i].at == i){
               printf("At time %d: Thread %d of Process %d moves from new to ready\n", i, threadQueue[i].ID, threadQueue[i].pID);
               threadQueue[i].st = i;
           }
           if(threadQueue[i].at > i){
               printf("At time %d: Thread %d of Process %d moves from ready to running\n", i, threadQueue[i].ID, threadQueue[i].pID);
           }

           for(j = 0; j < threadQueue[i].numBursts; j++){
               threadTime += threadQueue[i].cpuBurst[j];
               if(threadQueue[i].numBursts +1 != j){
                   threadTime += threadQueue[i].tSwitch;
                   threadTime += threadQueue[i].ioBurst[j];
               }
           }

           if(i > threadQueue[i].at){
               printf("At time %d: Thread %d of Process %d moves from running to terminated\n", i, threadQueue[i].ID, threadQueue[i].pID);
               threadQueue[i].ft = i;
               threadQueue[i].tat = i - threadQueue[i].at;
               threadTime = 0;
           }
       }
   }

   if(detailed == true){
       for(i = 0; i < threadQueue->threadCount; i++){
           printf("Thread %d of Process %d: \n", threadQueue[i].ID, threadQueue[i].pID);
           printf("\n");
           printf("arrival time: %d\n", threadQueue[i].at);
           printf("service time: %d units\n", threadQueue[i].st);
           printf("turnaround time: %d units\n", threadQueue[i].tat);
           printf("finish time %d units\n", threadQueue[i].ft);
           printf("\n");
       }
   }

   printf("Total Time required is %d time units\n", 0);
   printf("Average Turnaround Time is %d time units\n", 0);
   printf("CPU Utilization is %d%%\n", 0);
   free(threadQueue);
   exit(1);
   return;
}

void parseData(bool verbose, bool detailed){

    size_t len = 0;
    ssize_t read;

    int i = 0, lastI = 0, initSim = 0;
    char* line = NULL;
    char* token = NULL;
    int data[3];
    const char s[2] = " ";

    schedulerInfo* thisSim;
    thisSim = initSumulation();
    int bCount = 0;
    int ioCount = 0;
     Thread* threads = malloc(sizeof(Thread*)*MAX_THREADS*thisSim->numProcesses);
    int threadCount = 0;
    while ((read = getline(&line, &len, stdin)) != -1) {

        token = strtok(line, s);
        while(token != NULL){

            data[i++] = atoi(token);
            token = strtok(NULL, s);
        }

        initSim++;

        if(initSim == 1){
            thisSim->numProcesses = data[0];
            thisSim->tSwitch = data[1];
            thisSim->pSwitch = data[2];
        }
        
        if((i == 2 && lastI == 2) || initSim == 2) {
            threads[threadCount].pID= data[0];
            threadCount++;
        } else if (i == 3 && lastI == 2){

            threads[threadCount].ID= data[0];
            threads[threadCount].at = data[1];
            threads[threadCount].numBursts = data[2];
            threadCount++;
        } else if(i == 2 && lastI == 3) {

            if(initSim != 1){
                threads[threadCount].cpuBurst[bCount] = data[1];
                threadCount++;
                bCount = 0;
                ioCount = 0;
            }

        } else if (i == 3 && lastI == 3) {
            threads[threadCount].cpuBurst[bCount] = data[1];
            threads[threadCount].ioBurst[ioCount] = data[2];
            threadCount++;
            bCount++;
            ioCount++;
         }
         lastI = i;
         i = 0;
     }

     
     threads->threadCount = threadCount;
     
     fcfs(threads, thisSim, verbose, detailed);
     
     
     return;
}
