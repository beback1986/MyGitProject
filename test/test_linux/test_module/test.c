#include <linux/module.h>

//MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void)
{
	uint64_t a = 1048576;
	uint32_t b = 4096;

	void (* func_p)(void);
/*	unsigned int cpu = get_cpu();
	struct module *mod;
	printk("this module: %p==%p\n", &__this_module, THIS_MODULE );
	printk("module state: %d\n", THIS_MODULE->state );
	printk("module name: %s\n", THIS_MODULE->name );
	list_for_each_entry(mod, *(&THIS_MODULE->list.prev), list )
		printk(KERN_ALERT "module name: %s\n", mod->name );*/
	func_p = __symbol_get("hello_func");
	printk("func_p's addr is %p\n", func_p);
	if(!func_p){
		printk("func can not find!\n");
	}
	else{
		__symbol_put("hello_func");
		func_p();
		printk("symbol exist. p=%p\n", func_p);
	}
	printk("a/b = %u.\n", a/b);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "module state: %d\n", THIS_MODULE->state );
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("beback");
