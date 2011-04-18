#ifndef __KERNEL__
#define __KERNEL__
#endif
#ifndef MODULE
#define MODULE
#endif
                                                                               
//#include <linux/config.h>
#include <linux/module.h>
                                                                               
MODULE_LICENSE("GPL");
#ifdef CONFIG_SMP
#define __SMP__
#endif

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <asm/uaccess.h>

#include <linux/slab.h>
#include <linux/moduleparam.h>
#include <linux/errno.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/aio.h>

#include "asdf.h"

int asdf_major =   ASDF_MAJOR;
int asdf_minor =   ASDF_MINOR;
int asdf_devs  =   ASDF_DEVS;

int asdf_quantum = ASDF_QUANTUM;
int asdf_qset =    ASDF_QSET;

struct asdf_dev *asdf_devices;

static void asdf_setup_cdev(struct asdf_dev *dev, int index)
{
	int err, devno = MKDEV(asdf_major, asdf_minor + index);
   
	cdev_init(&dev->cdev, &asdf_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &asdf_fops;                                          /*登记fileoperation接口函数*/
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
	printk(KERN_NOTICE "Error %d adding asdf%d", err, index);
}

int asdf_trim(struct asdf_dev *dev)
{
	struct asdf_qset *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	for (dptr = dev->data; dptr; dptr = next) { /* all the list items 察看struct asdf_qset链表上的数据 */
		if (dptr->data) {                                          /*如果有数据*/
			for (i = 0; i < qset; i++)                            /*释放空间*/
				kfree(dptr->data[i]);
			kfree(dptr->data);
			dptr->data = NULL;
		}
		next = dptr->next;
		kfree(dptr);
	}
	dev->size = 0;
	dev->quantum = asdf_quantum;          /*定义了asdf_qset中内存分配形式的初始值quantum,qset*/
	dev->qset = asdf_qset;
	dev->data = NULL;
	return 0;
}

struct asdf_qset *asdf_follow(struct asdf_dev *dev, int n)                /*n表示链表中有多少个struct asdf_qset*/
{
 struct asdf_qset *qs = dev->data;

        /* Allocate first qset explicitly if need be */
 if (! qs) {                                                                        /*如果为空的处理*/
  qs = dev->data = kmalloc(sizeof(struct asdf_qset), GFP_KERNEL);
  if (qs == NULL)
   return NULL;  /* Never mind */
  memset(qs, 0, sizeof(struct asdf_qset));
 }

 /* Then follow the list */                                /*遍历整个链表，如果struct asdf_qset为空，进行初始化*/
 while (n--) {                                                     /*遍历n-1次*，从struct asdf_dev传下来的struct asdf_qset开始*/
  if (!qs->next) {                                                /*如果为空，初始化，设空值*/
   qs->next = kmalloc(sizeof(struct asdf_qset), GFP_KERNEL);
   if (qs->next == NULL)
    return NULL;                                               /* 若此为最后一个struct asdf_qset,函数返回 Never mind */
   memset(qs->next, 0, sizeof(struct asdf_qset));
  }
  qs = qs->next;                                            /*若不为空的时候将qs指针值设为指向下一个struct qset的指针*/
  continue;
 }
 return qs;                                                    /*返回指向最后一个struct asdf_qset的指针*/
}


int asdf_init_module(void)
{
	int result,i;
	dev_t dev;

	if(asdf_major){
		dev = MKDEV(asdf_major,asdf_minor);
		result = register_chrdev_region(dev,asdf_devs,"asdf");                          /*2.6内核模块注册方法*/
	}
	else{
		result = alloc_chrdev_region(&dev,asdf_minor,asdf_devs,"asdf");
		asdf_major = MAJOR(dev);
	}
	if(result < 0)
		return result;
 
	asdf_devices = kmalloc(asdf_devs*sizeof(struct asdf_dev),GFP_KERNEL);
	if(!asdf_devs){
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(asdf_devices,0,asdf_devs*sizeof(struct asdf_dev));
	/* initalize each devices*/
	for(i=0;i<asdf_devs;i++)
	{ 
		asdf_devices[i].quantum = asdf_quantum;
		asdf_devices[i].qset = asdf_qset;
		init_MUTEX(&asdf_devices[i].sem);
		asdf_setup_cdev(&asdf_devices[i], i);
	}
	return 0;       /* succeed */

fail_malloc:
	unregister_chrdev_region(dev,asdf_devs);
	return result;
}

void asdf_cleanup_module(void)
{
 int i;
 dev_t devno = MKDEV(asdf_major, asdf_minor);

 /* Get rid of our char dev entries */
 if (asdf_devices) {
  for (i = 0; i < asdf_devs; i++) {
   /*now we clean the data source in struct asdf_dev -> strcut asdf_qet -> data*/
   asdf_trim(asdf_devices + i);
   cdev_del(&asdf_devices[i].cdev);
  }
  kfree(asdf_devices);
 }

 /* cleanup_module is never called if registering failed */
 unregister_chrdev_region(devno, asdf_devs);

 /* and call the cleanup functions for friend devices */
 //asdf_p_cleanup();
 //asdf_access_cleanup();
}

static int asdf_open(struct inode *inode,struct file *filp)
{
	struct asdf_dev *dev;
	/*find the device */
	dev = container_of(inode->i_cdev,struct asdf_dev ,cdev);                        /*这里注意以下2.6内核模块注册的*/
	filp->private_data = dev;                   /* for other methods */                        /*方式，与2.4所用的函数有家大区别*/

	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (down_interruptible(&dev->sem))
			return -ERESTARTSYS;
		asdf_trim(dev); /* ignore errors */
		up(&dev->sem);
	}       
	printk("^_^:open %s\n",current->comm);
	return 0;           /* success */
}

static int asdf_release(struct inode *inode,struct file *filp)
{
	printk("^_^:close\n");
	return 0;
}

static ssize_t asdf_read(struct file *filp,char *buf,size_t count,loff_t *f_pos)
{
 struct asdf_dev *dev = filp->private_data;
 struct asdf_qset *dptr;    /*frist  linking chain*/ 
 int quantum = dev->quantum;
 int qset = dev->qset;
 int itemsize = quantum*qset;    /*how many byte in this linking chain*/
 int item ,rest;
 int s_pos,q_pos;
 ssize_t retval = 0;
 
 if(down_interruptible(&dev->sem))
  return -ERESTARTSYS;
 if(*f_pos>=dev->size)
 goto out;
 if(*f_pos+count > dev->size)
  count = dev->size - *f_pos;
 
 item = (long)*f_pos / itemsize;
 rest = (long)*f_pos % itemsize;
 s_pos = rest / quantum;
 q_pos = rest % quantum;
 
 dptr = asdf_follow(dev,item);                                                          /*定位f_pos指针所在的struct asdf_qset*/
 
 if (dptr == NULL || !dptr->data || ! dptr->data[s_pos])
  goto out; /* don't fill holes */

 /* read only up to the end of this quantum */
 if (count > quantum - q_pos)
  count = quantum - q_pos;

 if (copy_to_user(buf, dptr->data[s_pos] + q_pos, count)) {           /*精确定位，并拷贝数据送到用户空间*/
  retval = -EFAULT;
  goto out;
 }
 *f_pos += count;
 retval = count;

  out:
 up(&dev->sem);
 return retval;
 
}

static ssize_t asdf_write(struct file *filp,const char *buf,size_t count,loff_t *f_pos)
{
 
 struct asdf_dev *dev = filp->private_data;
 struct asdf_qset *dptr;
 int quantum = dev->quantum, qset = dev->qset;
 int itemsize = quantum * qset;
 int item, s_pos, q_pos, rest;
 ssize_t retval = -ENOMEM; /* value used in "goto out" statements */

 if (down_interruptible(&dev->sem))
  return -ERESTARTSYS;

 /* find listitem, qset index and offset in the quantum */
 item = (long)*f_pos / itemsize;
 rest = (long)*f_pos % itemsize;
 s_pos = rest / quantum; q_pos = rest % quantum;

 /* follow the list up to the right position */
 dptr = asdf_follow(dev, item);
 if (dptr == NULL)
  goto out;
 if (!dptr->data) {
  dptr->data = kmalloc(qset * sizeof(char *), GFP_KERNEL);
  if (!dptr->data)
   goto out;
  memset(dptr->data, 0, qset * sizeof(char *));
 }
 if (!dptr->data[s_pos]) {
  dptr->data[s_pos] = kmalloc(quantum, GFP_KERNEL);
  if (!dptr->data[s_pos])
   goto out;
 }
 /* write only up to the end of this quantum */
 if (count > quantum - q_pos)
  count = quantum - q_pos;

 if (copy_from_user(dptr->data[s_pos]+q_pos, buf, count)) {
  retval = -EFAULT;
  goto out;
 }
 *f_pos += count;
 retval = count;

        /* update the size */
 if (dev->size < *f_pos)
  dev->size = *f_pos;

  out:
 up(&dev->sem);
 return retval;
 
}

static loff_t asdf_lseek(struct file *filp,loff_t offset,int orig)
{
 struct asdf_dev *dev = filp->private_data;
 loff_t newpos;
 
 switch(orig){
  case 0: newpos = offset;
   break;
  case 1: newpos = filp->f_pos + offset;
   break;
  case 2: newpos = dev->size + offset;
   break;
  default:
   return -EINVAL;
  }
 if(newpos<0)  return -EINVAL;
 return filp->f_pos = newpos;
}


static int asdf_ioctl(struct inode *inode,struct file *filp,unsigned int cmd,unsigned long arg)
{
 int err = 0;
 int retval = 0;
/* int tmp;
*/ 
 if(_IOC_TYPE(cmd)!= ASDF_IOC_MAGIC)  return -ENOTTY;
 if(_IOC_NR(cmd)>ASDF_IOC_MAXNR)   return -ENOTTY;
 
 if(_IOC_DIR(cmd)&_IOC_READ)
  err=!access_ok(VERIFY_WRITE,(void __user*)arg,_IOC_SIZE(cmd));
 else if(_IOC_DIR(cmd)&_IOC_WRITE)
  err=!access_ok(VERIFY_READ,(void __user*)arg,_IOC_SIZE(cmd));
 if(err)
  return -EFAULT;
  
 switch(cmd){
       /*reset the value quantum and qset*/
  case ASDF_IOCRESET: asdf_quantum = ASDF_QUANTUM;
     asdf_qset = ASDF_QSET;
     break;
       /* set the value of quantum by user*/   
  case ASDF_IOCSQUANTUM: retval = __get_user(asdf_quantum,(int __user*)arg);
     break;
     
  case ASDF_IOCTQUANTUM: asdf_quantum = arg;
     break;
     
  case ASDF_IOCGQUANTUM:  retval = __put_user(asdf_quantum,(int __user*)arg);
     break;
     
  case ASDF_IOCQQUANTUM:  return asdf_quantum;
  
  /* set the value of qset by user*/   
  case ASDF_IOCSQSET: retval = __get_user(asdf_qset,(int __user*)arg);
     break;
     
  case ASDF_IOCTQSET: asdf_qset = arg;
     break;
     
  case ASDF_IOCGQSET:     retval = __put_user(asdf_qset,(int __user*)arg);
     break;
     
  case ASDF_IOCQQSET:     return asdf_qset;
  
  default:  /* redundant, as cmd was checked against MAXNR */
  return -ENOTTY;
 }
 return retval;
}
  
  
module_init(asdf_init_module);
module_exit(asdf_cleanup_module);
