#include <linux/linkage.h>
#include <linux/kernel.h>
#include <linux/ktime.h>
#include <linux/timekeeping.h>

asmlinkage void sys_get_time(unsigned long *sec, unsigned long *nsec){
	struct timespec ts;
	getnstimeofday(&ts);
	*sec = ts.tv_sec;
	*nsec = ts.tv_nsec;

}
