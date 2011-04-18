
#include <linux/module.h>
#include <asm/uaccess.h>		//get_fs(),set_fs()更改FS段寄存器
#include <linux/fs.h>
#include <linux/cred.h>
#include <linux/dcache.h>
#include <linux/mount.h>
#include <linux/sched.h>
#include <linux/fdtable.h>
#include <linux/list.h>

#define ABSPATH_MAX_LEN 128

#define TEST_FS_FILE "/home/beback/test_fs"			//the location of the file to test

static int gen_abspath(struct dentry *dp, char *abspath);

static void trave_vfsmount(int deps, struct vfsmount *root)
{
	struct list_head *pos = NULL;
	struct vfsmount *vfsmnt_pos = NULL;
	char abspath[ABSPATH_MAX_LEN];
	
	list_for_each(pos, &(root->mnt_mounts)){
		vfsmnt_pos = list_entry(pos, struct vfsmount, mnt_child);
		gen_abspath(vfsmnt_pos->mnt_mountpoint,abspath);
		printk("mding_fs:deps %d mnt_devname is:%-20s,mnt_mountpoint is:%s\n", 
				deps, vfsmnt_pos->mnt_devname,abspath);
//		printk("mding_fs:device name is:%s\n", vfsmnt_pos->mnt_devname);
		trave_vfsmount(deps+1, vfsmnt_pos);
	}
	return ;
}
/*
static void trave_sbs(struct list_head *sbs_head)
{
	struct super_block *sb_pos = NULL;
	char abspath[ABSPATH_MAX_LEN];

	list_for_each_entry(sb_pos, sbs_head, s_list){
		gen_abspath(sb_pos->s_root,abspath);
		printk("mding_fs:s_type->name is:%-15s,s_root is:%s\n",sb_pos->s_type->name,abspath);
	}
	return ;
}
static void trave_fss(struct file_system_type *p)
{
	while(p){
		printk("mding_fs:file_system->name is: %s\n",p->name);
		p = p->next;
	}
}
*/
static void add_parent_path(char *path, const char *parent, const int parent_len)
{
	int path_len,i;

	path_len = strlen(path);
	if(path_len>0){
		for(i=path_len-1;i>=0;i--)
			path[i+parent_len] = path[i];
	}
	memcpy(path,parent,parent_len);
	return ;
}
static int gen_abspath(struct dentry *dp, char *abspath)
{
	int abspath_len;
	char parent_path[ABSPATH_MAX_LEN];

	memset(abspath,'\0',ABSPATH_MAX_LEN);
	if(dp == dp->d_parent)
		sprintf(abspath,"%s",dp->d_name.name);
	else{
		while(dp != dp->d_parent){
			abspath_len = strlen(abspath);
			if((strlen(dp->d_name.name)+abspath_len+1) > ABSPATH_MAX_LEN)
				return 0;
			sprintf(parent_path,"/%s",dp->d_name.name);
			add_parent_path(abspath,parent_path,strlen(parent_path));
			dp = dp->d_parent;
		}
	}
	return 1;
}

static int test_fs(const char * filename)
{
	mm_segment_t old_fs;
	struct file *file_p = NULL;
	struct dentry *dentry_p = NULL;
	struct inode *inode_p = NULL;
	struct super_block *super_p = NULL;

	struct list_head *pos = NULL;	//used for an iterator

//	struct list_head *sbs = 0xc089908c;	//address of the global value 'super_blocks',(from include/linux/fs.h:1128)

//	struct file_system_type **fss_p = 0xc0a6927c;	//address of the pointer 'file_systems',(from fs/filesystems.c:32)
//	struct file_system_type *fss;

	char abspath[ABSPATH_MAX_LEN];	//used to store abspath

	printk("mding_fs:hello\n");
	old_fs = get_fs();
	set_fs(KERNEL_DS);

	printk("mding_fs:hello\n");
	file_p = filp_open(filename, O_RDONLY, 0);
	if(IS_ERR(file_p))
	{
		printk("mding_fs:filp_open error\n");
		set_fs(old_fs);
		return 0;
	}
	printk("mding_fs:file_p addr %x\n",(unsigned int)file_p);
	dentry_p = file_p->f_dentry;
	inode_p = dentry_p->d_inode;
	printk("*************************************************************\n");
	printk("mding_fs:fields in file:\n");
	printk("mding_fs:f_vfsmnt->mnt_root->d_name.name: %s\n",file_p->f_vfsmnt->mnt_root->d_name.name);
	printk("mding_fs:f_vfsmnt->mnt_mountpoint->d_name.name: %s\n",file_p->f_vfsmnt->mnt_mountpoint->d_name.name);
	printk("mding_fs:f_vfsmnt->mnt_sb: %x\n",(int)file_p->f_vfsmnt->mnt_sb);
	printk("mding_fs:f_op: %x\n",(int)file_p->f_op);

	trave_vfsmount(1, file_p->f_vfsmnt->mnt_parent);
/*
	printk("mding_fs:&(f_vfsmnt->mnt_mounts): %x\n",(int)(&(file_p->f_vfsmnt->mnt_mounts)) );
	printk("mding_fs:f_vfsmnt->mnt_mounts.prev: %x\n",(int)file_p->f_vfsmnt->mnt_mounts.prev );
	printk("mding_fs:f_vfsmnt->mnt_mounts.next: %x\n",(int)file_p->f_vfsmnt->mnt_mounts.next );
	printk("mding_fs:&(f_vfsmnt->mnt_child): %x\n",(int)(&(file_p->f_vfsmnt->mnt_child)) );
	printk("mding_fs:f_vfsmnt->mnt_child.prev: %x\n",(int)file_p->f_vfsmnt->mnt_child.prev );
	printk("mding_fs:f_vfsmnt->mnt_child.next: %x\n",(int)file_p->f_vfsmnt->mnt_child.next );
*/
	printk("*************************************************************\n");
	printk("mding_fs:fields in dentry:\n");
	gen_abspath(dentry_p,abspath);
	printk("mding_fs:abspath is: %s\n", abspath);
	printk("mding_fs:d_sb: %x\n",(int)dentry_p->d_sb);

	printk("*************************************************************\n");
	printk("mding_fs:fields in inode:\n");
	printk("mding_fs:i_sb: %x\n",(int)inode_p->i_sb);
	printk("mding_fs:i_fop: %x\n",(int)inode_p->i_fop);
	printk("mding_fs:i_mtime.tv_sec: %x\n",inode_p->i_mtime.tv_sec);

	printk("*************************************************************\n");
	printk("mding_fs:fields in super:\n");
//	trave_sbs(sbs);

	printk("*************************************************************\n");
	printk("mding_fs:file_systems:\n");
//	fss = *(fss_p);
//	trave_fss(fss);

	printk("*************************************************************\n");
	printk("mding_fs:about current task:\n");
	gen_abspath(current->fs->root.dentry,abspath);
	printk("mding_fs:current->fs->root is: %s\n", abspath);

	if(file_p)
		filp_close(file_p, NULL);

	set_fs(old_fs);

	return 1;
}
static int mding_fs_init(void)
{
	test_fs(TEST_FS_FILE);
	return 0;
}

static void mding_fs_exit(void)
{
	printk(KERN_ALERT "mding_fs:module state: %d\n", THIS_MODULE->state );
}

module_init(mding_fs_init);
module_exit(mding_fs_exit);


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("beback");
