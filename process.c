#define _GNU_SOURCE
#include "process.h"
#include <errno.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#define GET_TIME 334
#define TO_DMESG 333


//// executes a process and print start/finish time to dmesg command.
int proc_exec(struct process proc) {
	//// flush std buffer.
	fflush(stdout);
	int pid = fork();

	if (pid < 0) {
		perror("fork");
		return -1;
	}

	//// child
	if (pid == 0) {
		
		unsigned long start_sec, start_nsec, end_sec, end_nsec;
		char to_dmesg[256];

		//// get process starting time
		syscall(GET_TIME, &start_sec, &start_nsec);

		//// execute proc.t_exec times
		for (int i = 0; i < proc.t_exec; i++) {
			T_ELAPSE();
#ifdef DEBUG
			if (i % 100 == 0)
				fprintf(stderr, "%s: %d/%d\n", proc.name, i, proc.t_exec);
#endif
		}

		//// get process finish time
		syscall(GET_TIME, &end_sec, &end_nsec);

		//// write output message to dmesg
		sprintf(to_dmesg, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start_sec, start_nsec, end_sec, end_nsec);
		syscall(TO_DMESG, to_dmesg);
		
		exit(0);
	}

	return pid;
}

//// put a process back to waiting
int block(int pid)
{
	struct sched_param param;
	
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}

//// wake up a process to run
int wakeup(int pid)
{
	//// Create a template param for setting schedule priority.
	struct sched_param param;
	
	//// Set priority to 0
	param.sched_priority = 0;
	int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}
