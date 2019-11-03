#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>

#define DATA_SIZE 20

pthread_mutex_t mutex;
pthread_cond_t cond_var;

struct shared_memory
{
	int content; 
	int read;
	int data[DATA_SIZE];
	int data_num;
};
  
struct shared_memory *shared;

//消费者线程函数

void *consumer(void *param) {
	pthread_mutex_lock(&mutex);

	if(shared -> data_num == 0) {
		pthread_mutex_unlock(&mutex);
	}
	else {
		int num = shared -> data[shared -> data_num - 1];
		printf("消费者线程ID为 : %lu, 消费者消费的数据为 : %d\n", pthread_self(), num);
		shared -> data[shared -> data_num - 1] = 0;
		shared -> data_num --;
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit(0);
}

int main(int argc, char *argv[]) {
	int* timestep = (int*) argv[1];

	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_var, NULL);

	void *shm = NULL;
	int shm_id;

	shm_id = shmget((key_t)4068, sizeof(struct shared_memory), 0666|IPC_CREAT);

	shm = shmat(shm_id, 0, 0);

	shared = (struct shared_memory*) shm;

	int permit;
	while(1) {
		permit = shared -> read;
		if(permit) {
			shared -> content = 0;

			//三个线程
			
			pthread_t consumer_1;
			pthread_t consumer_2;
			pthread_t consumer_3;

			pthread_create(&consumer_1, NULL, consumer, NULL);
			pthread_create(&consumer_2, NULL, consumer, NULL);
			pthread_create(&consumer_3, NULL, consumer, NULL);

			pthread_join(consumer_1, NULL);
			pthread_join(consumer_2, NULL);
			pthread_join(consumer_3, NULL);
			
			shared -> content = 1;
		}
		sleep(5);
	}
	return 0;
}
