#ifndef TZ_LOGGER_QCOM_DB_DEFS_H
#define TZ_LOGGER_QCOM_DB_DEFS_H

#ifdef SCM_FUSE_READ
#undef SCM_FUSE_READ
#undef SCM_CMD_HDCP
#endif

#define TZ_OWNER_UNKNOW 5

// by using an enum we avoid defines substitution
enum missing_defs {
    // from include/soc/qcom/scm.h
    SCM_FUSE_READ = 0x7,
    SCM_CMD_HDCP = 0x01,

    // from include/soc/qcom/scm-boot.h
    SCM_BOOT_ADDR = 0x1,
    SCM_BOOT_ADDR_MC = 0x11,

    // from include/soc/qcom/cache_dump.h
    L1C_SERVICE_ID = 3,
    L1C_BUFFER_SET_COMMAND_ID = 4,
    CACHE_BUFFER_DUMP_COMMAND_ID = 5,
    L1C_BUFFER_GET_SIZE_COMMAND_ID = 6,
    L2C_BUFFER_SET_COMMAND_ID = 7,
    L2C_BUFFER_GET_SIZE_COMMAND_ID = 8,

    // from arch/arm/mach-msm/scmmod.c
    SMCMOD_SVC_CRYPTO = (1),
    SMCMOD_CRYPTO_CMD_CIPHER = (1),
    SMCMOD_CRYPTO_CMD_MSG_DIGEST_FIXED = (2),
    SMCMOD_CRYPTO_CMD_MSG_DIGEST = (3),

    // from drivers/devfreq/devfreq_spdm.h
    SPDM_SCM_SVC_ID = 0x9,
    SPDM_SCM_CMD_ID = 0x4,

    // from drivers/soc/qcom/ocmem_priv.h
    OCMEM_SVC_ID = 15,
    OCMEM_LOCK_CMD_ID = 0x1,
    OCMEM_UNLOCK_CMD_ID = 0x2,
    OCMEM_ENABLE_DUMP_CMD_ID = 0x3,
    OCMEM_DISABLE_DUMP_CMD_ID = 0x4,

    // from drivers/soc/qcom/scm.c
    SCM_IO_READ = 0x1,
    SCM_IO_WRITE = 0x2,
    IS_CALL_AVAIL_CMD = 1,
    GET_FEAT_VERSION_CMD = 3,
    RESTORE_SEC_CFG = 2,

    // from drivers/devfreq/governor_msm_adreno_tz.c
    TZ_INIT_ID = 0x6,
    TZ_INIT_ID_64 = 0x9,
    TZ_RESET_ID = 0x3,
    TZ_UPDATE_ID = 0x4,
    TZ_RESET_ID_64 = 0x7,
    TZ_UPDATE_ID_64 = 0x8,
    TZ_V2_UPDATE_ID_64 = 0xA,
    TZ_V2_INIT_ID_64 = 0xB,

    // from drivers/soc/qcom/watchdog_v2.c
    SCM_SET_REGSAVE_CMD = 0x2,
    SCM_SVC_SEC_WDOG_DIS = 0x7,

    // from arch/arm/mach-msm/krait-scm.c
    CPU_CONFIG_CMD = 5,
    CPU_CONFIG_QUERY_CMD = 6,

    // from drivers/power/qcom/msm-pm.c
    SCM_CMD_TERMINATE_PC = (0x2),

    // from drivers/thermal/msm_thermal.c
    THERM_SECURE_BITE_CMD = 8,

    // from drivers/power/reset/msm-poweroff.c
    SCM_WDOG_DEBUG_BOOT_PART = 0x9,
    SCM_DLOAD_CMD = 0x10,
    SCM_IO_DISABLE_PMIC_ARBITER = 1,

    // from drivers/thermal/supply_lm_core.c
    SUPPLY_LM_GET_MIT_CMD = 2,
    SUPPLY_LM_STEP1_REQ_CMD = 3,

    // from drivers/soc/qcom/pil-msa.c
    MSS_RESTART_ID = 0xA,

    // from drivers/soc/qcom/subsys-pil-tz.c
    PAS_INIT_IMAGE_CMD = 1,
    PAS_MEM_SETUP_CMD,
    PAS_AUTH_AND_RESET_CMD = 5,
    PAS_SHUTDOWN_CMD,

    // from drivers/soc/qcom/qdsp6v2/apr.c
    SCM_Q6_NMI_CMD = 0x1,

    // from drivers/soc/qcom/memory_dump_v2.c
    SCM_CMD_DEBUG_LAR_UNLOCK = 0x4,

    // from drivers/crypto/msm/ice.c
    RESTORE_SEC_CFG_CMD = 0x2,

    // from drivers/gpu/msm/kgsl_sharedmem.c
    MEM_PROTECT_LOCK_ID2 = 0x0A,
    MEM_PROTECT_LOCK_ID2_FLAT = 0x11,

    // from drivers/soc/qcom/shared_memory.c
    SHARED_HEAP_SVC_ID = 0x2,
    SHARED_HEAP_CMD_ID = 0xB,

    // from drivers/iommu/msm_iommu_sec.c
    IOMMU_SECURE_MAP2 = 0x0B,
    IOMMU_SECURE_MAP2_FLAT = 0x12,
    IOMMU_SECURE_PTBL_INIT = 4,
    IOMMU_SECURE_PTBL_SIZE = 3,
    IOMMU_SECURE_UNMAP2 = 0x0C,
    IOMMU_SECURE_UNMAP2_FLAT = 0x13,
    IOMMU_SET_CP_POOL_SIZE = 5,
    IOMMU_DUMP_SMMU_FAULT_REGS = 0X0C,

    // from drivers/media/platform/msm/vidc/venus_hfi.c
    TZBSP_MEM_PROTECT_VIDEO_VAR = 0x8,
    TZBSP_VIDEO_SET_STATE = 0xa,

    // from drivers/soc/qcom/scm-xpu.c
    XPU_ERR_FATAL = 0xe,

    // from drivers/usb/dwc3/dwc3-msm.c
    DWC3_MSM_RESTORE_SCM_CFG_CMD = 0x2,

    // from drivers/usb/phy/phy-msm-usb.c
    MSM_OTG_CMD_ID = 0x09,

    // from drivers/video/msm/mdss/mdss_mdp.c
    MEM_PROTECT_SD_CTRL = 0xF,
    MEM_PROTECT_SD_CTRL_FLAT = 0x14,

    //from drivers/misc/qseecom.c
    SCM_SAVE_PARTITION_HASH_ID = 0x01,
    SCM_IS_ACTIVATED_ID = 0x02,

    // from drivers/thermal/lmh_lite.c
    LMH_CHANGE_PROFILE = 0x01,
    LMH_GET_PROFILES = 0x02,
    LMH_CTRL_QPMDA = 0x03,
    LMH_TRIM_ERROR = 0x04,
    LMH_GET_INTENSITY = 0x06,
    LMH_GET_SENSORS = 0x07,

    // from drivers/input/misc/fpc1020_tee.c
    TZ_BLSP_MODIFY_OWNERSHIP_ID = 3,

    // from drivers/soc/qcom/early_random.c
    PRNG_CMD_ID	= 0x01,
    TZ_SVC_CRYPTO = 10,
};

// from ioctl ( qseecom )
enum tz_undefined_cmds {
    // TZ_SVC_APP_MGR
    START = 1,SHUTDOWN,LOOKUP,STATE,REGION,REGISTER,LOAD_SERVICES_IMAGE,UNLOAD_SERVICES_IMAGE,
    // TZ_SVC_LISTENER
    REGISTER2 = 1,DEREGISTER,RESPONSE_HANDLER,
    // TZ_SVC_EXTERNAL
    LOAD_EXTERNAL_IMAGE = 1,UNLOAD_EXTERNAL_IMAGE,
    // TZ_SVC_RPMB
    PROVISION_KEY = 1,ERASE,
    // TZ_SVC_KEYSTORE
    KS_GEN_KEY = 1,DEL_KEY,GET_MAX_KEYS,SET_PIPE_KEY,UPDATE_KEY,
    // TZ_SVC_APP_ID_PLACEHOLDER
    QSAPP_SEND_DATA = 1,GPAPP_OPEN_SESSION,GPAPP_CLOSE_SESSION,GPAPP_INVOKE_COMMAND,GPAPP_REQUEST_CANCELLATION,
};

#endif // TZ_LOGGER_QCOM_DB_DEFS_H
