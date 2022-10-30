#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100000

void timer_handler(int alarm);

void preempt_disable(void)
{

	sigset_t ss;
    /* Ignore Ctrl-C */
    	sigemptyset(&ss);
    	sigaddset(&ss, SIGVTALRM);
    	sigprocmask(SIG_BLOCK, &ss, NULL);
}

void preempt_enable(void)
{
	sigset_t ss;
    /* Ignore Ctrl-C */
        sigemptyset(&ss);
        sigaddset(&ss, SIGVTALRM);
        sigprocmask(SIG_UNBLOCK, &ss, NULL);
}

void preempt_start(bool preempt)
{
	
	if(preempt){
	struct sigaction sa;
	struct itimerval timer;
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGVTALRM,&sa,NULL);

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = HZ;
 	/* ... and every 250 msec after that. */
 	timer.it_interval.tv_sec = 0;
 	timer.it_interval.tv_usec = HZ;
	setitimer (ITIMER_VIRTUAL, &timer, NULL);
	timer_handler(SIGVTALRM);
	}
}

void preempt_stop(void)
	struct itimerval timer;
	setitimer(0);
}

void timer_handler(){

uthread_yield();
}
