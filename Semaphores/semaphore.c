#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<time.h>
#include<dirent.h>
#include<string.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/time.h>
#include<sched.h>

#define NUM_THREADS 2

sem_t mutex1, mutex2;

pthread_t thread_array[NUM_THREADS];

void *funct1(void *arg)
{

	printf("Entered funct1 and mutex1 is about to post\n");
	sleep(5); // Do not use sleep in your program, this is just for demo
       	sem_post(&mutex1);
	printf("Mutex1 waiting for Mutex2 to post\n");
     	sem_wait(&mutex2);
 	printf("Mutex2 posted, so exiting the program\n");

        pthread_exit(NULL);
}

void *funct2(void *arg)
{
 
	printf("Mutex2 waiting for Mutex1 to post\n");
       	sem_wait(&mutex1);
	printf("Mutex1 posted now I am(Mutex2) posting\n");
	sleep(5);
       	sem_post(&mutex2);


        pthread_exit(NULL);
}
int main(int argc, char *argv[])
{

        int iter, i;
        sem_init(&mutex1, 0, 0);
        sem_init(&mutex2, 0, 0);

        if (pthread_create(&thread_array[0], NULL, funct1, (void *)0))
        {
                printf("Error creating thread");
                exit(-1);
        }
        if (pthread_create(&thread_array[1], NULL, funct2, (void *)1))
        {
                printf("Error creating thread");
                exit(-1);
        }
 	for(iter=0; iter<NUM_THREADS ; iter++)
        {
                if (pthread_join(thread_array[iter], NULL))
                {
                        printf("Error joining thread");
                        exit(-1);
                }
        }

        printf("All threads completed.\n\n");

        return 0;

}

