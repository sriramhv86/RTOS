/*
   In this example the two thread attributes are main_sched_attr and rt_sched_attr,
   those are just names and is nothing to do with the main thread.
   main_sched_attr has highest priority and rt has the next highest.
*/

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<pthread.h>
#include<semaphore.h>
#include<sched.h>


#define NUM_THREADS 2

sem_t mutex1, mutex2;

pthread_t thread_array[NUM_THREADS];

pthread_attr_t rt_sched_attr; // pthread attribute for rt_thread
pthread_attr_t main_sched_attr; // ..
int rt_max_prio, rt_min_prio;
struct sched_param rt_param;
struct sched_param main_param;




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

        int iter, i, rc ,scope;
        sem_init(&mutex1, 0, 0);
        sem_init(&mutex2, 0, 0);

	pthread_attr_init(&rt_sched_attr);
        pthread_attr_init(&main_sched_attr);
        pthread_attr_setinheritsched(&rt_sched_attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&rt_sched_attr, SCHED_FIFO);
        pthread_attr_setinheritsched(&main_sched_attr, PTHREAD_EXPLICIT_SCHED);
        pthread_attr_setschedpolicy(&main_sched_attr, SCHED_FIFO);
        rt_max_prio = sched_get_priority_max(SCHED_FIFO);
        rt_min_prio = sched_get_priority_min(SCHED_FIFO);


        rt_param.sched_priority = rt_max_prio-1;
        pthread_attr_setschedparam(&rt_sched_attr, &rt_param);

        main_param.sched_priority = rt_max_prio;
        pthread_attr_setschedparam(&main_sched_attr, &main_param);


	
	//Here instead of NULL you have to assign the attributes which includes priority

        if (pthread_create(&thread_array[0], &main_sched_attr, funct1, (void *)0)) 
        {
                printf("Error creating thread");
                exit(-1);
        }
        if (pthread_create(&thread_array[1], &rt_sched_attr, funct2, (void *)1))
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

