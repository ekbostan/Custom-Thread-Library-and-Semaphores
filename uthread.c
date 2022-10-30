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


static int currNodeCount = 0;

struct uthread_tcb
{
    uthread_ctx_t *context;     // pointer to context- top of stack
    unsigned short TID;         // thread ID
    void *stack;                // checks to see if a stack exists for thread
    void (*startFunc)(void *);  // function of the thread
    void *argument;             // argument of the passed function
    int state;                  // defined state
    int ret;                    // return value
    struct uthread_tcb *joiner; // joiner thread- idk wtf this means though

    /* TODO Phase 2 */
};

struct uthread_tcb *runningBlock;
int TID;

queue_t queue;
// int queueSize = queue_length(queue);

struct uthread_tcb *uthread_current(void)
{
    struct uthread_tcb *front = queue->queue_first->node_value;
   

    if (front->state == RUNNING)
    {
        runningBlock = front;
    }
    else
    {
        front->state = RUNNING;
	runningBlock = front;
    }
    // finds first element in queue and returns it
    // check if first element of queue is set to running, if not set it to running
    return runningBlock;

    /* TODO Phase 2/3 */
}
void uthread_exit(void)
{
    /* TODO Phase 2 */
    // get current running thread from front of queue
    struct uthread_tcb *current = uthread_current();
    // destroy the stack and context
    uthread_ctx_destroy_stack(current->context);
    free(current->stack);
    // set current state to terminated
    current->state = TERMINATED;

    // yield to the next thread in the queue
    uthread_yield();
}
void uthread_yield(void)
{
    // save current context of running thread from uthread_current function
    
    struct uthread_tcb *current = uthread_current();
    // get the next value in the queue and save that context
    struct uthread_tcb *next = queue->queue_first->next->node_value;
    // switch state to ready of current running thread
    if (current->state == RUNNING)
    {
        current->state = READY;
    }

    // dequeue front of queue and enqueue it to end of queue
    if (queue_dequeue(queue, (void **)&current) == -1)
    {
        printf("Error in dequeueing first queue element in yield function\n");
    }
    queue_enqueue(queue, current);

    // set current running thread to next block in the queue-> state to running
   
    printf("next->TID: %d", next->TID);
    printf("next->state: %d", next->state);
    if (next->state == READY)
    {
        next->state = RUNNING;
    }

    uthread_ctx_switch(current->context, next->context);
    if (current->state == TERMINATED)
    {
        queue_delete(queue, current);
        uthread_ctx_destroy_stack(current->stack);
        free(current);
        current = NULL;
    }

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
        queue_enqueue(queue, block);
	currNodeCount++;
        block->TID = TID++;
        block->state = READY;
        block->startFunc = func;
        block->argument = arg;
        block->joiner = NULL;
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
    /* TODO Phase 2 */
   if(preempt == 1){return -1;}
    // create an empty idle thread and swap the context with the thread to run
    if (queue == NULL)
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
        mainThread->joiner = NULL;
        queue = queue_create();
        queue_enqueue(queue, mainThread);
	currNodeCount++;
        if (create1 != 0)
        {
            printf("Main thread context initialization unsuccessful");
            return -1;
        }
    }
    else
    {
        if (uthread_create(func, arg) == -1)
        {
            printf("Failed to create new thread in run function\n");
        }
        else
        {
            printf("New thread creation in run function successful\n");
        }
    }

    while (queue->queue_size != 0)
    {
        uthread_yield();
    }

    return 0;
    // creating initial thread

    // uthread_create(func, arg);
}

void uthread_block(void)
{
    /* TODO Phase 3 */
	printf("block function");
}

void uthread_unblock(struct uthread_tcb *uthread)
{
    /* TODO Phase 3 */
	printf("unblock: %p", uthread);
}
