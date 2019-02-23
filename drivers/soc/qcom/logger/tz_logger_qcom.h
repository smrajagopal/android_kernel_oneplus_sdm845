#ifndef TZ_LOGGER_QCOM_H
#define TZ_LOGGER_QCOM_H

#include <linux/kallsyms.h>
#include <misc/tz_logger.h>
#include "tz_logger_qcom_db.h"

#define TZ_MAX_MATCH 3 /// max SVC/SMD pair matches

typedef struct tzlog_ctx_any {
    tzlog_ctx base;
    bool v2;
    // flags
    u32 is64 : 1;
    char fname[KSYM_NAME_LEN];
    char scm_fname[KSYM_NAME_LEN];
    u32 svc_id;
    u32 cmd_id;
    u8 nargs;
    struct tz_match_info matches[TZ_MAX_MATCH];
    u8 nmatches;
} tzlog_ctx_any;

typedef struct tzlog_ctx_armv8 {
    tzlog_ctx_any base;
    struct tz_owner_info *oinfo;
    u32 own_id;
    struct scm_desc *desc;
} tzlog_ctx_armv8;

typedef struct tzlog_ctx_armv7 {
    tzlog_ctx_any base;
    void *cmd_buf;
    size_t cmd_len;
    void *resp_buf;
    size_t resp_len;
} tzlog_ctx_armv7;

typedef struct tzlog_ctx_armv7_atomic {
    tzlog_ctx_any base;
    u8 nargs;
    u8 nrets;
    u32 args[5];
    u32 *rets[4];
} tzlog_ctx_armv7_atomic;

#define FNID_OWNER(id) ((id >> 24) & 0x3F)
#define FNID_SERVICE(id) ((id >> 8) & 0xFF)
#define FNID_CMD(id) (id & 0xFF)

#define FLAGS_ATOMIC 0x1
#define FLAGS_REGISTERS 0x2
#define FLAGS_SCM64 0x4

#define SEMANTIC_HAVE_CTX -1

#endif // TZ_LOGGER_QCOM_H
