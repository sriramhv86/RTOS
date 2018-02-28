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

struct weather
{
        float temperature;
        char unit;
        struct timeval timestamp;
};


int main(int argc, char *argv[])
{

	struct timeval tod1;
	struct timeval tod2;
	struct weather w;
	int count=0, count1=0, rc, i, j, fd, size, shmid, parentid, childpid;
	float *shm;
	float array[200];
	key_t key;
	pid_t pid1, pid2;

	fd = open("DataFile.txt", O_RDONLY);
	 if (fd < 0)
        {
                perror("DataFile.txt");
                exit(1);
        }
        else
	{
                //printf("Temperature\t\tUnits\t\tTimesec\n");

                while((size= read(fd,&w,sizeof(struct weather)) !=0))
                {
                       array[count] =  w.temperature;
                       // printf("%f\t\t%c\t\t%ld\n",w.temperature,w.unit,w.timestamp);
                        count++;
                }
		printf("count of data read from the file %d\n", count);
        }

        close(fd);
        parentid = getpid();
        printf("I am the parent with id %d\n", parentid);

        key = 9876;
        shmid = shmget(key, 800, IPC_CREAT | 0666);
        if(shmid<0)
        {
                perror("shmget");
                exit(1);
        }

        shm = shmat(shmid, NULL, 0);

        if(shm == (float *) -1)
         {
                perror("shmat");
                exit(1);
        }

	shm = shmat(shmid, NULL, 0);
	for(i=0; i<=count-1; i++)
	{
		shm[i]=array[i];
	}
	shmdt(shm);


	pid1 = fork();
	pid2 = fork();
	if(getpid()!=parentid){

		int count1;
		float tim1, tim2, time;
		float avg_time=0, temp_sum=0, avg_temp=0;
		float temp[200];
		//childpid = getpid();
		//printf("I am the child with id %d\n", getpid());
		shm = shmat(shmid, NULL, 0);
		if(shm == (float *) -1)
		{
			perror("shmat");
			exit(1);
		}
		for(j=0; j<=199; j++){
			rc = gettimeofday(&tod1, NULL);
			temp[j] = shm[j];
			//printf("this is to check\n");
			rc = gettimeofday(&tod2, NULL);
			//printf("value is %f\n", temp[j]);
                	time += ((tod2.tv_sec - tod2.tv_sec)*1000000) + (tod2.tv_usec - tod1.tv_usec);
		}
		for(j=0; j<=199; j++){
			temp_sum = temp_sum + temp[j];
		}
		avg_temp = temp_sum/200;
		//printf("Total time %f\n", time);
		avg_time = time/200;
		printf("I am child with id:%d Average temp:%f Total time in usec:%.2f Average time in usec:%.2f  \n", getpid(), avg_temp, time, avg_time);
		//printf("The average time taken by child_process 1: %f\n", avg_time);
		exit(0);
	}


	wait(pid1);
	wait(pid2);
	printf("I am the parent with id:%d and all my children are dead :(\n", parentid);


	return 0;

}
