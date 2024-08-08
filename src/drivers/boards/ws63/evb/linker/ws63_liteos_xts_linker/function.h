/*
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2021-2021. All rights reserved.
 * Description:  Default memory configurations
 *
 * Create: 2021-03-09
 */

// PLAT ROM组件KEEP 请在此添加
#ifdef USE_WS63_ROM_REPO
// rom repo platform componets
#define PLAT_ROM_KEEP(sections_patterns) \
    KEEP(SORT(*libsfc_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_sfc_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libpinctrl_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_pinctrl_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libpinctrl_port_ws63_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libwatchdog_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_watchdog_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libwatchdog_port_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libdrv_timer_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_timer_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libtimer_port_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libsystick_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libsystick_port_rom.a:)(sections_patterns)) \
    KEEP(SORT(*liblibboundscheck_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libtcxo_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_tcxo_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libtcxo_port_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libversion_rom.a:)(sections_patterns))
#else
// master platform componets
#define PLAT_ROM_KEEP(sections_patterns) \
    KEEP(SORT(*libsfc.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_sfc.a:)(sections_patterns)) \
    KEEP(SORT(*libpinctrl.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_pinctrl.a:)(sections_patterns)) \
    KEEP(SORT(*libpinctrl_port_ws63.a:)(sections_patterns)) \
    KEEP(SORT(*libwatchdog.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_watchdog.a:)(sections_patterns)) \
    KEEP(SORT(*libwatchdog_port.a:)(sections_patterns)) \
    KEEP(SORT(*libdrv_timer.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_timer.a:)(sections_patterns)) \
    KEEP(SORT(*libtimer_port.a:)(sections_patterns)) \
    KEEP(SORT(*libsystick.a:)(sections_patterns)) \
    KEEP(SORT(*libsystick_port.a:)(sections_patterns)) \
    KEEP(SORT(*liblibboundscheck.a:)(sections_patterns)) \
    KEEP(SORT(*libtcxo.a:)(sections_patterns)) \
    KEEP(SORT(*libhal_tcxo.a:)(sections_patterns)) \
    KEEP(SORT(*libtcxo_port.a:)(sections_patterns))
#endif

// PLAT ROM组件NO KEEP 请在此添加
#define PLAT_ROM_NO_KEEP(sections_patterns)


// WIFI ROM组件KEEP 请在此添加
#define WIFI_ROM_KEEP(sections_patterns) \
    KEEP(SORT(*libwifi_driver_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libwifi_alg_radar_sensor_rom.a:)(sections_patterns))

// WIFI ROM组件NO KEEP 请在此添加
#define WIFI_ROM_NO_KEEP(sections_patterns)


// BTC ROM组件KEEP 请在此添加
#define BTC_ROM_KEEP(sections_patterns) \
    KEEP(SORT(*libbgtp_rom.a:)(sections_patterns))

// BTC ROM组件NO KEEP 请在此添加
#define BTC_ROM_NO_KEEP(sections_patterns)


// BTH ROM组件KEEP 请在此添加
#define BTH_ROM_KEEP(sections_patterns) \
    KEEP(SORT(*libbt_host_rom.a:)(sections_patterns)) \
    KEEP(SORT(*libbg_common_rom.a:)(sections_patterns))

// BTH ROM组件NO KEEP 请在此添加
#define BTH_ROM_NO_KEEP(sections_patterns)

#define LITEOS_TASK_RAM_TEXT() \
    SORT(*libbase.a:*los_task.c.obj)(.text.LOS_ListDelete) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsPreemptable) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskReSched) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskSuspend) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskAdd2TimerList) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTimerListDelete) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskScan) \
    SORT(*libbase.a:*los_task.c.obj)(.text.LOS_CurTaskIDGet) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsCurTaskNameGet) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskSwitchCheck) \
    SORT(*libbase.a:*los_task.c.obj)(.text.LOS_IdleHandlerHookReg) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsIdleHandler) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsIdleTask) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskProcSignal) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskEntry) \
    SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskResume) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsSysTaskSuspend) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskWait) \
    SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskWake) \
    SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskYield) \
    SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskDelay) \
    SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskLock) \
    SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskUnlock) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemInfoGet) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.OsGetRealPtr) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemMergeNode) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemListDelete.isra.7) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemSplitNode) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemAllocWithCheck) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemFreeNode) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.LOS_MemAlloc) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.LOS_MemAllocAlign) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemFree) \
    SORT(*libbase.a:*llos_memory.c.obj)(.text.LOS_MemFree) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.LOS_MemRealloc) \
    SORT(*libbase.a:*los_memory.c.obj)(.text.LOS_MemInfoGet) \
    SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrDelete) \
    SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrTask) \
    SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrStart) \
    SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrScan) \
    SORT(*libbase.a:*los_swtmr.c.obj)(.text.LOS_SwtmrCreate) \
    SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrIdVerify) \
    SORT(*libbase.a:*los_swtmr.c.obj)(.text.LOS_SwtmrStart) \
    SORT(*libbase.a:*los_swtmr.c.obj)(.text.LOS_SwtmrStop) \
    SORT(*libbase.a:*los_swtmr.c.obj)(.text.LOS_SwtmrDelete)

// PLAT 非ROM组件请在此添加
#if defined(WS63_SUPPORT_HIGH_THROUGHPUT)
#define PLAT_NONROM(sections_patterns) \
    SORT(*libuart.a:)(sections_patterns) \
    SORT(*libhal_reboot.a:)(sections_patterns) \
    SORT(*libhal_uart_synopsys.a:)(sections_patterns) \
    SORT(*libliteos_port.a:)(sections_patterns) \
    SORT(*libalgorithm.a:)(sections_patterns) \
    SORT(*libchip_ws63.a:)(sections_patterns) \
    SORT(*libmbedtls.a:)(sections_patterns) \
    SORT(*libuart_port_synopsys.a:)(sections_patterns) \
    SORT(*libat.a:)(sections_patterns) \
    SORT(*libcpu_utils.a:)(sections_patterns) \
    SORT(*libgcc.a:)(sections_patterns) \
    SORT(*libdfx_diag.a:)(sections_patterns) \
    SORT(*libdfx_exception.a:)(sections_patterns) \
    SORT(*libdfx_log.a:)(sections_patterns) \
    SORT(*libdfx_panic.a:)(sections_patterns) \
    SORT(*libdfx_port_ws63.a:)(sections_patterns) \
    SORT(*libdfx_preserve.a:)(sections_patterns)
#else
#define PLAT_NONROM(sections_patterns) \
    SORT(*libc.a:)(sections_patterns) \
    SORT(*libuart.a:)(sections_patterns) \
    SORT(*libhal_reboot.a:)(sections_patterns) \
    SORT(*libhal_uart_synopsys.a:)(sections_patterns) \
    SORT(*libliteos_port.a:)(sections_patterns) \
    SORT(*libalgorithm.a:)(sections_patterns) \
    SORT(*libchip_ws63.a:)(sections_patterns) \
    SORT(*libmbedtls.a:)(sections_patterns) \
    SORT(*libuart_port_synopsys.a:)(sections_patterns) \
    SORT(*libat.a:)(sections_patterns) \
    SORT(*libcpu_utils.a:)(sections_patterns) \
    SORT(*libgcc.a:)(sections_patterns) \
    SORT(*libdfx_diag.a:)(sections_patterns) \
    SORT(*libdfx_exception.a:)(sections_patterns) \
    SORT(*libdfx_log.a:)(sections_patterns) \
    SORT(*libdfx_panic.a:)(sections_patterns) \
    SORT(*libdfx_port_ws63.a:)(sections_patterns) \
    SORT(*libdfx_preserve.a:)(sections_patterns) \
    SORT(*libbase.a:)(sections_patterns) \
    SORT(*libws63_liteos_app.a:)(sections_patterns) \
    SORT(*libnon_os.a:)(sections_patterns) \
    SORT(*libproc.a:)(sections_patterns) \
    SORT(*libcompat.a:)(sections_patterns) \
    SORT(*libcmsis.a:)(sections_patterns) \
    SORT(*libdisk.a:)(sections_patterns) \
    SORT(*libmulti_partition.a:)(sections_patterns) \
    SORT(*libinit.a:)(sections_patterns) \
    SORT(*libm.a:)(sections_patterns) \
    SORT(*libfat.a:)(sections_patterns) \
    SORT(*libvfs.a:)(sections_patterns) \
    SORT(*libsec.a:)(sections_patterns) \
    SORT(*libcsysdeps.a:)(sections_patterns) \
    SORT(*libposix.a:)(sections_patterns) \
    SORT(*libinterrupt.a:)(sections_patterns) \
    SORT(*libtargets.a:)(sections_patterns) \
    SORT(*libdriverbase.a:)(sections_patterns) \
    SORT(*liblinux.a:)(sections_patterns) \
    SORT(*libbcache.a:)(sections_patterns) \
    SORT(*libramfs.a:)(sections_patterns) \
    SORT(*libconsole.a:)(sections_patterns) \
    SORT(*libriscv.a:)(sections_patterns) \
    SORT(*libosal.a:)(sections_patterns) \
    SORT(*libosal_adapt.a:)(sections_patterns) \
    SORT(*liblos_iperf.a:)(sections_patterns) \
    SORT(*libhal_uart.a:)(sections_patterns) \
    SORT(*libhal_tsensor.a:)(sections_patterns) \
    SORT(*libuart_port.a:)(sections_patterns) \
    SORT(*libhal_pmp.a:)(sections_patterns) \
    SORT(*libpmp_cfg_ws63.a:)(sections_patterns) \
    SORT(*libpmp_port.a:)(sections_patterns) \
    SORT(*libi2c_porting.a:)(sections_patterns) \
    SORT(*libsfc_flash_config_ws63.a:)(sections_patterns) \
    SORT(*libtsensor.a:)(sections_patterns) \
    SORT(*libtsensor_port.a:)(sections_patterns) \
    SORT(*libefuse.a:)(sections_patterns) \
    SORT(*libefuse_port.a:)(sections_patterns) \
    SORT(*libhal_efuse_v151.a:)(sections_patterns) \
    SORT(*libsoc_port.a:)(sections_patterns) \
    SORT(*libplat_patch.a:)(sections_patterns) \
    SORT(*libnv_ws63.a:)(sections_patterns) \
    SORT(*libupdate_common_ws63.a:)(sections_patterns) \
    SORT(*libapp_init.a:)(sections_patterns) \
    SORT(*libplt_at.a:)(sections_patterns) \
    SORT(*libdiag_dfx_cmd.a:)(sections_patterns) \
    SORT(*libnv.a:)(sections_patterns) \
    SORT(*libupdate_common.a:)(sections_patterns) \
    SORT(*libmips.a:)(sections_patterns) \
    SORT(*libcpup.a:)(sections_patterns) \
    SORT(*libhal_gpio_v150.a:)(sections_patterns) \
    SORT(*libgpio_port.a:)(sections_patterns) \
    SORT(*libboundscheck_sscanf_s.a:)(sections_patterns)
#endif

// WIFI非ROM组件 请在此添加
#ifdef WIFI_TCM_OPTIMIZE
#define WIFI_NONROM(sections_patterns) \
    SORT(*libwifi_driver_hmac.a:)(sections_patterns) \
    SORT(*libwifi_driver_dmac.a:)(sections_patterns) \
    SORT(*libwpa_supplicant.a:)(sections_patterns) \
    SORT(*libwifi_service.a:)(sections_patterns) \
    SORT(*libwifi_at.a:)(sections_patterns) \
    SORT(*liblwip.a:)(sections_patterns) \
    SORT(*liblwip_tcm.a:)(sections_patterns) \
    SORT(*libwifi_promisc.a:)(sections_patterns) \
    SORT(*libwifi_auto_adjust_freq.a:)(sections_patterns) \
    SORT(*libwifi_csa.a:)(sections_patterns) \
    SORT(*libwifi_alg_temp_protect.a:)(sections_patterns) \
    SORT(*libwifi_alg_txbf.a:)(sections_patterns) \
    SORT(*libwifi_alg_anti_interference.a:)(sections_patterns) \
    SORT(*libwifi_alg_edca_opt.a:)(sections_patterns) \
    SORT(*libwifi_roam.a:)(sections_patterns) \
    SORT(*libwifi_alg_cca_opt.a:)(sections_patterns) \
    SORT(*libwifi_tx_amsdu.a:)(sections_patterns) \
    SORT(*libwifi_sdp.a:)(sections_patterns) \
    SORT(*libwifi_blacklist.a:)(sections_patterns) \
    SORT(*libwifi_sr.a:)(sections_patterns) \
    SORT(*libwifi_frag.a:)(sections_patterns) \
    SORT(*libwifi_mbo.a:)(sections_patterns) \
    SORT(*libwifi_bsrp_nfrp.a:)(sections_patterns) \
    SORT(*libwifi_slp.a:)(sections_patterns) \
    SORT(*libwifi_radar_sensor.a:)(sections_patterns) \
    SORT(*libwifi_apf.a:)(sections_patterns) \
    SORT(*libwifi_repeater.a:)(sections_patterns) \
    SORT(*libwifi_csi.a:)(sections_patterns) \
    SORT(*libwifi_wapi.a:)(sections_patterns) \
    SORT(*libwifi_wps.a:)(sections_patterns) \
    SORT(*libwifi_psd.a:)(sections_patterns) \
    SORT(*libwifi_latency.a:)(sections_patterns) \
    SORT(*libwifi_roam.a:)(sections_patterns) \
    SORT(*libwifi_uapsd_sta.a:)(sections_patterns) \
    SORT(*libwifi_ant_sel.a:)(sections_patterns) \
    SORT(*libwifi_twt.a:)(sections_patterns) \
    SORT(*libwifi_11k.a:)(sections_patterns) \
    SORT(*libwifi_11v.a:)(sections_patterns) \
    SORT(*libwifi_11r.a:)(sections_patterns) \
    SORT(*libwifi_btcoex.a:)(sections_patterns) \
    SORT(*libwifi_uapsd_ap.a:)(sections_patterns) \
    SORT(*libwifi_dnb.a:)(sections_patterns) \
    SORT(*libwifi_m2u.a:)(sections_patterns)
#else
#define WIFI_NONROM(sections_patterns) \
    SORT(*libwifi_driver_hmac.a:)(sections_patterns) \
    SORT(*libwifi_driver_dmac.a:)(sections_patterns) \
    SORT(*libwpa_supplicant.a:)(sections_patterns) \
    SORT(*libwifi_service.a:)(sections_patterns) \
    SORT(*libwifi_at.a:)(sections_patterns) \
    SORT(*liblwip.a:)(sections_patterns) \
    SORT(*liblwip_tcm.a:)(sections_patterns) \
    SORT(*libwifi_promisc.a:)(sections_patterns) \
    SORT(*libwifi_auto_adjust_freq.a:)(sections_patterns) \
    SORT(*libwifi_csa.a:)(sections_patterns) \
    SORT(*libwifi_alg_temp_protect.a:)(sections_patterns) \
    SORT(*libwifi_alg_txbf.a:)(sections_patterns) \
    SORT(*libwifi_alg_anti_interference.a:)(sections_patterns) \
    SORT(*libwifi_alg_edca_opt.a:)(sections_patterns) \
    SORT(*libwifi_roam.a:)(sections_patterns) \
    SORT(*libwifi_alg_cca_opt.a:)(sections_patterns) \
    SORT(*libwifi_tx_amsdu.a:)(sections_patterns) \
    SORT(*libwifi_sdp.a:)(sections_patterns) \
    SORT(*libwifi_blacklist.a:)(sections_patterns) \
    SORT(*libwifi_sr.a:)(sections_patterns) \
    SORT(*libwifi_frag.a:)(sections_patterns) \
    SORT(*libwifi_mbo.a:)(sections_patterns) \
    SORT(*libwifi_bsrp_nfrp.a:)(sections_patterns) \
    SORT(*libwifi_slp.a:)(sections_patterns) \
    SORT(*libwifi_radar_sensor.a:)(sections_patterns) \
    SORT(*libwifi_apf.a:)(sections_patterns) \
    SORT(*libwifi_repeater.a:)(sections_patterns) \
    SORT(*libwifi_csi.a:)(sections_patterns) \
    SORT(*libwifi_wapi.a:)(sections_patterns) \
    SORT(*libwifi_wps.a:)(sections_patterns) \
    SORT(*libwifi_psd.a:)(sections_patterns) \
    SORT(*libwifi_latency.a:)(sections_patterns) \
    SORT(*libwifi_roam.a:)(sections_patterns) \
    SORT(*libwifi_uapsd_sta.a:)(sections_patterns) \
    SORT(*libwifi_ant_sel.a:)(sections_patterns) \
    SORT(*libwifi_twt.a:)(sections_patterns) \
    SORT(*libwifi_11k.a:)(sections_patterns) \
    SORT(*libwifi_11v.a:)(sections_patterns) \
    SORT(*libwifi_11r.a:)(sections_patterns) \
    SORT(*libwifi_btcoex.a:)(sections_patterns) \
    SORT(*libwifi_uapsd_ap.a:)(sections_patterns) \
    SORT(*libwifi_dnb.a:)(sections_patterns) \
    SORT(*libwifi_m2u.a:)(sections_patterns)
#endif

#define RADAR_NONROM(sections_patterns) \
    SORT(*libradar_sensing.a:)(sections_patterns) \
    SORT(*libradar_at.a:)(sections_patterns)

// BTC非ROM组件 请在此添加
#define BTC_NONROM(sections_patterns) \
    SORT(*libbgtp.a:)(sections_patterns)

// BTH非ROM组件 请在此添加
#define BTH_NONROM(sections_patterns) \
    SORT(*libbg_common.a:)(sections_patterns) \
    SORT(*libbt_host.a:)(sections_patterns) \
    SORT(*libbth_gle.a:)(sections_patterns) \
    SORT(*libbth_sdk.a:)(sections_patterns) \
    SORT(*libbt_at.a:)(sections_patterns) \
    SORT(*libbt_app.a:)(sections_patterns) \
    SORT(*libsamples.a:)(sections_patterns) \
    SORT(*libtestsuite.a:)(sections_patterns)

// WIFI cali非ROM 请在此添加
#define WIFI_CALI_NONROM(sections_patterns)

// BTC cali非ROM 请在此添加
#define BTC_CALI_NONROM(sections_patterns)

// BTH cali非ROM 请在此添加
#define BTH_CALI_NONROM(sections_patterns)

// PLAT SRAM请在此添加
#if defined(WS63_SUPPORT_HIGH_THROUGHPUT)
#define PLAT_SRAM_NONROM(sections_patterns) \
    SORT(*libws63_liteos_app.a:)(sections_patterns) \
    SORT(*libosal.a:)(sections_patterns) \
    SORT(*libosal_adapt.a:)(sections_patterns) \
    SORT(*libnon_os.a:)(sections_patterns) \
    SORT(*libproc.a:)(sections_patterns) \
    SORT(*libcompat.a:)(sections_patterns) \
    SORT(*libbase.a:)(sections_patterns) \
    SORT(*libcmsis.a:)(sections_patterns) \
    SORT(*libdisk.a:)(sections_patterns) \
    SORT(*libmulti_partition.a:)(sections_patterns) \
    SORT(*libinit.a:)(sections_patterns) \
    SORT(*libm.a:)(sections_patterns) \
    SORT(*libfat.a:)(sections_patterns) \
    SORT(*libvfs.a:)(sections_patterns) \
    SORT(*libsec.a:)(sections_patterns) \
    SORT(*libcsysdeps.a:)(sections_patterns) \
    SORT(*libposix.a:)(sections_patterns) \
    SORT(*liblos_iperf.a:)(sections_patterns) \
    SORT(*libinterrupt.a:)(sections_patterns) \
    SORT(*libtargets.a:)(sections_patterns) \
    SORT(*libdriverbase.a:)(sections_patterns) \
    SORT(*libriscv.a:)(sections_patterns) \
    SORT(*liblinux.a:)(sections_patterns) \
    SORT(*libbcache.a:)(sections_patterns) \
    SORT(*libramfs.a:)(sections_patterns) \
    SORT(*libconsole.a:)(sections_patterns) \
    SORT(*libpm_port_ws63.a:)(sections_patterns) \
    SORT(*libsfc_port_ws63.a:)(sections_patterns) \
    SORT(*libtest_sfc.a:)(sections_patterns)
#else
#define PLAT_SRAM_NONROM(sections_patterns) \
    SORT(*libbase.a:*los_hwi.c.obj)(sections_patterns) \
    SORT(*libbase.a:*los_tick.c.obj)(sections_patterns) \
    SORT(*libbase.a:*los_sched.c.obj)(sections_patterns) \
    SORT(*libbase.a:*los_exc.c.obj)(sections_patterns) \
    SORT(*libbase.a:*los_sortlink.c.obj)(sections_patterns) \
    SORT(*libinterrupt.a:*riscv_himideerv200_plic.c.obj)(sections_patterns) \
    SORT(*libriscv.a:*exception.S.obj)(sections_patterns) \
    SORT(*libriscv.a:*trap.S.obj)(sections_patterns) \
    SORT(*libriscv.a:*dispatch.S.obj)(sections_patterns) \
    SORT(*libriscv.a:*cache.c.obj)(sections_patterns) \
    SORT(*libriscv.a:*task.c.obj)(sections_patterns) \
    SORT(*libcsysdeps.a:*memcpy.S.obj)(sections_patterns) \
    SORT(*libcsysdeps.a:*memset.S.obj)(sections_patterns) \
    SORT(*libosal.a:*osal_spinlock.c.obj)(sections_patterns) \
    SORT(*libosal.a:*osal_interrupt.c.obj)(sections_patterns) \
    SORT(*libosal.a:*osal_addr.c.obj)(sections_patterns) \
    SORT(*libosal_adapt.a:*osal_adapt_timer.c.obj)(sections_patterns) \
    SORT(*libosal_adapt.a:*osal_adapt_task.c.obj)(sections_patterns) \
    SORT(*libpm_port_ws63.a:)(sections_patterns) \
    SORT(*libsfc_port_ws63.a:)(sections_patterns) \
    SORT(*libtest_sfc.a:)(sections_patterns)
#endif

// WIFI SRAM请在此添加
#define WIFI_SRAM_NONROM(sections_patterns)

// BTC SRAM请在此添加
#define BTC_SRAM_NONROM(sections_patterns)

// BTH SRAM请在此添加
#define BTH_SRAM_NONROM(sections_patterns)

// PLAT 需要放在tcm中的 请在此添加
#define PLAT_TCM_NONROM(sections_patterns)

// LWIP性能相关请在此添加
#define LWIP_PERFORMANCE_SECTION() \
    SORT(*liblwip_tcm.a:)(.text.get_socket) \
    SORT(*liblwip_tcm.a:)(.text.tryget_socket) \
    SORT(*liblwip_tcm.a:)(.text.tryget_socket_unconn) \
    SORT(*liblwip_tcm.a:)(.text.sock_inc_used) \
    SORT(*liblwip_tcm.a:)(.text.tryget_socket_unconn_nouse) \
    SORT(*liblwip_tcm.a:)(.text.tryget_socket_unconn_locked) \
    SORT(*liblwip_tcm.a:)(.text.done_socket) \
    SORT(*liblwip_tcm.a:)(.text.lwip_recvfrom) \
    SORT(*liblwip_tcm.a:)(.text.lwip_recv_tcp) \
    SORT(*liblwip_tcm.a:)(.text.event_callback) \
    SORT(*liblwip_tcm.a:)(.text.lwip_select) \
    SORT(*liblwip_tcm.a:)(.text.select_check_waiters) \
    SORT(*liblwip_tcm.a:)(.text.netconn_apimsg) \
    SORT(*liblwip_tcm.a:)(.text.netconn_recv_data) \
    SORT(*liblwip_tcm.a:)(.text.netconn_tcp_recvd_msg) \
    SORT(*liblwip_tcm.a:)(.text.netconn_tcp_recvd) \
    SORT(*liblwip_tcm.a:)(.text.netconn_recv_data_tcp) \
    SORT(*liblwip_tcm.a:)(.text.netconn_recv_tcp_pbuf_flags) \
    SORT(*liblwip_tcm.a:)(.text.netconn_recv_udp_raw_netbuf_flags) \
    KEEP(SORT(*liblwip_tcm.a:)(.text.netconn_recv)) \
    KEEP(SORT(*liblwip_tcm.a:)(.text.netconn_sendto)) \
    SORT(*liblwip_tcm.a:)(.text.netconn_send) \
    SORT(*liblwip_tcm.a:)(.text.netconn_write_partly) \
    SORT(*liblwip_tcm.a:)(.text.netconn_write_vectors_partly) \
    SORT(*liblwip_tcm.a:)(.text.lwip_netconn_do_send) \
    SORT(*liblwip_tcm.a:)(.text.lwip_netconn_do_write) \
    SORT(*liblwip_tcm.a:)(.text.netbuf_alloc) \
    SORT(*liblwip_tcm.a:)(.text.netbuf_free) \
    SORT(*liblwip_tcm.a:)(.text.tcpip_thread) \
    KEEP(SORT(*liblwip_tcm.a:)(.text.tcpip_thread_handle_msg)) \
    SORT(*liblwip_tcm.a:)(.text.tcpip_inpkt) \
    SORT(*liblwip_tcm.a:)(.text.tcpip_input) \
    SORT(*liblwip_tcm.a:)(.text.tcpip_callback) \
    SORT(*liblwip_tcm.a:)(.text.tcpip_try_callback) \
    SORT(*liblwip_tcm.a:)(.text.tcpip_send_msg_wait_sem) \
    SORT(*liblwip_tcm.a:)(.text.inet_cksum_pseudo_base) \
    SORT(*liblwip_tcm.a:)(.text.inet_chksum_pseudo) \
    SORT(*liblwip_tcm.a:)(.text.etharp_output_to_arp_index) \
    SORT(*liblwip_tcm.a:)(.text.etharp_output) \
    SORT(*liblwip_tcm.a:)(.text.ip4_addr_isbroadcast_u32) \
    SORT(*liblwip_tcm.a:)(.text.ip_input) \
    SORT(*liblwip_tcm.a:)(.text.ip4_route) \
    SORT(*liblwip_tcm.a:)(.text.ip4_input) \
    SORT(*liblwip_tcm.a:)(.text.ip4_output_if) \
    SORT(*liblwip_tcm.a:)(.text.ip4_output_if_opt) \
    SORT(*liblwip_tcm.a:)(.text.ip4_output_if_src) \
    SORT(*liblwip_tcm.a:)(.text.ip4_output_if_opt_src) \
    SORT(*liblwip_tcm.a:)(.text.pbuf_alloc) \
    SORT(*liblwip_tcm.a:)(.text.pbuf_add_header_impl) \
    SORT(*liblwip_tcm.a:)(.text.pbuf_add_header) \
    SORT(*liblwip_tcm.a:)(.text.pbuf_remove_header) \
    SORT(*liblwip_tcm.a:)(.text.pbuf_header_impl) \
    SORT(*liblwip_tcm.a:)(.text.pbuf_header) \
    SORT(*liblwip_tcm.a:)(.text.pbuf_free) \
    KEEP(SORT(*liblwip_tcm.a:)(.text.pbuf_alloced_custom)) \
    SORT(*liblwip_tcm.a:)(.text.pbuf_init_alloced_pbuf) \
    SORT(*liblwip_tcm.a:)(.text.pbuf_alloc_from_ptk_buf) \
    SORT(*liblwip_tcm.a:)(.text.ethernet_input) \
    SORT(*liblwip_tcm.a:)(.text.ethernet_output) \
    SORT(*liblwip_tcm.a:)(.text.sys_mutex_lock) \
    SORT(*liblwip_tcm.a:)(.text.sys_mutex_unlock) \
    SORT(*liblwip_tcm.a:)(.text.sys_sem_signal) \
    SORT(*liblwip_tcm.a:)(.text.sys_arch_sem_wait) \
    SORT(*liblwip_tcm.a:)(.text.sys_mbox_post) \
    SORT(*liblwip_tcm.a:)(.text.sys_mbox_trypost) \
    SORT(*liblwip_tcm.a:)(.text.sys_arch_mbox_fetch_ext) \
    SORT(*liblwip_tcm.a:)(.text.driverif_output) \
    SORT(*liblwip_tcm.a:)(.text.driverif_input) \
    SORT(*liblwip_tcm.a:)(.text.ip4_frag) \
    SORT(*liblwip_tcm.a:)(.text.ip4_reass) \
    SORT(*liblwip_tcm.a:)(.text.ip_addr_cmp) \
    SORT(*liblwip_tcm.a:)(.text.ip_addr_copy_fun) \
    SORT(*liblwip_tcm.a:)(.text.ip_addr_isany) \
    SORT(*liblwip_tcm.a:)(.text.mem_free) \
    SORT(*liblwip_tcm.a:)(.text.mem_malloc) \
    SORT(*liblwip_tcm.a:)(.text.memp_free) \
    SORT(*liblwip_tcm.a:)(.text.memp_malloc) \
    SORT(*liblwip_tcm.a:)(.text.mem_trim) \
    SORT(*liblwip_tcm.a:)(.text.netif_default) \
    SORT(*liblwip_tcm.a:)(.text.netif_find) \
    SORT(*liblwip_tcm.a:)(.text.netif_find_by_ifindex) \
    SORT(*liblwip_tcm.a:)(.text.netif_find_by_ipaddr) \
    SORT(*liblwip_tcm.a:)(.text.netif_find_dst_ipaddr) \
    SORT(*liblwip_tcm.a:)(.text.netif_ipaddr_isbrdcast) \
    SORT(*liblwip_tcm.a:)(.text.netif_loop_output) \
    SORT(*liblwip_tcm.a:)(.text.netif_loop_output_ipv4) \
    SORT(*liblwip_tcm.a:)(.text.raw_remove) \
    SORT(*liblwip_tcm.a:)(.text.raw_send) \
    SORT(*liblwip_tcm.a:)(.text.raw_sendto) \
    SORT(*liblwip_tcm.a:)(.text.tcp_abort) \
    SORT(*liblwip_tcm.a:)(.text.tcp_accept) \
    SORT(*liblwip_tcm.a:)(.text.tcp_active_pcbs) \
    SORT(*liblwip_tcm.a:)(.text.tcp_arg) \
    SORT(*liblwip_tcm.a:)(.text.tcp_backlog_accepted) \
    SORT(*liblwip_tcm.a:)(.text.tcp_backlog_delayed) \
    SORT(*liblwip_tcm.a:)(.text.tcp_listen_with_backlog_and_err) \
    SORT(*liblwip_tcm.a:)(.text.tcp_new_ip_type) \
    SORT(*liblwip_tcm.a:)(.text.tcp_recv) \
    SORT(*liblwip_tcm.a:)(.text.tcp_shutdown) \
    SORT(*liblwip_tcm.a:)(.text.tcp_sndbuf_init) \
    SORT(*liblwip_tcm.a:)(.text.tcp_write) \
    SORT(*liblwip_tcm.a:)(.text.tcp_input) \
    SORT(*liblwip_tcm.a:)(.text.tcp_output)

#ifdef LWIP_TCM_OPTIMIZE
#define LWIP_TCM_SECTION() \
    LWIP_PERFORMANCE_SECTION()
#define LWIP_FLASH_SECTION()
#else
#define LWIP_TCM_SECTION()

#define LWIP_FLASH_SECTION() \
    LWIP_PERFORMANCE_SECTION()
#endif

// WIFI需要放在tcm中的 请在此添加
#ifdef WIFI_TCM_OPTIMIZE
#define WIFI_TCM_NONROM(sections_patterns) \
    SORT(*libwifi_driver_tcm.a:)(sections_patterns)
#define WIFI_TCM_SECTION() \
    KEEP(*(.wifi.tcm.text .wifi.tcm.rodata))
#define WIFI_FLASH_SECTION()
#define WIFI_FLASH_NONROM(sections_patterns)
#else // WIFI_TCM_OPTIMIZE
#define WIFI_TCM_NONROM(sections_patterns)
#define WIFI_TCM_SECTION()
#define WIFI_FLASH_SECTION() \
    KEEP(*(.wifi.tcm.text .wifi.tcm.rodata))
#define WIFI_FLASH_NONROM(sections_patterns) \
    SORT(*libwifi_driver_tcm.a:)(sections_patterns)
#endif // WIFI_TCM_OPTIMIZE

// BTC需要放在tcm中的 请在此添加
#define BTC_TCM_NONROM(sections_patterns) \
    SORT(*libbgtp.a:bgtp_sleep_sw_ram**.o*)(sections_patterns) \
    SORT(*libbgtp.a:)(.text.bt_thread_handle) \
    SORT(*libbgtp.a:)(.text.bt_crypto_thread_handle) \
    SORT(*libbgtp.a:dts_interrupt_itcm**.o*)(sections_patterns) \
    SORT(*libbgtp.a:dts_interrupt_dev_only**.o*)(sections_patterns) \
    SORT(*libbgtp.a:evt_sched_mgr_ram**.o*)(sections_patterns) \
    SORT(*libbgtp.a:evt_sched_ram**.o*)(sections_patterns) \
    SORT(*libbgtp.a:evt_sched_util_ram**.o*)(sections_patterns) \
    SORT(*libbgtp.a:evt_prog_ram**.o*)(sections_patterns)

// WIFI需要单独放在DTCM中的变量在此添加
#if defined(CONFIG_BGLE_RAM_SIZE_16K) && !defined(WIFI_TCM_OPTIMIZE) // liteos-app
#define WIFI_OPTIMIZE_DTCM(sections_patterns) \
    PLAT_NONROM(sections_patterns) \
    WIFI_NONROM(sections_patterns) \
    RADAR_NONROM(sections_patterns) \
    BTC_NONROM(sections_patterns) \
    BTH_NONROM(sections_patterns) \
    WIFI_CALI_NONROM(sections_patterns) \
    BTC_CALI_NONROM(sections_patterns) \
    BTH_CALI_NONROM(sections_patterns) \
    PLAT_SRAM_NONROM(sections_patterns) \
    WIFI_SRAM_NONROM(sections_patterns) \
    BTC_SRAM_NONROM(sections_patterns) \
    BTH_SRAM_NONROM(sections_patterns)
#else
#define WIFI_OPTIMIZE_DTCM(sections_patterns)
#endif