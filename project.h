#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct node {
	void* value;
	struct node* next;
};

struct queue {
	int count;
	struct node* head;
	struct node* back;
};
typedef struct queue* queue_t;

queue_t queue_create(void);
int queue_destroy(queue_t queue);
int queue_enqueue(queue_t queue, void *data);
int queue_dequeue(queue_t queue, void **data);
int queue_delete(queue_t queue, void *data);
typedef int (*queue_func_t)(queue_t queue, void *data, void *arg);
int queue_iterate(queue_t queue, queue_func_t func, void *arg, void **data);
int queue_length(queue_t queue);

queue_t queue_create(void) {
	// Initialize queue by setting up the head/tail and start counting number elements
	struct queue* newQueue = (struct queue*)malloc(sizeof(struct queue));
	if(newQueue == NULL)
		return NULL;

	newQueue->head = NULL;
	newQueue->back = NULL;
	newQueue->count = 0;
	return newQueue;
}

int queue_destroy(queue_t queue) {
	// Confirm whether the queue is empty before deletion
	if (queue->head == NULL && queue->back == NULL) {
		free(queue);
		return 0;
	}
	return -1;
}

int queue_enqueue(queue_t queue, void *data) {
	struct node* newNode;
	void* newVal;
	
	if(queue == NULL || data == NULL)
		return -1;

	newVal= data;
	newNode = (struct node*)malloc(sizeof(struct node));
	if(newNode == NULL)
		return -1;

	newNode->next = NULL;
	newNode->value = newVal;

	// If the queue is empty, the enqueued value becomes the head
	if(queue->head == NULL) {
		queue->head = newNode;
		queue->back = newNode;
		queue->count++;
	} else { // Else, the new value is added to the end of the queue to implement FIFO
		queue->back->next = newNode;
		queue->back = queue->back->next;
		queue->count++;
	}
	return 0;
}

int queue_dequeue(queue_t queue, void **data) {
	if(queue == NULL || queue->count == 0 || data == NULL)
		return -1;

	// If there is only one element in the queue, then dequeue it and ensure both head and back are null
	if(queue->head == queue->back) {
		*data = queue->head->value;
		queue->head = NULL;
		queue->back = NULL;
		queue->count--;
	}
	else { // Else, take remove the value of the head and shift the head to the next node
		*data = queue->head->value;
		queue->head = queue->head->next;
		queue->count--;
	}
	return 0;
}

int queue_delete(queue_t queue, void* data) {
	if(queue == NULL || data == NULL)
		return -1;

	struct node* currentNode = queue->head;
	// If the beginning of the queue is the item to delete, reassign head
	if(currentNode->value == data) {
		queue->head = queue->head->next;
		free(currentNode);
		queue->count--;
		return 0;
	}
	else { // Else check whether the next nodes match
		while(currentNode->next != NULL) {
			if(currentNode->next->value == data) {
				currentNode->next = currentNode->next->next;
				queue->count--;
				currentNode = NULL;
				free(currentNode);
				return 0;
			} else {
				// Check whether to keep going through the queue
				if(currentNode->next->next != NULL) 
					currentNode = currentNode->next;
				else 
					break;
			}
		}
	}
	currentNode = NULL;
	free(currentNode);
	return -1;
}

int queue_iterate(queue_t queue, queue_func_t func, void* arg, void** data) {
	if(queue == NULL || func == NULL || queue->head == NULL)
		return -1;
	
	struct node* currentNode = queue->head;
	struct node* nextNode = queue->head->next;
	// Run the function on the head; if a 1 is returned (TRUE), then data should not be null
	// Store returned value in data
	if(func(queue, currentNode->value, arg)) {
		if(data != NULL) {
			*data = currentNode->value;
			return 0;
		}
	}

	// Check if there is more than one node present in your queue 
	if(nextNode == NULL) 
        return 0;
    
	currentNode = nextNode;
	nextNode = nextNode->next;

	while (currentNode != NULL) {
		if (func(queue, currentNode->value, arg)) {
			if (data != NULL) {
				*data = currentNode->value;
				return 0;
			}
		}

		// Reassign current node if it was deleted
		if(currentNode->value == NULL) {
			currentNode = nextNode;
			nextNode = nextNode->next;
		}

		currentNode = currentNode->next;
		if(nextNode->next != NULL)
			nextNode = nextNode->next;
	}
	return 0;
}

int queue_length(queue_t queue) {
	if (queue == NULL)
		return -1;
	return queue->count;
}
