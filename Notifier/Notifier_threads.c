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
#include <mqueue.h>


#define NUM_THREADS 2
#define MQNAME "/rtosq"
#define MQNAMEACK "/ack"
#define MQMESG "Notification sent!"
#define MQMESGACK "Ack sent!"

int rc, rcack;
struct mq_attr mqAttr, mqAttrack;
mqd_t rtos, ack;

void *mywritefunct(void *arg) // Write function: Checks time and sends a message to read function
{

	char buffer[2048];
      	while(1){
                printf ("Sending message.\n");

                //write your condition
                        rc = mq_send (rtos, MQMESG, sizeof (MQMESG), 1);
                        if (rc < 0) 
			{
                                printf ("Error %d (%s) on mq_send.\n",
                                errno, strerror (errno));
                                exit (1);
                        }

                printf("Send successful\n");

                printf ("Waiting to Receiving on mqd_t %d.\n", ack);

                if(mq_receive (ack, buffer, sizeof (buffer), NULL))
                        printf("Notiffication successfully sent going back to checking again\n");

                else{
                        printf ("Error %d (%s) on mq_receive.\n", errno, strerror (errno));
                        exit (1);
                }

        }


       	pthread_exit(NULL);
}

void *myreadfunct(void *arg) // Read function: Receives message from write function to send Notification
{

	int ret;
	char buffer[2048];
	
  	while(1){

                printf ("Waiting to Receiving on mqd_t %d.\n", rtos);

                if(mq_receive (rtos, buffer, sizeof (buffer), NULL)){
                        ret = system("telegram-send --stdin < Notifier.txt \n");
                        printf("Notification sent via telegram\n");
                }

                else{
                        printf ("Error %d (%s) on mq_receive.\n", errno, strerror (errno));
                        exit (1);
                }
                //printf ("Received [%s].\n", buffer);

                rcack = mq_send (ack, MQMESGACK, sizeof (MQMESGACK), 1);
                if (rcack < 0) {
                        printf ("Error %d (%s) on mq_send.\n", errno, strerror (errno));
                        exit (1);
                }

                printf("Ack sent successfully\n");
        }


    	pthread_exit(NULL);
}



int main(int argc, char *argv[])
{


  	int iter, i, fd;
	pthread_t thread_array[NUM_THREADS];

/*-----------------------CREATING TWO MESSAGE QUEUES FOR NOTIFICATION AND ACK--------------------*/

	printf ("Unlinking if there is any queue present already.\n");
    	rc = mq_unlink (MQNAME);
    	if (rc < 0) {
        	printf ("Warning %d (%s) on mq_unlink.\n",errno, strerror (errno));
    	}
	rcack = mq_unlink (MQNAMEACK);
        if (rcack < 0) {
                printf ("Warning %d (%s) on mq_unlink.\n",errno, strerror (errno));
        }


   	mqAttr.mq_maxmsg = 10;
    	mqAttr.mq_msgsize = 1024;
    	rtos = mq_open (MQNAME, O_RDWR|O_CREAT, S_IWUSR|S_IRUSR, &mqAttr);

    	if (rtos < 0) {
        	printf ("Error %d (%s) on mq_open.\n",errno, strerror (errno));
        	exit (1);
    	}
    	printf ("Opened mqd_t of %d.\n", rtos);

	mqAttrack.mq_maxmsg = 10;
        mqAttrack.mq_msgsize = 1024;
        ack = mq_open (MQNAMEACK, O_RDWR|O_CREAT, S_IWUSR|S_IRUSR, &mqAttrack);

        if (ack < 0) {
                printf ("Error %d (%s) on mq_open.\n",errno, strerror (errno));
                exit (1);
        }
        printf ("Opened mqd_t of %d.\n", ack);


/*-------------------------CREATING A TEXT FILE TO SAVE THE DATA FOR TELEGRAM------------------*/

	fd = open("Notifier.txt", O_CREAT|O_RDWR, S_IRWXU);
	 if (fd < 0)
        {
                perror("Notifier.txt");
                exit(1);
        }

        close(fd);

/*----------------------------CREATING THREADS FOR READ AND WRITE FUNCTIONS--------------------*/

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
