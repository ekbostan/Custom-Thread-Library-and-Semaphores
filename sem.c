#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	size_t internal_count;
	queue_t sem_queue;
};
 int test_and_set(int mem)
{
    int oldval = mem;
    mem = 1;
    return oldval;
}
void spinlock_lock(int lock)
{
    while (test_and_set(lock) == 1);
}
void spinlock_unlock(int lock)
{
    lock = 0;
}

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


	spinlock_lock(sem->internal_count);
	while(sem ->internal_count == 0){
	pthread_t id = currentThread->TID;
	queue_enqueue(sem->sem_queue, (void*)id);
	uthread_block();
	
	
	}
	sem->internal_count--;
	spinlock_unlock(sem->internal_count);
	
	return 0;
	}

int sem_up(sem_t sem)
{
	if(sem == NULL){
        return -1;
        }

	spinlock_lock(sem->internal_count);
	sem->internal_count++;
	if(sem->internal_count >0){
	pthread_t tid,
	queue_dequeue(sem->sem_queue,(void*)tid);
	uthread_unblock(tid);
	
	}
	spinlock_unlock(sem->internal_count);

	return 0;

}
