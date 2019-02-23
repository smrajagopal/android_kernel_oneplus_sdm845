#include <misc/tz_logger.h>
#include <linux/slab.h>
#include <linux/circ_buf.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/hashtable.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/kallsyms.h>
#include <linux/stacktrace.h>
#include <linux/mm.h>

// overall size MUST be a power of 2
// NTOE: maximum allocatable size is (1UL << (PAGE_SHIFT + MAX_ORDER - 1)) = (1UL << 22)
#define TZLOG_BUFSZ (1 << 24) // 16MB
#define DEVICE_NAME "tzlog"
#define CLASS_NAME "logging"
#define CONTEXTS_BITS 5 // 32 buckets

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tux_mind");
MODULE_DESCRIPTION("A simple logger for TEEs");
MODULE_VERSION("0.1");

// debugging
#pragma GCC diagnostic ignored "-Wunused-label"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"

typedef struct ctx_holder {
    struct task_struct *task;
    tzlog_ctx *ctx;
    struct hlist_node hash_list;
} ctx_holder;

static struct circ_buf logbuf;

static tzlog_config config;
static int major_number;
static u32 log_record_id = 0;
static struct class* tzlog_class = NULL;
static struct device* tzlog_device = NULL;
static DECLARE_WAIT_QUEUE_HEAD(queue);

static DEFINE_SPINLOCK(logger_lock);
static DEFINE_SPINLOCK(contexts_lock);
static DEFINE_HASHTABLE(contexts, CONTEXTS_BITS);

// hashtable implementation prefers high bits.
// It assumes that they have higher entropy.
// For addresses this is not true, they ususally are in a large memory region
// that have almost always the same high bits.
// thus we use the lower 4 bytes of the address as a key.
#define lookup_key(ptr) lower_32_bits((long) ptr)

// Kenrel API

tzlog_ctx *tzlog_getctx() {
    ctx_holder *hld;
    struct task_struct *task = current;
    u32 key = lookup_key(task);

    spin_lock(&contexts_lock);
    hash_for_each_possible(contexts, hld, hash_list, key) {
        if(hld->task == task) {
            goto release_and_return;
        }
    }

    hld = kmalloc(sizeof(ctx_holder), GFP_ATOMIC);

    if(!hld) {
        goto release_and_return;
    }

    if(!(hld->ctx = config.ctx_alloc())) {
        goto release_and_return;
    }

    hld->ctx->call_state = TZ_CALL_BEFORE;
    hld->ctx->skip = false;
    hld->ctx->ret = 0;
    hld->ctx->id = log_record_id++ % 100;

    hld->task = task;
    hash_add(contexts, &hld->hash_list, key);

release_and_return:
    spin_unlock(&contexts_lock);

    if(!hld) {
        pr_err("%s: ENOMEM: cannot create a new context holder\n", __func__);
    } else if(!hld->ctx) {
        pr_err("%s: cannot create a new context\n", __func__);
        kfree(hld);
    } else {
        return hld->ctx;
    }

    return NULL;
}

void tzlog_delctx() {
    ctx_holder *hld = NULL;
    struct task_struct *task = current;

    spin_lock(&contexts_lock);
    hash_for_each_possible(contexts, hld, hash_list, lookup_key(task)) {
        if(hld->task == task) {
            hash_del(&hld->hash_list);
            break;
        }
    }

    spin_unlock(&contexts_lock);

    if(hld) {
        config.ctx_free(hld->ctx);
        kfree(hld);
    } else {
        pr_err("TZLOGGER: unable to find a context to delete for pid %d\n", task->pid);
    }
}

void tzlog_basectx_free(tzlog_ctx *ctx) {
    kfree(ctx);
}

void tzlog_puts(char *fmt, ...) {
    //tzlog_ctx *ctx = tzlog_getctx();
    va_list args;
    size_t cnt, required_sz;
    char *tmpbuf;
    int head, tail;
    size_t buf_sz, buf_sz_end;

    va_start(args, fmt);

    spin_lock(&logger_lock);

    head = logbuf.head;
    tail = logbuf.tail;

    // leaky approach, ignore tail
    buf_sz_end = CIRC_SPACE_TO_END(head, 0, TZLOG_BUFSZ);
    buf_sz = CIRC_SPACE(head, tail, TZLOG_BUFSZ);
    // even if we are in an atomic context logbuf.buf belongs to VMALLOC,
    // no page faults will occurs.
    required_sz = vsnprintf(logbuf.buf + head, buf_sz_end, fmt, args);

    va_end(args);

    BUG_ON(required_sz > TZLOG_BUFSZ);

    if(required_sz > buf_sz_end) {
        //TODO: handle this
        pr_err("TZLOGGER: ENOTIMPLEMENTED message out of bounds, last part dropped\n");
    }

    if(required_sz > buf_sz) {
        pr_err_once("TZLOGGER: Buffer full\n");
        logbuf.tail = (tail+(required_sz-buf_sz)) & (TZLOG_BUFSZ-1);
    }

    /*

    head = logbuf.head;
    tail = logbuf.tail;
    buf_sz = CIRC_SPACE(head, tail, TZLOG_BUFSZ);
    buf_sz_end = CIRC_SPACE_TO_END(head, tail, TZLOG_BUFSZ);

    if(!buf_sz) {
        pr_err_once("TZLOGGER: Buffer full\n");
        buf_sz = TZLOG_BUFSZ;
        buf_sz_end = (head - tail)
    }

    required_sz = vsnprintf(logbuf.buf + head, buf_sz_end, fmt, args);

    //NOTE: this will not work if required_size > TZLOG_BUFSZ
    BUG_ON(required_sz > TZLOG_BUFSZ);

    if(required_sz > buf_sz_end && buf_sz_end < buf_sz) {
        tmpbuf = kmalloc(required_sz+1,
                         ctx->atomic ? GFP_ATOMIC : GFP_KERNEL);

        if(!tmpbuf) {
            pr_err("ENOMEM: written only %zd bytes out of %zd requested from ctx#%02d\n",
                   cnt, required_sz, ctx->id);
            vprintk(fmt, args);
            goto exit_commit;
        }

        vsnprintf(tmpbuf, required_sz+1, fmt, args);
        memcpy(logbuf.buf, tmpbuf + buf_sz_end, required_sz - buf_sz_end);
        kfree(tmpbuf);
    }
    */

    logbuf.head = (head+required_sz) & (TZLOG_BUFSZ -1);

    spin_unlock(&logger_lock);

    wake_up(&queue);
}

noinline void tzlog_print_stacktrace(tzlog_ctx *ctx, int skip) {
    // stack walk
    //TODO: save_stack_trace_user(&trace); // actually not implemented in ARM
    int i;
    unsigned long stack_entries[MAX_STACK_ENTRIES];
    struct stack_trace trace = {
        .nr_entries = 0,
        .max_entries = MAX_STACK_ENTRIES,
        .entries = stack_entries,
        .skip = skip
    };

    save_stack_trace(&trace);

    if(!trace.nr_entries) { return; }

    tzlog_puts("%02d Call Stack:\n", ctx->id);

    for(i=0;i<trace.nr_entries;i++) {
        tzlog_puts("%02d [<%08lx>] %ps\n", ctx->id, trace.entries[i], (void *)trace.entries[i]);
    }
}


// device I/O operations

/** @brief This function is called whenever device is being read from user space i.e. data is
 *  being sent from the device to the user. In this case is uses the copy_to_user() function to
 *  send the buffer string to the user and captures any errors.
 *  @param filep A pointer to a file object (defined in linux/fs.h)
 *  @param buffer The pointer to the buffer to which this function writes the data
 *  @param len The length of the b
 *  @param offset The offset if required
 */
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset){
   size_t sent, to_send;
   ssize_t res;
   int cnt;
   size_t pinned = (len/PAGE_SIZE) + ((len%PAGE_SIZE)>0);
   struct page **pages;

   pr_debug("TZLOGGER: reading up to %zu bytes\n", len);

   if(!len) {
       res = 0;
       goto exit_with_res;
   }

   pages = kmalloc(sizeof(struct page) * pinned, GFP_KERNEL);

   if(!pages) {
       pinned = 0;
       pr_err("TZLOGGER: unable to allocate pages\n");
       goto retry;
   }

   // we cannot trigger a page fault while holding a spin_lock ( which is required in atomic contexts )
   // thus we have to pin destination pages
   cnt = get_user_pages_fast((unsigned long) buffer, (int) pinned, 1, pages);

   if(cnt<0) {
       pr_err("TZLOGGER: get_user_pages_fast: %d\n", cnt);
       pinned = 0;
   } else {
       pinned = (size_t)cnt;
   }

retry:

   spin_lock(&logger_lock);

   cnt = CIRC_CNT_TO_END(logbuf.head, logbuf.tail, TZLOG_BUFSZ);
   to_send = min((size_t) (cnt > 0 ? cnt : 0), len); // len < pinned * PAGE_SIZE
   to_send = min(SIZE_MAX/2, to_send); // we return ssize_t, which maximum is SIZE_MAX/2

   if(!to_send) { // blocking I/O
       goto release_sleep_and_retry;
   }

   sent = (size_t) (to_send - copy_to_user(buffer, logbuf.buf+logbuf.tail, to_send));

   pr_debug("TZLOGGER: sent %zu bytes\n", sent);

   if(!sent) {
       goto exit_with_lock;
   }

   // assert(TZLOG_BUFSZ < INT_MAX)
   logbuf.tail = (logbuf.tail + ((int)sent)) & (TZLOG_BUFSZ - 1);

release_sleep_and_retry:
exit_with_lock:

   spin_unlock(&logger_lock);

   if(!to_send) { // blocking I/O
       if(wait_event_interruptible(queue, logbuf.head != logbuf.tail)) {
           return -EINTR;
       }

       goto retry;
   }

   if(pages) {
       for(cnt=0;cnt<pinned;cnt++) {
           if (!PageReserved(pages[cnt]) && sent) {
               SetPageDirty(pages[cnt]);
           }
           put_page(pages[cnt]);
       }

       kfree(pages);
   }

   res = (ssize_t) sent;

exit_with_res:

   return res;
}

/** @brief Devices are represented as file structure in the kernel. The file_operations structure from
 *  /linux/fs.h lists the callback functions that you wish to associated with your file operations
 *  using a C99 syntax structure. char devices usually implement open, read, write and release calls
 */
static struct file_operations fops =
{
   .read = dev_read
};

// driver stuff

static int __init tzlog_init(void) {
    int err = 0;
    config.ctx_free = tzlog_basectx_free;

    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if(major_number<0) {
        pr_err("Failed to register tzlog device\n");
        return major_number;
    }

    tzlog_class = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(tzlog_class)) {
        pr_err("Failed to register logger class\n");
        err = PTR_ERR(tzlog_class);
        goto exit_unreg;
    }

    tzlog_device = device_create(tzlog_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if(IS_ERR(tzlog_device)) {
        pr_err("Failed to create device\n");
        err = PTR_ERR(tzlog_device);
        goto exit_with_class;
    }

    pr_info("Device tzlog created succesfully\n");

    logbuf.buf = vmalloc(TZLOG_BUFSZ);
    if(!logbuf.buf) {
        pr_err("Failed to allocate tzlog buffer ( ENOMEM )\n");
        goto exit_with_device;
    }

    err = tzlog_config_init(&config);
    if(err) {
        pr_err("Failed to initialize tzlog configuration\n");
        goto exit_with_device;
    }

    config.probe->attach();

    hash_init(contexts);

    return 0;

    exit_with_device:
    device_destroy(tzlog_class, MKDEV(major_number, 0));
    class_unregister(tzlog_class);
    exit_with_class: // B)
    class_destroy(tzlog_class);
    exit_unreg:
    unregister_chrdev(major_number, DEVICE_NAME);
    major_number = -1;
    return err;
}

static void __exit tzlog_exit(void) {
    config.probe->detach();
    vfree(logbuf.buf);
    device_destroy(tzlog_class, MKDEV(major_number, 0));
    class_unregister(tzlog_class);
    class_destroy(tzlog_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    pr_info("tzlog removed\n");
}

module_init(tzlog_init)
module_exit(tzlog_exit)
