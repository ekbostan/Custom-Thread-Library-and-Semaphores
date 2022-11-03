/*
 * Thread creation and yielding test
 *
 * Tests the creation of multiples threads and the fact that a parent thread
 * should get returned to before its child is executed. The way the printing,
 * thread creation and yielding is done, the program should output:
 *
 * thread1
 * thread2
 * thread3
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <uthread.h>

void thread3(void *arg)
{
	(void)arg;

	uthread_yield();
	printf("thread3\n");
}

void thread2(void *arg)
{
	(void)arg;

	uthread_create(thread3, NULL);
	uthread_yield();
	printf("thread2\n");
}

void thread1(void *arg)
{
	(void)arg;

	uthread_create(thread2, NULL);
	uthread_yield();
	printf("thread1\n");
	uthread_yield();
}

int main(void)
{
	uthread_run(false, thread1, NULL);
	return 0;
}
ekbostan@COE-CS-pc42:~/p2/apps$ cd ../libuthread
ekbostan@COE-CS-pc42:~/p2/libuthread$ cat context.c
#include <stdio.h>
#include <stdlib.h>

#include "private.h"
#include "uthread.h"

/* Size of the stack for a thread (in bytes) */
#define UTHREAD_STACK_SIZE 32768

void uthread_ctx_switch(uthread_ctx_t *prev, uthread_ctx_t *next)
{
	/*
	 * swapcontext() saves the current context in structure pointer by @prev
	 * and actives the context pointed by @next
	 */
	if (swapcontext(prev, next)) {
		perror("swapcontext");
		exit(1);
	}
}

void *uthread_ctx_alloc_stack(void)
{
	return malloc(UTHREAD_STACK_SIZE);
}

void uthread_ctx_destroy_stack(void *top_of_stack)
{
	free(top_of_stack);
}

/*
 * uthread_ctx_bootstrap - Thread context bootstrap function
 * @func: Function to be executed by the new thread
 * @arg: Argument to be passed to the thread
 */
static void uthread_ctx_bootstrap(uthread_func_t func, void *arg)
{
	/*
	 * Enable interrupts right after being elected to run for the first time
	 */
	preempt_enable();

	/* Execute thread and when done, exit */
	func(arg);
	uthread_exit();
}

int uthread_ctx_init(uthread_ctx_t *uctx, void *top_of_stack,
		     uthread_func_t func, void *arg)
{
	/*
	 * Initialize the passed context @uctx to the currently active context
	 */
	if (getcontext(uctx))
		return -1;

	/*
	 * Change context @uctx's stack to the specified stack
	 */
	uctx->uc_stack.ss_sp = top_of_stack;
	uctx->uc_stack.ss_size = UTHREAD_STACK_SIZE;

	/*
	 * Finish setting up context @uctx:
	 * - the context will jump to function uthread_ctx_bootstrap() when
	 *   scheduled for the first time
	 * - when called, function uthread_ctx_bootstrap() will receive two
	 *   arguments: @func and @arg
	 */
	makecontext(uctx, (void (*)(void)) uthread_ctx_bootstrap,
		    2, func, arg);

	return 0;
}
