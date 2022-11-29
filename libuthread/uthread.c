#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>


#include "private.h"
#include "uthread.h"
#include "queue.h"
#include "queue.c"
#include "context.c"
#include "preempt.c"
#include "sem.c"
#include "sem.h"

// state definitions:
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define TERMINATED 3

//Concurrency is the composition ofindependently executing tasks
//Single execution sequence that represents a separately schedulable task

struct uthread_tcb
{
    uthread_ctx_t *context;     // pointer to context- top of stack
    int  TID;         		// thread ID
    int state;                  // defined state

   
    

    
};

int TID;
struct uthread_tcb* currentThread;
struct uthread_tcb* mainBlock;

static queue_t readyQueue;
static queue_t terminatedQueue;


int find_tid_in_queue(queue_t q){
        struct node* curNode = q->queue_first;

        
		return ((struct uthread_tcb*)curNode->node_value)->TID;
	
}

struct uthread_tcb *uthread_current(void)
{
   
    // finds first element in queue and returns it
    // check if first element of queue is set to running, if not set it to running
    return currentThread;
    
}
void uthread_exit(void)
{
    
    // get current running thread from front of running queue
    struct uthread_tcb *current = uthread_current();
    struct uthread_tcb *next;
    queue_dequeue(readyQueue,(void**)&next);
    next->state = RUNNING;
    currentThread= next;
    // set current state to terminated
    current->state = TERMINATED;
    queue_enqueue(terminatedQueue, current);
    // yield to the next thread in the queue
    uthread_yield();
}
void uthread_yield(void)
{
    // save current context of running thread from uthread_current function
    
    struct uthread_tcb *current=uthread_current();
    struct uthread_tcb *next;
    if(queue_dequeue(readyQueue,(void**)&next)==-1){return;}
    
    if(current == NULL){return;}

    current->state = READY;
    queue_enqueue(readyQueue, current);
    next->state = RUNNING;
    currentThread = next;
    // get the next value in the queue and save that context

    uthread_ctx_switch(current->context, next->context);
    // do a context switch between next in line and block dequeued to end of the line
}

int uthread_create(uthread_func_t func, void *arg)
{
    struct uthread_tcb *block = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
    void *stack = uthread_ctx_alloc_stack();
    block->context = malloc(sizeof(uthread_ctx_t));

    int create = uthread_ctx_init(block->context, stack, func, arg);
    if (create == 0)
    {
        queue_enqueue(readyQueue, block);
        block->TID = TID++;
        block->state = READY;
      
     
        return 0; // returns 0 if thread is created successfully
    }
    else
    {
        return -1;
    }
   
}


int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	if(preempt == 0){preempt=1;}
	//initialize queues: 
	
	readyQueue = queue_create();
	terminatedQueue = queue_create();

    if (readyQueue->queue_first == NULL)
    {
        struct uthread_tcb *mainThread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
        mainThread->TID = TID;
        mainThread->state = RUNNING;
        mainThread->context = malloc(sizeof(uthread_ctx_t));
	currentThread=mainThread;
        mainBlock = mainThread;	
    }
  
	uthread_create(func, arg);
     	while(readyQueue->queue_size != 0){
			
        uthread_yield();
	if((readyQueue->queue_size == 1)&&(find_tid_in_queue(readyQueue) == 0)){
		struct uthread_tcb last;
		queue_dequeue(readyQueue, (void**)&last);}
		currentThread = mainBlock;
     }
for(int i = terminatedQueue->queue_size;i>0;i--){
	struct uthread_tcb* hold; 
       	queue_dequeue(terminatedQueue,(void**)&hold);
	queue_delete(terminatedQueue,hold);
	uthread_ctx_destroy_stack(hold->context);
        free(hold);
}

    return 0;
    
 }
void uthread_block(void)
{
	struct uthread_tcb *current = uthread_current();
	current->state = BLOCKED;
	struct uthread_tcb *next;
	queue_dequeue(readyQueue,(void**)&next);
	uthread_ctx_switch(current->context, next->context);
	/* TODO Phase 3 */
	//set current thread equal to blocked state 
	//scheduling- make next thread to be current and do 
	//context switch 

}

void uthread_unblock(struct uthread_tcb *uthread)
{
	uthread->state = READY;
	queue_enqueue(readyQueue, uthread);
	//set state to ready and put it back in queue 
}
