#include <linux/module.h>
#include <asm/pgtable.h>	/* for VMALLOC_START, VMALLOC_END */
#include <linux/highmem.h>	/* for varible 'high_memory' used in macro VMALLOC_START and VMALLOC_END */
#include <linux/gfp.h>		/* for alloc_page(), __free_page(), GFP_KERNEL */
#include <linux/mm_types.h>	/* for struct page, struct mm_struct, struct vm_area_struct */
#include <linux/mm.h>		/* for page_address(), page_zonenum() */
#include <linux/mmzone.h>	/* for enum zone_type, ZONE_DMA, ZONE_NORMAL, ZONE_HIGHMEM etc. */
#include <asm/current.h>	/* for macro current */
#include <linux/sched.h>	/* for macro for_each_process, struct task_struct, varible init_pgtable */

struct page *addr_to_page(struct mm_struct *mm, unsigned long addr)
{
	struct page *page = NULL;
	pgd_t *pgd = pgd_offset(mm, addr);
//printk("mding_pgtable:pgd:%x\n", pgd);

	if (!pgd_none(*pgd)) {
		pud_t *pud = pud_offset(pgd, addr);
		if (!pud_none(*pud)) {
			pmd_t *pmd = pmd_offset(pud, addr);
			if (!pmd_none(*pmd)) {
				pte_t *ptep, pte;

				ptep = pte_offset_map(pmd, addr);
				pte = *ptep;
				if (pte_present(pte)){
					page = pte_page(pte);
//printk("mding_pgtable:pfn:%x\n",pte_pfn(pte));
				}
				pte_unmap(ptep);
			}
		}
	}
	return page;
}

void trave_mem_map(void)
{
	struct page *mem_map = 0xc0a0a080;
	int i;
	for(i=0;i<=20;i++){
		printk("mding_pgtable:page address:%x, flag:%x zone_type:%d\n",page_address(mem_map), mem_map->flags, page_zonenum(mem_map));
		mem_map++;
	}
	printk("mding_pgtable:sizeof strcut page:%d\n", sizeof(struct page));
}

int mding_pgtable_init(void)
{
/*
	struct mm_struct *init_mm = 0xc088a1a0;
	struct page *p = NULL;
	unsigned long addr;

	for(addr=0xc0400000; addr<0xc0410000; addr+=1024){
//		p = addr_to_page(init_mm, addr);
		p = vmalloc_to_page(addr);
		if(p){
//			printk("mding_pgtable:addr:%x, page address:%x flag:%x zone_type:%x\n", addr, page_address(p), p->flags, page_zonenum(p));
			printk("mding_pgtable:page addr:%x\n", page_address(p));
		}
		else
			printk("mding_pgtable:addr:%x, page do not exist\n", addr);
	}
	printk("PTE_PFN_MASK:%x\n",PTE_PFN_MASK);
	printk("ARCH_PFN_OFFSET:%x\n",ARCH_PFN_OFFSET);
*/
	trave_mem_map();
	return 0;
}

void mding_pgtable_exit(void)
{
	return ;
}

module_init(mding_pgtable_init);
module_exit(mding_pgtable_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("beback");
