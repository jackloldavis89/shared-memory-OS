#include <iostream>     /* input output*/
#include <sys/ipc.h>    /* shared memory */
#include <sys/shm.h>    /* shared memory */
#include <pthread.h>    /* threads */
#include <semaphore.h>  /* semaphores */
#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
using namespace std;

//process writes to shared memory

//semaphore
sem_t sem;

//items produced
int itemsProduced = 0;

//shared memory... cited: https://www.geeksforgeeks.org/ipc-shared-memory/
// ftok to generate unique key
key_t key = ftok("shmfile", 65);
// shmget returns an identifier in shmid
int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
// shmat to attach to shared memory
char* str = (char*)shmat(shmid, (void*)0, 0);

void* produce(void* arg) { 
    //set a random amount of items to produce, only known by producer
    srand (time(NULL)); //seed random using time(NULL)
    int toProduce = rand() % 100000 + 1; //between 1 - 100000

    while(str[0] != 'q') { //while its not in the quit turn
        sem_wait(&sem); //acquire -> critical area

        if(str[0] == 'p') { //if producer turn and semaphore acquired
            cout<< "Producing..." << endl;
            
            //produce items for the table if they aren't already there
            //item one will be held in str[1] amd item two will be held in str[2]
            if(str[1] != 'i') { 
                str[1] = 'i'; //item 1
                itemsProduced++; //increment produced
                if(itemsProduced == toProduce) { //if produced enough
                    cout << "Total needed: " <<  itemsProduced << endl; //tell how many the random value had
                    str[0] = 'l'; //last turn before q turn
                    return(NULL);} //return null
            }
            if(str[2] != 'i') {
                str[2] = 'i'; //item 2
                itemsProduced++; //increment produced
                if(itemsProduced == toProduce) { //if produced enough
                    cout << "Total needed: " <<  itemsProduced << endl; //tell how many the random value had
                    str[0] = 'l'; //last turn before q turn
                    return(NULL);} //return null
            }
        }

        sem_post(&sem); //release semaphore
        str[0] = 'c'; //set it to consumer's turn
    }
    //exit thread
    pthread_exit(NULL);
    return NULL;
}

int main()
{
    sem_init(&sem, 0, 1); //initiate binary semaphore

    str[0] = 'p'; //initiate to producer turn

    pthread_t ptid; //pthread type

    pthread_create(&ptid, NULL, &produce, NULL); //create thread for produce

    sem_wait(&sem); //init to wait

    //int for sem_getvalue
    int semV;
    while(str[0] != 'q') { //based on first character of shared data
    sem_getvalue(&sem, &semV); //get current semaphore value
        switch(str[0]) {
            case 'p': //producer turn
                if(semV < 1) {
                    sem_post(&sem); //release semaphore on producers turn
                }
                break;
            case 'c': //consumer turn
                if(semV > 0) {
                    sem_wait(&sem); //wait semaphore on consumers turn 
                }
                break;
            case 'l': //last turn
                break;
        }
    }
    cout << "Total produced: " <<  itemsProduced << endl;
    //set turn to quit for consumer
    str[0] = 'q';
    // detach from shared memory
    shmdt(str);
    // destroy the shared memory
    shmctl(shmid, IPC_RMID, NULL);
    cout << "Producer quit...\n";
}