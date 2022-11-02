#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	int internal_count;
};

sem_t sem_create(size_t count)
{
	sem_t sem_lock = (sem_t)malloc(sizeof(struct semaphore));
	if(sem_lock == NULL){
	return NULL;
	}
	else{
	sem_lock->internal_count = count;
	return sem_lock;
	}
}

int sem_destroy(sem_t sem)
{
	if(sem == NULL || sem->internal_count == 0){
	
	return -1;
	}
	else{
	
	free(sem);
	return 0;
	
	}
}

int sem_down(sem_t sem)
{
	if(sem == NULL){
	return -1;
	}


	spinlock_lock(sem->lock);
	while(sem ->internal_count == 0){
	
	
	
	}
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
	return 0;

}
