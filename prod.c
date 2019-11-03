#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>

#define DATA_SIZE 20

pthread_mutex_t mutex;
pthread_cond_t cond_var;
//共享内存结构体
struct shared_memory
{
	int content; 
	int read;
	int data[DATA_SIZE];
	int data_num;
};

struct shared_memory *shared;
//线程函数
void *producer(void *param) {
	pthread_mutex_lock(&mutex);

	if(shared -> data_num == 20) {
		pthread_mutex_unlock(&mutex);
	}
	else {
		int num = (rand() % 9)+1; //随机产生数据
		printf("生产者线程id为 : %lu, 生产出的数据为 : %d\n", pthread_self(), num);
		shared -> data[shared -> data_num - 1] = num;
		shared -> data_num ++;
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

	shared -> content = 1;
	shared -> read = 0;

	int permit;
	while(1) {
		permit = shared -> content;
		if(permit) {
			shared -> read = 0;
			
			//三个线程
			
			pthread_t producer_1;
			pthread_t producer_2;
			pthread_t producer_3;

			pthread_create(&producer_1, NULL, producer, NULL);
			pthread_create(&producer_2, NULL, producer, NULL);
			pthread_create(&producer_3, NULL, producer, NULL);

			pthread_join(producer_1, NULL);
			pthread_join(producer_2, NULL);
			pthread_join(producer_3, NULL);
			
			shared -> read = 1;
		}
		sleep(5);
	}
	return 0;
}

