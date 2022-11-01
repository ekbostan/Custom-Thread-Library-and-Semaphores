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

// state definitions:
#define RUNNING 0
#define READY 1
#define BLOCKED 2
#define TERMINATED 3




struct uthread_tcb
{
    uthread_ctx_t *context;     // pointer to context- top of stack
    unsigned short TID;         // thread ID
    void *stack;                // checks to see if a stack exists for thread
    void (*startFunc)(void *);  // function of the thread
    void *argument;             // argument of the passed function
    int state;                  // defined state
    int ret;                    // return value
    

    /* TODO Phase 2 */
};

int TID;

static queue_t runningQueue;
static queue_t readyQueue;
static queue_t terminatedQueue;
// int queueSize = queue_length(queue);

void terminateThreads(struct uthread_tcb *block){
	queue_delete(terminatedQueue, (void**)&block);
	uthread_ctx_destroy_stack(block->context);
	free(block);
}

struct uthread_tcb *uthread_current(void)
{
    struct uthread_tcb  *front = runningQueue->queue_first->node_value;
    if(front == NULL){return NULL;}

    printf("front node value in current: %p\n", front);
  


    // finds first element in queue and returns it
    // check if first element of queue is set to running, if not set it to running
    return front;
    /* TODO Phase 2/3 */
}
void uthread_exit(void)
{
    /* TODO Phase 2 */
    // get current running thread from front of running queue
    struct uthread_tcb *current = uthread_current();
    struct uthread_tcb *next = readyQueue->queue_first->node_value;
    struct uthread_tcb *readyDequeue;
    struct uthread_tcb *runningDequeue;

    queue_dequeue(readyQueue,(void**)&readyDequeue);
   queue_dequeue(runningQueue, (void**)&runningDequeue); 
    next->state = RUNNING;
    queue_enqueue(runningQueue, next);
    // set current state to terminated
    current->state = TERMINATED;
    queue_enqueue(terminatedQueue, current);

  
    // yield to the next thread in the queue
    uthread_yield();
}
void uthread_yield(void)
{
	printf("entering yield function\n");
    // save current context of running thread from uthread_current function
    
    struct uthread_tcb *current = uthread_current();
    printf("current node: %p\n", current);
    struct uthread_tcb *next;
     printf("node next created in yield\n");
    if(queue_dequeue(readyQueue,(void**)&next)==-1){return;}
    printf("ready queue element dequeued in yield\n");

    if(current == NULL){return;}

    current->state = READY;
    queue_enqueue(readyQueue, current);
    printf("queued current queue into ready queue in yield\n");
    struct uthread_tcb *runningDequeue;
    queue_dequeue(runningQueue, (void**)&runningDequeue);
    printf("dequeued from running queue in yield\n");

    next->state = RUNNING;
    queue_enqueue(runningQueue, next);
    printf("enqueued next node into running queue in yield\n");
  
    // get the next value in the queue and save that context


    uthread_ctx_switch(current->context, next->context);
    printf("context switch occurred\n");
  
    // do a context switch between next in line and block dequeued to end of the line
}

int uthread_create(uthread_func_t func, void *arg)
{
    /* TODO Phase 2 */
   
    struct uthread_tcb *block = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
    void *stack = uthread_ctx_alloc_stack();

    block->context = malloc(sizeof(uthread_ctx_t));
    block->stack = stack;

    int create = uthread_ctx_init(block->context, stack, func, arg);
    
    if (create == 0)
    {
        queue_enqueue(readyQueue, block);
        block->TID = TID++;
        block->state = READY;
        block->startFunc = func;
        block->argument = arg;
     
        printf("Thread creation successful\n");
        return 0; // returns 0 if thread is created successfully
    }
    else
    {
        printf("Thread creation unsuccessful\n");
        return -1;
    }
}


int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	if(preempt == 0){printf("false preempt\n");}
	//initialize queues: 
	runningQueue = queue_create();
	readyQueue = queue_create();
	terminatedQueue = queue_create();
	printf("entering run function\n");
    printf("queue_first in initial run: %p\n", runningQueue->queue_first);
    if (runningQueue->queue_first == NULL)
    {
        struct uthread_tcb *mainThread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));

        mainThread->TID = TID;
        mainThread->state = RUNNING;
        mainThread->context = malloc(sizeof(uthread_ctx_t));
        void *stack1 = uthread_ctx_alloc_stack();
        // initialize context for main thread with null function and argument
        int create1 = uthread_ctx_init(mainThread->context, stack1, NULL, NULL);
        mainThread->stack = stack1;
        mainThread->startFunc = NULL;
        mainThread->argument = NULL;

        queue_enqueue(runningQueue, mainThread);

        if (create1 != 0)
        {
            printf("Main thread context initialization unsuccessful");
            return -1;
        }
	else{
		printf("Main thread creation successful\n");
    }
  
  
        if (uthread_create(func, arg) == -1)
        {
            printf("Failed to create new thread in run function\n");
        }
        else
        {
            printf("New thread creation in run function successful\n");
        }
   
     while(readyQueue != 0){
			
        uthread_yield();
	
     }
	while(terminatedQueue != 0){
		queue_iterate(terminatedQueue, &terminateThreads);
	}

	printf("terminated queue size: %d\n", terminatedQueue->queue_size);
    }
    return 0;
    // creating initial thread

     // uthread_create(func, arg)
     //
 }
