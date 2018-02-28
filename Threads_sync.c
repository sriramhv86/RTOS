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

#define NUM_THREADS 3

pthread_mutex_t sync_mutex = PTHREAD_MUTEX_INITIALIZER;

float *array;


struct weather
{
        float temperature;
        char unit;
        struct timeval timestamp;
};

void *myfunction(void *arg){

		struct timeval tod1;
		struct timeval tod2;
                int rc, j, count1;
                float tim1, tim2, time=0;
                float avg_time=0, temp_sum=0, avg_temp=0;
                float temp[200];

		//pthread_mutex_lock(&sync_mutex);//This is the changed version
                for(j=0; j<=199; j++){
                        rc = gettimeofday(&tod1, NULL);
			pthread_mutex_lock(&sync_mutex);
                        temp[j] = array[j];
			pthread_mutex_unlock(&sync_mutex);
                        //printf("this is to check\n");
                        rc = gettimeofday(&tod2, NULL);
                        //printf("from thread:%d value is %f\n",(int)arg, temp[j]);
                        time += ((tod2.tv_sec - tod2.tv_sec)*1000000) + (tod2.tv_usec - tod1.tv_usec);
                }
		//pthread_mutex_unlock(&sync_mutex);
                for(j=0; j<=199; j++){
                        temp_sum = temp_sum + temp[j];
                }
                avg_temp = temp_sum/200;
                avg_time = time/200;
                printf("I am thread with id:%d Average temperature:%f Total time in usec:%.2f Average time in usec:%.2f\n", (int)arg, avg_temp, time, avg_time);
                pthread_exit(NULL);
        }


int main(int argc, char *argv[])
{

	pthread_t thread_array[NUM_THREADS];
	array = (float *)malloc(200*sizeof(float));
	struct weather w;
	int ret_value, count, count1, iter, i, fd, size, shmid, parentid, childpid;

	fd = open("DataFile.txt", O_RDONLY);
	 if (fd < 0)
        {
                perror("DataFile.txt");
                exit(1);
        }
        else
	{

                while((size= read(fd,&w,sizeof(struct weather)) !=0))
                {
                       array[count] =  w.temperature;
                        count++;
                }
		printf("Total no of data read from the file %d\n", count);
        }

        close(fd);
	for(iter=0; iter<NUM_THREADS ; iter++)
	{
    		if (pthread_create(&thread_array[iter], NULL, myfunction, (void *)iter))
		{
      			printf("Error creating thread");
      			exit(-1);
    		}
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
