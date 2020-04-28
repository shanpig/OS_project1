#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>
#include "process.h"
#include "scheduler.h"

int main(int argc, char* argv[])
{

	int policy;
	int proc_num;
	struct process *proc;
	char sched_policy[256];

	//// Get scheduling policy and number of processes
	scanf("%s", sched_policy);
	scanf("%d", &proc_num);

	//// Set a list pointer for assigned processes.
	proc = (struct process *)malloc(proc_num * sizeof(struct process));

	//// Read the processes from data and add to proc.
	for (int i = 0; i < proc_num; i++) {
		scanf("%s%d%d", proc[i].name,
			&proc[i].t_ready, &proc[i].t_exec);
	}

	//// Set scheduling policy.
	if (strcmp(sched_policy, "FIFO")==0){
		policy = FIFO;
	}
	else if (strcmp(sched_policy, "RR")==0){
		policy = RR;
	}
	else if (strcmp(sched_policy, "SJF")==0){
		policy = SJF;
	}
	else if (strcmp(sched_policy, "PSJF")==0){
		policy = PSJF;
	}
	else {
		fprintf(stderr, "Unknown policy %s", sched_policy);
		exit(0);
	}
	

	//// Scheduling begins.
	scheduling(proc, proc_num, policy);

	exit(0);
}
