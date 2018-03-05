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
    printf ("Sending message.\n");
    rtos = mq_open (MQNAME, O_RDWR);
    if (rtos < 0) {
        printf ("Error %d (%s) on mq_open.\n",
            errno, strerror (errno));
        exit (1);
    }
    printf ("Opened mqd_t of %d.\n", rtos);

    rc = mq_send (rtos, MQMESG, sizeof (MQMESG), 1);
    if (rc < 0) {
        printf ("Error %d (%s) on mq_send.\n",
            errno, strerror (errno));
        exit (1);
    }

    printf("Send successful\n");

    mq_close (rtos);

    return 0;
}
