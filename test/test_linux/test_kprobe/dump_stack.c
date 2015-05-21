#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <asm/percpu.h>
#include <asm/mmu_context.h>
#include <linux/blkdev.h>
#include <asm/ptrace.h>
#include <linux/binfmts.h>
#include <linux/sunrpc/sched.h>
#include <linux/acct.h>

struct kprobe kp;

static int handler_pre(struct kprobe *kp, struct pt_regs *regs)
{
	printk("sxb: mode %x\n", regs->di);
	dump_stack();
	return 0;
}

static int __init kprobes_register(void)
{
	kp.symbol_name = "enfs_lock";
	kp.pre_handler = handler_pre;

	register_kprobe(&kp);
	return 0;
}

static void kprobes_unregister(void)
{
	unregister_kprobe(&kp);
}

module_init(kprobes_register);
module_exit(kprobes_unregister);
MODULE_LICENSE("GPL");
