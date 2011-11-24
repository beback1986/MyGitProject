
#include <linux/module.h>
#include <asm/uaccess.h>		//get_fs(),set_fs()更改FS段寄存器
//#include <linux/syscalls.h>
#include <linux/fs.h>
#include <linux/cred.h>
#include <linux/dcache.h>
#include <linux/mount.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/fdtable.h>

#define TEST_FILE "/home/beback/test_fs"

void read_file(char *filename)
{
	long (*sys_open)(const char *,int,int);
	long (*sys_read)(unsigned int,char *,size_t);
	long (*sys_write)(unsigned int, const char *, size_t);
	long (*sys_close)(unsigned int);

	int fd;
	int length;
	char buf[13]="hello stdout";
	mm_segment_t old_fs;

	struct files_struct * cur_files;
	struct file * cur_file;
	struct dentry * cur_dentry;
	struct qstr * cur_qstr;

	sys_open = 0xc04a7288;
	sys_read = 0xc04a94c9;
	sys_write = 0xc04a92f8;
	sys_close = 0xc04a70f7;

	fd = 0;
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	printk("current pid is %d \n",(int)task_tgid_vnr(current));
	cur_files = current->files;
	cur_file = fcheck_files(cur_files,1);
	cur_dentry = cur_file->f_dentry;
	cur_qstr = &(cur_dentry->d_name);
	printk("current stdout name is %s\n",cur_qstr->name);

	length = sys_write(1,buf,13);
	printk("write length is %d\n",(int)length);

/*	fd = sys_open(filename, O_RDONLY, 0);
	printk("mding:openning %s,fd now is %d\n",filename,fd);
	if (fd >= 0) {
		printk("mding:begin to read,fd is %d\n",fd);
		while (sys_read(fd, buf, 1) == 1)
			printk("%c", buf[0]);
		printk("\n");
		sys_close(fd);
	}
	else{
		printk("mding: can not open the file\n");
	}*/
	set_fs(old_fs);
}
EXPORT_SYMBOL(read_file);

void test_file(const char * filename)
{
	mm_segment_t old_fs;
	struct file *file = NULL;

	old_fs = get_fs();
	set_fs(KERNEL_DS);

	file = filp_open(filename, O_RDWR, 0);
	printk("mding:file ptr addr %d\n",(int)file);
	printk("mding:file ptr addr %o\n",(unsigned int)file);
	printk("mding:file ptr addr %x\n",(unsigned int)file);
	if(IS_ERR(file))
	{
		printk("mding:filp_open error\n");
		set_fs(old_fs);
		return ;
	}

	printk("mding:file's name is %s\n",file->f_dentry->d_name.name);
	printk("mding:file's i_mtime is %d\n",(int)file->f_dentry->d_inode->i_mtime.tv_sec);
	printk("mding:file's vfsmount root is %s\n",file->f_vfsmnt->mnt_root->d_name.name);
	printk("mding:file's vfsmount mountpoint is %s\n",file->f_vfsmnt->mnt_mountpoint->d_name.name);

	if(file)
		filp_close(file, NULL);

	set_fs(old_fs);

	return ;
}
EXPORT_SYMBOL(test_file);

void hello_func(void)
{
	printk("hello_func is called!!!!!!!!\n");
}
EXPORT_SYMBOL(hello_func);

static int mding_init(void)
{
	printk("mding init!!!\n");
	return 0;
}

static void mding_exit(void)
{
	printk(KERN_ALERT "mding:module state: %d\n", THIS_MODULE->state );
}

module_init(mding_init);
module_exit(mding_exit);


//MODULE_LICENSE("Dual BSD/GPL");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("beback");
