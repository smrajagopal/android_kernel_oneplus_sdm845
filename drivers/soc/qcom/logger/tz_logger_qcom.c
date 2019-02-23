#include <misc/tz_logger.h>
#include <soc/qcom/scm.h>
#include <linux/slab.h>
#include <linux/kallsyms.h>
#include <linux/stacktrace.h>
#include <linux/sched.h>
#include <macro_foreach.h>

#include "tz_logger_qcom.h"
#include "tz_logger_qcom_db.h"

// debugging
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunreachable-code"

static void attach(void);
static void detach(void);

static tzlog_probe_t qcom_probe = {
    .attach = attach,
    .detach = detach,
};

static bool enabled = false;
static const char *scm2_par_type_str[] = {
    "SCM_VAL",
    "SCM_RO",
    "SCM_RW",
    "SCM_BUFVAL",
};

static tzlog_ctx *new_ctx(void) {
    bool v2 = is_scm_armv8();
    size_t sz = v2 ?
                sizeof(tzlog_ctx_armv8) :
                max(sizeof(tzlog_ctx_armv7_atomic), sizeof(tzlog_ctx_armv7));
    tzlog_ctx_any *ctx = kmalloc(sz, GFP_ATOMIC);

    if(ctx) {
        ctx->v2 = v2;
    }

    return (tzlog_ctx *) ctx;
}

static void ctx_free(tzlog_ctx *ctx) {
    // release custom resources here
    tzlog_basectx_free(ctx);
}

static void attach(void) {
    enabled = true;
    pr_err("TZLOGGER: Qualcom probe attached\n");
}

static void detach(void) {
    enabled = false;
    pr_err("TZLOGGER: Qualcom probe detached\n");
}

int tzlog_config_init(tzlog_config *cfg) {
    cfg->ctx_alloc = new_ctx;
    cfg->ctx_free = ctx_free;
    cfg->probe = &qcom_probe;
    return 0;
}

static noinline void find_caller_names(tzlog_ctx_any *ctx, unsigned long addr, int skip) {
    unsigned long stack_entries[1];
    struct stack_trace trace = {
        .nr_entries = 0,
        .max_entries = 1,
        .entries = stack_entries,
        .skip = skip + 1
    };

    sprint_symbol_no_offset(ctx->scm_fname, addr);

    save_stack_trace(&trace);
    if(trace.nr_entries) {
        sprint_symbol_no_offset(ctx->fname, stack_entries[0]);
    } else {
        memcpy(ctx->fname, "NOT_FOUND", strlen("NOT_FOUND")+1);
    }
}

#define FLAGS_CHARS(ctx) ctx->base.atomic ? 'A' : '-', ctx->is64 ? '6' : '-'
#define FMT_START "%02d [%5lu.%06lu] %c%c %s:"

#define FMT_NOTFOUND0 " NOT_FOUND"
#define FMT_NOTFOUND1 ""
#define FMT_NOTFOUND2 ""
#define FMT_NOTFOUND3 ""
#define FMT_NOTFOUND4 ""
#define FMT_NOTFOUND5 ""
#define FMT_NOTFOUND6 ""

#define FMT_TRACE " from %s:%d called %s with"

#define FMT_OWNER1 ""
#define FMT_OWNER2 " OWNER='%s'"

#define FMT_OPEN_MATCHES " ["
#define FMT_HEX_MATCH "{ \"SVC\": %#0X, \"CMD\": %#0X }"
#define FMT_STRING_MATCH "{\"SVC\":\"%s\", \"CMD\":\"%s\"}"
#define FMT_STRING_MATCH_CONT ", " FMT_STRING_MATCH

#define FMT_MATCH_START0 FMT_HEX_MATCH
#define FMT_MATCH_START1 FMT_STRING_MATCH
#define FMT_MATCH_START2 FMT_STRING_MATCH
#define FMT_MATCH_START3 FMT_STRING_MATCH
#define FMT_MATCH_START4 FMT_STRING_MATCH
#define FMT_MATCH_START5 FMT_STRING_MATCH
#define FMT_MATCH_START6 FMT_STRING_MATCH

#define FMT_CLOSE_MATCHES "]"
#define LOG_ARGS1(ctx) , ctx->base.id, (unsigned long)ts, rem_nsec, FLAGS_CHARS(ctx), ctx->fname, current->comm, current->pid, ctx->scm_fname
#define LOG_ARGS2(ctx) LOG_ARGS1(ctx), ((tzlog_ctx_armv8 *) ctx)->oinfo ? ((tzlog_ctx_armv8 *) ctx)->oinfo->name : "NOT_FOUND"

#define MATCH_NOTFOUND_ARGS(ctx) ctx->svc_id, ctx->cmd_id
#define MATCH_ARGS0(ctx) , MATCH_NOTFOUND_ARGS(ctx)
#define MATCH_ARGS1(ctx) , ctx->matches[0].svc->name, ctx->matches[0].cmd->name
#define MATCH_ARGS2(ctx) , ctx->matches[1].svc->name, ctx->matches[1].cmd->name MATCH_ARGS1(ctx)
#define MATCH_ARGS3(ctx) , ctx->matches[2].svc->name, ctx->matches[2].cmd->name MATCH_ARGS2(ctx)
#define MATCH_ARGS4(ctx) , ctx->matches[3].svc->name, ctx->matches[3].cmd->name MATCH_ARGS3(ctx)
#define MATCH_ARGS5(ctx) , ctx->matches[4].svc->name, ctx->matches[4].cmd->name MATCH_ARGS4(ctx)
#define MATCH_ARGS6(ctx) , ctx->matches[5].svc->name, ctx->matches[5].cmd->name MATCH_ARGS5(ctx)

#define PUTS(ctx, version, n, n1) \
    tzlog_puts( FMT_START FMT_NOTFOUND##n \
                FMT_TRACE FMT_OWNER##version FMT_OPEN_MATCHES \
                FMT_MATCH_START##n REP##n1(FMT_STRING_MATCH_CONT) \
                FMT_CLOSE_MATCHES "\n" \
                \
                LOG_ARGS##version(ctx) MATCH_ARGS##n(ctx)\
    )

#define NAMED_LOG_CASE(ctx, version, name, n, n1) name: PUTS(ctx, version, n, n1); break;
#define LOG_CASE(ctx, version, n, n1) NAMED_LOG_CASE(ctx, version, case n, n, n1)

/**
 * @brief print scm call
 */
static void _log_scm_call(tzlog_ctx_any *ctx) {
    u64 ts = local_clock();
    unsigned long rem_nsec = do_div(ts, 1000000000);

    // I know that this seems stupid but we avoid kmalloc in this way

    if(ctx->v2) {
        switch (ctx->nmatches) {
        LOG_CASE(ctx, 2, 0, 0)
        LOG_CASE(ctx, 2, 1, 0)
        LOG_CASE(ctx, 2, 2, 1)
        NAMED_LOG_CASE(ctx, 2, default, 3, 2) // MAX_MATCHES
        }
    } else {
        switch (ctx->nmatches) {
        LOG_CASE(ctx, 1, 0, 0)
        LOG_CASE(ctx, 1, 1, 0)
        LOG_CASE(ctx, 1, 2, 1)
        NAMED_LOG_CASE(ctx, 1, default, 3, 2) // MAX_MATCHES
        }
    }
}

#define FMT_TYPE ", %s"
#define FMT_VALUE ", %#llx"
#define TYPE_PARAM(i) , scm2_par_type_str[types[i]]
#define VALUE_PARAM(i) , desc->args[i]

#define NAMED_CASE(name, n, n1) name:\
    tzlog_puts("%02d %d parameters (%s" REP##n1(FMT_TYPE) "): %#llx" REP##n1(FMT_VALUE) "\n", \
    id, num CALL_MACRO_X_FOR_EACH2(TYPE_PARAM, ARGS##n) CALL_MACRO_X_FOR_EACH2(VALUE_PARAM, ARGS##n));\
    break;

#define A_CASE(n, n1) NAMED_CASE(case n, n, n1)

static void log_scm2_parameters(int id, struct scm_desc *desc) {
    u8 i,num;
    enum scm_arg_types types[MAX_SCM_ARGS] = {0};

    num = desc->arginfo & 0xf;

    if(!num) {
        tzlog_puts("%02d 0 parameters\n", id);
        return;
    } else if (num > MAX_SCM_ARGS) {
        tzlog_puts("%02d EINVAL num=%d exceed MAX_SCM_ARGS=%d\n",
               id, num, MAX_SCM_ARGS);
        num = MAX_SCM_ARGS;
    }

    for(i=0;i<num;i++) {
        types[i] = (enum scm_arg_types) ((desc->arginfo >> (4 + (2*i))) & 0x3);
    }

    switch (num) {
    A_CASE(1, 0)
    A_CASE(2, 1)
    A_CASE(3, 2)
    A_CASE(4, 3)
    A_CASE(5, 4)
    A_CASE(6, 5)
    A_CASE(7, 6)
    A_CASE(8, 7)
    A_CASE(9, 8)
    NAMED_CASE(default, 10, 9) // MAX_SCM_ARGS
    }
}

#define SMC64_MASK 0x40000000

static void rebuild_args2(tzlog_ctx_armv8 *ctx, va_list args) {
    u64 x0 = va_arg(args, u64);

    ctx->desc = va_arg(args, struct scm_desc *);

    ctx->own_id = FNID_OWNER(x0);
    ctx->base.svc_id = FNID_SERVICE(x0);
    ctx->base.cmd_id = FNID_CMD(x0);

    ctx->base.base.atomic = (x0 & BIT(31)) != 0;
    ctx->base.is64 = (x0 & SMC64_MASK) != 0;

    ctx->oinfo = tz_find_owner(ctx->own_id);
}

static void rebuild_args1_noatom(tzlog_ctx_armv7 *ctx, va_list args) {
    ctx->base.svc_id = va_arg(args, u32);
    ctx->base.cmd_id = va_arg(args, u32);
    ctx->cmd_buf = va_arg(args, void *);
    ctx->cmd_len = va_arg(args, size_t);
    ctx->resp_buf = va_arg(args, void *);
    ctx->resp_len = va_arg(args, size_t);
}

static void rebuild_args1_atom(tzlog_ctx_armv7_atomic *ctx, va_list args) {
    int i;

    ctx->base.svc_id = va_arg(args, u32);
    ctx->base.cmd_id = va_arg(args, u32);

    ctx->nargs = va_arg(args, u32);
    ctx->nrets = va_arg(args, u32);

    for(i=0;i<ctx->nargs;i++) {
        ctx->args[i] = va_arg(args, u32);
    }

    for(i=0;i<ctx->nrets;i++) {
        ctx->rets[i] = va_arg(args, u32 *);
    }
}

static void rebuild_args(int semantic, tzlog_ctx_any *ctx, va_list args) {
    if(ctx->v2) {
        rebuild_args2((tzlog_ctx_armv8 *)ctx, args);
    } else if (semantic == 1) {
        rebuild_args1_atom((tzlog_ctx_armv7_atomic *)ctx, args);
    } else {
        rebuild_args1_noatom((tzlog_ctx_armv7 *)ctx, args);
    }
}

static void dissect(tzlog_ctx_any *ctx) {
    tz_dissector_t dissector;
    int i;

    if(!ctx->v2) { tzlog_puts("%02d EOPNOTSUPP V1 dissectors\n", ctx->base.id); return; }

    for(i=0; i < ctx->nmatches; i++) {
        dissector = ctx->v2 ? ctx->matches[i].cmd->dissector2:
                              ctx->matches[i].cmd->dissector1;
        if(dissector) {
            dissector(ctx);
        }
    }
}

void tzlogger_call(int semantic, ...) {
    va_list args;
    tzlog_ctx_any *ctx;

    if(!enabled) {
        pr_err_once("TZLOGGER: probe disabled, discarding call\n");
        return;
    }

    if(semantic != SEMANTIC_HAVE_CTX) {
        ctx = (tzlog_ctx_any *) tzlog_getctx();
        if(!ctx) {
            pr_err("TZLOGGER: failed to get context, discarding call\n");
            return;
        } else if(ctx->base.skip) {
            return;
        }
    }

    va_start(args, semantic);

    if(semantic == SEMANTIC_HAVE_CTX) {
        ctx = va_arg(args, tzlog_ctx_any *);
    } else if(ctx->base.call_state == TZ_CALL_BEFORE) {

        if(!ctx->v2 && semantic == 2) {
            tzlog_puts("%02d GET_VERSION\n", ctx->base.id);
            goto stack_and_end;
        }

        find_caller_names(ctx, _RET_IP_, 4);
        rebuild_args(semantic, ctx, args);
        tz_find_commands(ctx);

        if(ctx->base.skip) { goto out; }

        _log_scm_call(ctx);

        if(ctx->v2) {
            log_scm2_parameters(ctx->base.id, ((tzlog_ctx_armv8 *) ctx)->desc);
        }
    } else if(ctx->base.call_state == TZ_CALL_AFTER) {
        ctx->base.ret = va_arg(args, int);
    }

    dissect(ctx);

    stack_and_end:

    // how many things can we put into the common code? stacktrace?

    switch(ctx->base.call_state) {
    case TZ_CALL_BEFORE:
        ctx->base.call_state = TZ_CALL_AFTER;
        break;
    case TZ_CALL_AFTER:
        tzlog_print_stacktrace(&ctx->base, 4);
        tzlog_puts("%02d END with ret=%d\n", ctx->base.id, ctx->base.ret);

        if(semantic != SEMANTIC_HAVE_CTX) {
            tzlog_delctx();
        }
        break;
    case TZ_CALL_PENDING:
        pr_err("TZLOGGER: %02d now pending...\n", ctx->base.id);

        tzlog_delctx();
        break;
    }

    out:

    va_end(args);
}
