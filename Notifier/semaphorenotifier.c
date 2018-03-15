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


#define NUM_THREADS 2

int rc;
long duration=0.0;
struct timeval tod1, tod2;
sem_t mutex, mutex1;
pthread_t thread_array[NUM_THREADS];

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
			printf("Received ack going back to checking again\n");
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
		printf("%ld\n", duration);
		ret = system("telegram-send \"Tram leaves in 15 mins!!\" \n");
		sem_post(&mutex1);
               	//printf("Notification sent via telegram\n");               
                //printf("Ack sent successfully\n");
       }

    	pthread_exit(NULL);
}



int main(int argc, char *argv[])
{

  	int iter, i, ra, scope;
	sem_init(&mutex, 0, 0);
	sem_init(&mutex1, 0, 0);


/*----------------------------CREATING THREADS FOR READ AND WRITE FUNCTIONS----------------------*/

    	if (pthread_create(&thread_array[0], NULL, myreadfunct, (void *)0))
	{
      		printf("Error creating thread");
      		exit(-1);
    	}
	if (pthread_create(&thread_array[1], NULL, mywritefunct, (void *)1))
        {
                printf("Error creating thread");
                exit(-1);
        }
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
