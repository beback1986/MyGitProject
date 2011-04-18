#ifndef _ASDF_H_
#define _ASDF_H_

#ifndef ASDF_MAJOR
#define ASDF_MAJOR 0   /* dynamic major by default */
#endif

#ifndef ASDF_MINOR
#define ASDF_MINOR 0   /* dynamic major by default */
#endif

#ifndef ASDF_DEVS
#define ASDF_DEVS 4    /* asdf0 through asdf3 */
#endif

#ifndef ASDF_QUANTUM
#define ASDF_QUANTUM 4000
#endif

#ifndef ASDF_QSET
#define ASDF_QSET    1000
#endif

#include <linux/ioctl.h>
#include <linux/cdev.h>


static int asdf_open(struct inode *inode,struct file *filp);
static int asdf_release(struct inode *inode,struct file *filp);
static ssize_t asdf_read(struct file *filp,char *buf,size_t count,loff_t *f_pos);
static ssize_t asdf_write(struct file *filp,const char *buf,size_t count,loff_t *f_pos);
static loff_t asdf_lseek(struct file *file,loff_t offset,int orig);
static int asdf_ioctl(struct inode *, struct file *, unsigned int, unsigned long);
/*static int asdf_mmap(struct file *, struct vm_area_struct *);
*/
struct file_operations asdf_fops={
 .owner=   THIS_MODULE,
 .open=    asdf_open,
 .release=   asdf_release,
 .read=    asdf_read,
 .write=   asdf_write,
 .llseek=  asdf_lseek,
 .ioctl=   asdf_ioctl,
 /*.mmap=    asdf_mmap,*/          
};
struct asdf_qset {
 void **data;
 struct asdf_qset *next;
};

struct asdf_dev {                //自定义的数据结构
 struct asdf_qset *data;
 struct asdf_dev *next;         
 int vmas;                            //active mapping
 int quantum;                     //current size of quantu
 int qset;                             //current array size
 int order;                            //current allocation order
 size_t size;                        //32 bits will suffice
 struct semaphore  sem;   //Mutual exclusion
 struct cdev cdev;
};

int asdf_trim(struct asdf_dev *dev);                    
 

extern int asdf_major;
extern int asdf_devs;
extern int asdf_quantum;
extern int asdf_qset;

/*
 * Ioctl definitions
 */

/* Use 'k' as magic number */
#define ASDF_IOC_MAGIC  'k'
/* Please use a different 8-bit number in your code */

#define ASDF_IOCRESET    _IO(ASDF_IOC_MAGIC, 0)

/*
 * S means "Set" through a ptr,
 * T means "Tell" directly with the argument value
 * G means "Get": reply by setting through a pointer
 * Q means "Query": response is on the return value
 */
#define ASDF_IOCSQUANTUM  _IOW(ASDF_IOC_MAGIC,  1, int)      /*设定9个ioctl控制参数，对quantum和*/
#define ASDF_IOCSQSET     _IOW(ASDF_IOC_MAGIC,  2, int)            /*qset的值设定与读取*/
#define ASDF_IOCTQUANTUM  _IO(ASDF_IOC_MAGIC,   3)
#define ASDF_IOCTQSET     _IO(ASDF_IOC_MAGIC,   4)
#define ASDF_IOCGQUANTUM  _IOR(ASDF_IOC_MAGIC,  5, int)
#define ASDF_IOCGQSET     _IOR(ASDF_IOC_MAGIC,  6, int)
#define ASDF_IOCQQUANTUM  _IO(ASDF_IOC_MAGIC,   7)
#define ASDF_IOCQQSET     _IO(ASDF_IOC_MAGIC,   8)

#define ASDF_IOC_MAXNR 8

#endif /* _ASDF_H_ */
