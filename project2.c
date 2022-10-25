
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"
struct node {
	struct node* next;
	void* node_value;
};
struct queue {
	int queue_size;
	struct node* queue_first;
	struct node* queue_last;
	
};

queue_t queue_create(void)
{
	struct queue* Queue = (struct queue*)malloc(sizeof(struct queue));
	if(Queue != NULL){
	Queue->queue_size = 0;
	Queue->queue_first = NULL;
	Queue->queue_last = NULL;
	return Queue;
	}
	else{
	return NULL;
	}
}

int queue_destroy(queue_t queue)
{
	if(queue == NULL || queue->queue_size != 0){
	free(queue);
	queue= NULL;
	return 0;
	}
	else{
	return -1;
	}
}

int queue_enqueue(queue_t queue, void *data)
{
	if(data == NULL){
	return -1;
	}
	if(queue == NULL){
	return -1;
	}
	struct node*  Queue_node = (struct node*)malloc(sizeof(struct node));
	if(Queue_node == NULL){
	return -1;
	}
	else{
	Queue_node->node_value = data;
	Queue_node->next = NULL;
		if(queue->queue_size != 0){
			queue->queue_last->next = Queue_node;
			queue->queue_last = Queue_node;
		}
		else{
		queue->queue_last = Queue_node;
		queue->queue_first = Queue_node;
		}
	queue->queue_size++;	
	}
	return 0;
}

int queue_dequeue(queue_t queue, void **data)
{
	if(queue->queue_size == 0){
	return -1;
	}
	if(data == NULL){
        return -1;
        }
        if(queue == NULL){
        return -1;
        }
	else{
	 *data = queue->queue_first->node_value
                if(queue->queue_first != queue->queue_last){
                queue->queue_first = queue->queue_first->next;
                }
                else{
                queue->queue_first = NULL;
                queue->queue_last = NULL;
                }
        queue->queue_size--;
        }
        return 0;
}

int queue_delete(queue_t queue, void *data)
{
	/* Check if the queue is empty or not */
	if(queue->queue_size == 0){
        return -1;
        }
        if(data == NULL){
        return -1;
        }
        if(queue == NULL){
        return -1;
        }
	if(queue->queue_first == quueu->queue_last){
		return -1;
	}
	else{
	
	struct node*  Queue_node_current = queue->queue_first;
	struct node*  Queue_node_previous;
	
		if(Queue_node_current->node_value == data){
			free(Queue_node_previous);
			queue->queue_first = queue->queue_first->next;
			free(Queue_node_current);
			queue->queue_lengtj--;
			return 0;
		}
		else{
		while(Queue_node_current->next !=NULL){
		Queue_node_previous = Queue_node_current;
		Queue_node_current = Queue_node_current->next;
		if(Queue_node_current->node_value != data){
			if(Queue_node_current->next ==NULL)

		}
		


}

int queue_iterate(queue_t queue, queue_func_t func)
{
	 if(queue->queue_size == 0){
        return -1;
        }
        if(func == NULL){
        return -1;
        }
        if(queue == NULL){
        return -1;
        }
        if(queue->queue_first == quueu->queue_last){
                return -1;
        }










	
}

int queue_length(queue_t queue)
{
	if(queue != NULL)
		return queue->queue_size;
	else
		return -1;
}


