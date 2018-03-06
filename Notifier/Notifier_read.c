#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>
#include<fcntl.h>
#include<errno.h>

#define MQNAME "/rtosq"
#define MQMESG "Notification sent!"

int main(int argc, char**argv)
{
	int rc, ret, fd;
	char buffer[2048];
	
	fd = open("Notifier", O_CREAT|O_RDONLY, S_IRWXU);
        printf("%d\n", fd);
        if(fd!=0){
	       // fprintf(stderr, "Error Number %d\n", errno);
        	perror("Program");
        }


	mqd_t rtos;

//	while(1){
    		rtos = mq_open (MQNAME, O_RDWR);
    		if (rtos < 0) {
        		printf ("Error %d (%s) on mq_open.\n",
        		errno, strerror (errno));
        		exit (1);
    		}	

    		printf ("Opened mqd_t of %d.\n", rtos);
		printf ("Waiting to Receiving on mqd_t %d.\n", rtos);

    		if(mq_receive (rtos, buffer, sizeof (buffer), NULL)){ 
			ret = system("telegram-send --stdin < Notifier \n");
			printf("Notification sent via telegram\n");
		}

    		else{
        		printf ("Error %d (%s) on mq_receive.\n",
        		errno, strerror (errno));
        		exit (1);
    		}
    		//printf ("Received [%s].\n", buffer);

  		rc = mq_send (rtos, MQMESG, sizeof (MQMESG), 1);
    		if (rc < 0) {
        		printf ("Error %d (%s) on mq_send.\n",
        		errno, strerror (errno));
        		exit (1);
    		}

    		printf("Ack sent successfully\n");
//	}


   	return 0;
}

