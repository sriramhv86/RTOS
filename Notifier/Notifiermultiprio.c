
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<time.h>
#include<sys/types.h>
#include<pthread.h>
#include<mqueue.h>
#include<sys/time.h>
#include<sched.h>

#define NUM_THREADS 6
#define MQNAME "/rtosq"
#define MQNAMEACK "/ack"
#define MQMESG "Tram leaves in 5 mins from now!"
#define MQMESGACK "Ack sent!"

int rc, rcack;
long duration=0.0;
struct mq_attr mqAttr, mqAttrack;
struct timeval tod1, tod2;
mqd_t rtos, ack;
pthread_t thread_array[NUM_THREADS];
pthread_attr_t rt_sched_attr; 
pthread_attr_t main_sched_attr; 
int rt_max_prio, rt_min_prio;
struct sched_param rt_param;
struct sched_param main_param;

/*------------- Write function: Checks time and sends a message to read function-----------------*/

void *mywritefunct(void *arg) 
{

	time_t t;
	struct tm tm;
	char buffer[2048];
     	printf("Entered write function\n");

	while(1){
		
		t = time(NULL);
                tm = *localtime(&t);

		if((tm.tm_sec%5)==0){
			gettimeofday(&tod1, NULL);
                        rc = mq_send (rtos, MQMESG, sizeof (MQMESG), 1);
                        if (rc < 0) 
			{
                                //printf ("Error %d (%s) on mq_send.\n", errno, strerror (errno));
                                exit (1);
                        }

                	//printf("Send successful\n");

                	//printf ("Waiting to receive ack\n");

                	mq_receive (ack, buffer, sizeof (buffer), NULL);
                        	//printf("Received ack going back to checking again\n");

                	//else{
                        //	printf ("Error %d (%s) on mq_receive.\n", errno, strerror (errno));
                        //exit (1);
                	//}
		}

        }


       	pthread_exit(NULL);
}

/*----------Read function: Receives message from write function to send Notification-------------*/

void *myreadfunct(void *arg)
{

	int ret, sz, fd;
	char buffer[2048];
		
	printf("Entered read function \n");
	fd = open("Notifier.txt", O_CREAT|O_RDWR, S_IRWXU);
        if (fd < 0)
        {
                perror("Notifier.txt");
                exit(1);
        }
	

 	while(1){

                //printf ("Waiting to receive message to send notification\n");

                if(mq_receive (rtos, buffer, sizeof (buffer), NULL)){
		gettimeofday(&tod2, NULL);
		duration =(tod2.tv_usec - tod1.tv_usec);
		printf("Time required to send the notification in usec is:%ld\n", duration);
			sleep(1); // This sleep is added to mimic the time taken by below 
				  // instruction as telegram-send doesn't work in sudo mode

                        //ret = system("telegram-send --stdin < Notifier.txt \n");
                        //printf("Notification sent via telegram\n");
                }

                else{
                        printf ("Error %d (%s) on mq_receive.\n", errno, strerror (errno));
                        exit (1);
                }
                //printf ("Received [%s].\n", buffer);

                rcack = mq_send (ack, MQMESGACK, sizeof (MQMESGACK), 1);
                if (rcack < 0) {
                        //printf ("Error %d (%s) on mq_send.\n", errno, strerror (errno));
                        exit (1);
                }

                //printf("Ack sent successfully\n");
       }

    	pthread_exit(NULL);
}

/*-----------------------------RANDOM FUNCTION TO KEEP A THREAD BUSY-----------------------------*/

void *randfunct1(void *arg)
{
	while(1){
  		double c = 0; 
		int n, m; 
		for (n = 0; n < 1000; n++)
    			for (m = 0; m < 1000; m++){
				c++;
        			c= c*c*c*c*c;
		}			
	}

        pthread_exit(NULL);
}


/*---------------------------------------------MAIN----------------------------------------------*/


int main(int argc, char *argv[])
{


  	int iter, i, rc, scope;
	
   	pthread_attr_init(&rt_sched_attr);
   	pthread_attr_init(&main_sched_attr);
   	pthread_attr_setinheritsched(&rt_sched_attr, PTHREAD_EXPLICIT_SCHED);
   	pthread_attr_setschedpolicy(&rt_sched_attr, SCHED_FIFO);
   	pthread_attr_setinheritsched(&main_sched_attr, PTHREAD_EXPLICIT_SCHED);
   	pthread_attr_setschedpolicy(&main_sched_attr, SCHED_FIFO);
   	rt_max_prio = sched_get_priority_max(SCHED_FIFO);
   	rt_min_prio = sched_get_priority_min(SCHED_FIFO);

   	rt_param.sched_priority = rt_min_prio;
   	pthread_attr_setschedparam(&rt_sched_attr, &rt_param);

  	main_param.sched_priority = rt_min_prio+1;
   	pthread_attr_setschedparam(&main_sched_attr, &main_param);



/*-----------------------CREATING TWO MESSAGE QUEUES FOR NOTIFICATION AND ACK--------------------*/

	//printf ("Unlinking if there is any queue present already.\n");
    	rc = mq_unlink (MQNAME);
    	if (rc < 0) {
        	printf ("Warning %d (%s) on mq_unlink.\n",errno, strerror (errno));
    	}
	rcack = mq_unlink (MQNAMEACK);
        if (rcack < 0) {
                printf ("Warning %d (%s) on mq_unlink.\n",errno, strerror (errno));
        }


   	mqAttr.mq_maxmsg = 1;
    	mqAttr.mq_msgsize = 1024;
    	rtos = mq_open (MQNAME, O_RDWR|O_CREAT, S_IWUSR|S_IRUSR, &mqAttr);

    	if (rtos < 0) {
        	printf ("Error %d (%s) on mq_open.\n",errno, strerror (errno));
        	exit (1);
    	}
    	//printf ("Opened mqd_t of %d.\n", rtos);

	mqAttrack.mq_maxmsg = 1;
        mqAttrack.mq_msgsize = 1024;
        ack = mq_open (MQNAMEACK, O_RDWR|O_CREAT, S_IWUSR|S_IRUSR, &mqAttrack);

        if (ack < 0) {
                printf ("Error %d (%s) on mq_open.\n",errno, strerror (errno));
                exit (1);
        }
        //printf ("Opened mqd_t of %d.\n", ack);


/*----------------------------CREATING THREADS FOR READ AND WRITE FUNCTIONS----------------------*/

    	if (pthread_create(&thread_array[0], &main_sched_attr, mywritefunct, (void *)0))
	{
      		printf("Error creating thread");
      		exit(-1);
    	}
	if (pthread_create(&thread_array[1], &rt_sched_attr, myreadfunct, (void *)1))
        {
                printf("Error creating thread");
                exit(-1);
        }
	if (pthread_create(&thread_array[2],  NULL, randfunct1, (void *)2))
        {
                printf("Error creating thread");
                exit(-1);
        }
	if (pthread_create(&thread_array[3],  NULL, randfunct1, (void *)3))
        {
                printf("Error creating thread");
                exit(-1);
        }
 	if (pthread_create(&thread_array[4],  NULL, randfunct1, (void *)4))
        {
                printf("Error creating thread");
                exit(-1);
        }
        if (pthread_create(&thread_array[5],  NULL, randfunct1, (void *)5))
        {
                printf("Error creating thread");
                exit(-1);
        }
/* 	if (pthread_create(&thread_array[6], NULL, randfunct1, (void *)6))
        {
                printf("Error creating thread");
                exit(-1);
        }
        if (pthread_create(&thread_array[7], NULL, randfunct2, (void *)7))
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
