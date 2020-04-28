#define _GNU_SOURCE
#include "process.h"
#include "scheduler.h"
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <sys/wait.h>

//// Time quantum for RR alg.
#define TIME_QUANTUM 500


static int current_t; //// Current unit time
static int last_stime; //// Last switch time for RR scheduling.
static int running; //// Index of running process. -1 if no process running.
static int finished_proc_num; //// Number of finish Process.

//// Sorting function for qsort.
int cmpr(const void *a, const void *b) {
	return ((struct process *)a)->t_ready - ((struct process *)b)->t_ready;
}

//// Return index of next process.
int next_proc(struct process *proc, int proc_num, int policy) {

	//// for non-preemptive alg., continue the running process.
	if ( running != -1 && (policy == SJF || policy == FIFO) )
		return running;

	int ret = -1;

	//// for SJF, iterate over current ready process to find shortest job.
	if (policy == SJF || policy == PSJF) {
		for (int i = 0; i < proc_num; i++) {
			if (proc[i].pid == -1 || proc[i].t_exec == 0)
				continue;
			if (ret == -1 || proc[i].t_exec < proc[ret].t_exec)
				ret = i;
		}
	}

	//// for FIFO, iterate over all to find earliest job.
	else if (policy == FIFO) {
		for(int i = 0; i < proc_num; i++) {
			if (proc[i].pid == -1 || proc[i].t_exec == 0)
				continue;
			if (ret == -1 || proc[i].t_ready < proc[ret].t_ready)
				ret = i;
		}
    }

	//// For RR, choose next job every time quantum elapse.
	else if (policy == RR) {

		//// On finish a job, choose the first ready job.
		if (running == -1) {
			for (int i = 0; i < proc_num; i++) {
				if (proc[i].pid != -1 && proc[i].t_exec > 0){
					ret = i;
					break;
				}
			}
		}
		
		//// Every time quantum, choose the next ready job.
		else if ((current_t - last_stime) % TIME_QUANTUM == 0)  {
			ret = (running + 1) % proc_num;
			while (proc[ret].pid == -1 || proc[ret].t_exec == 0)
				ret = (ret + 1) % proc_num;
		}

		//// Other times, continue
		else
			ret = running;
	}

	return ret;
}

//// Scheduling 
int scheduling(struct process *proc, int proc_num, int policy)
{
	qsort(proc, proc_num, sizeof(struct process), cmpr);

	//// initiallize pid to "not ready" 
	for (int i = 0; i < proc_num; i++)
		proc[i].pid = -1;

	//// Setting high priority for schedular.
	wakeup(getpid());
	
	current_t = 0; //// Set time startpoint
	running = -1; //// not running
	finished_proc_num = 0; //// finished processes.
	
	//// Start the scheduling with time elapse.
	while(1) {

		//// Print process name and id if finished.
		if (running != -1 && proc[running].t_exec == 0) {
		
#ifdef DEBUG
			char msg[200];
			snprintf(msg, 200, "[project1] [finish] %s ends at time %d.\n", proc[running].name, current_t);
			fprintf(stderr, "%s", msg);
#endif

			//// wait for the running proccess to finish
			waitpid(proc[running].pid, NULL, 0);
			printf("%s %d\n", proc[running].name, proc[running].pid);
			running = -1;
			finished_proc_num++;

			//// All process finished.
			if (finished_proc_num == proc_num){
				syscall(333, "Task finished.");
				break;
			}
		}

		//// Check if the process is ready to execute.
		for (int i = 0; i < proc_num; i++) {
			//// If ready, then execute->block
			if (proc[i].t_ready == current_t) {
				proc[i].pid = proc_exec(proc[i]);
				block(proc[i].pid);
#ifdef DEBUG
				fprintf(stderr, "%s ready : time %d.\n", proc[i].name, current_t);
#endif
			}
		}

		//// Select next process.
		int next = next_proc(proc, proc_num, policy);
		if (next != -1) {
			//// Switching between process.
			if (running != next) {
				block(proc[running].pid);
				wakeup(proc[next].pid);

				last_stime = current_t;
				running = next;
			}
		}

		//// Time elapse 1 unit every cycle.
		
		T_ELAPSE(); 
		if (running != -1) //// execute 1 unit of time from running proccess
			proc[running].t_exec--;
		current_t++; //// elapse current time unit.

	}

	return 0;
}
