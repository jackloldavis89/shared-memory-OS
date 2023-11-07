#include <iostream>     /* input output*/
#include <sys/ipc.h>    /* shared memory */
#include <sys/shm.h>    /* shared memory */
#include <pthread.h>    /* threads */
#include <semaphore.h>  /* semaphores */
using namespace std;

//process reads from shared memory and incremements consumed

//semaphore
sem_t sem;

//items consumed
int itemsConsumed = 0;

//shared memory... cited: https://www.geeksforgeeks.org/ipc-shared-memory/
// ftok to generate unique key
key_t key = ftok("shmfile", 65);
 // shmget returns an identifier in shmid
int shmid = shmget(key, 1024, 0666 | IPC_CREAT);
 // shmat to attach to shared memory
char* str = (char*)shmat(shmid, (void*)0, 0);

void* consume(void* arg) {

    while(str[0] != 'q') { //while not in the quit turn
        sem_wait(&sem); //acquire -> critical area

        if(str[0] == 'c') { //if acquired semaphore and consumer's turn
            cout<< "Consuming... " << endl;

            //consume items from the table if they exist
            if(str[1] == 'i') {
                str[1] = '-'; //item 1
                itemsConsumed++; //increment consumed
            }
            if(str[2] == 'i') {
                str[2] = '-'; //item 2
                itemsConsumed++; //increment consumed
            }

            cout<<"consumed: " << itemsConsumed <<endl;
        }
        sem_post(&sem); //release semaphore
        str[0] = 'p'; //set to producers turn
    }
    //exit thread
    pthread_exit(NULL);
    return NULL;
}

int main()
{
    pthread_t ptid; //create pthread type
    
    pthread_create(&ptid, NULL, &consume, NULL); //create thread for produce

    //sem_wait(&sem); //init to wait
    
    int semV; //used to determine current semaphore value
    while(str[0] != 'q') { //turn based on 0 character of shared data
        sem_getvalue(&sem, &semV);
        //cout<<str[0]<<endl;
        switch(str[0]) {
            case 'p': //producer turn
                if(semV > 0) {
                    sem_wait(&sem); //wait semaphore on producers turn 
                }
                break;
            case 'c': //consumer turn
                if(semV < 1) {
                    sem_post(&sem); //release semaphore on consumers turn
                }
                break;
            case 'l': //last turn
                cout<< "Checking for last items...\n";
                if(str[1] == 'i') {
                str[1] = '-'; //item 1
                itemsConsumed++;
                }
                if(str[2] == 'i') {
                    str[2] = '-'; //item 2
                    itemsConsumed++;
                }
                cout<<"Total consumed: " << itemsConsumed <<endl;
                str[0] = 'q';
                break;
        }
    }
    cout<< "Consumer quit...\n";

    // detach from shared memory, destruction of this memory is handeled by producer
    shmdt(str);
}