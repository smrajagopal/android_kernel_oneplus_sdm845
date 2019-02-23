#include <linux/memory.h>
#include <linux/types.h>
#include <soc/qcom/scm.h>
#include <uapi/linux/qseecom.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/fdtable.h>
#include <soc/qcom/qseecomi.h>
#include <linux/uaccess.h>

#include <misc/tz_logger.h>
#include "tz_logger_qcom.h"

#define FILL_RESP(resp, desc) do { resp.result = desc->ret[0]; \
    resp.resp_type = desc->ret[1];\
    resp.data = desc->ret[2]; } while(0)

#define DORMIENT_SECS 60
/// pass this point only after \c{DORMIENT_SECS} seconds, use to collect crashed from PSTORE
#define DORMIENT if((local_clock()/1000000000) <= DORMIENT_SECS) { return; }

struct inc_info { struct task_struct *task; struct list_head list; tzlog_ctx_armv8 ctx;} pending;
static LIST_HEAD(pending_list);
static DEFINE_SPINLOCK(inc_lock);

static void dump_memory_scm(int id, const char *tag, void* start, unsigned int nword){
    unsigned int i;
    u64 *ptr = (u64*) start;
    u64 val = 0, prev;
    bool dots = false;

    for(i=0;i<min(3u,nword);i++) {
        val = *ptr++;
        tzlog_puts("%02d %s %04d: %016llx\n", id, tag, i, val);
    }

    for(;i<(nword > 3 ? nword-3u : 0);i++){
        prev = val;
        val = *ptr++;

        if(prev || val || i<=3 || i+3u>=nword) {
            tzlog_puts("%02d %s %04d: %016llx\n", id, tag, i, val);
            dots = false;
        } else if (!dots) {
            dots = true;
            tzlog_puts("%02d %s ...\n", id, tag);
        }
    }

    for(;i<nword;i++) {
        tzlog_puts("%02d %s %04d: %016llx\n", id, tag, i, *ptr++);
    }
}

static void enqueue_context(tzlog_ctx_armv8 *ctx) {
    struct inc_info *iinfo;
    gfp_t flags = ctx->base.base.atomic ? GFP_ATOMIC : GFP_KERNEL;

    iinfo = kmalloc(sizeof(struct inc_info) + sizeof(struct scm_desc), flags);
    if(!iinfo) {
        tzlog_puts("%02d ENOMEM unable to allocate new pending structure\n", ctx->base.base.id);
        return;
    }

    iinfo->task = current;
    memcpy(&iinfo->ctx, ctx, sizeof(tzlog_ctx_armv8));

    iinfo->ctx.desc = (struct scm_desc *) ((void *)iinfo + sizeof(struct scm_desc));
    memcpy(iinfo->ctx.desc, ctx->desc, sizeof(struct scm_desc));

    spin_lock(&inc_lock);
    list_add_tail(&iinfo->list, &pending_list);
    spin_unlock(&inc_lock);

    pr_err("%02d enqueued for completion\n", ctx->base.base.id);
    ctx->base.base.call_state = TZ_CALL_PENDING;
}

// IOMMU dissectors

struct msm_scm_paddr_list {
    unsigned int list;
    unsigned int list_size;
    unsigned int size;
};

struct msm_scm_mapping_info {
    unsigned int id;
    unsigned int ctx_id;
    unsigned int va;
    unsigned int size;
};

struct msm_scm_map2_req {
    struct msm_scm_paddr_list plist;
    struct msm_scm_mapping_info info;
    unsigned int flags;
};

void tz_logger_dissect2_IOMMU_SECURE_MAP2_FLAT(tzlog_ctx_armv8 *ctx) {
    struct scm_desc *desc = ctx->desc;
    struct msm_scm_map2_req map = {{0}, {0}, 0};

    map.plist.list = desc->args[0];
    map.plist.list_size = desc->args[1];
    map.plist.size = desc->args[2];
    map.info.id = desc->args[3];
    map.info.ctx_id = desc->args[4];
    map.info.va = desc->args[5];
    map.info.size = desc->args[6];

#ifdef CONFIG_MSM_IOMMU_TLBINVAL_ON_MAP
    map.flags = IOMMU_TLBINVAL_FLAG;
#endif

    tzlog_puts("%02d { .plist.list = %#x, .plist.list_size=%d, .plist.size=%d, "
           ".info.id=%d, .info.ctx_id=%d, .info.va=%#x, .info.size=%d }\n",
           ctx->base.base.id, map.plist.list, map.plist.list_size, map.plist.size,
           map.info.id, map.info.ctx_id, map.info.va, map.info.size);
}

// TZ_APP dissectors

enum qseecom_client_handle_type {
    QSEECOM_CLIENT_APP = 1,
    QSEECOM_LISTENER_SERVICE,
    QSEECOM_SECURE_SERVICE,
    QSEECOM_GENERIC,
    QSEECOM_UNAVAILABLE_CLIENT_APP,
};

struct qseecom_client_handle {
    u32  app_id;
    u8 *sb_virt;
    phys_addr_t sb_phys;
    unsigned long user_virt_sb_base;
    size_t sb_length;
    struct ion_handle *ihandle;		/* Retrieve phy addr */
    char app_name[MAX_APP_NAME_SIZE];
};

struct qseecom_listener_handle {
    u32               id;
};

struct qseecom_dev_handle {
    enum qseecom_client_handle_type type;
    union {
        struct qseecom_client_handle client;
        struct qseecom_listener_handle listener;
    };
    bool released;
    int               abort;
    wait_queue_head_t abort_wq;
    atomic_t          ioctl_count;
    bool  perf_enabled;
    bool  fast_load_enabled;
    enum qseecom_bandwidth_request_mode mode;
};

static char *_resp2str[] = {"QSEOS_APP_ID", "QSEOS_LISTENER_ID"};

static __always_inline char *resp2str(enum qseecom_command_scm_resp_type type) {
    int offset = (type - QSEOS_APP_ID);
    if(offset < ARRAY_SIZE(_resp2str)) {
        return _resp2str[offset];
    }
    return "NOT_FOUND";
}

static unsigned long __qseecom_kphys_to_uvirt(struct qseecom_dev_handle *data,
                        phys_addr_t kphys)
{
    return (kphys - data->client.sb_phys) + data->client.user_virt_sb_base;
}

static struct qseecom_dev_handle* get_qseecom_data(u32 app_id) {
    struct fdtable *fdt = files_fdtable(current->files);
    struct file **files = fdt->fd;

    for(files=fdt->fd;*files;files++) {
        if(!strcmp((*files)->f_path.dentry->d_iname, "qseecom") &&
                ((struct qseecom_dev_handle *) (*files)->private_data)->client.app_id == app_id)
        { break; }
    }

    if(!*files) {
        return NULL;
    }

    return (struct qseecom_dev_handle *)(*files)->private_data;
}

void fingerprint_cmd_dissector(tzlog_ctx_armv8 *ctx, struct qseecom_dev_handle *data) {
    uint32_t *ptr;

    ptr = (uint32_t *) __qseecom_kphys_to_uvirt(data, ctx->desc->args[1]);
    tzlog_puts("%02d FINGERPRINT: command=%#x\n", ctx->base.base.id, *ptr);
}

void tz_logger_dissect2_RESPONSE_HANDLER(tzlog_ctx_armv8 *ctx) {
    int i;
    bool found = false;
    struct inc_info *iinfo = NULL;
    struct task_struct *task = current;
    struct qseecom_command_scm_resp resp;

    if(ctx->base.base.call_state != TZ_CALL_AFTER) { return; }

    FILL_RESP(resp, ctx->desc);

    if(resp.result == QSEOS_RESULT_INCOMPLETE ) { return; }

    spin_lock(&inc_lock);

    list_for_each_entry(iinfo, &pending_list, list) {
        if(iinfo->task == task) {
            found=true;
            list_del(&iinfo->list);
            break;
        }
    }

    spin_unlock(&inc_lock);

    if(found) {
        tzlog_puts("%02d completing %02d\n", ctx->base.base.id, iinfo->ctx.base.base.id);

        // update pending data
        for(i=0;i<MAX_SCM_RETS;i++) {
            iinfo->ctx.desc->ret[i] = ctx->desc->ret[i];
        }
        iinfo->ctx.base.base.ret = ctx->base.base.ret;
        iinfo->ctx.base.base.call_state = TZ_CALL_AFTER;

        tzlogger_call(SEMANTIC_HAVE_CTX, &iinfo->ctx);

        kfree(iinfo);
    } else {
        tzlog_puts("%02d no dissectors are waiting\n", ctx->base.base.id);
    }
}

void tz_logger_dissect2_LOOKUP(tzlog_ctx_armv8 *ctx) {
    struct qseecom_command_scm_resp resp;
    struct scm_desc *desc = ctx->desc;
    int len;
    char *name;

    if(ctx->base.base.call_state != TZ_CALL_AFTER) { return; }

    name = (char *) phys_to_virt(desc->args[0]);
    len = (int) desc->args[1];

    FILL_RESP(resp, desc);

    switch(resp.result) {
    case QSEOS_RESULT_SUCCESS:
        switch(resp.resp_type) {
        case QSEOS_APP_ID:
            tzlog_puts("%02d LOOKUP(%.*s): %u\n", ctx->base.base.id, len, name, resp.data);
            break;
        default:
            tzlog_puts("%02d LOOKUP(%.*s): unknow response type %x, data=%u\n",
                       ctx->base.base.id, len, name, resp.resp_type, resp.data);
        }
        break;
    case QSEOS_RESULT_FAILURE:
        tzlog_puts("%02d LOOKUP(%.*s): NOT_FOUND\n", ctx->base.base.id, len, name);
        break;
    case QSEOS_RESULT_INCOMPLETE:
        enqueue_context(ctx);
        break;
    default:
        tzlog_puts("%02d LOOKUP(%.*s): unknown result %x\n", ctx->base.base.id, len, name, resp.result);
    }
}

void tz_logger_dissect2_START(tzlog_ctx_armv8 *ctx) {
    struct qseecom_command_scm_resp resp;

    if(ctx->base.base.call_state != TZ_CALL_AFTER) { return; }

    FILL_RESP(resp, ctx->desc);

    if(resp.result == QSEOS_RESULT_INCOMPLETE) {
        enqueue_context(ctx);
        return;
    }

    if(resp.result != QSEOS_RESULT_SUCCESS || ctx->base.base.ret ) {
        tzlog_puts("%02d APP_START: resp.result=%u, ret=%u\n",
                   ctx->base.base.id, resp.result, ctx->base.base.ret);
        return;
    }

    tzlog_puts("%02d successfully started app with app_id=%u\n",
               ctx->base.base.id, resp.data);
}

void tz_logger_dissect2_QSAPP_SEND_DATA(tzlog_ctx_armv8 *ctx) {
    struct qseecom_send_cmd_req req;
    struct qseecom_command_scm_resp resp;
    struct scm_desc *desc = ctx->desc;
    u32 app_id = (u32) desc->args[0];
    struct qseecom_dev_handle *data = get_qseecom_data(app_id);

    if(!data) {
        tzlog_puts("%02d unable to find device file for app_id=%u\n", ctx->base.base.id, app_id);
        return;
    }

    req.cmd_req_buf = (void *) __qseecom_kphys_to_uvirt(data, desc->args[1]);
    req.cmd_req_len = desc->args[2];
    req.resp_buf = (void *) __qseecom_kphys_to_uvirt(data, desc->args[3]);
    req.resp_len = desc->args[4];

    if(ctx->base.base.call_state == TZ_CALL_BEFORE) {
        tzlog_puts("%02d app_id=%u ( %s )\n", ctx->base.base.id, app_id, data->client.app_name);

        if (!strcmp(data->client.app_name, "fpctzappfingerprint")) {
            fingerprint_cmd_dissector(ctx, data);
        }

        tzlog_puts("%02d request:\n", ctx->base.base.id);
        dump_memory_scm(ctx->base.base.id, ">", req.cmd_req_buf, ((req.cmd_req_len-1) / 8)+1);
        tzlog_puts("%02d response:\n", ctx->base.base.id);
        dump_memory_scm(ctx->base.base.id, ">", req.resp_buf, ((req.resp_len-1) / 8)+1);
    } else if(desc->ret[0] == QSEOS_RESULT_INCOMPLETE) {
        enqueue_context(ctx);
    } else {
        tzlog_puts("%02d request:\n", ctx->base.base.id);
        dump_memory_scm(ctx->base.base.id, "<", req.cmd_req_buf, ((req.cmd_req_len-1) / 8)+1);
        tzlog_puts("%02d response:\n", ctx->base.base.id);
        dump_memory_scm(ctx->base.base.id, "<", req.resp_buf, ((req.resp_len-1) / 8)+1);

        FILL_RESP(resp, desc);

        tzlog_puts("%02d result=%#x, type=%#x ( %s ), data=%#x\n", ctx->base.base.id,
               resp.result, resp.resp_type, resp2str(resp.resp_type), resp.data);
    }
}
