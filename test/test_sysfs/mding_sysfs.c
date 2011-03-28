#include <linux/module.h>
#include <linux/list.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

static int trave_kset(struct kset *kset_root)
{
	struct kobject *p;
	list_for_each_entry(p, &kset_root->list, entry){
		printk("mding_sysfs:name is:%-15s,kset is:%x ,parent is:%x ,parent->name is:%s\n", 
										p->name, (int)p->kset, (int)p->parent, p->parent->name;
	}
	return 1;
}

int mding_sysfs_init(void)
{
	struct kset **bus_kset_addr;
	struct kset *bus_kset;

	bus_kset_addr = 0xc0a2dda4;
	bus_kset = *bus_kset_addr;
	printk("mding_sysfs:bus_kset->kobj->name is:%-15s,bus_kset->kobj.parent is:%x ,bus_kset->kobj.kset is:%x\n", 
										bus_kset->kobj.name, bus_kset->kobj.parent, bus_kset->kobj.kset);

	trave_kset(bus_kset);
	return 0;
}

void mding_sysfs_exit(void)
{
	return ;
}

module_init(mding_sysfs_init);
module_exit(mding_sysfs_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("beback");
