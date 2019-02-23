#ifndef MISC_TZ_LOGGER_H
#define MISC_TZ_LOGGER_H

#include <linux/list.h>
#include <linux/types.h>

#define MAX_STACK_ENTRIES 24

enum tz_logger_call_state {
    TZ_CALL_BEFORE, ///< you are before the tz call
    TZ_CALL_AFTER, ///< you are after the tz call ( results are ready )
    TZ_CALL_PENDING ///< you are pending, awaiting an async completion
};

/// logging context
typedef struct tzlog_ctx {
    int id; ///< record ID
    enum tz_logger_call_state call_state; ///< call state
    u8 atomic : 1; ///< is this an atomic call?
    u8 skip : 1; ///< shall we skip this record?
    int ret; ///< returned value
} tzlog_ctx;

typedef struct tzlog_probe_t {
    /**
     * @brief attach probe to the running kernel
     */
    void (*attach)(void);
    /**
     * @brief detach probe from running kernel
     */
    void (*detach)(void);
} tzlog_probe_t;

typedef struct tzlog_config {
    /**
     * @brief free the context and all its used resources
     */
    void (*ctx_free)(tzlog_ctx *ctx);

    /**
     * @brief create a new tzlog_ctx
     * @return the new CTX or NULL on error
     */
    tzlog_ctx *(*ctx_alloc)(void);

    /**
     * @brief platofrm-specific probe
     */
    tzlog_probe_t *probe;
} tzlog_config;

// ----------
// kernel API
// ----------

/**
 * @brief free base context data
 * @param ctx to free
 */
extern void tzlog_basectx_free(tzlog_ctx *ctx);

/**
 * @brief get current CTX
 * @return current CTX or a new one if this is your first time
 */
extern tzlog_ctx *tzlog_getctx(void);

/**
 * @brief delete current ctx
 */
extern void tzlog_delctx(void);

/**
 * @brief print your messages into the TZ logger
 * @param fmt format string
 */
extern __printf(1, 2)  void tzlog_puts(char *fmt, ...);

/**
 * @brief tzlog_print_stacktrace print stack trace
 * @param ctx context
 * @param skip number of stack traces to skip ( at least 3 )
 */
void tzlog_print_stacktrace(tzlog_ctx *ctx, int skip);

// ------------------------------------
// to be implemented by platofrm driver
// ------------------------------------

/**
 * @brief initialize tzlogger configuration
 * @param configuration to init
 * @return 0 on success
 */
int tzlog_config_init(tzlog_config *cfg);

/**
 * @brief tzlogger_call call the logger to log a record
 * @param semantic specify call semantic (optional)
 */
extern void tzlogger_call(int semantic, ...);

# endif //MISC_TZ_LOGGER_H
