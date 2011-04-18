#include <linux/module.h>
#include <linux/ioport.h>	/* for struct resource */
#include <asm/pgtable.h>	/* for VMALLOC_START, VMALLOC_END */
#include <linux/highmem.h>	/* for varible 'high_memory' used in macro VMALLOC_START and VMALLOC_END */
#include <linux/gfp.h>		/* for alloc_page(), __free_page(), GFP_KERNEL */
#include <linux/mm_types.h>	/* for struct page, struct mm_struct, struct vm_area_struct */
#include <linux/mm.h>		/* for page_address(), page_zonenum() */
#include <linux/mmzone.h>	/* for enum zone_type, ZONE_DMA, ZONE_NORMAL, ZONE_HIGHMEM etc. */
#include <asm/current.h>	/* for macro current */
#include <linux/sched.h>	/* for macro for_each_process, struct task_struct, varible init_mm */

void trave_resources(struct resource *res, int depth)
{
	struct resource *sib;
	int i;

	for(i = depth; i > 0; i--)
		printk("  ");
	printk("mding_mm:%08x-%08x\n", res->start, res->end);
	sib = res->child;
	while(sib){
		trave_resources(sib,depth+1);
		sib = sib->sibling;
	}
}

void test_alloc(void)
{
	struct page *page = NULL;
	int i;
	printk("mem_map address is:%x\n", mem_map);
	printk("PAGE_SHIFT is:%x, PAGE_OFFSITE is:%x, ARCH_PFN_OFFSET is:%x\n", PAGE_SHIFT, PAGE_OFFSET, ARCH_PFN_OFFSET);
	printk("mding_mm:VMALLOC_START:%x  VMALLOC_END:%x\n", VMALLOC_START, VMALLOC_END);
	for(i=0;i<100;i++){
		page = alloc_page(GFP_KERNEL );	//__GFP_HIGHMEM
		printk("mding_mm:alloc_page addr:%x pfn:%x  page_address:%x  flag:%x  zone_type:%x\n", page, page-mem_map, page_address(page), page->flags, page_zonenum(page));
//	printk("mding_mm:__get_free_page addr:%x\n", __get_free_page(GFP_KERNEL | __GFP_HIGHMEM));
//	__free_page(page);
    }
	printk("mding_mm:ZONE_NORMAL is:%x", ZONE_NORMAL);
	return ;
}

void test_page_virtual(void)
{
#if defined(WANT_PAGE_VIRTUAL)
	printk("mding_mm:WANT_PAGE_VIRTUAL is set\n");
#endif
#if defined(HASHED_PAGE_VIRTUAL)
	printk("mding_mm:HASHED_PAGE_VIRTUAL is set\n");
#endif
	return ;
}

void get_all_vm_area(struct vm_area_struct *head)
{
	struct vm_area_struct *p;
	p = head;
	while(p){
		printk("mding_mm:%08x-%08x\n",p->vm_start,p->vm_end);
		p = p->vm_next;
	}
	return ;
}

void trave_process(void)
{
	struct task_struct *p;
	struct mm_struct *init_mm_p = 0xc088a1a0;	//addr of init_mm in [arch/x86/kernel/init_task.c]
	printk("mding:init_mm->pgd addr:%08x, init_mm->mm->pgd->pgd:%x, init_mm->owner addr:%08x, init_mm->owner->pid:%d\n", init_mm_p->pgd, init_mm_p->pgd->pgd, init_mm_p->owner, init_mm_p->owner->pid);
	for_each_process(p){	//this loop is from process 1, not from process 0
		printk("mding_mm:pid:%04d, mm addr:%08x, active_mm addr:%08x, ", p->pid, p->mm, p->active_mm);
		printk("%s, ", (p->mm == p->active_mm)? "same":"diff");
		if(p->mm)
			printk("pgd addr:%x, pgd->pgd:%x", p->mm->pgd, p->mm->pgd->pgd);
		printk("\n");
	}
}

int mding_mm_init(void)
{
//	long * mem_map_addr = 0xc0a0a080;
//	struct mm_struct *init_mm_p = 0xc088a1a0;	//addr of init_mm in [arch/x86/kernel/init_task.c]
/*
	struct resource *iomem_resource;

	iomem_resource = 0xc0894140;
	trave_resources(iomem_resource,0);	//test for:test kernel resource tree
*/

	test_page_virtual();
	test_alloc();				//test for:test alloc_page() function and address
/*
	get_all_vm_area(init_mm_p->mmap);	//test for:get all vm area for current process
*/

/*
	trave_process();
	printk("%x\n",(*mem_map_addr));
*/
	return 0;
}

void mding_mm_exit(void)
{
	return ;
}

module_init(mding_mm_init);
module_exit(mding_mm_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("beback");
