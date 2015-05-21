#include <sys/resource.h>

#include <glusterfs/xlator.h>
#include <glusterfs/event.h>
#include <glusterfs/mem-pool.h>
#include <glusterfs/call-stub.h>
#include <glusterfs/iobuf.h>
#include <glusterfs/logging.h>
#include <glusterfs/glusterfs.h>

#include "lic_rpc_ops.h"

#define DEFAULT_LOG_PATH		"/var/log/glusterfs/lic.log"
#define DEFAULT_EVENT_POOL_SIZE		(16384)

enum _lic_mem_types {
        lic_mt_call_pool_t = gf_common_mt_end + 1,
        lic_mt_end,
};

static int
glusterfs_ctx_defaults_init(glusterfs_ctx_t *ctx)
{
	cmd_args_t    *cmd_args = NULL;
	struct rlimit  lim = {0, };
	call_pool_t   *pool = NULL;

	xlator_mem_acct_init(THIS, lic_mt_end);

	ctx->process_uuid = generate_glusterfs_ctx_id ();
	if (!ctx->process_uuid)
		return -1;

	ctx->page_size  = 128 * GF_UNIT_KB;

	ctx->iobuf_pool = iobuf_pool_new ();
	if (!ctx->iobuf_pool)
		return -1;

	ctx->event_pool = event_pool_new (DEFAULT_EVENT_POOL_SIZE);
	if (!ctx->event_pool)
		return -1;

	pool = GF_CALLOC (1, sizeof (call_pool_t),
			  lic_mt_call_pool_t);
	if (!pool)
		return -1;

	/* frame_mem_pool size 112 * 64 */
	pool->frame_mem_pool = mem_pool_new (call_frame_t, 32);
	if (!pool->frame_mem_pool)
		return -1;

	/* stack_mem_pool size 256 * 128 */
	pool->stack_mem_pool = mem_pool_new (call_stack_t, 16);

	if (!pool->stack_mem_pool)
		return -1;

	ctx->stub_mem_pool = mem_pool_new (call_stub_t, 16);
	if (!ctx->stub_mem_pool)
		return -1;

	ctx->dict_pool = mem_pool_new (dict_t, 32);
	if (!ctx->dict_pool)
		return -1;

	ctx->dict_pair_pool = mem_pool_new (data_pair_t, 512);
	if (!ctx->dict_pair_pool)
		return -1;

	ctx->dict_data_pool = mem_pool_new (data_t, 512);
	if (!ctx->dict_data_pool)
		return -1;

	INIT_LIST_HEAD (&pool->all_frames);
	LOCK_INIT (&pool->lock);
	ctx->pool = pool;

	pthread_mutex_init (&(ctx->lock), NULL);

	cmd_args = &ctx->cmd_args;

	INIT_LIST_HEAD (&cmd_args->xlator_options);

	lim.rlim_cur = RLIM_INFINITY;
	lim.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &lim);

	return 0;
}

static int
logging_init(glusterfs_ctx_t *ctx)
{
	int ret = 0;

	ret = gf_log_init(ctx, DEFAULT_LOG_PATH, NULL);
        if (ret) {
                fprintf (stderr, "ERROR: failed to open logfile %s\n",
                         DEFAULT_LOG_PATH);
		goto out;
        }

        gf_log_set_loglevel(GF_LOG_TRACE);

out:
        return ret;
}

pthread_t lic_event_thread;

void *lic_event_thread_worker(void *event_pool)
{
        
	if (event_dispatch(event_pool)) {
		gf_log("lic", GF_LOG_ERROR, "event_dispatch_start failed");
	} else {
		gf_log("lic", GF_LOG_ERROR, "event_dispatch_start stop normaly");
	}

	return NULL;
}

int lic_init(void)
{
	glusterfs_ctx_t  *ctx = NULL;
	int ret = 0;

	ctx = glusterfs_ctx_new();
        if (!ctx) {
		ret = -1;
		goto out;
	}

        gf_mem_acct_enable_set (ctx);

        ret = glusterfs_globals_init (ctx);
        if (ret)
                goto out;

	THIS->ctx = ctx;

	ret = glusterfs_ctx_defaults_init(ctx);
	if (ret)
                goto out;

        ret = logging_init(ctx);
        if (ret)
                goto out;

	ret = lic_rpc_init();
	if (ret)
		goto out;

        ret = pthread_create(&lic_event_thread, NULL, lic_event_thread_worker, ctx->event_pool);
	if (ret)
		goto out;

out:
	return ret;
}

int lic_check(void)
{
        call_frame_t     *frame = NULL;
	int              ret = 0;

        frame = create_frame(THIS, THIS->ctx->pool);
	if (!frame) {
		ret = -1;
                gf_log(THIS->name, GF_LOG_ERROR,
                        "Failed create frame for lic_sys_exec");
		goto out;
	}
	ret = lic_sys_exec(frame);

out:
	return ret;
}
