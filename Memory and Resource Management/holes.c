#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_PROCESS 10

typedef struct Process{
	char ID;
	int memUsage;
    int insertionPoint;
    int count;
	int numMemorySwaps;
	int lengthInMemory;
    struct Process* next;
} Process;

void insertProcess(int array[], Process* p);
int largestHole(int totalMemory[]);
void firstFit(Process* processQueue, int totalLoads);
void bestFit(Process* processQueue);
void worstFit(Process* processQueue);
void nextFit(Process* processQueue);

void insertProcess(int array[], Process* p){
    int i;
    double totalMemPercent = 0;
    double memUsagePercent;
    double totalMem = 0;
    double holeCount = 0;
    for(i = 0; i < 128; i++){
        if(array[i] == 1){
            totalMem++;
        }
        if(array[i] == 1 && array[i+1] == 0){
            holeCount++;
        }
    }
    memUsagePercent = ((p->memUsage / 128.00)*100.00);
    totalMemPercent = ((totalMem/128.00)*100.00);
    printf("pid loaded, #process = %c "
            "#holes = %.lf, %%memusage = %.1lf, cumulative"
            "%%mem = %.1lf\n", p->ID, holeCount, memUsagePercent, totalMemPercent);
}

int largestHole(int totalMemory[]){
    int i;
    int largest = 0;
    int templarge = 0;
    for(i = 0; i < 128; i++){
        if(totalMemory[i] == 0){
            templarge = templarge+1;
        }
        if(totalMemory[i] == 1){
            if(templarge > largest){
                largest = templarge;
                templarge = 0;
            }
        }
    }
    if(templarge > largest){
        largest = templarge;
    }
    return largest;
}

int main(int argc, char ** argv){

    FILE * fp = fopen(argv[1], "r");
    int i = 0, j = 0;
    char* data[10];
    char* data1 = malloc(sizeof(char)*10 +1);
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    Process* processQueue = malloc(sizeof(Process));
	Process* temp = processQueue;
    if(fp == NULL){
		printf("File not opened\n");
		exit(1);
	}
	for(i = 0; i < 10; i++){
		data[i] = malloc(sizeof(char)*10);
	}
    while ((read = getline(&line, &len, fp)) != -1) {
        strcpy(data[j++], line);
    }
    for(i = 0; i < j; i++){
    	data1 = strtok(data[i], " ");
        temp->ID = (char) data1[0];
    	data1 = strtok(NULL, " ");
        temp->memUsage = atoi(data1);
        temp->next = malloc(sizeof(Process));
        temp->numMemorySwaps = 0;
        temp->count = 0;
        if((j-i) == 1){
            temp->next = NULL;
        } else {
             temp = temp->next;
        }
    }
    int totalLoads = j * 3;
	printf("First fit algorithm\n");
    firstFit(processQueue, totalLoads);
    bestFit(processQueue);
	printf("Best fit algorithm\n" );
	worstFit(processQueue);
	printf("Worst fit algorithm\n" );
	printf("Total loads = %d, average"
			" #process = %.1lf, average #holes = %.1lf"
		   ", cumulative %%mem = %d\n", totalLoads, 0.0, 0.0, 0);
	//nextFit(processQueue);
    return 0;
}
void printQueue(Process* p){
    Process* tempPrint = p;
    while(p != NULL){
        printf(" %c ", p->ID);
        p = p->next;
    }
    printf("\n");
}

void firstFit(Process* processQueue, int totalLoads){
    int i = 0, j = 0, k = 0;
    int totalMemory[128];
    int space = 0;
    bool noSpace = true;
    for(i = 0; i < 128; i++){
        totalMemory[i] = 0;
    }
    int x;
    int totalSwaps = 0;
    bool done = false;
    Process* runningHead = NULL;
    while (processQueue != NULL || done == false) {
        for (i = 0; i < 128; i++) {
            if (totalMemory[i] == 0){
                space++;
                if(processQueue == NULL){
                    i = 127;
                    done  = true;
                } else {
                    if (space == processQueue->memUsage-1){
                        noSpace = false;
                        processQueue->insertionPoint = i-space;
                        for (j = processQueue->insertionPoint; j < i; j++){
                            if(j >= 0){
                                if(j < 128){
                                    totalMemory[j] = 1;
                                }
                            }
                        }
                        insertProcess(totalMemory, processQueue);
                        processQueue->numMemorySwaps++;
                        space = 0;
                        if(processQueue->numMemorySwaps != 3){
                            if(runningHead == NULL){
                                runningHead = malloc(sizeof(Process));
                                runningHead->ID = processQueue->ID;
                                runningHead->memUsage = processQueue->memUsage;
                                runningHead->insertionPoint = processQueue->insertionPoint;
                                runningHead->numMemorySwaps = processQueue->numMemorySwaps;
                                runningHead->next = NULL;
                            } else {
                                Process* temp = runningHead;
                                while(temp->next != NULL){
                                    temp = temp->next;
                                }
                                Process* temp2 = malloc(sizeof(Process));
                                temp2->ID = processQueue->ID;
                                temp2->memUsage = processQueue->memUsage;
                                temp2->insertionPoint = processQueue->insertionPoint;
                                temp2->numMemorySwaps = processQueue->numMemorySwaps;
                                temp2->count = processQueue->count;
                                temp2->next = NULL;
                                temp->next = temp2;

                            }
                        }
                        if(processQueue != NULL){
                            if(processQueue->next == NULL){
                                processQueue = NULL;
                            } else {
                                processQueue = processQueue->next;
                            }
                        } else {
                            i = 127;
                            noSpace = false;
                        }
                    }
                }
            }
            if(processQueue != NULL){
                if (largestHole(totalMemory) < processQueue->memUsage){
                    noSpace = true;
                }
            }
        }
        if(processQueue != NULL){
            if (largestHole(totalMemory) > processQueue->memUsage){
                noSpace = false;
            }
        }
        if(noSpace == true) {
            for(k = runningHead->insertionPoint; k < (runningHead->memUsage + runningHead->insertionPoint); k++){
                if(k >= 0){
                    if(k < 128){
                        totalMemory[k] = 0;
                    }
                }
            }
            if(runningHead->numMemorySwaps < 3){
                Process* insertTemp = processQueue;
                while(insertTemp->next != NULL){
                    insertTemp = insertTemp->next;
                }
                Process* toAddToQueue = malloc(sizeof(Process));
                toAddToQueue->ID = runningHead->ID;
                toAddToQueue->insertionPoint = runningHead->insertionPoint;
                toAddToQueue->numMemorySwaps = runningHead->numMemorySwaps;
                toAddToQueue->memUsage = runningHead->memUsage;
                toAddToQueue->count = runningHead->count;
                toAddToQueue->next = NULL;
                insertTemp->next = toAddToQueue;
                runningHead = runningHead->next;
                totalSwaps++;
                if (largestHole(totalMemory) > processQueue->memUsage){
                    noSpace = false;
                }
            } else {
                runningHead = runningHead->next;
            }
            noSpace = false;

        }
    }

}

int largestHole2(char totalMemory[]){
    int i;
    int largest = 0;
    int templarge = 0;
    for(i = 0; i < 128; i++){
        if(totalMemory[i] == 'x'){
            templarge = templarge+1;
        }
        if(totalMemory[i] != 'x'){
            if(templarge > largest){
                largest = templarge;
                templarge = 0;
            }
        }
    }
    if(templarge > largest){
        largest = templarge;
    }
    return largest;
}

void insertProcess2(char array[], Process* p){
    int i;
    double totalMemPercent = 0;
    double memUsagePercent;
    double totalMem = 0;
    double holeCount = 0;

    for(i = 0; i < 128; i++){
        //printf("%c", array[i]);
        if(array[i] != 'x'){
            totalMem++;
        }
        if(array[i] == 'x' && array[i+1] != 'x'){
            holeCount++;
        }
    }
	//printf("\n" );
    memUsagePercent = ((p->memUsage / 128.00)*100.00);
    totalMemPercent = ((totalMem/128.00)*100.00);
    printf("pid loaded, #process = %c "
            "#holes = %.lf, %%memusage = %.1lf, cumulative"
            "%%mem = %.1lf\n", p->ID, holeCount, memUsagePercent, totalMemPercent);

}

void bestFit(Process* processQueue){
    Process* runningHead = NULL;
    int i = 0, j = 0, k = 0;
    char totalMemory[128];
    int space = 0;
    bool noSpace = false;
    for(i = 0; i < 128; i++){
        totalMemory[i] = 'x';
    }
    int smallestIndex = 0;
    int smallestSpace = 128;
    int count = 0;
    while(processQueue != NULL && count < 24){
        for(i = 0; i < 128; i++){
            if(totalMemory[i] == 'x'){
                space++;
            } else {
                if(space < smallestSpace && space >= processQueue->memUsage){
                    smallestSpace = space;
                    smallestIndex = i - processQueue->memUsage;
                    processQueue->insertionPoint = smallestIndex;
                    space = 0;
                }
            }
        }
		if(largestHole2(totalMemory) < processQueue->memUsage){
			noSpace = true;
		}
        if(noSpace == true){
            for(j = runningHead->insertionPoint; j < (runningHead->memUsage + runningHead->insertionPoint); j++){
                totalMemory[j] = 'x';
            }
            Process* insertQueue = processQueue;
            while(insertQueue->next != NULL){
                insertQueue = insertQueue->next;
            }
            Process* temp = malloc(sizeof(Process));
            temp->ID = runningHead->ID;
            temp->memUsage = runningHead->memUsage;
            temp->insertionPoint = runningHead->insertionPoint;
            temp->count = runningHead->count;
            temp->next = NULL;
            if(temp->count > 3){
                printf("Process: %c completed\n", temp->ID);
                insertQueue->next = NULL;
            } else {
                insertQueue->next = temp;
            }
            if(runningHead->next != NULL){
                runningHead = runningHead->next;
            }
            noSpace = false;
        } else {
            for(j = smallestIndex; j < (processQueue->memUsage + smallestIndex); j++){
                totalMemory[j] = processQueue->ID;
            }
            insertProcess2(totalMemory, processQueue);
            if(runningHead == NULL){
                runningHead = malloc(sizeof(Process));
                runningHead->ID = processQueue->ID;
                runningHead->memUsage = processQueue->memUsage;
                runningHead->insertionPoint = processQueue->insertionPoint;
                runningHead->count = processQueue->count;
                runningHead->next = NULL;
				processQueue = processQueue->next;
            } else {
                count++;
                Process* insertRunning = runningHead;
                Process* temp = malloc(sizeof(Process));
                while(insertRunning->next != NULL){
                    insertRunning = insertRunning->next;
                }
                temp->ID = processQueue->ID;
                temp->memUsage = processQueue->memUsage;
                temp->insertionPoint = processQueue->insertionPoint;
                temp->count = processQueue->count;
                temp->count++;
                temp->next = NULL;
                insertRunning->next = temp;
                if(processQueue->next != NULL){
                    processQueue = processQueue->next;
                }
            }
        }
		space = 0;
		noSpace = false;
		smallestSpace = 128;
    }
}

void worstFit(Process* processQueue){
    Process* runningHead = NULL;
    int i = 0, j = 0, k = 0;
    char totalMemory[128];
    int totalSwaps = 0;
    int space = 0;
    bool noSpace = false;
    for(i = 0; i < 128; i++){
        totalMemory[i] = 'x';
    }
    int largestIndex = 0;
    int largestSpace = 0;
    int count = 0;
    while(processQueue != NULL && count < 24){
        for(i = 0; i < 128; i++){
            if(totalMemory[i] == 'x'){
                space++;
            } else {
                if(space > largestSpace && space >= processQueue->memUsage){
                    largestSpace = space;
                    largestIndex = i-space;
                    processQueue->insertionPoint = largestIndex;
                    space = 0;
                }
            }
			if(largestHole2(totalMemory) < processQueue->memUsage){
			//	printf("found no space \n" );
				noSpace = true;
			}
		}
	   if(noSpace == true){
	        for(j = runningHead->insertionPoint; j < (runningHead->memUsage + runningHead->insertionPoint); j++){
	           totalMemory[j] = 'x';
	        }
	        Process* insertQueue = processQueue;
	        while(insertQueue->next != NULL){
	            insertQueue = insertQueue->next;
	        }
	        Process* temp = malloc(sizeof(Process));
	        temp->ID = runningHead->ID;
	        temp->memUsage = runningHead->memUsage;
	        temp->insertionPoint = runningHead->insertionPoint;
	        temp->count = runningHead->count;
	        temp->next = NULL;
	        if(temp->count > 3){
	            printf("Process: %c completed\n", temp->ID);
	            insertQueue->next = NULL;
	       } else {
	          insertQueue->next = temp;
		   }
	       if(runningHead->next != NULL){
	           runningHead = runningHead->next;
        	} else{
				processQueue = NULL;
	            printf("Error: running head NULL\n");
	         }
	        noSpace = false;
	    } else {
			//printf("Insertion point: %d\n", smallestIndex);
	        for(j = largestIndex; j < (processQueue->memUsage + largestIndex); j++){
	            totalMemory[j] = processQueue->ID;
	        }
	        insertProcess2(totalMemory, processQueue);
	        if(runningHead == NULL){
	            runningHead = malloc(sizeof(Process));
	            runningHead->ID = processQueue->ID;
			    runningHead->memUsage = processQueue->memUsage;
	            runningHead->insertionPoint = processQueue->insertionPoint;
	            runningHead->count = processQueue->count;
	            runningHead->next = NULL;
				processQueue = processQueue->next;
	        } else {
	            count++;
	            Process* insertRunning = runningHead;
	            Process* temp = malloc(sizeof(Process));
	            while(insertRunning->next != NULL){
	                insertRunning = insertRunning->next;
	            }
				temp->ID = processQueue->ID;
	            temp->memUsage = processQueue->memUsage;
	            temp->insertionPoint = processQueue->insertionPoint;
	            temp->count = processQueue->count;
	            temp->count++;
	            temp->next = NULL;
                insertRunning->next = temp;
	            if(processQueue->next != NULL){
	                processQueue = processQueue->next;
	            } else {
	               printf("Error, running queueu empty\n");
	            }
         	}
	  	}
		space = 0;
		noSpace = false;
		largestSpace = 0;
		}

}
/*
void nextFit(Process* pQ){
	int lastIndex = 0;
	bool noSpace = false;
	char totalMemory[128];
	Process* runningHead = NULL;
	int i = 0; j = 0;
	for(i = 0; i < 128; i++){
		totalMemory[i] = NULL;
	}

    while(pQ != NULL){
        for(i = lastIndex < memory){
            if(totalMemory[i] == NULL){
                space++;
                if(space >= processMemory){
                    insertProcess2(totalMemory, processQueue);
                    lastIndex = i;
                    break;
                }
            }
            if(i >= totalMemory){
                i = 0;
            }
        }
        //no space found
        if(noSpace){
			for(j = runningHead->insertionPoint; j < (runningHead->memUsage + runningHead->insertionPoint); j++){
	           totalMemory[j] = NULL;
	        }
	        Process* insertQueue = processQueue;
	        while(insertQueue->next != NULL){
	            insertQueue = insertQueue->next;
	        }
	        Process* temp = malloc(sizeof(Process));
	        temp->ID = runningHead->ID;
	        temp->memUsage = runningHead->memUsage;
	        temp->insertionPoint = runningHead->insertionPoint;
	        temp->count = runningHead->count;
	        temp->next = NULL;
	        if(temp->count > 3){
	            printf("Process: %c completed\n", temp->ID);
	            insertQueue->next = NULL;
	       } else {
	          insertQueue->next = temp;
		   }
	       if(runningHead->next != NULL){
	           runningHead = runningHead->next;
        }
    }

*/
