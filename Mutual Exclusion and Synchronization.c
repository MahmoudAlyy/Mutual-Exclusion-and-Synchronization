#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#define SHARED 1
#define message 10
#define size_of_buffer 5


/// QUEUE START

int intArray[size_of_buffer];
int front = 0;
int rear = -1;
int itemCount = 0;
int pos;
int temp;

int peek()
{
    return intArray[front];
}

bool isEmpty()
{
    return itemCount == 0;
}

bool isFull()
{
    return itemCount == size_of_buffer;
}

int size()
{
    return itemCount;
}

int insert(int data)
{

    if(!isFull())
    {

        if(rear == size_of_buffer-1)
        {
            rear = -1;
        }

        intArray[++rear] = data;
        pos = rear;
        itemCount++;
    }
    return rear;
}

int removeData()
{
    int ret = front;
    int data = intArray[front++];

    if(front == size_of_buffer)
    {
        front = 0;
    }

    itemCount--;
    return ret;
}

///QUEUE END



sem_t int_sem;
sem_t buffer_sem;
sem_t buffer_size;
sem_t buffer_no;
int integer = 0;
int buffer = 0;



void *counter(int x)
{
    int r = rand() % 10;
    sleep(r);
    printf("Counter thread %d received a message\n",x);
    printf("Counter thread %d waiting to write\n",x);
    sem_wait(&int_sem);
    integer++;
    printf("Counter thread %d now adding to counter, counter value = %d \n",x,integer);
    sem_post(&int_sem);

}

void *monitor()
{
    int mon_value = 0;

    while(1)
    {

        int r = rand() % 5;
        sleep(r);

//get from int
        printf("Monitor thread: waiting to read counter\n");
        sem_wait(&int_sem);
        mon_value = integer;
        integer = 0;
        printf("Monitor thread: reading a count value of %d \n",mon_value);
        sem_post(&int_sem);

        while(isFull())
        {
            printf("Monitor thread: Buffer full!!\n");
            int r = rand() % 5;
            sleep(r);
        }


//produce
        sem_wait(&buffer_size);
        sem_wait(&buffer_sem);
        temp = insert(mon_value);
//        mon_value = 0;
        printf("Monitor thread: writing to buffer at position %d\n",temp);
        sem_post(&buffer_sem);
        sem_post(&buffer_no);
    }

}

void *collector()
{
//consumer
    while(1)
    {
        int r = rand() % 6;
        sleep(r);

        while(isEmpty())
        {
            printf("Collector thread: Buffer Empty!!\n");
            int r = rand() % 6;
            sleep(r);
        }

        sem_wait(&buffer_no);
        sem_wait(&buffer_sem);
        int poped = removeData();
        printf("Collector thread: reading from buffer at position %d \n",poped);
        sem_post(&buffer_sem);
        sem_post(&buffer_size);
    }

}

int main()
{


    int i;

    sem_init(&int_sem, SHARED, 1);
    sem_init(&buffer_sem, SHARED, 1);
    sem_init(&buffer_size, SHARED, size_of_buffer);
    sem_init(&buffer_no, SHARED, 0);




    pthread_t *counter_id = malloc(message*sizeof(pthread_t));
    pthread_t *monitor_id = malloc(1*sizeof(pthread_t));
    pthread_t *collecter_id = malloc(1*sizeof(pthread_t));



    for (i=0; i<message; i++)
        pthread_create(&counter_id[i], NULL, counter, i);

    pthread_create(&monitor_id, NULL, monitor, NULL);
    pthread_create(&collecter_id, NULL, collector, NULL);


   //for (i=0; i<message; i++)
   //    pthread_join(counter_id[i], NULL);

    sleep(25);



    return 0;
}
