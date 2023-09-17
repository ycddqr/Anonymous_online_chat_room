#include "threadpool.h"
#include "queue.h"
#include <stdlib.h>
#include <stdio.h>

//	生产者线程入口函数
static void *pro_run(void *arg)
{
	ThreadPool *thread = (ThreadPool *)arg;
	for(;;)
	{
		//	调用生产者业务逻辑函数 生产出数据
		void *data = thread->pro_work();
		if(NULL == data) continue;
		//	放入仓库
		push_threadpool(thread,data);
	}
	return NULL;
}
//	消费者线程入口函数
static void *con_run(void *arg)
{
	ThreadPool *thread = (ThreadPool *)arg;
	for(;;)
	{
		//	从仓库中获取数据
		void *data = pop_threadpool(thread);
		//	调用消费者业务逻辑函数
		thread->con_work(data);
	}
	return NULL;
}

//	创建线程池并初始化
ThreadPool *create_threadpool(int con_cnt,int pro_cnt,int store_cal,con_WorkFP con_fp,pro_WorkFP pro_fp)
{
	//	申请线程池结构内存
	ThreadPool *pool = malloc(sizeof(ThreadPool));
	//	初始化成员
	pool->con_thread_cnt = con_cnt;
	pool->pro_thread_cnt = pro_cnt;
	pool->con_tids = malloc(sizeof(pthread_t)*con_cnt);
	pool->pro_tids = malloc(sizeof(pthread_t)*pro_cnt);
	//	创建仓库
	pool->store = create_array_queue(store_cal);
	//	入口函数
	pool->con_work = con_fp;
	pool->pro_work = pro_fp;
	//	初始化互斥量和条件变量
	pthread_mutex_init(&pool->hlock,NULL);
	pthread_mutex_init(&pool->tlock,NULL);
	pthread_cond_init(&pool->empty,NULL);
	pthread_cond_init(&pool->full,NULL);
	return pool;
}

//	启动线程池
void start_threadpool(ThreadPool *thread)
{
	//	启动生产者线程
	for(int i = 0; i < thread->pro_thread_cnt; i++)
	{
		pthread_create(thread->pro_tids+i,NULL,pro_run,thread);
	}
	//	启动消费者线程
	for(int i = 0; i < thread->con_thread_cnt; i++)
	{
		pthread_create(thread->con_tids+i,NULL,con_run,thread);
	}
}

//	生产数据
void push_threadpool(ThreadPool *thread,void *data)
{
	//	队尾上锁
	pthread_mutex_lock(&thread->tlock);
	//	如果一直队满，睡眠不生产
	while(full_array_queue(thread->store))
	{
		//	唤醒消费者线程
		pthread_cond_signal(&thread->empty);
		//	睡入满仓的条件变量，解锁队尾
		pthread_cond_wait(&thread->full,&thread->tlock);
	}
	//	把数据存入仓库
	push_array_queue(thread->store,data);
	//	唤醒消费者线程
	pthread_cond_signal(&thread->empty);
	//	解锁队尾
	pthread_mutex_unlock(&thread->tlock);
}

//	消费数据
void *pop_threadpool(ThreadPool *thread)
{
	//	队头上锁
	pthread_mutex_lock(&thread->hlock);
	//	如果一直队空，睡眠
	while(empty_array_queue(thread->store))
	{
		//	唤醒生产者线程
		pthread_cond_signal(&thread->full);
		//	睡入空仓的条件变量，解锁队头
		pthread_cond_wait(&thread->empty,&thread->hlock);
	}
	//	把数据从仓库取出
	void *data = head_array_queue(thread->store);
	pop_array_queue(thread->store);
	//	唤醒生产者线程
	pthread_cond_signal(&thread->full);
	//	解锁队头
	pthread_mutex_unlock(&thread->hlock);
	return data;
}

//	销毁线程池
void destroy_threadpool(ThreadPool *thread)
{
	//	杀死生产者，消费者线程
	for(int i = 0; i < thread->pro_thread_cnt; i++)
	{
		pthread_cancel(thread->pro_tids[i]);
//		pthread_join(thread->pro_tids[i],NULL);
	}
	for(int i = 0; i < thread->con_thread_cnt; i++)
	{
		pthread_cancel(thread->con_tids[i]);
//		pthread_join(thread->con_tids[i],NULL);
	}
	free(thread->pro_tids);
	free(thread->con_tids);
	pthread_mutex_destroy(&thread->hlock);
	pthread_mutex_destroy(&thread->tlock);
	pthread_cond_destroy(&thread->empty);
	pthread_cond_destroy(&thread->full);
	destroy_array_queue(thread->store);

	free(thread);
}

