#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define BUFFERSIZE 5

//declaring semaphores
sem_t s;
sem_t n;
sem_t e;
sem_t message;
sem_t countersem;

//declaring data,buffer,conunternumber
int counterNumber=1;
int data=0;
int buffer[BUFFERSIZE];

//counter
void* Message(void* arg){

 sem_wait(&countersem);
 int tempCounterNumber=counterNumber;
 counterNumber++;
 sem_post(&countersem);
 //use srand to generate diffrent sequence every time
 while(1){
 srand(time(0));
 int r=(rand()%(8-2+1))+2;
 sleep(r);
 printf("Counter thread %d : Received a message \n",tempCounterNumber);
 int test=sem_trywait(&message);
 if(test!=0){
   printf("Counter thread %d :Waiting to write \n",counterNumber);
   sem_wait(&message);
 }
 data++;
 printf("Counter thread %d : now adding to counter , counter value=: %d \n",tempCounterNumber,data);
 sem_post(&message);
}
}
//producer
void* monitor(void* arg){

//produce
int i=0;
//use srand to generate diffrent sequence every time
while(1){
srand(time(0));
int r=(rand()%(8-2+1))+2;
sleep(r);
printf("Monitor thread : waiting to read counter \n");
sem_wait(&message);
int temp=data;
data=0;
printf("Monitor thread : reading a count value of %d \n",temp);
sem_post(&message);
//append
int test=sem_trywait(&e);
if(test!=0){
  printf("Monitor thread : Buffer is full \n");
  sem_wait(&e);
}
sem_wait(&s);
buffer[i]=temp;
printf("Monitor thread: writing to buffer at position %d \n",i);
i++;
if(i==BUFFERSIZE){
  i=0;
}
sem_post(&s);
sem_post(&n);

}
}
//consumer
void* collector(void* arg){
  int i=0;
  //use srand to generate diffrent sequence every time
  while(1){
  srand(time(0));
  int r=(rand()%(8-2+1))+2;
  sleep(r);
  int test=sem_trywait(&n);
  if(test!=0){
    printf("Collector thread : Nothing in the buffer \n");
    sem_wait(&n);
  }
  sem_wait(&s);
  int data=buffer[i];
  printf("Collector thread : reading %d from buffer at position %d \n",data,i);
  i++;
  if(i==BUFFERSIZE){
    i=0;
  }
  sem_post(&s);
  sem_post(&e);
}
}

void main(){

//declare threads
  int counters;
  pthread_t messageThread[counters];
  pthread_t monitorThread;
  pthread_t collectorThread;

 //initialize semaphores
  sem_init(&s,0,1);
  sem_init(&n,0,0);
  sem_init(&e,0,BUFFERSIZE);
  sem_init(&message,0,1);
  sem_init(&countersem,0,1);

  printf("please enter number of counters : \n");
  scanf("%d",&counters);


//creating threads
  for(int i=0;i<counters;i++){
    pthread_create(&messageThread[i],NULL,Message,NULL);
  }
  pthread_create(&monitorThread,NULL,monitor,NULL);
  pthread_create(&collectorThread,NULL,collector,NULL);

//joining threads
  for(int i=0;i<counters;i++){
    pthread_join(messageThread[i],NULL);
  }
  pthread_join(monitorThread,NULL);
  pthread_join(collectorThread,NULL);

}
