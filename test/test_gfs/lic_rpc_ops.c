#include <limits.h>

#include <glusterfs/rpc/xdr-generic.h>
#include <glusterfs/rpc/protocol-common.h>
#include <glusterfs/rpc/rpc-clnt.h>
#include <glusterfs/rpc/cli1-xdr.h>

struct rpc_clnt_procedure gluster_cli_actors[GLUSTER_CLI_MAXVALUE] = {
//        [GLUSTER_CLI_NULL]             = {"NULL",              NULL },
//        [GLUSTER_CLI_PROBE]            = {"PROBE_QUERY",       gf_cli_probe},
//        [GLUSTER_CLI_DEPROBE]          = {"DEPROBE_QUERY",     gf_cli_deprobe},
//        [GLUSTER_CLI_LIST_FRIENDS]     = {"LIST_FRIENDS",      gf_cli_list_friends},
//        [GLUSTER_CLI_UUID_RESET]       = {"UUID_RESET",        gf_cli3_1_uuid_reset},
//        [GLUSTER_CLI_UUID_GET]         = {"UUID_GET",          gf_cli3_1_uuid_get},
//        [GLUSTER_CLI_CREATE_VOLUME]    = {"CREATE_VOLUME",     gf_cli_create_volume},
//        [GLUSTER_CLI_DELETE_VOLUME]    = {"DELETE_VOLUME",     gf_cli_delete_volume},
//        [GLUSTER_CLI_START_VOLUME]     = {"START_VOLUME",      gf_cli_start_volume},
//        [GLUSTER_CLI_STOP_VOLUME]      = {"STOP_VOLUME",       gf_cli_stop_volume},
//        [GLUSTER_CLI_RENAME_VOLUME]    = {"RENAME_VOLUME",     gf_cli_rename_volume},
//        [GLUSTER_CLI_DEFRAG_VOLUME]    = {"DEFRAG_VOLUME",     gf_cli_defrag_volume},
//        [GLUSTER_CLI_GET_VOLUME]       = {"GET_VOLUME",        gf_cli_get_volume},
//        [GLUSTER_CLI_GET_NEXT_VOLUME]  = {"GET_NEXT_VOLUME",   gf_cli_get_next_volume},
//        [GLUSTER_CLI_SET_VOLUME]       = {"SET_VOLUME",        gf_cli_set_volume},
//        [GLUSTER_CLI_ADD_BRICK]        = {"ADD_BRICK",         gf_cli_add_brick},
//        [GLUSTER_CLI_REMOVE_BRICK]     = {"REMOVE_BRICK",      gf_cli_remove_brick},
//        [GLUSTER_CLI_REPLACE_BRICK]    = {"REPLACE_BRICK",     gf_cli_replace_brick},
//        [GLUSTER_CLI_LOG_ROTATE]       = {"LOG ROTATE",        gf_cli_log_rotate},
//        [GLUSTER_CLI_GETSPEC]          = {"GETSPEC",           gf_cli_getspec},
//        [GLUSTER_CLI_PMAP_PORTBYBRICK] = {"PMAP PORTBYBRICK",  gf_cli_pmap_b2p},
//        [GLUSTER_CLI_SYNC_VOLUME]      = {"SYNC_VOLUME",       gf_cli_sync_volume},
//        [GLUSTER_CLI_RESET_VOLUME]     = {"RESET_VOLUME",      gf_cli_reset_volume},
//        [GLUSTER_CLI_FSM_LOG]          = {"FSM_LOG",           gf_cli_fsm_log},
//        [GLUSTER_CLI_GSYNC_SET]        = {"GSYNC_SET",         gf_cli_gsync_set},
//        [GLUSTER_CLI_PROFILE_VOLUME]   = {"PROFILE_VOLUME",    gf_cli_profile_volume},
//        [GLUSTER_CLI_QUOTA]            = {"QUOTA",             gf_cli_quota},
//        [GLUSTER_CLI_TOP_VOLUME]       = {"TOP_VOLUME",        gf_cli_top_volume},
//        [GLUSTER_CLI_GETWD]            = {"GETWD",             gf_cli_getwd},
//        [GLUSTER_CLI_STATUS_VOLUME]    = {"STATUS_VOLUME",     gf_cli_status_volume},
//        [GLUSTER_CLI_STATUS_ALL]       = {"STATUS_ALL",        gf_cli_status_volume_all},
//        [GLUSTER_CLI_MOUNT]            = {"MOUNT",             gf_cli_mount},
//        [GLUSTER_CLI_UMOUNT]           = {"UMOUNT",            gf_cli_umount},
//        [GLUSTER_CLI_HEAL_VOLUME]      = {"HEAL_VOLUME",       gf_cli_heal_volume},
//        [GLUSTER_CLI_STATEDUMP_VOLUME] = {"STATEDUMP_VOLUME",  gf_cli_statedump_volume},
//        [GLUSTER_CLI_LIST_VOLUME]      = {"LIST_VOLUME",       gf_cli_list_volume},
//        [GLUSTER_CLI_CLRLOCKS_VOLUME]  = {"CLEARLOCKS_VOLUME", gf_cli_clearlocks_volume},
//        [GLUSTER_CLI_COPY_FILE]        = {"COPY_FILE",         gf_cli_copy_file},
//        [GLUSTER_CLI_SYS_EXEC]         = {"SYS_EXEC",          gf_cli_sys_exec},
};

struct rpc_clnt_program lic_prog = {
        .progname  = "Gluster CLI",
        .prognum   = GLUSTER_CLI_PROGRAM,
        .progver   = GLUSTER_CLI_VERSION,
        .numproc   = GLUSTER_CLI_MAXVALUE,
        .proctable = gluster_cli_actors,
};

struct rpc_clnt *g_rpc = NULL;
int    connected = 0;
static pthread_cond_t      conn  = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t     conn_mutex = PTHREAD_MUTEX_INITIALIZER;

void lic_cond_init(void)
{
       pthread_mutex_init (&conn_mutex, NULL);
       pthread_cond_init (&conn, NULL);
}

static void
seconds_from_now (unsigned secs, struct timespec *ts)
{
        struct timeval tv = {0,};

        gettimeofday (&tv, NULL);

        ts->tv_sec = tv.tv_sec + secs;
        ts->tv_nsec = tv.tv_usec * 1000;
}

int32_t
lic_await_connected (unsigned conn_timo)
{
        int32_t                 ret = 0;
        struct  timespec        ts = {0,};

        if (!conn_timo)
                return 0;

        pthread_mutex_lock (&conn_mutex);
        {
                seconds_from_now(conn_timo, &ts);
                while (!connected && !ret) {
                        ret = pthread_cond_timedwait(&conn, &conn_mutex,
                                                      &ts);
                }
        }
        pthread_mutex_unlock (&conn_mutex);


        return ret;
}

int32_t
lic_broadcast_connected (void)
{
        pthread_mutex_lock (&conn_mutex);
        {
                connected = 1;
                pthread_cond_broadcast (&conn);
        }

        pthread_mutex_unlock (&conn_mutex);

        return 0;
}

int
lic_submit_request(void *req, call_frame_t *frame, rpc_clnt_prog_t *prog,
		int procnum, fop_cbk_fn_t cbkfn, xdrproc_t xdrproc)
{
        int                     ret         = -1;
        struct iovec            iov         = {0};
        struct iobuf            *iobuf      = NULL;
        ssize_t                 xdr_size    = 0;

        if (req) {
                xdr_size = xdr_sizeof(xdrproc, req);
                iobuf = iobuf_get2(THIS->ctx->iobuf_pool, xdr_size);
                if (!iobuf) {
                        goto out;
                };

                iov.iov_base = iobuf->ptr;
                iov.iov_len  = iobuf_size (iobuf);

                /* Create the xdr payload */
                ret = xdr_serialize_generic (iov, req, xdrproc);
                if (ret == -1)
                        goto out;

                iov.iov_len = ret;
        }

	lic_await_connected(60);

        /* Send the msg */
        ret = rpc_clnt_submit (g_rpc, prog, procnum, cbkfn,
                               &iov, 1,
                               NULL, 0, NULL, frame, NULL, 0, NULL, 0, NULL);
        ret = 0;

out:
        if (iobuf)
                iobuf_unref (iobuf);
        return ret;
}

int lic_dict_add_words(dict_t *dict, const char **words, int wordcount)
{
        char  *cmd = NULL;
	int   len = 0;
	int   i = 0;
	int   ret = 0;

	for (i=0; i<wordcount; i++)
                len += strlen(words[i]) + 1;

	if (len < 1)
		goto out;

        cmd = GF_CALLOC(1, len, gf_common_mt_char);
	if (!cmd) {
		ret = -1;
		goto out;
	}

        for (i=0; i<wordcount; i++) {
                strncat(cmd, words[i], strlen(words[i]));
                if (i < wordcount-1)
                        strncat (cmd, " ", strlen (" "));
        }

        ret = dict_set_dynstr (dict, "cmd-str", cmd);
        if (ret)
                goto out;

out:
	return ret;
}

int lic_sys_exec_cbk(struct rpc_req *req, struct iovec *iov,
                     int count, void *myframe)
{
        int                     ret     = -1;
        int                     output_count     = -1;
        int                     i     = -1;
        char                   *output  = NULL;
        char                   *command = NULL;
        char                    output_name[PATH_MAX] = "";
        gf_cli_rsp              rsp     = {0, };
        dict_t                  *dict   = NULL;
        call_frame_t            *frame  = NULL;

        frame = myframe;

	gf_log(frame->this->name, GF_LOG_TRACE, "Recieve response.");

	if (count != 1) {
                gf_log(frame->this->name, GF_LOG_ERROR,
                        "count is not 1. %d.", count);
		ret = -1;
		goto out;
	}

        if (req->rpc_status == -1) {
                gf_log(frame->this->name, GF_LOG_ERROR,
                        "rpc_status is -1.");
                ret = -1;
                goto out;
        }

        ret = xdr_to_generic (*iov, &rsp, (xdrproc_t)xdr_gf_cli_rsp);
        if (ret < 0) {
                gf_log(frame->this->name, GF_LOG_ERROR,
                        "Failed to decode xdr response");
                goto out;
        }

        dict = dict_new ();

        if (!dict) {
                gf_log("lic", GF_LOG_ERROR, "Failed to alloc dict.");
                ret = -1;
                goto out;
        }

        ret = dict_unserialize (rsp.dict.dict_val, rsp.dict.dict_len, &dict);

        if (ret) {
                gf_log("lic", GF_LOG_ERROR, "Unserialize response failed.");
                goto out;
	}

        if (rsp.op_ret) {
                gf_log("cli", GF_LOG_ERROR, "%s", rsp.op_errstr ? rsp.op_errstr :
                         "Command failed.");
                ret = rsp.op_ret;
                goto out;
        }

        ret = dict_get_int32 (dict, "output_count", &output_count);
        if (ret) {
                gf_log("cli", GF_LOG_NOTICE, "Command executed successfully.");
                ret = 0;
                goto out;
        }

        ret = dict_get_str (dict, "command", &command);
        if (ret) {
                gf_log("cli", GF_LOG_ERROR,
                        "Unable to get command from dict");
                goto out;
        }

        for (i = 1; i <= output_count; i++) {
                memset (output_name, '\0', sizeof (output_name));
                snprintf (output_name, sizeof (output_name),
                          "output_%d", i);
                ret = dict_get_str(dict, output_name, &output);
                if (ret) {
                        gf_log("cli", GF_LOG_ERROR, "Failed to get %s.",
                                output_name);
                }
                if (output) {
                        printf("%s", output);
                        output = NULL;
                }
        }
	printf("\n");

        ret = 0;
out:
        if (dict)
                dict_unref (dict);

        free (rsp.dict.dict_val);

        return ret;
}

int lic_sys_exec(call_frame_t *frame)
{
        gf_cli_req  req = {{0,}};
        dict_t      *dict = NULL;
	int         cmd_args_count = 0;
        int         ret = 0;

        dict = dict_new();
        if (!dict)
                goto out;

#if 0
        char        cmd_arg_name[PATH_MAX] = "";
	snprintf(cmd_arg_name, sizeof(cmd_arg_name), "cmd_arg_%d", cmd_args_count++);
        ret = dict_set_str (dict, cmd_arg_name, "ls");
        if (ret) {
                gf_log ("", GF_LOG_ERROR, "Unable to set %s in dict", cmd_arg_name);
                goto out;
        }
#endif
        ret = dict_set_int32(dict, "cmd_args_count", cmd_args_count);
        if (ret) {
                gf_log("", GF_LOG_ERROR, "Unable to set cmd_args_count in dict");
		goto out;
	}

        ret = dict_set_str(dict, "command", "ls");
        if (ret) {
                gf_log("", GF_LOG_ERROR, "Unable to set command in dict");
                goto out;
        }

        ret = dict_set_str(dict, "volname", "N/A");
        if (ret) {
                gf_log("", GF_LOG_ERROR, "Unable to set volname in dict");
                goto out;
        }

#if 0
	ret = lic_dict_add_words(dict, argv, argc);
        if (ret) {
                gf_log("", GF_LOG_ERROR, "Unable to set words in dict");
                goto out;
        }
#endif

        ret = dict_allocate_and_serialize(dict, &(req.dict.dict_val),
                                           &(req.dict.dict_len));
        if (ret) {
                gf_log("", GF_LOG_ERROR, "Unable to serialize the dict");
                goto out;
        }

	ret = lic_submit_request(&req, frame, &lic_prog,
			GLUSTER_CLI_SYS_EXEC, lic_sys_exec_cbk,
			(xdrproc_t)xdr_gf_cli_req);

out:
	return ret;
}

int
lic_rpc_notify(struct rpc_clnt *rpc, void *mydata, rpc_clnt_event_t event,
		void *data)
{
	xlator_t	*this = NULL;
	int		ret = 0;

	this = mydata;

	switch (event) {
	case RPC_CLNT_CONNECT:
		lic_broadcast_connected();
		gf_log(this->name, GF_LOG_TRACE, "got RPC_CLNT_CONNECT");
		break;

	case RPC_CLNT_DISCONNECT:
		gf_log(this->name, GF_LOG_TRACE, "got RPC_CLNT_DISCONNECT");
		connected = 0;
		break;

	default:
		gf_log (this->name, GF_LOG_TRACE,
			"got some other RPC event %d", event);
		ret = 0;
		break;
	}

	return ret;
}

int lic_rpc_init(void)
{
	struct rpc_clnt	*rpc = NULL;
	dict_t		*options = NULL;
	int		ret = 0;
	xlator_t	*this = NULL;

	this = THIS;
	options = dict_new();
	if (!options)
		goto out;

	lic_cond_init();

	ret = rpc_transport_unix_options_build(&options, DEFAULT_GLUSTERD_SOCKFILE, 0);
	if (ret)
		goto out;

	rpc = rpc_clnt_new(options, this->ctx, this->name, 16);
	if (!rpc) {
		ret = -1;
		goto out;
	}

	ret = rpc_clnt_register_notify(rpc, lic_rpc_notify, this);
	if (ret) {
		gf_log ("lic", GF_LOG_ERROR, "failed to register notify");
		goto out;
	}

	ret = rpc_clnt_start (rpc);
out:
	if (ret) {
		if (rpc)
			rpc_clnt_unref (rpc);
		rpc = NULL;
	} else {
		g_rpc = rpc;
	}

	return ret;
}
