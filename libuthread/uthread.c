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

//Concurrency is the composition of independently executing tasks
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
//This function gets the currently active thread by calling uthread_current() 
//and gets the first thread in the ready queue by calling queue_dequeue() on the ready queue. 
//It sets the currently active thread's state to terminated and the dequeued thread's state to run. 
//It enqueues the thread held in the current block to the terminated queue and sets the currently active thread global variable to the thread dequeued from the ready queue.
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
//This function gets the currently active thread by calling uthread_current() 
//and gets the first thread in the ready queue by calling queue_dequeue() on the ready queue. 
//It sets the currently active thread's state to ready and the dequeued thread's state to run. 
//Lastly it sets the currently active thread to the ready dequeued thread and performs a context switch on the two threads by calling uthread_ctx_switch(current->context, next->context). 
//The context switch runs the switched thread's function and calls uthread_exit().
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

//This function first initializes the two globally set queues, readyQueue and terminatedQueue, by calling queue_create(). 
//Then it checks if the ready queue is empty; if so, it creates the main idle thread by allocating memory, allocating a context, 
//setting the TID to 0 and setting its state to running. 
//Then the initial thread is created by calling uthread_create() with the function and argument passed in. 
//Then a while loop is entered by checking if the readyQueue is not empty. In the while loop, uthread_yield() is called. 
//The while loop exits when the ready queue only has one thread in it and that thread is the main thread.
//Lastly, the terminated queue is dequeued one by one in a loop and the terminated queue's nodes are deleted using the following functions: 
//queue_delete(), uthread_ctx_destroy_stack(), and free().

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
