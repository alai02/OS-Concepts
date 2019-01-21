/* 
 * Name: Alex Lai
 * Date: Monday March 27th, 2017
 * Assignment: A3
 * Course: CIS-3110
 * Student #: 0920158
 * Email: Alai02@mail.uoguelph.ca
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

/*
 * This struct contains all the info that is needed to be stored
 * for each philosopher created
 */
typedef struct {
  int philPos;
  int count;
  int maxEat;
  sem_t *aFork;
  sem_t *sem;
} philInfo;

//function declaration to aviod compiler pass through errors
void *philProcesses(void *philParamsInfo);

/* 
 * This function loops through all the semaphores created
 * and initializes them using the semaphore libarys init function
 */
void startMutexLock(sem_t *sem, sem_t *aFork, int numPhil) {
  int i;
  for(i = 0; i < numPhil; i++) {
    sem_init(&aFork[i], 0, 1);
  }

  sem_init(sem, 0, numPhil - 1);
}

/* runThreads
 * IN: Pointer to an array of processes, Pointer to array of forks and locks to them
 * Number of philosophers and the maximum amount of time to eat
 * Function that will take all the processes and run them concurrently
 * The loop initalizes all the processes variables and runs them using 
 * the pthread create function in the pthreads library
 */
void runThreads(pthread_t *phils, sem_t *aFork, sem_t *sem, int numPhil, int maxEat) {
  int i;
  for (i = 0; i < numPhil; i++) {
    philInfo *sharedTheadInfo = malloc(sizeof(philInfo));
    sharedTheadInfo->aFork = aFork;
    sharedTheadInfo->maxEat = maxEat;
    sharedTheadInfo->philPos = i;
    sharedTheadInfo->sem = sem;
    sharedTheadInfo->count = numPhil;
    pthread_create(&phils[i], NULL, philProcesses, (void *)sharedTheadInfo);
  }
}

/*
 * IN: void pointer to struct containing the philosphers info
 * function takes in the process info and will switch between eating and thinking
 * depending on if the other concurrent processes are. 
 */
void *philProcesses(void *philParamsInfo) {
    int i;
    philInfo aPhil = * (philInfo *) philParamsInfo; //extract philosophers info 
    i = 0;
    bool allGood = true;
    int timesEaten = 0;
    while(allGood){
        printf("Philosopher %d is thinking\n", aPhil.philPos +1);
        sem_wait(aPhil.sem);
        sem_wait(&aPhil.aFork[aPhil.philPos]);
        sem_wait(&aPhil.aFork[(aPhil.philPos + 1) % aPhil.count]);
        printf("Philosopher %d is eating\n", aPhil.philPos +1);
        sem_post(&aPhil.aFork[aPhil.philPos]);
        sem_post(&aPhil.aFork[(aPhil.philPos + 1) % aPhil.count]);
        sem_post(aPhil.sem);
        i++;
        timesEaten++;
        if(timesEaten >= aPhil.maxEat){
            allGood = false;
        }
    }
    printf("Philosopher %d is thinking\n", aPhil.philPos +1);
    pthread_exit(NULL);
}


/*
 * IN: 2 command line arguements (both integers)
 * OUT: outputs the current state of the philsophers while they have a session of eating and thinking
 * The main error checks the input and sets up the processes and runs them by calling runThreads.
 * Known limitations: NONE
 * Return: 0 on success
 */
int main(int argc, char *argv[]) {
    if(argc != 3){
        printf("You entered: %d, Inncorrect number of arguments, please include the number of philosophers and the time it takes to eat\n", argc);
        exit(1);
    }
    int numPhil = atoi(argv[1]);
    int eatCount = atoi(argv[2]);
    if(numPhil < 3){
        printf("Not enough philosophers at the table yet\n");
        exit(1);
    }
    if(eatCount < 1 || eatCount > 1000){
        printf("Sorry, a philosopher must eat atleast 1 time but no more than 1000 times\n");
        exit(1);
    }
    sem_t sem;
    sem_t forks[numPhil];
    pthread_t philosophers[numPhil];


    startMutexLock(&sem, forks, numPhil);
    runThreads(philosophers, forks, &sem, numPhil, eatCount);
    pthread_exit(NULL);
}