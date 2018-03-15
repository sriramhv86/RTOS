/* 
   This program uses semaphores to send notification via telegram app. 
   Two threads are write and read. 
   Semaphore is initialized and is set to zero at the beginning.
   A wait signal is initialized at the beginning of the read thread.
   A post signal is generated when the time's matching the bus timining from write thread.
   As soon as the read thread receives the post signal it sends the notification via telegram.
   
*/

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

#define NUM_THREADS 3

int rc;
long duration=0.0;
struct timeval tod1, tod2;
sem_t mutex, mutex1;
pthread_t thread_array[NUM_THREADS];
pthread_attr_t rt_sched_attr; // pthread attribute for rt_thread
pthread_attr_t main_sched_attr; // ..
int rt_max_prio, rt_min_prio;
struct sched_param rt_param;
struct sched_param main_param;


/*------------- Write function: Checks time and sends a message to read function-----------------*/

void *mywritefunct(void *arg) 
{

	time_t t;
	struct tm tm;
     	printf("Entered the write function now\n");

	while(1){
		
		t = time(NULL);
                tm = *localtime(&t);
		if((tm.tm_sec%5)==0)
		{
			gettimeofday(&tod1, NULL);
			sem_post(&mutex);
                	//printf("Send successful\n");
                	//printf ("Waiting to receive ack\n");
			sem_wait(&mutex1);
                        //exit (1);
                }
	}

       	pthread_exit(NULL);
}

/*----------Read function: Receives message from write function to send Notification-------------*/

void *myreadfunct(void *arg)
{

	int ret, sz, fd;		
	printf("Entered read function \n");
	fd = open("Notifier.txt", O_CREAT|O_RDWR, S_IRWXU);
        if (fd < 0)
        {
                perror("Notifier.txt");
                exit(1);
        }
	
 	while(1){

                //printf ("Waiting to receive message to send notification\n");
		sem_wait(&mutex);
		gettimeofday(&tod2, NULL);
                duration =(tod2.tv_usec - tod1.tv_usec);
		printf("Time taken to send notification in usec is:%ld\n", duration);
		sleep(1);
		//ret = system("telegram-send \"Tram leaves in 15 mins!!\" \n");
		sem_post(&mutex1);
               	//printf("Notification sent via telegram\n");               
                //printf("Ack sent successfully\n");
       }

    	pthread_exit(NULL);
}


/*-----------------------------RANDOM FUNCTION TO KEEP A THREAD BUSY-----------------------------*/

void *randfunct1(void *arg)
{
	printf("Entered the rand funct\n");
        while(1){
                int c=0;
                int n, m;
                for (n = 0; n < 1000; n++)
                        for (m = 0; m < 1000; m++){
				c++;
				c= c*c*c*c;
                                //printf("this is 1c %d\n", c);
		}
        }
        //return NULL;

        pthread_exit(NULL);
}


/*-----------------------------------------MAIN FUNCTION-----------------------------------------*/

int main(int argc, char *argv[])
{

  	int iter, i, ra, scope;
	pthread_t thread_array[NUM_THREADS];
	sem_init(&mutex, 0, 0);
	sem_init(&mutex1, 0, 0);
	if(rc!=0)
	{
		printf("Error initializing semaphore\n");
		exit(-1);
	}

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


/*----------------------------CREATING THREADS FOR READ AND WRITE FUNCTIONS----------------------*/

    	if (pthread_create(&thread_array[0], &main_sched_attr, myreadfunct, (void *)0))
	{
      		printf("Error creating thread");
      		exit(-1);
    	}
	if (pthread_create(&thread_array[1], &rt_sched_attr, mywritefunct, (void *)1))
        {
                printf("Error creating thread");
                exit(-1);
        }
	 if (pthread_create(&thread_array[2], NULL, randfunct1, (void *)2))
        {
                printf("Error creating thread");
                exit(-1);
        }
/*        if (pthread_create(&thread_array[3], NULL, randfunct1, (void *)3))
        {
                printf("Error creating thread");
                exit(-1);
        }
        if (pthread_create(&thread_array[4], NULL, randfunct1, (void *)4))
        {
                printf("Error creating thread");
                exit(-1);
        }
        if (pthread_create(&thread_array[5], NULL, randfunct1, (void *)5))
        {
                printf("Error creating thread");
                exit(-1);
        }
        if (pthread_create(&thread_array[6], NULL, randfunct1, (void *)6))
        {
                printf("Error creating thread");
                exit(-1);
        }
        if (pthread_create(&thread_array[7], NULL, randfunct1, (void *)7))
        {
                printf("Error creating thread");
                exit(-1);
        }
*/


/*------------------------------JOINING ALL THE THREADS THAT WERE CREATED------------------------*/

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
