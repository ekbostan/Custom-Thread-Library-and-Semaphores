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
#define INIT 0
#define RUNNING 1
#define READY 2
#define WAITING 3
#define FINISHED 4

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
unsigned short TID;

queue_t queue;
// int queueSize = queue_length(queue);
static ucontext_t context[5];

int getRunningBlock(queue_t library, void *data)
{
    struct uthread_tcb *current = (struct uthread_tcb *)data;

    if (current->state == RUNNING)
    {
        runningBlock = current;
        return 1;
    }
    return 0;
}

struct uthread_tcb *uthread_current(void)
{

    queue_func_t function = &getRunningBlock;

    int iterate = queue_iterate(queue, function);
    if (iterate == 0)
    {
        printf("Current thread success: TID= %lu\n", runningBlock->TID);
        return runningBlock;
    }
    else
    {
        printf("Current thread not found; error in queue iterate\n");
        return NULL;
    }

    /* TODO Phase 2/3 */
}
void uthread_exit(void)
{
    /* TODO Phase 2 */
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

    // create an empty idle thread and swap the context with the thread to run
    if (queue == NULL)
    {
        struct uthread_tcb *mainThread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));

        mainThread->TID = TID;
        mainThread->state = READY;
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
        if (create1 != 0)
        {
            printf("Main thread context initialization unsuccessful");
            return -1;
        }
        else
        {
            // initial thread creation after confirming main thread is created successfully
            struct uthread_tcb *initialThread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
            initialThread->TID = TID++;
            initialThread->context = malloc(sizeof(uthread_ctx_t));
            void *stack2 = uthread_ctx_alloc_stack();
            int create2 = uthread_ctx_init(initialThread->context, stack2, func, arg);

            initialThread->stack = stack2;
            initialThread->startFunc = func;
            initialThread->argument = arg;
            initialThread->joiner = NULL;

            if (create2 != 0)
            {
                printf("Initial thread creation unsuccessful");
                return -1;
            }
            else
            {
                uthread_ctx_switch(mainThread->context, initialThread->context);
                runningBlock = initialThread;
            }
        }
    }
    return 0;
    // creating initial thread

    // uthread_create(func, arg);
}

void uthread_block(void)
{
    /* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
    /* TODO Phase 3 */
}

void printHello()
{
    printf("Hello!");
}

int main(void)
{
    queue_iterate(queue, printHello);

    return 0;
}