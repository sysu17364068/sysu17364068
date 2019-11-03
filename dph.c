#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_cond_t cond_var;

//初始化哲学家结构体
typedef struct Philosopher 
{
	int num;	//哲学家代号
	int condition;  //为0时表示正在思考，为1时表示正在吃饭
	int chop_z;	//为0时表示未被使用，为1时表示正在使用（左边的筷子）
	int chop_y;	//右边的筷子
	struct Philosopher* left_ph;	//左边的哲学家
	struct Philosopher* right_ph;	//右边的哲学家
	pthread_t act;			//线程代号
} Phil;

//哲学家开始吃饭
void pickup_forks(Phil* ph) 
{
	pthread_mutex_lock(&mutex);
	if(ph->chop_z==0 && ph->chop_y==0) //如果左右筷子都空闲，开始吃饭
	{
		ph -> chop_z = 1;
		ph -> chop_y = 1;
		ph -> condition = 1;
		ph -> left_ph -> chop_y = 1;
		ph -> right_ph -> chop_z = 1;
		printf("%d号哲学家正在吃饭\n", ph->num);
		sleep(2);
	}
	else 				   //如果不空闲，开始等待
	{
		pthread_cond_wait(&cond_var, &mutex);
		pickup_forks(ph);
	}
	pthread_mutex_unlock(&mutex);
}

//哲学家停止吃饭
void return_forks(Phil* ph) 
{
	int thinking = 0;
	thinking = rand() % 5 + 1;	//取1~3秒随机数
	pthread_mutex_lock(&mutex);
	ph -> chop_z = 0;		//将左右筷子都放下
	ph -> chop_y = 0;
	ph -> condition = 0;
	ph -> left_ph -> chop_y = 0;
	ph -> right_ph -> chop_z = 0;

	printf("%d号哲学家正在思考\n", ph->num);
	pthread_cond_signal(&cond_var);
	pthread_mutex_unlock(&mutex);
	sleep(thinking);
}

void* behave(void *arg) 
{
	Phil* ph = (Phil*) arg;
	while(1) 
	{
		printf("%d号哲学家感到饥饿\n", ph->num);
		pickup_forks(ph);
		return_forks(ph);
	}
}

int main(void) 
{
	Phil* first = NULL;
	//初始化结构体
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_var, NULL);
	first = (Phil*) malloc(sizeof(Phil));
	first -> num = 0;
	first -> condition = 0;
	first -> chop_z = 0;
	first -> chop_y = 0;
	first -> left_ph = NULL;
	first -> right_ph = NULL;
	Phil* ph = NULL;
	Phil* tail = first;
	for(int i=0; i<4; i++) 
	{
		ph = (Phil*) malloc(sizeof(Phil));
		ph -> num = i+1;
		ph -> condition = 0;
		ph -> chop_z = 0;
		ph -> chop_y = 0;
		ph -> left_ph = NULL;
		ph -> right_ph = NULL;

		tail -> right_ph = ph;
		tail = tail -> right_ph;
		tail -> left_ph = ph;
	}
	tail -> right_ph = first;
	first -> left_ph = tail;

	ph = first;
	
	//启动多线程
	for(int j=0; j<5; j++) 
	{
		pthread_create(&(ph->act), NULL, behave, (void*)ph);
		ph = ph -> right_ph;
	}

	Phil* pher = first;
	for(int i=0; i<5; i++) 
	{
		pthread_join(pher->act, NULL);
		pher = pher -> right_ph;
	}
	return 0;
}
