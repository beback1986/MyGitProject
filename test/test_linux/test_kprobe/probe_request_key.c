#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>
#include <asm/percpu.h>
#include <asm/mmu_context.h>
#include <linux/blkdev.h>
#include <asm/ptrace.h>
#include <linux/binfmts.h>
#include <linux/sunrpc/sched.h>
#include <linux/sunrpc/rpc_pipe_fs.h>
#include <linux/acct.h>
#include <linux/delay.h>
#include <linux/bio.h>
#include <scsi/scsi_eh.h>
#include <scsi/scsi_cmnd.h>
#include <scsi/scsi_device.h>
#include <scsi/scsi_host.h>

#define arg_1	di
#define arg_2	si
#define arg_3	dx
#define arg_4	cx
#define arg_5	r8
#define arg_6	r9
#define arg_7(regs) (*(unsigned long*)((regs)->sp+8))

#define r_ret	ax

struct private_data {
	unsigned long pd_arg_1;
	unsigned long pd_arg_2;
	unsigned long pd_arg_3;
	unsigned long pd_arg_4;
	unsigned long pd_arg_5;
	unsigned long pd_arg_6;
	unsigned long pd_arg_7;
};

typedef void (*print_hooks_t)(unsigned long arg);

struct print_hooks {
	unsigned int	ph_mask;
	print_hooks_t	ph_arg_1;
	print_hooks_t	ph_arg_2;
	print_hooks_t	ph_arg_3;
	print_hooks_t	ph_arg_4;
	print_hooks_t	ph_arg_5;
	print_hooks_t	ph_arg_6;
	print_hooks_t	ph_arg_7;
};

#define PH_ARG1_ENT	0x00000001
#define PH_ARG2_ENT	0x00000002
#define PH_ARG3_ENT	0x00000004
#define PH_ARG4_ENT	0x00000008
#define PH_ARG5_ENT	0x00000010
#define PH_ARG6_ENT	0x00000020
#define PH_ARG7_ENT	0x00000040
#define PH_ARG1_RET	0x00000080
#define PH_ARG2_RET	0x00000100
#define PH_ARG3_RET	0x00000200
#define PH_ARG4_RET	0x00000400
#define PH_ARG5_RET	0x00000800
#define PH_ARG6_RET	0x00001000
#define PH_ARG7_RET	0x00002000

static void save_args(struct private_data *pd, struct pt_regs *regs)
{
	pd->pd_arg_1 = regs->arg_1;
	pd->pd_arg_2 = regs->arg_2;
	pd->pd_arg_3 = regs->arg_3;
	pd->pd_arg_4 = regs->arg_4;
	pd->pd_arg_5 = regs->arg_5;
	pd->pd_arg_6 = regs->arg_6;
	pd->pd_arg_7 = arg_7(regs);
}

static void print_args(struct private_data *pd, struct print_hooks *ph, bool is_ent)
{
	if (ph == NULL)
		return ;
	if (is_ent) {
		if (ph->ph_mask & PH_ARG1_ENT) {
			ph->ph_arg_1(pd->pd_arg_1);
		}
		if (ph->ph_mask & PH_ARG2_ENT) {
			ph->ph_arg_2(pd->pd_arg_2);
		}
		if (ph->ph_mask & PH_ARG3_ENT) {
			ph->ph_arg_3(pd->pd_arg_3);
		}
		if (ph->ph_mask & PH_ARG4_ENT) {
			ph->ph_arg_4(pd->pd_arg_4);
		}
		if (ph->ph_mask & PH_ARG5_ENT) {
			ph->ph_arg_5(pd->pd_arg_5);
		}
		if (ph->ph_mask & PH_ARG6_ENT) {
			ph->ph_arg_6(pd->pd_arg_6);
		}
		if (ph->ph_mask & PH_ARG7_ENT) {
			ph->ph_arg_7(pd->pd_arg_7);
		}
	} else {
		if (ph->ph_mask & PH_ARG1_RET) {
			ph->ph_arg_1(pd->pd_arg_1);
		}
		if (ph->ph_mask & PH_ARG2_RET) {
			ph->ph_arg_2(pd->pd_arg_2);
		}
		if (ph->ph_mask & PH_ARG3_RET) {
			ph->ph_arg_3(pd->pd_arg_3);
		}
		if (ph->ph_mask & PH_ARG4_RET) {
			ph->ph_arg_4(pd->pd_arg_4);
		}
		if (ph->ph_mask & PH_ARG5_RET) {
			ph->ph_arg_5(pd->pd_arg_5);
		}
		if (ph->ph_mask & PH_ARG6_RET) {
			ph->ph_arg_6(pd->pd_arg_6);
		}
		if (ph->ph_mask & PH_ARG7_RET) {
			ph->ph_arg_7(pd->pd_arg_7);
		}
	}
}

#define probe_entry_int(sym, hooks)							\
static int handle_ent__##sym(struct kretprobe_instance *ri, struct pt_regs *regs)	\
{											\
	struct private_data *pd = (void *)ri->data;					\
											\
	printk("(%d)"#sym" -> ", current->pid);						\
	save_args(pd, regs);								\
	print_args(pd, hooks, 1);							\
	printk("\n");									\
	return 0;									\
}											\
static int handle_ret__##sym(struct kretprobe_instance *ri, struct pt_regs *regs)	\
{											\
	struct private_data *pd = (void *)ri->data;					\
	int ret = (int)regs->r_ret;							\
											\
	printk("(%d)"#sym" <- (%d) ", current->pid, ret);				\
	print_args(pd, hooks, 0);							\
	printk("\n");									\
	return 0;									\
}											\
struct kretprobe kr_##sym = {								\
	.kp.symbol_name		= #sym,							\
	.entry_handler		= handle_ent__##sym,					\
	.handler		= handle_ret__##sym,					\
	.data_size		= sizeof(struct private_data),				\
	/* Probe up to 20 instances concurrently. */					\
	.maxactive		= 20,							\
};

#define probe_entry_long(sym, hooks)							\
static int handle_ent__##sym(struct kretprobe_instance *ri, struct pt_regs *regs)	\
{											\
	struct private_data *pd = (void *)ri->data;					\
											\
	printk("(%d)"#sym" -> ", current->pid);						\
	save_args(pd, regs);								\
	print_args(pd, hooks, 1);							\
	printk("\n");									\
	return 0;									\
}											\
static int handle_ret__##sym(struct kretprobe_instance *ri, struct pt_regs *regs)	\
{											\
	struct private_data *pd = (void *)ri->data;					\
	unsigned long long ret = (int)regs->r_ret;					\
											\
	printk("(%d)"#sym" <- (%lld) ", current->pid, ret);				\
	print_args(pd, hooks, 0);							\
	printk("\n");									\
	return 0;									\
}											\
struct kretprobe kr_##sym = {								\
	.kp.symbol_name		= #sym,							\
	.entry_handler		= handle_ent__##sym,					\
	.handler		= handle_ret__##sym,					\
	.data_size		= sizeof(struct private_data),				\
	/* Probe up to 20 instances concurrently. */					\
	.maxactive		= 20,							\
};

#define probe_entry_pointer(sym, hooks)							\
static int handle_ent__##sym(struct kretprobe_instance *ri, struct pt_regs *regs)	\
{											\
	struct private_data *pd = (void *)ri->data;					\
											\
	printk("(%d)"#sym" -> ", current->pid);						\
	save_args(pd, regs);								\
	print_args(pd, hooks, 1);							\
	printk("\n");									\
	return 0;									\
}											\
static int handle_ret__##sym(struct kretprobe_instance *ri, struct pt_regs *regs)	\
{											\
	struct private_data *pd = (void *)ri->data;					\
	void *ret = (void *)regs->r_ret;						\
											\
	if (IS_ERR(ret))								\
		printk("(%d)"#sym" <- (%ld) ", current->pid, PTR_ERR(ret));		\
	else										\
		printk("(%d)"#sym" <- (0) ", current->pid);				\
	print_args(pd, hooks, 0);							\
	printk("\n");									\
	return 0;									\
}											\
struct kretprobe kr_##sym = {								\
	.kp.symbol_name		= #sym,							\
	.entry_handler		= handle_ent__##sym,					\
	.handler		= handle_ret__##sym,					\
	.data_size		= sizeof(struct private_data),				\
	/* Probe up to 20 instances concurrently. */					\
	.maxactive		= 20,							\
};

void print_str(unsigned long arg)
{
	char *name = (void *)arg;
	printk("%s ", name);
}

void print_pointer(unsigned long arg)
{
	void *pointer = (void *)arg;
	printk("%p ", pointer);
}

struct idmap {
	struct rpc_pipe		*idmap_pipe;
	void			*idmap_upcall_data;
	struct mutex		idmap_mutex;
};

void print_idmap(unsigned long arg)
{
	struct idmap *idmap = (void *)arg;
	struct rpc_pipe *pipe = idmap->idmap_pipe;

	printk("nreaders(%d) flags(%x) ", pipe->nreaders, pipe->flags);
}

struct print_hooks nfs_idmap_request_key_hooks = {
	.ph_mask = PH_ARG2_ENT|PH_ARG7_ENT,
	.ph_arg_2 = print_str,
	.ph_arg_7 = print_pointer,
};

struct print_hooks nfs_idmap_legacy_upcall_hooks = {
	.ph_mask = PH_ARG3_ENT,
	.ph_arg_3 = print_idmap,
};

probe_entry_int(nfs_idmap_get_key, NULL)
probe_entry_long(nfs_idmap_request_key, &nfs_idmap_request_key_hooks)
probe_entry_pointer(request_key_with_auxdata, NULL)
probe_entry_pointer(request_key, NULL)
probe_entry_pointer(request_key_and_link, NULL)
probe_entry_pointer(construct_key_and_link, NULL)
probe_entry_pointer(search_process_keyrings, NULL)
probe_entry_int(nfs_idmap_legacy_upcall, &nfs_idmap_legacy_upcall_hooks)
probe_entry_long(idmap_pipe_downcall, NULL)

static struct kretprobe *krs[] = {
	[0] = &kr_nfs_idmap_get_key,
	[1] = &kr_nfs_idmap_request_key,
	[2] = &kr_request_key_with_auxdata,
	[3] = &kr_request_key_and_link,
	[4] = &kr_search_process_keyrings,
	[5] = &kr_construct_key_and_link,
	[6] = &kr_request_key,
	[7] = &kr_nfs_idmap_legacy_upcall,
	[8] = &kr_idmap_pipe_downcall,
};

static int __init kprobes_register(void)
{
	int ret = 0;

	ret = register_kretprobes(krs, 9);
	if (ret)
		printk("register kr failed. %d\n", ret);
	printk("probe module run\n");
	return 0;
}

static void kprobes_unregister(void)
{
	unregister_kretprobes(krs, 9);
	printk("probe module stop\n");
}

module_init(kprobes_register);
module_exit(kprobes_unregister);
MODULE_LICENSE("GPL");
