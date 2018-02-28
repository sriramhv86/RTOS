#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <dirent.h>

struct weather
{
	float temperature;
	char unit;
	struct timeval timestamp;
};


int main(int argc, char**argv)
{
	struct weather w;
	float *buf = (float *)malloc(100*sizeof(float));
	int fd, size;
	float average, num;
	int count=0, i;
	fd = open("DataFile.txt", O_RDONLY);

	if (fd < 0)
	{
		perror("DataFile.txt");
    		exit(1);
	}

	else
	{
		printf("Temperature\t\tUnits\t\tTimesec\n");

		while((size= read(fd,&w,sizeof(struct weather)) !=0))
		{
			buf[count] =  w.temperature;
			printf("%f\t\t%c\t\t%ld.%ld\n",w.temperature,w.unit,w.timestamp.tv_sec,w.timestamp.tv_usec);
			count++;
		}

		printf("Total no of bytes read is:%d\n",count);
	}

	close(fd);
	//average = num/count;
	//printf("%f",average);
	for(i=count-1;i>=0;i--)
	{
		//printf("buffer %d contains %f\n",i, buf[i]);
		num+=buf[i];
	}
	average = num/count;
	printf("Average temperature is:%f\n", average);
//	free(buf);
	return 0;
}
