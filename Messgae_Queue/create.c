#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <mqueue.h>

#define MQNAME "/rtosq"

int main(int argc, char**argv)
{

    int rc;
    mqd_t rtos;
    struct mq_attr mqAttr;

    printf ("Unlinking if there is any present queue.\n");
    rc = mq_unlink (MQNAME);
    if (rc < 0) {
        printf ("Warning %d (%s) on mq_unlink.\n",
            errno, strerror (errno));
    }

    mqAttr.mq_maxmsg = 10;
    mqAttr.mq_msgsize = 1024;
    rtos = mq_open (MQNAME, O_RDWR|O_CREAT, S_IWUSR|S_IRUSR, &mqAttr);
    if (rtos < 0) {
        printf ("Error %d (%s) on mq_open.\n",
            errno, strerror (errno));
        exit (1);
    }
    printf ("Opened mqd_t of %d.\n", rtos);

    return 0;

}
