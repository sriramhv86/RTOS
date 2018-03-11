#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>

#define MQNAME "/rtosq"

int main(int argc, char**argv)
{
    mqd_t rtos;
    rtos = mq_open (MQNAME, O_RDWR);
    if (rtos < 0) {
        printf ("Error %d (%s) on mq_open.\n",
            errno, strerror (errno));
        exit (1);
    }
    printf ("Opened mqd_t of %d.\n", rtos);

    int rc;
    char buffer[2048];
    printf ("Waiting to Receiving on mqd_t %d.\n", rtos);
    rc = mq_receive (rtos, buffer, sizeof (buffer), NULL);
    if (rc < 0) {
        printf ("Error %d (%s) on mq_receive.\n",
            errno, strerror (errno));
        exit (1);
    }
    printf ("Received [%s].\n", buffer);

   return 0;
}

