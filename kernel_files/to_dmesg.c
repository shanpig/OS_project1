#include <linux/linkage.h>
#include <linux/kernel.h>

asmlinkage void sys_to_dmesg(char msg[]){
	printk("%s",msg);
}
