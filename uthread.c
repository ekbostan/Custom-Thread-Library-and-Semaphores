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
   int  TID;         // thread ID
    void *stack;                // checks to see if a stack exists for thread
    void (*startFunc)(void *);  // function of the thread
    void *argument;             // argument of the passed function
    int state;                  // defined state
    int ret;                    // return value
    

    /* TODO Phase 2 */
};

int TID;
struct uthread_tcb* currentThread;
struct uthread_tcb* mainBlock;
//static queue_t runningQueue;
static queue_t readyQueue;
static queue_t terminatedQueue;
// int queueSize = queue_length(queue);

void terminateThreads(struct uthread_tcb *block){
	//queue_delete(terminatedQueue,(void*)&block);
	uthread_ctx_destroy_stack(block->context);
	free(block);


}
int find_tid_in_queue(queue_t q){
        struct node* curNode = q->queue_first;

        
		return ((struct uthread_tcb*)curNode->node_value)->TID;
	
}

struct uthread_tcb *uthread_current(void)
{
   // struct uthread_tcb  *front = runningQueue->queue_first->node_value;
   // if(front == NULL){return NULL;}
  

    // finds first element in queue and returns it
    // check if first element of queue is set to running, if not set it to running
    return currentThread;
    /* TODO Phase 2/3 */
}
void uthread_exit(void)
{
    /* TODO Phase 2 */
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
    //struct uthread_tcb *runningDequeue;
    //queue_dequeue(runningQueue, (void**)&runningDequeue);
    next->state = RUNNING;
    //queue_enqueue(runningQueue, next);
  currentThread = next;
    // get the next value in the queue and save that context


    uthread_ctx_switch(current->context, next->context);
   
  
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
	
	readyQueue = queue_create();
	terminatedQueue = queue_create();
    if (readyQueue->queue_first == NULL)
    {
        struct uthread_tcb *mainThread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));

        mainThread->TID = TID;
        mainThread->state = RUNNING;
       mainThread->context = malloc(sizeof(uthread_ctx_t));
       // void *stack1 = uthread_ctx_alloc_stack();
        // initialize context for main thread with null function and argument
        //int create1 = uthread_ctx_init(mainThread->context, stack1, NULL, NULL);
       // mainThread->stack = stack1;
       // mainThread->startFunc = NULL;
       // mainThread->argument = NULL;
	currentThread=mainThread;
        mainBlock = mainThread;	
        //queue_enqueue(readyQueue, currentThread);
	//

    }
	uthread_create(func, arg);
      
  
  
    
  
     while(readyQueue->queue_size != 0){
			
        uthread_yield();
//	printf("Running queue size%d\n",runningQueue->queue_size);
	printf("ready queue size:%d\n",readyQueue->queue_size);
	printf("Terminated siee%d\n",terminatedQueue->queue_size);
//	int j = find_tid_in_queue(runningQueue);
//	printf("TID_running:%d\n",j);
	 int k = find_tid_in_queue(readyQueue);
 	 printf("TID_ready:%d\n",k);
	if((readyQueue->queue_size == 1)&&(find_tid_in_queue(readyQueue) == 0)){
		struct uthread_tcb last;
		queue_dequeue(readyQueue, (void**)&last);}
     }
	//queue_iterate(terminatedQueue,(void*)terminateThreads);	
     
     
printf("fuck you porquet\n");
printf("ready queue size:%d\n",readyQueue->queue_size);
printf("current thread TID: %d\n", currentThread->TID);
//printf("Terminated size%d\n",terminatedQueue->queue_size);

//for(int i = terminatedQueue->queue_size;i>0;i--)
//{

//	queue_iterate(terminatedQueue,(void*)terminateThreads);

//}
  
  
    return 0;
    // creating initial thread

     // uthread_create(func, arg)
     //
 }
