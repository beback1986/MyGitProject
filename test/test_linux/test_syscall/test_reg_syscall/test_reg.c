#include<linux/kernel.h>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/unistd.h>

#include<linux/time.h>

#include<asm/uaccess.h> // for copy_to_user
#include<linux/sched.h> // for current macro

#define __NR_mding		285
#define __NR_read		3

static long saved;
static long *systable;

static int sys_mding(void)
{
	printk("sys_mding:Being called!!\n");
	return 32766;
}

int mding_reg_syscall(void)
{
	systable = 0xc071d918;
	int addr_test = 0;
	printk("mding_reg_syscall:sys_read addr is:%x\n",systable[__NR_mding]);
	saved = systable[__NR_mding];
//	systable[__NR_mding] = 0;		//难道是该页内存被保护起来了？？？不能不能对其进行写操作？？？那保护的开关在拿呢？？？
	printk("mding_reg_syscall:saved is:%x\n",saved);
	printk("mding_reg_syscall:sys_mding is:%x\n",sys_mding);
	printk("mding_reg_syscall:addr_test address is:%x\n",&addr_test);
	return 0;
}

void mding_dereg_syscall(void)
{
//	systable[__NR_mding] = saved;
	return ;
}

module_init(mding_reg_syscall);
module_exit(mding_dereg_syscall);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("yuan ding");
