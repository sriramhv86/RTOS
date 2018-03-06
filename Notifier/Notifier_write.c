#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>

#define MQNAME "/rtosq"
#define MQMESG "Hello there!"

int main(int argc, char**argv)
{

	mqd_t rtos;
        int rc;
        char buffer[2048];
	
//	while(1){
        	printf ("Sending message.\n");

        	rtos = mq_open (MQNAME, O_RDWR);
        	if (rtos < 0) {
        		printf ("Error %d (%s) on mq_open.\n",
        		errno, strerror (errno));
        		exit (1);
        	}

        	printf ("Opened mqd_t of %d.\n", rtos);


		//if(##write your condition){
        		rc = mq_send (rtos, MQMESG, sizeof (MQMESG), 1);
        		if (rc < 0) {
        			printf ("Error %d (%s) on mq_send.\n",
        			errno, strerror (errno));
        			exit (1);
        		}

        	printf("Send successful\n");

        	sleep(2);

        	printf ("Waiting to Receiving on mqd_t %d.\n", rtos);

        	if(mq_receive (rtos, buffer, sizeof (buffer), NULL))
        		printf("Notiffication successfully sent going back to checking again\n");

        	else{
        		printf ("Error %d (%s) on mq_receive.\n",
        		errno, strerror (errno));
        		exit (1);
        	}
//        	printf ("Received [%s].\n", buffer);

        	mq_close (rtos);
//	}

        return 0;
}
