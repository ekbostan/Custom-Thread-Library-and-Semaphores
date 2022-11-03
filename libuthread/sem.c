#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"
#include "uthread.h"

struct semaphore {
	size_t internal_count;
	queue_t sem_queue;

};

sem_t sem_create(size_t count)
{
	sem_t sem_lock = (sem_t)malloc(sizeof(struct semaphore));
	if(sem_lock == NULL){
		return NULL;
	}
	else{
		sem_lock->internal_count = count;
		sem_lock->sem_queue = queue_create();
		return sem_lock;
	}
}

int sem_destroy(sem_t sem)
{
	if(sem == NULL || sem->internal_count > 0){
	
	return -1;
	}
	else{
	queue_destroy(sem->sem_queue);
	free(sem);
	return 0;
	
	}
}

int sem_down(sem_t sem)
{
	if(sem == NULL){
		return -1;
	}
	if(sem ->internal_count == 0){
		struct uthread_tcb* block = uthread_current();
		queue_enqueue(sem->sem_queue, block);
		uthread_block();
	}
	else{
		sem->internal_count--;
	}
	return 0;
}

int sem_up(sem_t sem)
{
	if(sem == NULL){
        	return -1;
        }
	sem->internal_count++;
	if(sem->internal_count > 0){
		struct uthread_tcb *upBlock;
		queue_dequeue(sem->sem_queue,(void**)&upBlock);
		uthread_unblock(upBlock);
	
	}
	
	return 0;
}
