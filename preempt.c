#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100000

void timer(int alarm){


uthread_yield();

}


void preempt_disable(void)
{
	/* TODO Phase 4 */
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
}

void preempt_start(bool preempt)
{
	struct sigaction sa;
	struct itimerval timer;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = timer;
	sigaction(SIGVTALRM,&sa,NULL);

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = HZ;
 	/* ... and every 250 msec after that. */
 	timer.it_interval.tv_sec = 0;
 	timer.it_interval.tv_usec = HZ;
	setitimer (ITIMER_REAL, &timer, NULL);
	timer(SIGVTALRM);

}

void preempt_stop(void)
{

	free(sa);
	free(timer);
}

