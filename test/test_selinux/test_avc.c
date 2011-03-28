#include <selinux/avc.h>
#include <selinux/flask.h>
#include <selinux/av_permissions.h>
#include <pthread.h>
#include <stdio.h>
/* 审核用的回调函数*/
void audit_print(const char *fmt, ...)
{
    /* 使用stdout代替默认的stderr */
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

void audit_interp(void *data, security_class_t class, char *buf, size_t buflen)
{
    /* 数据是一个文件名*/
    snprintf(buf, buflen, (char*)data);
}

/* 线程的功能实现函数，运行libselinux库的函数avc_netlink_loop，用户可定义线程的运行方式 */
void* create_thread_helper(void *arg)
{
    /* arg 是需要运行的函数*/
    void (*run)(void) = (void (*)(void))arg;
    /*设置本线程到立即取消模式*/
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    /*运行工作，run为libselinux库的函数avc_netlink_loop，用于侦听内核SELinux的消息*/
    run();
    /*永远不应该运行到这里*/
    return NULL;
}

/*libselinux库的函数avc_init将调用这个函数创建线程，
并利用这个线程运行侦听函数avc_netlink_loop侦听内核SELinux的消息。*/
void* create_thread(void (*run)(void))
{
    int rc;
    pthread_t *t = (pthread_t*)malloc(sizeof(pthread_t));
    if (!t) {
        puts("create_thread: out of memory");
        exit(99);               //99为用户自己定义的错误标识码，以方便查找错误的出处
    }

     /*创建一个新线程运行辅助函数create_thread_helper */
     rc = pthread_create(t, NULL, create_thread_helper, (void*)run);
     if (rc) {
        puts("create_thread failed");
        exit(2);
    }
    return t;
}

void stop_thread(void *thread)
{
    int rc = pthread_cancel(*((pthread_t*)thread));    //取消线程
    if (rc) {
        puts("trouble stopping thread");
        exit(2);
    }
    free(thread);
}

/*分配线程锁 */
void* alloc_lock(void)
{
    int rc;
    pthread_mutexattr_t pma;
    pthread_mutex_t *m = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
    if (!m) {
        puts("alloc_lock: out of memory");
       exit(99);
    }

    /*为了调试的目的，对错误检查设置锁*/
    rc = pthread_mutexattr_init(&pma);
    rc |= pthread_mutexattr_settype(&pma, PTHREAD_MUTEX_ERRORCHECK_NP);
    rc |= pthread_mutex_init(m, &pma);
    rc |= pthread_mutexattr_destroy(&pma);
    if (rc) {
        puts("trouble initializing lock");
        exit(3);
    }
    return m;
}

/*得到线程锁*/
void get_lock(void *lock)
{
    int rc = pthread_mutex_lock((pthread_mutex_t*)lock);
    if (rc) {
        puts("trouble obtaining lock");
        exit(3);
    }
}

/*释放线程锁*/
void release_lock(void *lock)
{
     int rc = pthread_mutex_unlock((pthread_mutex_t*)lock);
    if (rc) {
        puts("trouble releasing lock");
        exit(3);
    }
}

/*销毁线程锁*/
void free_lock(void *lock)
{
    int rc = pthread_mutex_destroy((pthread_mutex_t*)lock);
    if (rc) {
        puts("trouble destroying lock");
        exit(3);
    }
    free(lock);
}

/* 主函数 */
/*应用程序的功能有：设置回调函数，初始化AVC，从标准输入读出文件路径名，
得到进程域和文件的安全上下文，查找AVC或通过内核SELinux得到策略决策，打印统计信息 */
int main (int argc, char **argv) {
    security_context_t scon, fcon;
//	security_context_t s = "unconfined_u:unconfined_r:unconfine_t:s0-s0:c0.c1023";
	security_context_t rscon;
    security_id_t ssid, fsid;
    char buf[1024];
    struct avc_entry_ref aeref;
    struct avc_cache_stats acs;
    int rc, short_of_memory = 0;

    /*注册 logging回调函数*/
    struct avc_log_callback alc = {
        audit_print,
        audit_interp
    };

    /*设置线程回调函数*/
    struct avc_thread_callback atc = {
        create_thread,
        stop_thread
    };

    /*设置锁用于锁住回调 */
    struct avc_lock_callback akc = {
        alloc_lock,
        get_lock,
        release_lock,
        free_lock
    };

    avc_entry_ref_init(&aeref);
    /*初始化客体管理器 */
    if (avc_init("myprog", NULL, &alc, &atc, &akc) < 0) {
         puts("could not initialize avc");
        exit(1);
    }

    /*得到本进程的安全上下文和SID*/
    if (getcon(&scon) < 0) {
        puts("could not get self context");
        exit(5);
    }
//***************************************
	if (security_canonicalize_context(scon,&rscon)){
		printf("can not convert\n");
		exit(5);
	}
printf("This process's security context:\t%s\n",scon);
printf("This process's raw security context:\t%s\n",rscon);
//***************************************
    if (avc_context_to_sid(scon, &ssid) < 0) {
        puts("could not get self sid");
        exit(5);
    }

    /* 从标准输入读取文件名*/
    while (scanf("%s", buf) != EOF)
    {
//printf("thank you \n");
        /*如果内存空间不够，释放没用的cache条目*/
        if (short_of_memory)
            avc_cleanup();

        /*得到文件的安全上下文和SID*/
        if (getfilecon(buf, &fcon) < 0) {
            printf("couldn’t get file context for ’%s’\n", buf);
            continue;
        }
printf("This target's security context:\t%s\n",fcon);
        if (avc_context_to_sid(fcon, &fsid) < 0) {
            printf("could not get file sid for ’%s’\n", buf);
            exit(5);
        }

        /*查看本进程有否访问文件的权限*/
        errno = 0;
        rc = avc_has_perm(ssid, fsid, 
                 SECCLASS_FILE, FILE__READ | FILE__WRITE | FILE__UNLINK,
                &aeref, buf);
        if (rc == 0)          //允许操作
            printf("%s: granted\n", buf);
        else if (errno == EACCES)   //不允许操作
            printf("%s: denied\n", buf);
        else
            printf("%s: unexpected error: %s\n", buf, strerror(errno));
    }   //endwhile

    /*打印统计信息*/
    avc_av_stats();
    avc_sid_stats();
    avc_cache_stats(&acs);
    printf("entry_lookups:\t%d\n", acs.entry_lookups);
    printf("entry_hits:\t%d\n", acs.entry_hits);
    printf("entry_misses:\t%d\n", acs.entry_misses);
    printf("entry_discards:\t%d\n", acs.entry_discards);
    printf("cav_lookups:\t%d\n", acs.cav_lookups);
    printf("cav_hits:\t%d\n", acs.cav_hits);
    printf("cav_probes:\t%d\n", acs.cav_probes);
    printf("cav_misses:\t%d\n", acs.cav_misses);
    /*释放所有AVC资源*/
    avc_destroy();
    return 0;
}
