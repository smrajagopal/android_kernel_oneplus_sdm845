#ifndef TZ_LOGGER_QCOM_DB_H
#define TZ_LOGGER_QCOM_DB_H

struct tzlog_ctx_any;

/// analyze call data
typedef void (*tz_dissector_t)(struct tzlog_ctx_any *ctx);
/// filter log records, return false to skip logging
typedef bool (*tz_filter_t)(struct tzlog_ctx_any *ctx);

struct tz_command_info {
    u16 id;
    char *name;
    tz_filter_t filter;
    tz_dissector_t dissector1;
    tz_dissector_t dissector2;
};

struct tz_service_info {
    u16 id;
    char *name;
    struct tz_command_info *commands[]; // NULL terminated
};

struct tz_owner_info {
    u8 id;
    char *name;
};

struct tz_match_info {
    struct tz_service_info *svc;
    struct tz_command_info *cmd;
};

/**
 * @brief find_commands find matching commands
 * @param ctx logger context
 */
void tz_find_commands(struct tzlog_ctx_any *ctx);

/**
 * @brief find_owner finds the tz_owner_info associated with the given id
 * @param own_id the owner ID
 * @return the tz_owner_info struct that describe the owner
 */
struct tz_owner_info *tz_find_owner(u32 own_id);

#endif // TZ_LOGGER_QCOM_DB_H
