#include <macro_foreach.h>
#include <soc/qcom/qseecomi.h>
#include <soc/qcom/scm.h>

#include "tz_logger_qcom_db.h"
#include "tz_logger_qcom_db_defs.h"
#include "tz_logger_qcom_db_dissectors.h"
#include "tz_logger_qcom_db_filters.h"

#define CMD_DEF(cmd) &(struct tz_command_info) {cmd, #cmd, tz_filter$ ## cmd, (tz_dissector_t) tz_logger_dissect1_ ## cmd, (tz_dissector_t)tz_logger_dissect2_ ## cmd}
#define CMD_DEF_COMMA(cmd) CMD_DEF(cmd),
#define CMD_LST(...) {CALL_MACRO_X_FOR_EACH(CMD_DEF_COMMA, ##__VA_ARGS__) NULL}
#define SVC_DEF(name, ...) {name, #name, CMD_LST(__VA_ARGS__)}
#define SVC_VAR_NAME(name) svc_ ## name
#define SVC_VAR_NAME_PTR_COMMA(name) &SVC_VAR_NAME(name),
#define SVC_VAR(var_name, svc_id, ...) static struct tz_service_info SVC_VAR_NAME(var_name) = \
                        {svc_id, #svc_id, CMD_LST(__VA_ARGS__)} // avoid expansion by using '#'
#define SVC_LST(...) {CALL_MACRO_X_FOR_EACH(SVC_VAR_NAME_PTR_COMMA, ##__VA_ARGS__) NULL}
#define OWN_VAR_NAME(name) own_ ## name
#define OWN_VAR_NAME_PTR_COMMA(name) &OWN_VAR_NAME(name),
#define OWN_VAR(id) static struct tz_owner_info OWN_VAR_NAME(id) = {TZ_OWNER_ ## id, #id}
#define OWN_LST(...) {CALL_MACRO_X_FOR_EACH(OWN_VAR_NAME_PTR_COMMA, ##__VA_ARGS__) NULL}


// services with commands
SVC_VAR(l1c, L1C_SERVICE_ID, L1C_BUFFER_SET_COMMAND_ID, CACHE_BUFFER_DUMP_COMMAND_ID,
        L1C_BUFFER_GET_SIZE_COMMAND_ID, L2C_BUFFER_SET_COMMAND_ID, L2C_BUFFER_GET_SIZE_COMMAND_ID);
SVC_VAR(smcmod, SMCMOD_SVC_CRYPTO, SMCMOD_CRYPTO_CMD_CIPHER, SMCMOD_CRYPTO_CMD_MSG_DIGEST_FIXED, SMCMOD_CRYPTO_CMD_MSG_DIGEST);
SVC_VAR(spdm, SPDM_SCM_SVC_ID, SPDM_SCM_CMD_ID);
SVC_VAR(ocmem, OCMEM_SVC_ID, OCMEM_LOCK_CMD_ID, OCMEM_UNLOCK_CMD_ID, OCMEM_ENABLE_DUMP_CMD_ID, OCMEM_DISABLE_DUMP_CMD_ID);
SVC_VAR(boot, SCM_SVC_BOOT, CPU_CONFIG_CMD, CPU_CONFIG_QUERY_CMD, SCM_BOOT_ADDR, SCM_BOOT_ADDR_MC, SCM_CMD_TERMINATE_PC,
        SCM_DLOAD_CMD, SCM_SVC_SEC_WDOG_DIS, SCM_WDOG_DEBUG_BOOT_PART, THERM_SECURE_BITE_CMD, TZBSP_VIDEO_SET_STATE);
SVC_VAR(pil, SCM_SVC_PIL, MSS_RESTART_ID, PAS_AUTH_AND_RESET_CMD, PAS_INIT_IMAGE_CMD, PAS_MEM_SETUP_CMD, PAS_SHUTDOWN_CMD);
SVC_VAR(util, SCM_SVC_UTIL, IOMMU_DUMP_SMMU_FAULT_REGS, SCM_Q6_NMI_CMD, SCM_SET_REGSAVE_CMD);
SVC_VAR(tz, SCM_SVC_TZ, SCM_CMD_DEBUG_LAR_UNLOCK, SCM_IO_READ, SCM_IO_WRITE, TZ_BLSP_MODIFY_OWNERSHIP_ID);
SVC_VAR(io, SCM_SVC_IO, SCM_IO_READ, SCM_IO_WRITE, TZ_UPDATE_ID, TZ_RESET_ID);
SVC_VAR(info, SCM_SVC_INFO, GET_FEAT_VERSION_CMD, IS_CALL_AVAIL_CMD);
SVC_VAR(ssd, SCM_SVC_SSD);
SVC_VAR(fuse, SCM_SVC_FUSE, SCM_FUSE_READ);
SVC_VAR(pwr, SCM_SVC_PWR, SCM_IO_DISABLE_PMIC_ARBITER, SUPPLY_LM_GET_MIT_CMD, SUPPLY_LM_STEP1_REQ_CMD);
SVC_VAR(mp, SCM_SVC_MP, DWC3_MSM_RESTORE_SCM_CFG_CMD, IOMMU_SECURE_MAP2, IOMMU_SECURE_PTBL_INIT, IOMMU_SECURE_MAP2_FLAT,
        IOMMU_SECURE_UNMAP2_FLAT, IOMMU_SECURE_PTBL_SIZE, IOMMU_SECURE_UNMAP2, IOMMU_SET_CP_POOL_SIZE, MEM_PROTECT_LOCK_ID2,
        MEM_PROTECT_LOCK_ID2_FLAT, MEM_PROTECT_SD_CTRL, MSM_OTG_CMD_ID, RESTORE_SEC_CFG, TZBSP_MEM_PROTECT_VIDEO_VAR, XPU_ERR_FATAL);
SVC_VAR(dcvs, SCM_SVC_DCVS, TZ_INIT_ID, TZ_INIT_ID_64, TZ_RESET_ID_64, TZ_UPDATE_ID_64, TZ_V2_UPDATE_ID_64, TZ_V2_INIT_ID_64);
SVC_VAR(es, SCM_SVC_ES, SCM_IS_ACTIVATED_ID, SCM_SAVE_PARTITION_HASH_ID);
SVC_VAR(hdcp, SCM_SVC_HDCP, SCM_CMD_HDCP);
SVC_VAR(lmh, SCM_SVC_LMH, LMH_CHANGE_PROFILE, LMH_CTRL_QPMDA, LMH_GET_INTENSITY, LMH_GET_PROFILES, LMH_GET_SENSORS, LMH_TRIM_ERROR);
SVC_VAR(tzscheduler, SCM_SVC_TZSCHEDULER, QSEOS_APP_LOOKUP_COMMAND, QSEOS_APP_REGION_NOTIFICATION, QSEOS_APP_SHUTDOWN_COMMAND,
        QSEOS_APP_START_COMMAND, QSEOS_CLIENT_SEND_DATA_COMMAND, QSEOS_DELETE_KEY, QSEOS_DEREGISTER_LISTENER, QSEOS_GENERATE_KEY,
        QSEOS_LISTENER_DATA_RSP_COMMAND, QSEOS_LOAD_EXTERNAL_ELF_COMMAND, QSEOS_LOAD_SERV_IMAGE_COMMAND, QSEOS_REGISTER_LISTENER,
        QSEOS_RPMB_ERASE_COMMAND, QSEOS_RPMB_PROVISION_KEY_COMMAND, QSEOS_SET_KEY, QSEOS_TEE_CLOSE_SESSION, QSEOS_TEE_INVOKE_COMMAND,
        QSEOS_TEE_OPEN_SESSION, QSEOS_TEE_REQUEST_CANCELLATION, QSEOS_UNLOAD_EXTERNAL_ELF_COMMAND, QSEOS_UNLOAD_SERV_IMAGE_COMMAND,
        QSEOS_UPDATE_KEY_USERINFO);
SVC_VAR(sec_camera, SCM_SVC_SEC_CAMERA);
SVC_VAR(sh_heap, SHARED_HEAP_SVC_ID, SHARED_HEAP_CMD_ID);
SVC_VAR(crypto, TZ_SVC_CRYPTO, PRNG_CMD_ID);

SVC_VAR(app_mgr, TZ_SVC_APP_MGR, START,SHUTDOWN,LOOKUP,STATE,REGION,REGISTER,LOAD_SERVICES_IMAGE,UNLOAD_SERVICES_IMAGE);
SVC_VAR(listener, TZ_SVC_LISTENER, REGISTER2,DEREGISTER,RESPONSE_HANDLER);
SVC_VAR(external, TZ_SVC_EXTERNAL, LOAD_EXTERNAL_IMAGE, UNLOAD_EXTERNAL_IMAGE);
SVC_VAR(rpmb, TZ_SVC_RPMB, PROVISION_KEY,ERASE);
SVC_VAR(ks, TZ_SVC_KEYSTORE, KS_GEN_KEY,DEL_KEY,GET_MAX_KEYS,SET_PIPE_KEY,UPDATE_KEY);
SVC_VAR(apps, TZ_SVC_APP_ID_PLACEHOLDER, QSAPP_SEND_DATA,GPAPP_OPEN_SESSION,GPAPP_CLOSE_SESSION,GPAPP_INVOKE_COMMAND,GPAPP_REQUEST_CANCELLATION);

// direct assignment is impossible due to flexible array members
// you shall add any new service to this list.
static struct tz_service_info *SIP_services[] = SVC_LST(io, l1c, smcmod, spdm, ocmem, boot,
                                                   pil, util, tz, info, ssd, fuse, pwr,
                                                   mp, dcvs, es, hdcp, lmh, tzscheduler,
                                                   sec_camera, sh_heap, crypto);
static struct tz_service_info *QSEE_OS_services[] = SVC_LST(app_mgr, listener, external, rpmb, ks);
static struct tz_service_info *TZ_APPS_services[] = SVC_LST(apps);
#define v1_services SIP_services

OWN_VAR(MOBI_OS);
OWN_VAR(QSEE_OS);
OWN_VAR(TZ_APPS);
OWN_VAR(SIP);
// apparently not used
OWN_VAR(TZ_APPS_RESERVED);
OWN_VAR(ARM);
OWN_VAR(CPU);
OWN_VAR(OEM);
OWN_VAR(STD);

static struct tz_owner_info *owners[] = OWN_LST(
        MOBI_OS,QSEE_OS,TZ_APPS,SIP,
        TZ_APPS_RESERVED,ARM,CPU,OEM,STD
);

static struct tz_service_info **_services_by_owner(tzlog_ctx_armv8 *ctx) {
    switch(ctx->own_id){
    case TZ_OWNER_TZ_APPS:
        return TZ_APPS_services;
    case TZ_OWNER_QSEE_OS:
        return QSEE_OS_services;
    default:
        tzlog_puts("%02d services for owner %d NOT_FOUND, fall back to SIP\n", ctx->base.base.id, ctx->own_id);
    case TZ_OWNER_SIP:
        return SIP_services;
    }
}

static __always_inline struct tz_service_info **services_by_owner(tzlog_ctx_any *ctx) {
    return (ctx->v2) ? _services_by_owner((tzlog_ctx_armv8 *) ctx) : v1_services;
}

struct tz_owner_info *tz_find_owner(u32 own_id) {
    struct tz_owner_info **oinfo;
    for(oinfo=owners;*oinfo && (*oinfo)->id != own_id;oinfo++);
    return *oinfo;
}

void tz_find_commands(tzlog_ctx_any *ctx) {
    struct tz_service_info **sit;
    struct tz_command_info **cit;
    bool filtered=false;

    sit = services_by_owner(ctx);
    ctx->nmatches = 0;

    for(;*sit && ctx->nmatches < TZ_MAX_MATCH; sit++) {
        if((*sit)->id != ctx->svc_id) continue;

        for(cit=(*sit)->commands;*cit && ctx->nmatches < TZ_MAX_MATCH; cit++) {
            if((*cit)->id != ctx->cmd_id) continue;

            if((*cit)->filter && !(*cit)->filter(ctx)) {
                filtered = true;
            } else {
                ctx->matches[ctx->nmatches].svc = *sit;
                ctx->matches[ctx->nmatches++].cmd = *cit;
            }

        }
    }

    ctx->base.skip = !ctx->nmatches && filtered;
}
