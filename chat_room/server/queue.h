#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//	顺序队列结构
typedef struct ArrayQueue
{
	void **ptr;
	size_t cal;
	size_t front;	//	队头位置
	size_t rear;	//	队尾位置
	size_t cnt;
}ArrayQueue;

//	创建顺序队列
ArrayQueue* create_array_queue(size_t cal);

//	销毁
void destroy_array_queue(ArrayQueue* queue);

//	队空
bool empty_array_queue(ArrayQueue* queue);

//	队满
bool full_array_queue(ArrayQueue* queue);
//	入队
bool push_array_queue(ArrayQueue* queue,void *data);

//	出队
bool pop_array_queue(ArrayQueue* queue);

//	查看队头
void *head_array_queue(ArrayQueue* queue);

//	查看队尾
void *tail_array_queue(ArrayQueue* queue);

//	查看数量
size_t size_array_queue(ArrayQueue* queue);

#endif
