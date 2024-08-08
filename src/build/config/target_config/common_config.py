#!/usr/bin/env python3
# encoding=utf-8
# ============================================================================
# @brief    common config of build system
# Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
# ============================================================================


component_set = {
    'boot_rom':  ['hw_sec', 'non_os_libc', 'non_os_isr', 'non_os_osal'],
    'libra_public': ['libra_device'],
    'std_common_lib': [
        "app_version", "build_version", "testsuite", "common_headers", "connectivity",
        "cpu_load", "cpu_utils", "irmalloc", "lib_utils", "dfx_log", "dfx_log_reader",
        "dfx_panic", "dfx_preserve",
    ],
    "ssb_libs": ["codeloader_ssb", "lib_utils", "jlink_load", "algorithm"],
    'boot_common_lib': [
        "bootloader", "codeloader", "build_version", "common_headers", 'app_version',
        "cpu_utils", "irmalloc", "dfx_panic", "dfx_preserve", 'error_code',
    ],
    'sec': ['hal_sec_common', 'hal_sec_sha', 'hal_sec_trng2', 'hal_sectors',
               'sec_common', 'sec_rsa', 'sec_sha', 'sec_trng', "sec_port", "sec_rsa_port",],
    'security': ['hal_hash', 'hash', 'hash_port', 'hal_trng', 'trng', 'trng_port', 'hal_pke',
                    'pke', 'pke_port', 'hal_cipher', 'cipher', 'cipher_port', 'hal_km', 'km', 'km_port'],
    'mem': ['memory_core'],
    'mem_monitor': ['mem_monitor', 'hal_mem_monitor', 'mem_monitor_port'],
    'cpu': ['hal_cpu_core', 'reboot', 'hal_reg_config', 'hal_reboot', 'reboot_port'],
    'can': ['can', 'hal_can', 'can_port'],
    'watchdog': ['watchdog', 'hal_watchdog', 'watchdog_port'],
    'reboot_set':['reboot', 'hal_reboot', 'reboot_port'],
    'uart': ['hal_uart', 'uart', 'uart_port'],
    'xip_os': ['hal_xip', 'xip_port'],
    'xip_nonos': ['hal_xip', 'xip_port'],
    'adc': ['hal_adc', 'adc', 'adc_port'],
    'ipc': ['hal_ipc_old', 'ipc_old'],
    'spi_os': ['hal_spi', 'spi_cmsis'],
    'spi_nonos': ['hal_spi', 'spi_nonos'],
    'sfc_flash_old': ['sfc_old', 'hal_sfc_old'],
    'sfc_flash': ['sfc', 'hal_sfc'],
    'dma': ['hal_dma_v100', 'dma', 'dma_port'],
    'dmav151': ['hal_dma_v151', 'dma', 'dma_port'],
    'dmav120': ['hal_dma_v120', 'dma', 'dma_port'],
    'button': ['button', 'button_port'],
    'epmu': ['epmu', 'epmu_port'],
    'spi': ['spi', 'hal_spi', 'spi_porting'],
    'lpc': ['hal_lpc', 'lpc'],
    'otp': ['hal_otp', 'otp'],
    'i2c': ['hal_i2c', 'i2c', 'i2c_porting'],
    'pwm': ['hal_pwm', 'pwm', "pwm_port",],
    'mmc': ['hal_mmc', 'mmc'],
    'gpio_v150': ['hal_gpio_v150', 'gpio', "gpio_port"],
    'gpio_v100': ['hal_gpio_v100', 'gpio', "gpio_port"],
    'qspi': ['hal_qspi', 'qspi'],
    'hso_log': ["dfx_log", "dfx_log_reader"],
    'qdec' : ['hal_qdec', 'qdec', "qdec_port"],
    'keyscan' : ['hal_keyscan', 'keyscan', "keyscan_port"],
    'usb' : ["usb", "hal_usb", "usb_port"],
    'usb_unified' : ['usb_unified', 'usb_unified_open', 'usb_unified_port'],
    'pdm' : ['pdm', 'hal_pdm', "pdm_porting", "test_pdm"],
    'embed_flash' : ['eflash', 'hal_eflash', 'eflash_port'],
    'i2s' : ["i2s"],
    'flash' : ["flash", "flash_porting"],
    'sio_v150' : ['hal_sio', 'sio_port'],
    'sio_v151' : ['hal_sio', 'sio_port'],
    'cpu_trace': ['hal_cpu_trace', 'cpu_trace'],
    'sdio_slave': ['sdio', 'hal_sdio', 'sdio_port'],
    'time_set': ['systick', 'tcxo', 'drv_timer', 'hal_systick', 'hal_tcxo', 'hal_timer', 'rtc_unified', 'hal_rtc_unified', 'rtc_unified_port', 'timer_port', 'systick_port', 'tcxo_port'],
    'systick_set':['systick', 'hal_systick', 'systick_port'],
    'tcxo_set':['tcxo', 'hal_tcxo', 'tcxo_port'],
    'rtc_unified': ['rtc_unified', 'hal_rtc_unified', 'rtc_unified_port'],
    'local_stdlib': ['c', 'm', 'posix'],
    'audio': ['audio_api', 'audio_core', 'audio_drv', 'audio_sample', 'audio_vendor'],
    'bgh': ['bt_host_brandy_service', 'bt_host_brandy_sample', 'bt_host_sdk'],
    'bgh-master': ['bt_app', 'bth_sample', 'bth_sdk', 'bg_common', 'bth_audio_manager_wrapper'],
    'bgh_audio': ['bt_host_audio_manager_wrapper'],
    'media': ['audio_capture', 'audio_manager', 'audio_service', 'audio_stream', 'audio_utils',
              'media_sample_wrapper', 'player', 'media_common', 'audio_bluetooth_port', 'audio_hw',
              'audio_primary_port','format_hw', 'hmf_demuxer', 'media_hal_common', 'plugin_demuxer_hmf',
              'plugin_demuxer_mjpeg', 'plugin_protocol_https', 'audio_port_common', 'codec_hw', 'codec_plugin_vdec',
              'camera_codec_hw', 'hdi_camera_intf', 'plugin_demuxer_raw', 'audio_modem_port'],
    'media_melody': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils',
              'media_common', 'audio_bluetooth_port', 'audio_hw',
              'audio_primary_port', 'media_hal_common', 'audio_port_common', 'haid_manager', 'tone_player', 'tws_manager'],
    'media_mini': ['audio_capture', 'audio_manager', 'audio_service', 'audio_stream', 'audio_utils',
              'media_sample_wrapper', 'player', 'media_common', 'audio_bluetooth_port', 'audio_hw',
              'audio_primary_port','format_hw', 'hmf_demuxer', 'media_hal_common', 'plugin_demuxer_hmf',
              'plugin_demuxer_mjpeg', 'plugin_protocol_https', 'audio_port_common', 'plugin_demuxer_raw'],
    'media_target4': ['audio_manager', 'audio_service', 'audio_stream', 'audio_utils', 'media_common', 'audio_hw',
              'audio_primary_port', 'audio_bluetooth_port', 'audio_modem_port', 'media_hal_common',
              'audio_port_common', 'media_sample_wrapper'],
    'gpu': ['gfx_rm', 'gpu'],
    'gpu_test': ['test_gpu'],
    'graphic_uikit': ['cjson_static', 'display_gfx', 'display_gralloc', 'display_layer', 'graphic_input', 'graphic_service',
                'freetype_static', 'graphic_ui', 'graphic_utils', 'qrcodegen', 'icu4c', 'harfbuzz', 'uikit_ext'],
    'graphic_lvgl': ['display_gfx', 'display_gralloc', 'display_layer', 'graphic_input', 'freetype_static',
                'lvgl', 'lvgl_sample', 'lvgl_demos', 'lvgl_proprietary', 'lvgl_service'],
    'graphic_lvgl_mini': ['display_gfx', 'display_gralloc', 'display_layer', 'graphic_input', 'freetype_static',
                'lvgl', 'lvgl_sample', 'lvgl_demos', 'lvgl_proprietary_mini', 'lvgl_service'],
    'graphic_test': ['uikit_test'],
    'calendar': ['calendar', 'hal_calendar', 'calendar_port'],
    'update_app': ['update_common', 'update_storage', 'update_local', 'update_storage_brandy', 'update_common_brandy', 'lzma_21.07'],
    'update_recovery': ['update_common', 'update_local', 'update_common_brandy', 'lzma_21.07'],
    'pinctrl': ['pinctrl', 'hal_pinctrl', 'port_pinctrl'],
    'pmp_set': ['pmp', 'hal_pmp', 'pmp_port'],
    'mips': ['mips', 'hal_mips'],
    'dfx_set': ['dfx_diag', "diag_dfx_cmd", 'dfx_panic', 'dfx_preserve', 'dfx_log', 'dfx_log_reader', 'dfx_exception', 'log_file', 'soc_log'],
    'app_ux': ['ux_audio', 'ux_bt', 'ux_commu', 'ux_sensor', 'ux_spp', 'ux_cmn_header', 'ux_common'],
    'pm_set': ['pm', 'pm_porting', 'pm_sleep', 'pm_sleep_porting', 'pm_veto', 'pm_veto_porting', 'pm_dev', 'pm_dev_porting', 'pm_fsm', 'pm_fsm_porting'],
    'pm_pmu_set': ['pm_pmu', 'pm_pmu_porting'],
    'pm_clock_set': ['pm_clock', 'pm_clock_porting'],
    'ux_manager': ['fsm_box', 'fsm_master', 'fsm_pairing', 'ux_event', 'ux_earphone', 'ux_timer', 'ux_tone_config', 'ux_tone_service', 'ux_led_porting', 'ux_led_config',
                   'ux_led_frame', 'ux_master', 'at_ux_manager', 'at_ux_register', 'ux_voiceassistant', 'ux_feature'],
    'ux_module': ['ux_box', 'ux_button', 'ux_pairing', 'ux_touchpad', 'ux_charge'],
    'security_unified': ['hal_security_unified', 'drv_security_unified', 'security_unified_port'],
    'efuse_v151': ['efuse_port', 'hal_efuse_v151', 'efuse'],
    'efuse_v120': ['efuse', 'hal_efuse_v120', 'efuse_porting'],
    'efuse_v100': ['efuse', 'hal_efuse_v100', 'efuse_porting'],
    'efuse_v130': ['efuse', 'hal_efuse_v130', 'efuse_port'],
    'codec_set': ['l2hc_dec_16k', 'l2hc_dec_48k_10ms', 'l2hc_dec_48k_5ms',
                  'opus_dec_half_itcm', 'opus_dec_half_flash', 'opus_dec_all_itcm', 'opus_dec_all_flash',
                  'opus_enc_half_itcm', 'opus_enc_half_flash', 'opus_enc_all_itcm', 'opus_enc_all_flash',
                  'opus_enc_2.5ms_itcm', 'opus_enc_2.5ms_flash', 'l2hc_enc_16k', 'l2hc_enc_48k_10ms','l2hc_enc_48k_5ms',
                  'sbc_dec_half_itcm', 'sbc_dec_half_flash', 'sbc_dec_all_itcm', 'sbc_dec_all_flash',
                  'sbc_enc_half_itcm', 'sbc_enc_half_flash', 'sbc_enc_all_itcm', 'sbc_enc_all_flash'],
    'ohos_set' : ['bootstrap', 'samgr', 'ams', 'bms', 'bms_sign_mode', 'ace', 'ace_adapt', 'permission', 'appverify', 'jerryscript','js_player', 'js_audio_captuer','i18n', 'resmgr', 'syspara'],
    'wearable_set' : ['nativeabilityfwk', 'nativelauncher', 'ai_voice_service', 'ai_voice_engine_hal', 'ai_voice_audio_stream_wrapper', 'ohos_deps',
                      'broadcast', 'msg_center', 'msg_center_adapt', 'ohos_startup'],
}

defines_set = {
    'libsec_defines': [
        "SECUREC_HAVE_WCTOMB=0", "SECUREC_HAVE_MBTOWC=0",
        "SECUREC_ENABLE_SCANF_FLOAT=0", "SECUREC_ENABLE_SPRINTF_FLOAT=0"],
    'chip_defines': [
        "BS25_CHIP_FPGA=0", "BS25_CHIP_V100=0", "BRANDY_CHIP_FPGA=0", "BRANDY_CHIP_V100=0", "SOCMN1_CHIP_FPGA=0",
        "SOCMN1_CHIP_V100=0", "SOCMN1_CHIP_V200=0", "LIBRA_CHIP_CS=0", "LIBRA_CHIP_FPGA=0", "SW39_CHIP_FPGA=0",
        "TARGET_CHIP_BS25=0", "TARGET_CHIP_BRANDY=0", "TARGET_CHIP_SOCMN1=0", "TARGET_CHIP_SW39=0",
        "TARGET_CHIP_LIBRA=0", "CHIP=1", "CHIP_VERSION=1", "CHIP_WS63=0", "SW21_CHIP_FPGA=0",
        "TARGET_CHIP_SW21=0", "CHIP_WS53=0", "CONFIG_UART_SUPPORT_TX", "CONFIG_UART_SUPPORT_RX",
        "CHIP_BS20=0", "CHIP_BS21=0", "CHIP_BS21A=0", "CHIP_BS21E=0", "CHIP_BS22=0", "CHIP_BS26=0",
        ],
    'version_defines': [],
    'libepmu_defines':['EPMU_BUCK_LDO_EN', 'EPMU_RTC_EN', 'EPMU_PWR_KEY_EN', 'EPMU_GPIO_EN', 'EPMU_WDT_EN', 'EPMU_EXTEND_EN'],
}

common_ccflags = [
    '-std=gnu99',
    '-Wall',
    '-Werror',
    '-Wextra',
    '-Winit-self',
    '-Wpointer-arith',
    '-Wstrict-prototypes',
    '-Wno-type-limits',
    '-fno-strict-aliasing',
    '-Os',
    '-fno-unwind-tables',
]

common_linkflags = [
    '-Wl,--gc-section',
    '-nostdlib',
    '-static',
]

arch_config = {
    'riscv70': {
        'ccflags': [
            '--target=riscv32',
            '-march=rv32imfcxlinxma_xlinxmb_xlinxmc',
            '-mabi=ilp32f',
            '-ffunction-sections',
            '-fdata-sections',
            '-fno-common',
            '-fsigned-char',
            '-fstack-protector-strong',
            '-mllvm',
            '-allow-unalign-ldst=false',
            "-Wno-error=out-of-line-declaration",
            "-Wno-error=uninitialized",
            "-Wno-error=int-conversion",
            "-Wno-error=enum-conversion",
            "-Wno-error=long-long",
            "-Wno-unused-variable",
            "-Wno-error=unused-variable",
            "-Wno-error=unused-value",
            "-Wno-error=unused-function",
            "-Wno-error=sign-compare",
            "-Wno-error=unused-parameter",
            "-Wno-error=tautological-compare",
            "-Wno-error=bad-function-cast",
            "-Wno-error=incompatible-pointer-types",
            "-Wno-error=implicit-function-declaration",
            "-Wno-missing-braces",
            "-Wno-main-return-type",
            "-Wno-unused-command-line-argument",
            "-Wno-undef",
            '-g',
        ],
        'linkflags': [
            '-Wl,--gc-section,--cref,-M',
            '-static',
            '-nostdlib',
            '--target=riscv32',
        ],
        'rom_ccflags': [
            '--target=riscv32',
            '-march=rv32imfcxlinxma_xlinxmb_xlinxmc',
            '-mabi=ilp32f',
            '-ffunction-sections',
            '-fdata-sections',
            '-fno-common',
            '-fsigned-char',
            '-Wno-error=main'
            "-Wno-error=out-of-line-declaration",
            "-Wno-error=uninitialized",
            "-Wno-error=int-conversion",
            "-Wno-error=enum-conversion",
            "-Wno-error=long-long",
            "-Wno-unused-variable",
            "-Wno-error=unused-variable",
            "-Wno-error=unused-value",
            "-Wno-error=unused-function",
            "-Wno-error=sign-compare",
            "-Wno-error=unused-parameter",
            "-Wno-error=tautological-compare",
            "-Wno-error=bad-function-cast",
            "-Wno-error=incompatible-pointer-types",
            "-Wno-error=implicit-function-declaration",
            "-Wno-missing-braces",
            "-Wno-main-return-type",
            "-Wno-unused-command-line-argument",
            "-Wno-undef",
            "-g"
        ],
        'std_libs': [
        ]
    },

    'riscv32': {
        'ccflags': [
            '--target=riscv32',
            '-march=rv32imc_xlinxma_xlinxmb_xlinxmc_xlinxmd',
            '-mabi=ilp32',
            '-ffunction-sections',
            '-fdata-sections',
            '-fno-common',
            '-fsigned-char',
            '-fstack-protector-strong',
            '-mllvm',
            '-allow-unalign-ldst=false',
            "-Wno-error=out-of-line-declaration",
            "-Wno-error=uninitialized",
            "-Wno-error=int-conversion",
            "-Wno-error=enum-conversion",
            "-Wno-error=long-long",
            "-Wno-unused-variable",
            "-Wno-error=unused-variable",
            "-Wno-error=unused-value",
            "-Wno-error=unused-function",
            "-Wno-error=sign-compare",
            "-Wno-error=unused-parameter",
            "-Wno-error=tautological-compare",
            "-Wno-error=bad-function-cast",
            "-Wno-error=incompatible-pointer-types",
            "-Wno-error=implicit-function-declaration",
            "-Wno-missing-braces",
            "-Wno-main-return-type",
            "-Wno-unused-command-line-argument",
            "-Wno-undef",
            '-g',
        ],
        'linkflags': [
            '-Wl,--gc-section,--cref,-M',
            '-static',
            '-nostdlib',
            '--target=riscv32',
        ],
        'rom_ccflags': [
            '--target=riscv32',
            '-march=rv32imc_xlinxma_xlinxmb_xlinxmc_xlinxmd',
            '-mabi=ilp32',
            '-ffunction-sections',
            '-fdata-sections',
            '-fno-common',
            '-fsigned-char',
            '-Wno-error=main'
            "-Wno-error=out-of-line-declaration",
            "-Wno-error=uninitialized",
            "-Wno-error=int-conversion",
            "-Wno-error=enum-conversion",
            "-Wno-error=long-long",
            "-Wno-unused-variable",
            "-Wno-error=unused-variable",
            "-Wno-error=unused-value",
            "-Wno-error=unused-function",
            "-Wno-error=sign-compare",
            "-Wno-error=unused-parameter",
            "-Wno-error=tautological-compare",
            "-Wno-error=bad-function-cast",
            "-Wno-error=incompatible-pointer-types",
            "-Wno-error=implicit-function-declaration",
            "-Wno-missing-braces",
            "-Wno-main-return-type",
            "-Wno-unused-command-line-argument",
            "-Wno-undef",
            "-g"
        ],
        'std_libs': [
        ]
    },
    'cortex_m7':{
        'ccflags': [
            '-fomit-frame-pointer',
            '-ffreestanding',
            '-fdata-sections',
            '-ffunction-sections',
            '-specs=nano.specs',
            '-nostdlib',
            '-pipe',
            '-mno-unaligned-access',
            "-fdebug-types-section",
            '-Wtrampolines',
            '-Wlogical-op',
            '-funsigned-char',
            '-Wformat=2',
            '-Wdate-time',
            #'-Wshadow',
            '-Wno-return-local-addr',
            #'-Wconversion',
            #'-Wcast-align',
            '-Wfloat-equal',
            #'-Wswitch-default',
            '-Wvla',
            '-Wjump-misses-init',
            '-march=armv7-m',
            '-mfpu=fpv5-d16',
            '-mthumb',
            '--specs=nosys.specs',
            '-Wno-incompatible-pointer-types',
            '-Wno-sign-compare',
            "-Wno-error=enum-conversion",

            "-Wno-error=uninitialized",
            "-Wno-error=int-conversion",
            "-Wno-error=long-long",
            "-Wno-unused-variable",
            "-Wno-error=unused-variable",
            "-Wno-error=unused-value",
            "-Wno-error=unused-function",
            "-Wno-error=sign-compare",
            "-Wno-error=unused-parameter",
            "-Wno-error=tautological-compare",
            "-Wno-error=bad-function-cast",
            "-Wno-error=cast-function-type",
            "-Wno-error=discarded-qualifiers",
            "-Wno-error=missing-field-initializers",
            "-Wno-error=incompatible-pointer-types",
            "-Wno-error=implicit-function-declaration",
            "-Wno-missing-braces",
            '-Wno-implicit-function-declaration'
        ],
        'linkflags': [
            '-Wl,--gc-section,--cref,--relax,-M',
            '-mthumb',
            '-flto=16',
            '-march=armv7-m',
            '-nostdlib'
        ],
        'rom_ccflags': [
            '-fomit-frame-pointer',
            '-ffreestanding',
            '-fdata-sections',
            '-ffunction-sections',
            '-specs=nano.specs',
            '-nostdlib',
            '-pipe',
            '-mno-unaligned-access',
            '-mthumb',
            "-fdebug-types-section",
            '-Wtrampolines',
            '-Wlogical-op',
            '-Wjump-misses-init',
            '-march=armv7-m',
            '-fno-inline-functions-called-once',
            '-fno-inline-small-functions',
            '--specs=nosys.specs',
        ],
        'std_libs': [
        ]
    },
    'riscv31': {
        'ccflags': [
            '-ffreestanding',
            '-fdata-sections',
            '-Wno-implicit-fallthrough',
            '-ffunction-sections',
            '-nostdlib',
            '-pipe',
            '-mabi=ilp32',
            '-march=rv32imc',
            '-fno-tree-scev-cprop',
            '-fno-common',
            '-mpush-pop',
            '-msmall-data-limit=0',
            '-fno-ipa-ra',
            '-Wtrampolines',
            '-Wlogical-op',
            '-Wjump-misses-init',
            "-Wa,-enable-c-lbu-sb",
            "-Wa,-enable-c-lhu-sh",
            "-fimm-compare",
            "-femit-muliadd",
            "-fmerge-immshf",
            "-femit-uxtb-uxth",
            "-femit-lli",
            "-femit-clz",
            "-fldm-stm-optimize",
            '-g',
        ],
        'linkflags': [
            "-Wl,--enjal16",
            "-nostdlib",
            "-g",
        ],
        'rom_ccflags': [
            '-ffreestanding',
            '-fdata-sections',
            '-Wno-implicit-fallthrough',
            '-ffunction-sections',
            '-nostdlib',
            '-pipe',
            '-mabi=ilp32',
            '-march=rv32imc',
            '-fno-tree-scev-cprop',
            '-fno-common',
            '-mpush-pop',
            '-msmall-data-limit=0',
            '-fno-ipa-ra',
            '-Wtrampolines',
            '-Wlogical-op',
            '-Wjump-misses-init',
            "-Wa,-enable-c-lbu-sb",
            "-Wa,-enable-c-lhu-sh",
            "-fimm-compare",
            "-femit-muliadd",
            "-fmerge-immshf",
            "-femit-uxtb-uxth",
            "-femit-lli",
            "-femit-clz",
            "-fldm-stm-optimize",
            "-g",
        ],
        'std_libs': [
            'gcc', 'm', 'c'
        ]
    },
}

arch_family = {
    "arm": [
        "cortex_m7", "cortex_m3"
    ],
    "riscv": [
        "riscv70", "riscv31", "riscv32"
    ],
}


class CommonConfig:
    def __init__(self, arch):
        self.arch_config = arch_config[arch]
        self.arch_family = ''
        for arch_family_name in arch_family:
            if arch in arch_family[arch_family_name]:
                self.arch_family = arch_family_name
        if not self.arch_family:
            raise Exception(f"{arch} is not found in arch_family, please add it")

    def get_ram_ccflags(self):
        temp = []
        temp.extend(common_ccflags)
        temp.extend(self.arch_config['ccflags'])
        return temp

    def get_rom_ccflags(self):
        return common_ccflags + self.arch_config['rom_ccflags']

    def get_definse(self, set_name):
        try:
            return defines_set[set_name]
        except KeyError:
            print(f"{set_name} is not in defines_set!!")
            raise

    def get_linkflags(self):
        return common_linkflags + self.arch_config['linkflags']

    def get_std_libs(self):
        return self.arch_config['std_libs']

    def get_arch_family(self):
        return self.arch_family

    def get_component_set(self, set_name):
        try:
            return component_set[set_name]
        except KeyError:
            print(f"{set_name} is not in component_set!!")
            raise
