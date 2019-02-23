#include <linux/types.h> // scm.h broken without this include
#include <soc/qcom/scm.h>

#include "tz_logger_qcom.h"

bool tz_filter__is_not_scm_armv8(tzlog_ctx_any *ctx __always_unused) {
    return !is_scm_armv8();
}

bool tz_filter_skip(tzlog_ctx_any *ctx __always_unused) {
    return false;
}
