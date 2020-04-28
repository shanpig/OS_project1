#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

//// Elapse 1 unit of time
#define T_ELAPSE() { volatile unsigned long i; for (i = 0; i < 1000000UL; i++);	}						\

struct process {
	char name[32]; //// process name
	int t_ready; //// ready time
	int t_exec;  //// execution time
	pid_t pid; //// process id
	
};

//// execute process, return pid when finished.
int proc_exec(struct process proc);

//// put process back to waiting
int block(int pid);

//// wake up a process to run
int wakeup(int pid);

#endif
