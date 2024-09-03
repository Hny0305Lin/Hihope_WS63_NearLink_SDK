OUTPUT_ARCH("riscv")
ENTRY(reset_vector)
MEMORY
{
    ROM : ORIGIN = 0x109000, LENGTH = 0x43000
    ITCM : ORIGIN = 0x14C000, LENGTH = 0x4000
    DTCM : ORIGIN = 0x180000, LENGTH = 0x4000
    SRAM : ORIGIN = 0xA00000, LENGTH = ((0x88000 - 0x2000 - (0x100)) - 0x0)
    PRESERVE_SHARE_MEM : ORIGIN = (0xA00000 + (0x88000 - 0x2000 - (0x100))), LENGTH = (0x100)
    PROGRAM_STARTUP : ORIGIN = (0x230000 + 0x300), LENGTH = 0x400
    PROGRAM : ORIGIN = (0x230000 + 0x300) + 0x400, LENGTH = ((0x200000 - 0x300) - 0x0)- 0x400
}
SECTIONS
{
    startup : ALIGN(8) {
       
        KEEP (*(.text.entry))
        KEEP (*(.text.runtime.init))
        . = ALIGN(4);
        __zinitcall_app_run_start = .;
        KEEP(*(.zinitcall.app_run*.init))
        __zinitcall_app_run_end = .;
    } > PROGRAM_STARTUP
    .sysintcall.init : {
        . = ALIGN(4);
        __sysinitcall_start = .;
        __sysinitcall0_start = .;
        KEEP(*(SORT(.sysinitcall_level0_*)))
        __sysinitcall1_start = .;
        KEEP(*(SORT(.sysinitcall_level1_*)))
        __sysinitcall2_start = .;
        KEEP(*(SORT(.sysinitcall_level2_*)))
        __sysinitcall3_start = .;
        KEEP(*(SORT(.sysinitcall_level3_*)))
        __sysinitcall4_start = .;
        KEEP(*(SORT(.sysinitcall_level4_*)))
        __sysinitcall5_start = .;
        KEEP(*(SORT(.sysinitcall_level5_*)))
        __sysinitcall6_start = .;
        KEEP(*(SORT(.sysinitcall_level6_*)))
        __sysinitcall7_start = .;
        KEEP(*(SORT(.sysinitcall_level7_*)))
        __sysinitcall_end = .;
    } > PROGRAM_STARTUP
    .initcall.init : {
        __initcall0_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall0.init)))
        __initcall0_end = ABSOLUTE(.);
        __initcall1_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall1.init)))
        __initcall1_end = ABSOLUTE(.);
        __initcall2_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall2.init)))
        __initcall2_end = ABSOLUTE(.);
        __initcall3_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall3.init)))
        __initcall3_end = ABSOLUTE(.);
        __initcall4_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall4.init)))
        __initcall4_end = ABSOLUTE(.);
        __initcall5_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall5.init)))
        __initcall5_end = ABSOLUTE(.);
        __initcall6_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall6.init)))
        __initcall6_end = ABSOLUTE(.);
        __initcall7_start = ABSOLUTE(.);
        KEEP(*(SORT (.initcall7.init)))
        __initcall7_end = ABSOLUTE(.);
    } > PROGRAM_STARTUP
}
SECTIONS {
    .plat_romtext : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(SORT(*libsfc_rom.a:)(.text* .rodata*)) KEEP(SORT(*libhal_sfc_rom.a:)(.text* .rodata*)) KEEP(SORT(*libpinctrl_rom.a:)(.text* .rodata*)) KEEP(SORT(*libhal_pinctrl_rom.a:)(.text* .rodata*)) KEEP(SORT(*libpinctrl_port_ws63_rom.a:)(.text* .rodata*)) KEEP(SORT(*libwatchdog_rom.a:)(.text* .rodata*)) KEEP(SORT(*libhal_watchdog_rom.a:)(.text* .rodata*)) KEEP(SORT(*libwatchdog_port_rom.a:)(.text* .rodata*)) KEEP(SORT(*libdrv_timer_rom.a:)(.text* .rodata*)) KEEP(SORT(*libhal_timer_rom.a:)(.text* .rodata*)) KEEP(SORT(*libtimer_port_rom.a:)(.text* .rodata*)) KEEP(SORT(*libsystick_rom.a:)(.text* .rodata*)) KEEP(SORT(*libsystick_port_rom.a:)(.text* .rodata*)) KEEP(SORT(*liblibboundscheck_rom.a:)(.text* .rodata*)) KEEP(SORT(*libtcxo_rom.a:)(.text* .rodata*)) KEEP(SORT(*libhal_tcxo_rom.a:)(.text* .rodata*)) KEEP(SORT(*libtcxo_port_rom.a:)(.text* .rodata*)) KEEP(SORT(*libversion_rom.a:)(.text* .rodata*))
       
    } > ROM
    .wifi_romtext : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(SORT(*libwifi_driver_rom.a:)(.text* .rodata*)) KEEP(SORT(*libwifi_alg_radar_sensor_rom.a:)(.text* .rodata*))
       
    } > ROM
    .btc_romtext : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(SORT(*libbgtp_rom.a:)(.text* .rodata*))
       
    } > ROM
    .bth_romtext : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(SORT(*libbt_host_rom.a:)(.text* .rodata*)) KEEP(SORT(*libbg_common_rom.a:)(.text* .rodata*))
       
    } > ROM
}
SECTIONS
{
    .patch (0x14C000) : ALIGN(8) {
        __rom_patch_load__ = LOADADDR(.patch);
        __rom_patch_begin__ = .;
        KEEP(*(.patch_remap))
        KEEP(*(.patch_cmp))
        . = ALIGN(8);
        ASSERT(((__rom_patch_load__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
        ASSERT(((__rom_patch_begin__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > ITCM AT > PROGRAM
    .rom_ram_cb_text (0x14C928) : ALIGN(8) {
        FILL(0xFF)
        _SVENEER_TEXT = .; . = ALIGN(8); KEEP(*.o*(.__adddf3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__ashldi3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__divdf3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__extendsfdf2.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__fixunsdfsi.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__floatsidf.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__floatunsidf.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__muldf3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__truncdfsf2.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__udivdi3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.__umoddi3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.aes_add_round_key.romcall.text)); . = ALIGN(8); KEEP(*.o*(.aes_input.romcall.text)); . = ALIGN(8); KEEP(*.o*(.aes_key_expansion.romcall.text)); . = ALIGN(8); KEEP(*.o*(.aes_output.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgsub_pmu_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgtp_get_lowpower_clock_freq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgtp_prevent_sleep_set.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgtp_sleep_time_calc.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bgtp_start_sleep.romcall.text)); . = ALIGN(8); KEEP(*.o*(.ble_acl_calc_drift.romcall.text)); . = ALIGN(8); KEEP(*.o*(.ble_sca_get_cfg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bt_crypto_thread_handle.romcall.text)); . = ALIGN(8); KEEP(*.o*(.bt_rf_cali_rx_content.romcall.text)); . = ALIGN(8); KEEP(*.o*(.chnl_calc_ble_chnl_cls_to_chnl_map.romcall.text)); . = ALIGN(8); KEEP(*.o*(.chnl_calc_get_valid_map_num.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_acl_create_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_acl_end_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_acl_evt_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_acl_start_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_acl_state_set.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_adv_end_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_adv_evt_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_adv_start_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_adv_state_set.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_scan_end_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_scan_evt_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_scan_start_coex_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_ble_scan_state_set.romcall.text)); . = ALIGN(8); KEEP(*.o*(.coex_if_btstate_set.romcall.text)); . = ALIGN(8); KEEP(*.o*(.cos.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_ble_get_dft_data_ext_global_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_ble_get_local_feats.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_ble_get_local_states.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_ble_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_get_local_supp_cmds_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_get_local_supp_feats_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dm_co_reset.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dmac_tx_sched_timer_handler.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_bg_veto_sys_sleep.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_bg_veto_sys_wakeup.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_ble_int_clear_pending_irq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_ble_intc_delete.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_ble_intc_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_coex_exit_bgle_lowpower_process.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_disable_int.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_enable_int.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_gle_int_clear_pending_irq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_gle_intc_delete.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_gle_intc_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_hci_write_to_self.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_interrupt_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_lowpower_feature_get.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_task_create.romcall.text)); . = ALIGN(8); KEEP(*.o*(.dts_task_destroy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.em_offset_calc.romcall.text)); . = ALIGN(8); KEEP(*.o*(.encrypt_clear_sm3_tasks.romcall.text)); . = ALIGN(8); KEEP(*.o*(.encrypt_init_sm3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_finetimer_get_first_target_time.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_finetimer_prog_time.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_get_ee_type.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_get_es_type.romcall.text)); . = ALIGN(8); KEEP(*.o*(.es_get_priority_by_prio_idx.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_prog_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_prog_pop_elt_match_isr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_prog_program_eeq.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_check_offset.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_create_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_get_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_get_offset.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_insert_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_remove_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_resched_check.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_scan_update_param.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_trigger.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_trigger_cancel.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_sched_update_acl_param_with_instant.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_llcp_ack_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_llcp_desc_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_get_unknown_llcp_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_refresh_llcp_instant_ram.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_acl_update_active_tx_octets.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_adv_create.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_adv_get_dur_tbl_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_adv_get_intv_tbl_val.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_adv_set_adv_en.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_ext_adv_config_extheader_syncinfo.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_ext_adv_config_txpld_get_extheader_flag.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_ext_adv_set_adv_en.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_initiate_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_initiate_start.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_scan_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_ble_scan_start.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_create_intv.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_env_get.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_env_set.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_fsm_except_handler.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_rx_error_record.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_task_end.romcall.text)); . = ALIGN(8); KEEP(*.o*(.evt_task_comm_task_start.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_ble_rx_acl_data.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_bt_send_evt_msg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hci_bt_send_evt_msg_without_log.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_ble_tx_buff_free_list.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_ble_tx_buff_node.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_gle_tx_buff_free_list.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_gle_tx_buff_node.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_tx_s_buff_free_list.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_get_tx_s_buff_node.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_tx_desc_get_txpd_free_list.romcall.text)); . = ALIGN(8); KEEP(*.o*(.hw_em_tx_desc_get_txpd_node.romcall.text)); . = ALIGN(8); KEEP(*.o*(.is_sleep_prevent.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_conn_update_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_create_desc.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_enable_encryption_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_feats_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_exception_hci_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_llcp_reject_ext_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_llcp_unknown_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_init_desc_for_pwr_ctrl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_map_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_phy_upd_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_srv_data_length_get_pre_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_srv_data_length_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_start.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_acl_ver_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_adv_clear_assigned_adv_act.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_adv_comm_fsm_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_adv_fsm_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_adv_notify_per_adv_fsm.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_ext_initiate_set_acl_con_info.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_initiate_fsm_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_per_sync_mgr_fsm_get_id.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_ble_scan_fsm_get_sta_tbl.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_chnl_scan_add_map_assessment_module.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_chnl_scan_get_ble_chnl_cls.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_chnl_scan_get_ble_chnl_map.romcall.text)); . = ALIGN(8); KEEP(*.o*(.lm_chnl_scan_remove_map_assessment_module.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print0.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print1.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print2.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_print4.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_wifi_print0.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_wifi_print1.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_wifi_print2.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_wifi_print3.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_event_wifi_print4.romcall.text)); . = ALIGN(8); KEEP(*.o*(.log_oam_status_store.romcall.text)); . = ALIGN(8); KEEP(*.o*(.mem_frees.romcall.text)); . = ALIGN(8); KEEP(*.o*(.mem_news.romcall.text)); . = ALIGN(8); KEEP(*.o*(.memcmp.romcall.text)); . = ALIGN(8); KEEP(*.o*(.memcpy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.memmove.romcall.text)); . = ALIGN(8); KEEP(*.o*(.memset.romcall.text)); . = ALIGN(8); KEEP(*.o*(.new0fun.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_irq_clear.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_irq_disable.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_irq_enable.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_irq_free.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_irq_lock.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_irq_request.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_irq_restore.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_irq_set_priority.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_kfree.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_kmalloc.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_kthread_lock.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_kthread_unlock.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_timer_destroy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_timer_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_timer_mod.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_timer_stop.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_udelay.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_wait_uninterruptible.romcall.text)); . = ALIGN(8); KEEP(*.o*(.osal_wait_wakeup.romcall.text)); . = ALIGN(8); KEEP(*.o*(.panic.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pow.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_convert_tx_pwr_val_to_less_level.romcall.text)); . = ALIGN(8); KEEP(*.o*(.pwr_ctrl_get_ble_tx_estimate_power_value.romcall.text)); . = ALIGN(8); KEEP(*.o*(.rxpd_get_rx_phy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.rxpd_get_rx_phy_info.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sed_ble_set_rx_phy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sed_ble_set_tx_phy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_delay_once_time.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_delay_times.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_flash_num.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_flash_spi_infos.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_sfc_bus_dma_regs_base_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_sfc_bus_regs_base_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_sfc_cmd_databuf_base_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_sfc_cmd_regs_base_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_sfc_end_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_sfc_global_conf_base_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_sfc_start_addr.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_get_unknown_flash_info.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_lock.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_lock_init.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_register_hal_funcs.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_unlock.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sfc_port_unregister_hal_funcs.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sin.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_aes128.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_cmac_reverse.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_rand.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_reverse_octets.romcall.text)); . = ALIGN(8); KEEP(*.o*(.smp_xor.romcall.text)); . = ALIGN(8); KEEP(*.o*(.sqrt.romcall.text)); . = ALIGN(8); KEEP(*.o*(.strlen.romcall.text)); . = ALIGN(8); KEEP(*.o*(.txpd_ble_adv_set_txphy.romcall.text)); . = ALIGN(8); KEEP(*.o*(.uapi_diag_report_sys_msg.romcall.text)); . = ALIGN(8); KEEP(*.o*(.uapi_drv_cipher_trng_get_random.romcall.text));_EVENEER_TEXT = .;. = ALIGN(8);
        . = ALIGN(8);
        __rom_patch_end__ = .;
        ASSERT(((__rom_patch_end__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > ITCM AT > PROGRAM
    __rom_patch_size__ = __rom_patch_end__ - __rom_patch_begin__;
}
SECTIONS
{
    .plat_romdata (0x180000) : ALIGN(8) {
        __rom_data_load__ = LOADADDR(.plat_romdata);
        __rom_data_begin__ = .;
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(SORT(*libsfc_rom.a:)(.data*)) KEEP(SORT(*libhal_sfc_rom.a:)(.data*)) KEEP(SORT(*libpinctrl_rom.a:)(.data*)) KEEP(SORT(*libhal_pinctrl_rom.a:)(.data*)) KEEP(SORT(*libpinctrl_port_ws63_rom.a:)(.data*)) KEEP(SORT(*libwatchdog_rom.a:)(.data*)) KEEP(SORT(*libhal_watchdog_rom.a:)(.data*)) KEEP(SORT(*libwatchdog_port_rom.a:)(.data*)) KEEP(SORT(*libdrv_timer_rom.a:)(.data*)) KEEP(SORT(*libhal_timer_rom.a:)(.data*)) KEEP(SORT(*libtimer_port_rom.a:)(.data*)) KEEP(SORT(*libsystick_rom.a:)(.data*)) KEEP(SORT(*libsystick_port_rom.a:)(.data*)) KEEP(SORT(*liblibboundscheck_rom.a:)(.data*)) KEEP(SORT(*libtcxo_rom.a:)(.data*)) KEEP(SORT(*libhal_tcxo_rom.a:)(.data*)) KEEP(SORT(*libtcxo_port_rom.a:)(.data*)) KEEP(SORT(*libversion_rom.a:)(.data*))
       
        _SVENEER_ROM_RAM_PLAT_D = .;KEEP(*.o*(.data.g_sfc_v150_funcs));KEEP(*.o*(.data.g_hal_pin_ws63_funcs));KEEP(*.o*(.data.g_hal_watchdog_v151_funcs));KEEP(*.o*(.data.g_watchdog_base_addr));KEEP(*.o*(.data.g_watchdog_clock));KEEP(*.o*(.data.g_hal_timer_v150_funcs));KEEP(*.o*(.data.g_timer_clock_value));KEEP(*.o*(.data.g_hal_systick_ws63_funcs));KEEP(*.o*(.data.g_systick_clock));KEEP(*.o*(.data.strNullPointer));KEEP(*.o*(.data.strNullString));KEEP(*.o*(.data.wStrNullString));KEEP(*.o*(.data.g_hal_tcxo_v150_funcs));KEEP(*.o*(.data.g_tcxo_ticks_per_usec));_EVENEER_ROM_RAM_PLAT_D = .;
        ASSERT(((__rom_data_load__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
        ASSERT(((__rom_data_begin__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > DTCM AT > PROGRAM
    .wifi_romdata : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(SORT(*libwifi_driver_rom.a:)(.data*)) KEEP(SORT(*libwifi_alg_radar_sensor_rom.a:)(.data*))
       
        _SVENEER_ROM_RAM_WIFI_D = .;KEEP(*.o*(.data.g_ltf_and_gi));. += 2;KEEP(*.o*(.data.g_11b_per_cnt));KEEP(*.o*(.data.g_dmac_algorithm_main));KEEP(*.o*(.data.g_temp_protect_aggr_max_mpdu));KEEP(*.o*(.data.g_btcoex_aggr_max_mpdu));KEEP(*.o*(.data.g_smooth_phase));KEEP(*.o*(.data.g_rf_switch_cfg));KEEP(*.o*(.data.g_mac_pa_switch));. += 3;KEEP(*.o*(.data.g_hal_cfg_custom));KEEP(*.o*(.data.g_mac_res));KEEP(*.o*(.data.g_buf_size));. += 2;KEEP(*.o*(.data.g_mem_start_addr_cfg));KEEP(*.o*(.data.g_skb_size));_EVENEER_ROM_RAM_WIFI_D = .;
    } > DTCM AT > PROGRAM
    .btc_romdata : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(SORT(*libbgtp_rom.a:)(.data*))
       
        _SVENEER_ROM_RAM_BTC_D = .;KEEP(*.o*(.data.g_pmu_state_handler_tab));KEEP(*.o*(.data.g_min_deep_sleep_slot_time));KEEP(*.o*(.data.g_min_light_sleep_slot_time));KEEP(*.o*(.data.g_sleep_boundary_limit));KEEP(*.o*(.data.g_ble_con_evt_num_before_instant));KEEP(*.o*(.data.g_ble_isr_process_tbl));KEEP(*.o*(.data.g_current_hw_timer));KEEP(*.o*(.data.g_last_update_time));KEEP(*.o*(.data.g_clock_gt_mode_to));KEEP(*.o*(.data.g_company_identifier));. += 2;KEEP(*.o*(.data.g_local_bdaddr));KEEP(*.o*(.data.g_sw_subversion));KEEP(*.o*(.data.g_sw_version));. += 3;KEEP(*.o*(.data.g_btc_crypto_task_stack_depth));KEEP(*.o*(.data.g_btc_crypto_task_stack_priority));KEEP(*.o*(.data.g_dts_hci_config));KEEP(*.o*(.data.g_macro_cfg_flags));KEEP(*.o*(.data.g_btc_sem_evt));. += 1;KEEP(*.o*(.data.g_em_ble_enc_cipher_offset));KEEP(*.o*(.data.g_em_ble_enc_plain_offset));KEEP(*.o*(.data.g_em_ble_fal_end));KEEP(*.o*(.data.g_em_ble_fal_offset));KEEP(*.o*(.data.g_em_ble_int_queue_cnt));. += 1;KEEP(*.o*(.data.g_em_ble_int_queue_end));KEEP(*.o*(.data.g_em_ble_int_queue_offset));KEEP(*.o*(.data.g_em_ble_ral_end));KEEP(*.o*(.data.g_em_ble_ral_offset));KEEP(*.o*(.data.g_em_ble_rx_desc_count));. += 1;KEEP(*.o*(.data.g_em_ble_rx_desc_offset));KEEP(*.o*(.data.g_em_ble_tx_buff_cntl_cnt));. += 1;KEEP(*.o*(.data.g_em_ble_tx_buff_cntl_offset));KEEP(*.o*(.data.g_em_ble_tx_buff_cntl_size));KEEP(*.o*(.data.g_em_ble_tx_buff_data_count));KEEP(*.o*(.data.g_em_ble_tx_buff_data_offset));KEEP(*.o*(.data.g_em_ble_tx_buff_data_size));KEEP(*.o*(.data.g_em_ble_txcntl_desc_cnt));. += 1;KEEP(*.o*(.data.g_em_chnl_scan_offset));KEEP(*.o*(.data.g_em_comm_rx_buff_count));. += 1;KEEP(*.o*(.data.g_em_comm_rx_buff_offset));KEEP(*.o*(.data.g_em_comm_rx_buff_size));KEEP(*.o*(.data.g_em_comm_sed_count_extra));. += 1;KEEP(*.o*(.data.g_em_comm_sed_offset));KEEP(*.o*(.data.g_em_comm_tx_buff_data_s_count));. += 1;KEEP(*.o*(.data.g_em_comm_tx_buff_data_s_offset));KEEP(*.o*(.data.g_em_comm_tx_buff_data_s_size));KEEP(*.o*(.data.g_em_comm_txpd_desc_count_extra));. += 1;KEEP(*.o*(.data.g_em_comm_txpd_desc_offset));KEEP(*.o*(.data.g_em_eeq_normal_cnt));KEEP(*.o*(.data.g_em_eeq_urgent_cnt));KEEP(*.o*(.data.g_em_eeq_urgent_offset));KEEP(*.o*(.data.g_em_freq_tbl_offset));KEEP(*.o*(.data.g_em_gle_int_queue_cnt));. += 1;KEEP(*.o*(.data.g_em_gle_int_queue_end));KEEP(*.o*(.data.g_em_gle_int_queue_offset));KEEP(*.o*(.data.g_em_gle_rx_desc_count));. += 1;KEEP(*.o*(.data.g_em_gle_rx_desc_offset));KEEP(*.o*(.data.g_em_gle_tx_buff_data_count));. += 1;KEEP(*.o*(.data.g_em_gle_tx_buff_data_offset));KEEP(*.o*(.data.g_em_gle_tx_buff_data_size));KEEP(*.o*(.data.g_em_hadm_buff_size));KEEP(*.o*(.data.g_em_hadm_buffer_offset));KEEP(*.o*(.data.g_em_hadm_rf_block_size));KEEP(*.o*(.data.g_em_mdm_buff_count));. += 1;KEEP(*.o*(.data.g_em_mdm_buffer_offset));. += 2;KEEP(*.o*(.data.g_em_size));KEEP(*.o*(.data.g_max_nb_active_link));. += 3;KEEP(*.o*(.data.g_crypto_algo_support));KEEP(*.o*(.data.g_encrypt_ccmctr_setup_tab));KEEP(*.o*(.data.g_encrypt_key_deriv_tab));KEEP(*.o*(.data.g_encrypt_key_exchange_tab));KEEP(*.o*(.data.g_es_cancel_margin));KEEP(*.o*(.data.g_es_process_range));KEEP(*.o*(.data.g_es_start_latency));KEEP(*.o*(.data.g_es_stop_latency));KEEP(*.o*(.data.g_es_timer_prog_delay));. += 3;KEEP(*.o*(.data.g_es_last_finetarget_time));KEEP(*.o*(.data.g_es_last_update_time));KEEP(*.o*(.data.g_duration_max_dft));KEEP(*.o*(.data.g_evt_task_ble_adv_default_aux_channel));. += 1;KEEP(*.o*(.data.g_evt_task_ble_adv_default_aux_offset));KEEP(*.o*(.data.g_gle_isr_process_tbl));KEEP(*.o*(.data.g_ble_delay_send_chnl_report_timer));KEEP(*.o*(.data.g_ble_dft_min_valid_chnl_num));. += 1;KEEP(*.o*(.data.g_ble_dft_req_map_update_timer));KEEP(*.o*(.data.g_adv_cmd_class));. += 1;KEEP(*.o*(.data.g_ble_acl_rssi_avg_cnt));KEEP(*.o*(.data.g_ble_acl_rssi_avg_time));KEEP(*.o*(.data.g_dft_txpower_level));. += 1;KEEP(*.o*(.data.g_gle_acb_rssi_avg_cnt));KEEP(*.o*(.data.g_gle_acb_rssi_avg_time));KEEP(*.o*(.data.g_rf_rx_path_compensation_val));KEEP(*.o*(.data.g_rf_tx_path_compensation_val));_EVENEER_ROM_RAM_BTC_D = .;
        . = ALIGN(4);
    } > DTCM AT > PROGRAM
    .bth_romdata : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(SORT(*libbt_host_rom.a:)(.data*)) KEEP(SORT(*libbg_common_rom.a:)(.data*))
       
        _SVENEER_ROM_RAM_BTH_D = .;; _EVENEER_ROM_RAM_BTH_D = .;
        . = ALIGN(4);
        __rom_data_end__ = .;
        ASSERT(((__rom_data_end__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > DTCM AT > PROGRAM
    __rom_data_size__ = __rom_data_end__ - __rom_data_begin__;
}
SECTIONS
{
    .plat_rombss (0x1803e0) : ALIGN(8) {
        . = ALIGN(8);
        __rom_bss_begin__ = .;
    } > DTCM
    .wifi_rombss (0x180588) : ALIGN(8) {
    } > DTCM
    .btc_rombss (0x182458) : ALIGN(8) {
    } > DTCM
    .bth_rombss (0x182878) : ALIGN(8) {
        . += 0x10;
        . = ALIGN(4);
        __rom_bss_end__ = .;
        ASSERT(((__rom_bss_end__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > DTCM
    __rom_bss_size__ = __rom_bss_end__ - __rom_bss_begin__;
}
SECTIONS
{
    .wifi_pkt_ram (NOLOAD) : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        __wifi_pkt_ram_begin__ = .;
        . += (48 * 1024);
        __wifi_pkt_ram_end__ = .;
        __stack_top__ = __wifi_pkt_ram_end__;
    } > SRAM
}
SECTIONS
{
    .plat_text_tcm : ALIGN(8) {
        FILL(0xFF)
       
        . = ALIGN(8);
        PROVIDE (__int_stack_start = .);
        *(.int_stack);
        __tcm_text_load__ = LOADADDR(.plat_text_tcm);
        __tcm_text_begin__ = .;
       
        ASSERT(((__tcm_text_load__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
        ASSERT(((__tcm_text_begin__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > ITCM AT > PROGRAM
    .btc_text_tcm : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        SORT(*libbgtp.a:bgtp_sleep_sw_ram**.o*)(.text* .rodata*) SORT(*libbgtp.a:)(.text.bt_thread_handle) SORT(*libbgtp.a:)(.text.bt_crypto_thread_handle) SORT(*libbgtp.a:dts_interrupt_itcm**.o*)(.text* .rodata*) SORT(*libbgtp.a:dts_interrupt_dev_only**.o*)(.text* .rodata*) SORT(*libbgtp.a:evt_sched_mgr_ram**.o*)(.text* .rodata*) SORT(*libbgtp.a:evt_sched_ram**.o*)(.text* .rodata*) SORT(*libbgtp.a:evt_sched_util_ram**.o*)(.text* .rodata*) SORT(*libbgtp.a:evt_prog_ram**.o*)(.text* .rodata*)
    } > ITCM AT > PROGRAM
    .wifi_text_tcm : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
       
       
       
    } > ITCM AT > PROGRAM
    .patch_on_tcm : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(*(.patch.text.on.itcm .patch.rodata.on.itcm))
        . = ALIGN(4);
        __tcm_text_end__ = .;
        ASSERT(((__tcm_text_end__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > ITCM AT > PROGRAM
    __tcm_text_size__ = __tcm_text_end__ - __tcm_text_begin__;
    .plat_data_tcm : ALIGN(8) {
        FILL(0xFF)
       
        . = ALIGN(8);
        __tcm_data_load__ = LOADADDR(.plat_data_tcm);
        __tcm_data_begin__ = .;
       
        ASSERT(((__tcm_data_load__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
        ASSERT(((__tcm_data_begin__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > DTCM AT > PROGRAM
    .btc_data_tcm : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        SORT(*libbgtp.a:bgtp_sleep_sw_ram**.o*)(.data*) SORT(*libbgtp.a:)(.text.bt_thread_handle) SORT(*libbgtp.a:)(.text.bt_crypto_thread_handle) SORT(*libbgtp.a:dts_interrupt_itcm**.o*)(.data*) SORT(*libbgtp.a:dts_interrupt_dev_only**.o*)(.data*) SORT(*libbgtp.a:evt_sched_mgr_ram**.o*)(.data*) SORT(*libbgtp.a:evt_sched_ram**.o*)(.data*) SORT(*libbgtp.a:evt_sched_util_ram**.o*)(.data*) SORT(*libbgtp.a:evt_prog_ram**.o*)(.data*)
    } > DTCM AT > PROGRAM
    .wifi_data_tcm : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
       
        SORT(*libwifi_driver_tcm.a:)(.data*)
        SORT(*libc.a:)(.data*) SORT(*libuart.a:)(.data*) SORT(*libhal_reboot.a:)(.data*) SORT(*libhal_uart_synopsys.a:)(.data*) SORT(*libliteos_port.a:)(.data*) SORT(*libalgorithm.a:)(.data*) SORT(*libchip_ws63.a:)(.data*) SORT(*libmbedtls.a:)(.data*) SORT(*libuart_port_synopsys.a:)(.data*) SORT(*libat.a:)(.data*) SORT(*libcpu_utils.a:)(.data*) SORT(*libgcc.a:)(.data*) SORT(*libdfx_diag.a:)(.data*) SORT(*libdfx_exception.a:)(.data*) SORT(*libdfx_log.a:)(.data*) SORT(*libdfx_panic.a:)(.data*) SORT(*libdfx_port_ws63.a:)(.data*) SORT(*libdfx_preserve.a:)(.data*) SORT(*libbase.a:)(.data*) SORT(*libws63_liteos_app.a:)(.data*) SORT(*libnon_os.a:)(.data*) SORT(*libproc.a:)(.data*) SORT(*libcompat.a:)(.data*) SORT(*libcmsis.a:)(.data*) SORT(*libdisk.a:)(.data*) SORT(*libmulti_partition.a:)(.data*) SORT(*libinit.a:)(.data*) SORT(*libm.a:)(.data*) SORT(*libfat.a:)(.data*) SORT(*libvfs.a:)(.data*) SORT(*libsec.a:)(.data*) SORT(*libcsysdeps.a:)(.data*) SORT(*libposix.a:)(.data*) SORT(*libinterrupt.a:)(.data*) SORT(*libtargets.a:)(.data*) SORT(*libdriverbase.a:)(.data*) SORT(*liblinux.a:)(.data*) SORT(*libbcache.a:)(.data*) SORT(*libramfs.a:)(.data*) SORT(*libconsole.a:)(.data*) SORT(*libriscv.a:)(.data*) SORT(*libosal.a:)(.data*) SORT(*libosal_adapt.a:)(.data*) SORT(*liblos_iperf.a:)(.data*) SORT(*libhal_uart.a:)(.data*) SORT(*libhal_tsensor.a:)(.data*) SORT(*libuart_port.a:)(.data*) SORT(*libhal_pmp.a:)(.data*) SORT(*libpmp_cfg_ws63.a:)(.data*) SORT(*libpmp_port.a:)(.data*) SORT(*libi2c_porting.a:)(.data*) SORT(*libsfc_flash_config_ws63.a:)(.data*) SORT(*libtsensor.a:)(.data*) SORT(*libtsensor_port.a:)(.data*) SORT(*libefuse.a:)(.data*) SORT(*libefuse_port.a:)(.data*) SORT(*libhal_efuse_v151.a:)(.data*) SORT(*libsoc_port.a:)(.data*) SORT(*libplat_patch.a:)(.data*) SORT(*libnv_ws63.a:)(.data*) SORT(*libupdate_common_ws63.a:)(.data*) SORT(*libapp_init.a:)(.data*) SORT(*libplt_at.a:)(.data*) SORT(*libdiag_dfx_cmd.a:)(.data*) SORT(*libnv.a:)(.data*) SORT(*libupdate_common.a:)(.data*) SORT(*libmips.a:)(.data*) SORT(*libcpup.a:)(.data*) SORT(*libhal_gpio_v150.a:)(.data*) SORT(*libgpio_port.a:)(.data*) SORT(*libboundscheck_sscanf_s.a:)(.data*) SORT(*libwifi_driver_hmac.a:)(.data*) SORT(*libwifi_driver_dmac.a:)(.data*) SORT(*libwpa_supplicant.a:)(.data*) SORT(*libwifi_service.a:)(.data*) SORT(*libwifi_at.a:)(.data*) SORT(*liblwip.a:)(.data*) SORT(*liblwip_tcm.a:)(.data*) SORT(*libwifi_promisc.a:)(.data*) SORT(*libwifi_auto_adjust_freq.a:)(.data*) SORT(*libwifi_csa.a:)(.data*) SORT(*libwifi_alg_temp_protect.a:)(.data*) SORT(*libwifi_alg_txbf.a:)(.data*) SORT(*libwifi_alg_anti_interference.a:)(.data*) SORT(*libwifi_alg_edca_opt.a:)(.data*) SORT(*libwifi_roam.a:)(.data*) SORT(*libwifi_alg_cca_opt.a:)(.data*) SORT(*libwifi_tx_amsdu.a:)(.data*) SORT(*libwifi_sdp.a:)(.data*) SORT(*libwifi_blacklist.a:)(.data*) SORT(*libwifi_sr.a:)(.data*) SORT(*libwifi_frag.a:)(.data*) SORT(*libwifi_mbo.a:)(.data*) SORT(*libwifi_bsrp_nfrp.a:)(.data*) SORT(*libwifi_slp.a:)(.data*) SORT(*libwifi_radar_sensor.a:)(.data*) SORT(*libwifi_apf.a:)(.data*) SORT(*libwifi_repeater.a:)(.data*) SORT(*libwifi_csi.a:)(.data*) SORT(*libwifi_wapi.a:)(.data*) SORT(*libwifi_wps.a:)(.data*) SORT(*libwifi_psd.a:)(.data*) SORT(*libwifi_latency.a:)(.data*) SORT(*libwifi_roam.a:)(.data*) SORT(*libwifi_uapsd_sta.a:)(.data*) SORT(*libwifi_ant_sel.a:)(.data*) SORT(*libwifi_twt.a:)(.data*) SORT(*libwifi_11k.a:)(.data*) SORT(*libwifi_11v.a:)(.data*) SORT(*libwifi_11r.a:)(.data*) SORT(*libwifi_btcoex.a:)(.data*) SORT(*libwifi_uapsd_ap.a:)(.data*) SORT(*libwifi_dnb.a:)(.data*) SORT(*libwifi_m2u.a:)(.data*) SORT(*libradar_sensing.a:)(.data*) SORT(*libradar_at.a:)(.data*) SORT(*libradar_ai.a:)(.data*) SORT(*libbase.a:*los_hwi.c.obj)(.data*) SORT(*libbase.a:*los_tick.c.obj)(.data*) SORT(*libbase.a:*los_sched.c.obj)(.data*) SORT(*libbase.a:*los_exc.c.obj)(.data*) SORT(*libbase.a:*los_sortlink.c.obj)(.data*) SORT(*libinterrupt.a:*riscv_himideerv200_plic.c.obj)(.data*) SORT(*libriscv.a:*exception.S.obj)(.data*) SORT(*libriscv.a:*trap.S.obj)(.data*) SORT(*libriscv.a:*dispatch.S.obj)(.data*) SORT(*libriscv.a:*cache.c.obj)(.data*) SORT(*libriscv.a:*task.c.obj)(.data*) SORT(*libcsysdeps.a:*memcpy.S.obj)(.data*) SORT(*libcsysdeps.a:*memset.S.obj)(.data*) SORT(*libosal.a:*osal_spinlock.c.obj)(.data*) SORT(*libosal.a:*osal_interrupt.c.obj)(.data*) SORT(*libosal.a:*osal_addr.c.obj)(.data*) SORT(*libosal_adapt.a:*osal_adapt_timer.c.obj)(.data*) SORT(*libosal_adapt.a:*osal_adapt_task.c.obj)(.data*) SORT(*libpm_port_ws63.a:)(.data*) SORT(*libsfc_port_ws63.a:)(.data*) SORT(*libtest_sfc.a:)(.data*)
        . = ALIGN(4);
        __tcm_data_end__ = .;
        ASSERT(((__tcm_data_end__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > DTCM AT > PROGRAM
    __tcm_data_size__ = __tcm_data_end__ - __tcm_data_begin__;
    .plat_bss_tcm : ALIGN(8) {
        __tcm_bss_begin__ = .;
       
    } > DTCM
    .btc_bss_tcm : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        SORT(*libbgtp.a:bgtp_sleep_sw_ram**.o*)(.bss*) SORT(*libbgtp.a:)(.text.bt_thread_handle) SORT(*libbgtp.a:)(.text.bt_crypto_thread_handle) SORT(*libbgtp.a:dts_interrupt_itcm**.o*)(.bss*) SORT(*libbgtp.a:dts_interrupt_dev_only**.o*)(.bss*) SORT(*libbgtp.a:evt_sched_mgr_ram**.o*)(.bss*) SORT(*libbgtp.a:evt_sched_ram**.o*)(.bss*) SORT(*libbgtp.a:evt_sched_util_ram**.o*)(.bss*) SORT(*libbgtp.a:evt_prog_ram**.o*)(.bss*)
    } > DTCM
    .wifi_bss_tcm : ALIGN(8) {
       
        SORT(*libwifi_driver_tcm.a:)(.bss*)
        SORT(*libwifi_driver_dmac.a:)(.bss.g_dmac_frw_pool_index) SORT(*libwifi_driver_dmac.a:)(.bss.g_dmac_frw_pool)
        . = ALIGN(4);
        __tcm_bss_end__ = .;
    } > DTCM
    __tcm_bss_size__ = __tcm_bss_end__ - __tcm_bss_begin__;
}
SECTIONS
{
    .plat_text_sram : ALIGN(8) {
        FILL(0xFF)
       
        . = ALIGN(8);
        __sram_text_load__ = LOADADDR(.plat_text_sram);
        __sram_text_begin__ = .;
        SORT(*libbase.a:*los_task.c.obj)(.text.LOS_ListDelete) SORT(*libbase.a:*los_task.c.obj)(.text.OsPreemptable) SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskReSched) SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskSuspend) SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskAdd2TimerList) SORT(*libbase.a:*los_task.c.obj)(.text.OsTimerListDelete) SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskScan) SORT(*libbase.a:*los_task.c.obj)(.text.LOS_CurTaskIDGet) SORT(*libbase.a:*los_task.c.obj)(.text.OsCurTaskNameGet) SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskSwitchCheck) SORT(*libbase.a:*los_task.c.obj)(.text.LOS_IdleHandlerHookReg) SORT(*libbase.a:*los_task.c.obj)(.text.OsIdleHandler) SORT(*libbase.a:*los_task.c.obj)(.text.OsIdleTask) SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskProcSignal) SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskEntry) SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskResume) SORT(*libbase.a:*los_task.c.obj)(.text.OsSysTaskSuspend) SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskWait) SORT(*libbase.a:*los_task.c.obj)(.text.OsTaskWake) SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskYield) SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskDelay) SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskLock) SORT(*libbase.a:*los_task.c.obj)(.text.LOS_TaskUnlock) SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemInfoGet) SORT(*libbase.a:*los_memory.c.obj)(.text.OsGetRealPtr) SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemMergeNode) SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemListDelete.isra.7) SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemSplitNode) SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemAllocWithCheck) SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemFreeNode) SORT(*libbase.a:*los_memory.c.obj)(.text.LOS_MemAlloc) SORT(*libbase.a:*los_memory.c.obj)(.text.LOS_MemAllocAlign) SORT(*libbase.a:*los_memory.c.obj)(.text.OsMemFree) SORT(*libbase.a:*llos_memory.c.obj)(.text.LOS_MemFree) SORT(*libbase.a:*los_memory.c.obj)(.text.LOS_MemRealloc) SORT(*libbase.a:*los_memory.c.obj)(.text.LOS_MemInfoGet) SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrDelete) SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrTask) SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrStart) SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrScan) SORT(*libbase.a:*los_swtmr.c.obj)(.text.LOS_SwtmrCreate) SORT(*libbase.a:*los_swtmr.c.obj)(.text.OsSwtmrIdVerify) SORT(*libbase.a:*los_swtmr.c.obj)(.text.LOS_SwtmrStart) SORT(*libbase.a:*los_swtmr.c.obj)(.text.LOS_SwtmrStop) SORT(*libbase.a:*los_swtmr.c.obj)(.text.LOS_SwtmrDelete)
        SORT(*libbase.a:*los_hwi.c.obj)(*.text* *.rodata*) SORT(*libbase.a:*los_tick.c.obj)(*.text* *.rodata*) SORT(*libbase.a:*los_sched.c.obj)(*.text* *.rodata*) SORT(*libbase.a:*los_exc.c.obj)(*.text* *.rodata*) SORT(*libbase.a:*los_sortlink.c.obj)(*.text* *.rodata*) SORT(*libinterrupt.a:*riscv_himideerv200_plic.c.obj)(*.text* *.rodata*) SORT(*libriscv.a:*exception.S.obj)(*.text* *.rodata*) SORT(*libriscv.a:*trap.S.obj)(*.text* *.rodata*) SORT(*libriscv.a:*dispatch.S.obj)(*.text* *.rodata*) SORT(*libriscv.a:*cache.c.obj)(*.text* *.rodata*) SORT(*libriscv.a:*task.c.obj)(*.text* *.rodata*) SORT(*libcsysdeps.a:*memcpy.S.obj)(*.text* *.rodata*) SORT(*libcsysdeps.a:*memset.S.obj)(*.text* *.rodata*) SORT(*libosal.a:*osal_spinlock.c.obj)(*.text* *.rodata*) SORT(*libosal.a:*osal_interrupt.c.obj)(*.text* *.rodata*) SORT(*libosal.a:*osal_addr.c.obj)(*.text* *.rodata*) SORT(*libosal_adapt.a:*osal_adapt_timer.c.obj)(*.text* *.rodata*) SORT(*libosal_adapt.a:*osal_adapt_task.c.obj)(*.text* *.rodata*) SORT(*libpm_port_ws63.a:)(*.text* *.rodata*) SORT(*libsfc_port_ws63.a:)(*.text* *.rodata*) SORT(*libtest_sfc.a:)(*.text* *.rodata*)
        ASSERT(((__sram_text_load__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
        ASSERT(((__sram_text_begin__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > SRAM AT > PROGRAM
    .wifi_text_sram : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
       
    } > SRAM AT > PROGRAM
    .btc_text_sram : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
       
    } > SRAM AT > PROGRAM
    .bth_text_sram : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
       
    } > SRAM AT > PROGRAM
    .patch_on_sram : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
        KEEP(*(.patch.text.on.sram .patch.rodata.on.sram))
        . = ALIGN(4);
        *(.text.__riscv_*)
        . = ALIGN(4);
        __sram_text_end__ = .;
        ASSERT(((__sram_text_end__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > SRAM AT > PROGRAM
    __sram_text_size__ = __sram_text_end__ - __sram_text_begin__;
}
SECTIONS
{
    .wifi_text_cali : ALIGN(8) {
       
        __text_begin__ = .;
        FILL(0xFF)
       
    } > PROGRAM
    .btc_text_cali : ALIGN(8) {
        FILL(0xFF)
       
    } > PROGRAM
    .bth_text_cali : ALIGN(8) {
        FILL(0xFF)
       
    } > PROGRAM
    .plat_text_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libc.a:)(*.text* *.rodata*) SORT(*libuart.a:)(*.text* *.rodata*) SORT(*libhal_reboot.a:)(*.text* *.rodata*) SORT(*libhal_uart_synopsys.a:)(*.text* *.rodata*) SORT(*libliteos_port.a:)(*.text* *.rodata*) SORT(*libalgorithm.a:)(*.text* *.rodata*) SORT(*libchip_ws63.a:)(*.text* *.rodata*) SORT(*libmbedtls.a:)(*.text* *.rodata*) SORT(*libuart_port_synopsys.a:)(*.text* *.rodata*) SORT(*libat.a:)(*.text* *.rodata*) SORT(*libcpu_utils.a:)(*.text* *.rodata*) SORT(*libgcc.a:)(*.text* *.rodata*) SORT(*libdfx_diag.a:)(*.text* *.rodata*) SORT(*libdfx_exception.a:)(*.text* *.rodata*) SORT(*libdfx_log.a:)(*.text* *.rodata*) SORT(*libdfx_panic.a:)(*.text* *.rodata*) SORT(*libdfx_port_ws63.a:)(*.text* *.rodata*) SORT(*libdfx_preserve.a:)(*.text* *.rodata*) SORT(*libbase.a:)(*.text* *.rodata*) SORT(*libws63_liteos_app.a:)(*.text* *.rodata*) SORT(*libnon_os.a:)(*.text* *.rodata*) SORT(*libproc.a:)(*.text* *.rodata*) SORT(*libcompat.a:)(*.text* *.rodata*) SORT(*libcmsis.a:)(*.text* *.rodata*) SORT(*libdisk.a:)(*.text* *.rodata*) SORT(*libmulti_partition.a:)(*.text* *.rodata*) SORT(*libinit.a:)(*.text* *.rodata*) SORT(*libm.a:)(*.text* *.rodata*) SORT(*libfat.a:)(*.text* *.rodata*) SORT(*libvfs.a:)(*.text* *.rodata*) SORT(*libsec.a:)(*.text* *.rodata*) SORT(*libcsysdeps.a:)(*.text* *.rodata*) SORT(*libposix.a:)(*.text* *.rodata*) SORT(*libinterrupt.a:)(*.text* *.rodata*) SORT(*libtargets.a:)(*.text* *.rodata*) SORT(*libdriverbase.a:)(*.text* *.rodata*) SORT(*liblinux.a:)(*.text* *.rodata*) SORT(*libbcache.a:)(*.text* *.rodata*) SORT(*libramfs.a:)(*.text* *.rodata*) SORT(*libconsole.a:)(*.text* *.rodata*) SORT(*libriscv.a:)(*.text* *.rodata*) SORT(*libosal.a:)(*.text* *.rodata*) SORT(*libosal_adapt.a:)(*.text* *.rodata*) SORT(*liblos_iperf.a:)(*.text* *.rodata*) SORT(*libhal_uart.a:)(*.text* *.rodata*) SORT(*libhal_tsensor.a:)(*.text* *.rodata*) SORT(*libuart_port.a:)(*.text* *.rodata*) SORT(*libhal_pmp.a:)(*.text* *.rodata*) SORT(*libpmp_cfg_ws63.a:)(*.text* *.rodata*) SORT(*libpmp_port.a:)(*.text* *.rodata*) SORT(*libi2c_porting.a:)(*.text* *.rodata*) SORT(*libsfc_flash_config_ws63.a:)(*.text* *.rodata*) SORT(*libtsensor.a:)(*.text* *.rodata*) SORT(*libtsensor_port.a:)(*.text* *.rodata*) SORT(*libefuse.a:)(*.text* *.rodata*) SORT(*libefuse_port.a:)(*.text* *.rodata*) SORT(*libhal_efuse_v151.a:)(*.text* *.rodata*) SORT(*libsoc_port.a:)(*.text* *.rodata*) SORT(*libplat_patch.a:)(*.text* *.rodata*) SORT(*libnv_ws63.a:)(*.text* *.rodata*) SORT(*libupdate_common_ws63.a:)(*.text* *.rodata*) SORT(*libapp_init.a:)(*.text* *.rodata*) SORT(*libplt_at.a:)(*.text* *.rodata*) SORT(*libdiag_dfx_cmd.a:)(*.text* *.rodata*) SORT(*libnv.a:)(*.text* *.rodata*) SORT(*libupdate_common.a:)(*.text* *.rodata*) SORT(*libmips.a:)(*.text* *.rodata*) SORT(*libcpup.a:)(*.text* *.rodata*) SORT(*libhal_gpio_v150.a:)(*.text* *.rodata*) SORT(*libgpio_port.a:)(*.text* *.rodata*) SORT(*libboundscheck_sscanf_s.a:)(*.text* *.rodata*)
    } > PROGRAM
    .wifi_text_flash : ALIGN(8) {
        FILL(0xFF)
        KEEP(*(.wifi.tcm.text .wifi.tcm.rodata))
        SORT(*libwifi_driver_tcm.a:)(.text* .rodata*)
        SORT(*liblwip_tcm.a:)(.text.get_socket) SORT(*liblwip_tcm.a:)(.text.tryget_socket) SORT(*liblwip_tcm.a:)(.text.tryget_socket_unconn) SORT(*liblwip_tcm.a:)(.text.sock_inc_used) SORT(*liblwip_tcm.a:)(.text.tryget_socket_unconn_nouse) SORT(*liblwip_tcm.a:)(.text.tryget_socket_unconn_locked) SORT(*liblwip_tcm.a:)(.text.done_socket) SORT(*liblwip_tcm.a:)(.text.lwip_recvfrom) SORT(*liblwip_tcm.a:)(.text.lwip_recv_tcp) SORT(*liblwip_tcm.a:)(.text.event_callback) SORT(*liblwip_tcm.a:)(.text.lwip_select) SORT(*liblwip_tcm.a:)(.text.select_check_waiters) SORT(*liblwip_tcm.a:)(.text.netconn_apimsg) SORT(*liblwip_tcm.a:)(.text.netconn_recv_data) SORT(*liblwip_tcm.a:)(.text.netconn_tcp_recvd_msg) SORT(*liblwip_tcm.a:)(.text.netconn_tcp_recvd) SORT(*liblwip_tcm.a:)(.text.netconn_recv_data_tcp) SORT(*liblwip_tcm.a:)(.text.netconn_recv_tcp_pbuf_flags) SORT(*liblwip_tcm.a:)(.text.netconn_recv_udp_raw_netbuf_flags) KEEP(SORT(*liblwip_tcm.a:)(.text.netconn_recv)) KEEP(SORT(*liblwip_tcm.a:)(.text.netconn_sendto)) SORT(*liblwip_tcm.a:)(.text.netconn_send) SORT(*liblwip_tcm.a:)(.text.netconn_write_partly) SORT(*liblwip_tcm.a:)(.text.netconn_write_vectors_partly) SORT(*liblwip_tcm.a:)(.text.lwip_netconn_do_send) SORT(*liblwip_tcm.a:)(.text.lwip_netconn_do_write) SORT(*liblwip_tcm.a:)(.text.netbuf_alloc) SORT(*liblwip_tcm.a:)(.text.netbuf_free) SORT(*liblwip_tcm.a:)(.text.tcpip_thread) KEEP(SORT(*liblwip_tcm.a:)(.text.tcpip_thread_handle_msg)) SORT(*liblwip_tcm.a:)(.text.tcpip_inpkt) SORT(*liblwip_tcm.a:)(.text.tcpip_input) SORT(*liblwip_tcm.a:)(.text.tcpip_callback) SORT(*liblwip_tcm.a:)(.text.tcpip_try_callback) SORT(*liblwip_tcm.a:)(.text.tcpip_send_msg_wait_sem) SORT(*liblwip_tcm.a:)(.text.inet_cksum_pseudo_base) SORT(*liblwip_tcm.a:)(.text.inet_chksum_pseudo) SORT(*liblwip_tcm.a:)(.text.etharp_output_to_arp_index) SORT(*liblwip_tcm.a:)(.text.etharp_output) SORT(*liblwip_tcm.a:)(.text.ip4_addr_isbroadcast_u32) SORT(*liblwip_tcm.a:)(.text.ip_input) SORT(*liblwip_tcm.a:)(.text.ip4_route) SORT(*liblwip_tcm.a:)(.text.ip4_input) SORT(*liblwip_tcm.a:)(.text.ip4_output_if) SORT(*liblwip_tcm.a:)(.text.ip4_output_if_opt) SORT(*liblwip_tcm.a:)(.text.ip4_output_if_src) SORT(*liblwip_tcm.a:)(.text.ip4_output_if_opt_src) SORT(*liblwip_tcm.a:)(.text.pbuf_alloc) SORT(*liblwip_tcm.a:)(.text.pbuf_add_header_impl) SORT(*liblwip_tcm.a:)(.text.pbuf_add_header) SORT(*liblwip_tcm.a:)(.text.pbuf_remove_header) SORT(*liblwip_tcm.a:)(.text.pbuf_header_impl) SORT(*liblwip_tcm.a:)(.text.pbuf_header) SORT(*liblwip_tcm.a:)(.text.pbuf_free) KEEP(SORT(*liblwip_tcm.a:)(.text.pbuf_alloced_custom)) SORT(*liblwip_tcm.a:)(.text.pbuf_init_alloced_pbuf) SORT(*liblwip_tcm.a:)(.text.pbuf_alloc_from_ptk_buf) SORT(*liblwip_tcm.a:)(.text.ethernet_input) SORT(*liblwip_tcm.a:)(.text.ethernet_output) SORT(*liblwip_tcm.a:)(.text.sys_mutex_lock) SORT(*liblwip_tcm.a:)(.text.sys_mutex_unlock) SORT(*liblwip_tcm.a:)(.text.sys_sem_signal) SORT(*liblwip_tcm.a:)(.text.sys_arch_sem_wait) SORT(*liblwip_tcm.a:)(.text.sys_mbox_post) SORT(*liblwip_tcm.a:)(.text.sys_mbox_trypost) SORT(*liblwip_tcm.a:)(.text.sys_arch_mbox_fetch_ext) SORT(*liblwip_tcm.a:)(.text.driverif_output) SORT(*liblwip_tcm.a:)(.text.driverif_input) SORT(*liblwip_tcm.a:)(.text.ip4_frag) SORT(*liblwip_tcm.a:)(.text.ip4_reass) SORT(*liblwip_tcm.a:)(.text.ip_addr_cmp) SORT(*liblwip_tcm.a:)(.text.ip_addr_copy_fun) SORT(*liblwip_tcm.a:)(.text.ip_addr_isany) SORT(*liblwip_tcm.a:)(.text.mem_free) SORT(*liblwip_tcm.a:)(.text.mem_malloc) SORT(*liblwip_tcm.a:)(.text.memp_free) SORT(*liblwip_tcm.a:)(.text.memp_malloc) SORT(*liblwip_tcm.a:)(.text.mem_trim) SORT(*liblwip_tcm.a:)(.text.netif_default) SORT(*liblwip_tcm.a:)(.text.netif_find) SORT(*liblwip_tcm.a:)(.text.netif_find_by_ifindex) SORT(*liblwip_tcm.a:)(.text.netif_find_by_ipaddr) SORT(*liblwip_tcm.a:)(.text.netif_find_dst_ipaddr) SORT(*liblwip_tcm.a:)(.text.netif_ipaddr_isbrdcast) SORT(*liblwip_tcm.a:)(.text.netif_loop_output) SORT(*liblwip_tcm.a:)(.text.netif_loop_output_ipv4) SORT(*liblwip_tcm.a:)(.text.raw_remove) SORT(*liblwip_tcm.a:)(.text.raw_send) SORT(*liblwip_tcm.a:)(.text.raw_sendto) SORT(*liblwip_tcm.a:)(.text.tcp_abort) SORT(*liblwip_tcm.a:)(.text.tcp_accept) SORT(*liblwip_tcm.a:)(.text.tcp_active_pcbs) SORT(*liblwip_tcm.a:)(.text.tcp_arg) SORT(*liblwip_tcm.a:)(.text.tcp_backlog_accepted) SORT(*liblwip_tcm.a:)(.text.tcp_backlog_delayed) SORT(*liblwip_tcm.a:)(.text.tcp_listen_with_backlog_and_err) SORT(*liblwip_tcm.a:)(.text.tcp_new_ip_type) SORT(*liblwip_tcm.a:)(.text.tcp_recv) SORT(*liblwip_tcm.a:)(.text.tcp_shutdown) SORT(*liblwip_tcm.a:)(.text.tcp_sndbuf_init) SORT(*liblwip_tcm.a:)(.text.tcp_write) SORT(*liblwip_tcm.a:)(.text.tcp_input) SORT(*liblwip_tcm.a:)(.text.tcp_output)
        SORT(*libwifi_driver_hmac.a:)(.text* .rodata*) SORT(*libwifi_driver_dmac.a:)(.text* .rodata*) SORT(*libwpa_supplicant.a:)(.text* .rodata*) SORT(*libwifi_service.a:)(.text* .rodata*) SORT(*libwifi_at.a:)(.text* .rodata*) SORT(*liblwip.a:)(.text* .rodata*) SORT(*liblwip_tcm.a:)(.text* .rodata*) SORT(*libwifi_promisc.a:)(.text* .rodata*) SORT(*libwifi_auto_adjust_freq.a:)(.text* .rodata*) SORT(*libwifi_csa.a:)(.text* .rodata*) SORT(*libwifi_alg_temp_protect.a:)(.text* .rodata*) SORT(*libwifi_alg_txbf.a:)(.text* .rodata*) SORT(*libwifi_alg_anti_interference.a:)(.text* .rodata*) SORT(*libwifi_alg_edca_opt.a:)(.text* .rodata*) SORT(*libwifi_roam.a:)(.text* .rodata*) SORT(*libwifi_alg_cca_opt.a:)(.text* .rodata*) SORT(*libwifi_tx_amsdu.a:)(.text* .rodata*) SORT(*libwifi_sdp.a:)(.text* .rodata*) SORT(*libwifi_blacklist.a:)(.text* .rodata*) SORT(*libwifi_sr.a:)(.text* .rodata*) SORT(*libwifi_frag.a:)(.text* .rodata*) SORT(*libwifi_mbo.a:)(.text* .rodata*) SORT(*libwifi_bsrp_nfrp.a:)(.text* .rodata*) SORT(*libwifi_slp.a:)(.text* .rodata*) SORT(*libwifi_radar_sensor.a:)(.text* .rodata*) SORT(*libwifi_apf.a:)(.text* .rodata*) SORT(*libwifi_repeater.a:)(.text* .rodata*) SORT(*libwifi_csi.a:)(.text* .rodata*) SORT(*libwifi_wapi.a:)(.text* .rodata*) SORT(*libwifi_wps.a:)(.text* .rodata*) SORT(*libwifi_psd.a:)(.text* .rodata*) SORT(*libwifi_latency.a:)(.text* .rodata*) SORT(*libwifi_roam.a:)(.text* .rodata*) SORT(*libwifi_uapsd_sta.a:)(.text* .rodata*) SORT(*libwifi_ant_sel.a:)(.text* .rodata*) SORT(*libwifi_twt.a:)(.text* .rodata*) SORT(*libwifi_11k.a:)(.text* .rodata*) SORT(*libwifi_11v.a:)(.text* .rodata*) SORT(*libwifi_11r.a:)(.text* .rodata*) SORT(*libwifi_btcoex.a:)(.text* .rodata*) SORT(*libwifi_uapsd_ap.a:)(.text* .rodata*) SORT(*libwifi_dnb.a:)(.text* .rodata*) SORT(*libwifi_m2u.a:)(.text* .rodata*)
    } > PROGRAM
    .patch_on_flash : ALIGN(8) {
        FILL(0xFF)
        KEEP(*(.patch.text.on.flash .patch.rodata.on.flash))
    } > PROGRAM
    .btc_text_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libbgtp.a:)(.text* .rodata*)
    } > PROGRAM
    .bth_text_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libbg_common.a:)(.text* .rodata*) SORT(*libbt_host.a:)(.text* .rodata*) SORT(*libbth_gle.a:)(.text* .rodata*) SORT(*libbth_sdk.a:)(.text* .rodata*) SORT(*libbt_at.a:)(.text* .rodata*) SORT(*libbt_app.a:)(.text* .rodata*) SORT(*libsamples.a:)(.text* .rodata*) SORT(*libtestsuite.a:)(.text* .rodata*)
    } > PROGRAM
    .radar_text_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libradar_sensing.a:)(.text* .rodata*) SORT(*libradar_at.a:)(.text* .rodata*)
    } > PROGRAM
    .radar_ai_text_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libradar_ai.a:)(.text* .rodata*)
    } > PROGRAM
    .text : ALIGN(8) {
        FILL(0xFF)
        *(.text)
        *(.text*)
        *(.itcm.text)
        *(.rodata)
        *(.rodata*)
        *(.srodata)
        *(.srodata*)
        KEEP (*(.flashtext))
        KEEP (*(.flashtext*))
        . = ALIGN(4);
        __text_end__ = .;
    } > PROGRAM
    .plat_data_sram : ALIGN(8) {
        FILL(0xFF)
        __data_load__ = LOADADDR(.plat_data_sram);
        __data_begin__ = .;
        SORT(*libbase.a:*los_hwi.c.obj)(.data*) SORT(*libbase.a:*los_tick.c.obj)(.data*) SORT(*libbase.a:*los_sched.c.obj)(.data*) SORT(*libbase.a:*los_exc.c.obj)(.data*) SORT(*libbase.a:*los_sortlink.c.obj)(.data*) SORT(*libinterrupt.a:*riscv_himideerv200_plic.c.obj)(.data*) SORT(*libriscv.a:*exception.S.obj)(.data*) SORT(*libriscv.a:*trap.S.obj)(.data*) SORT(*libriscv.a:*dispatch.S.obj)(.data*) SORT(*libriscv.a:*cache.c.obj)(.data*) SORT(*libriscv.a:*task.c.obj)(.data*) SORT(*libcsysdeps.a:*memcpy.S.obj)(.data*) SORT(*libcsysdeps.a:*memset.S.obj)(.data*) SORT(*libosal.a:*osal_spinlock.c.obj)(.data*) SORT(*libosal.a:*osal_interrupt.c.obj)(.data*) SORT(*libosal.a:*osal_addr.c.obj)(.data*) SORT(*libosal_adapt.a:*osal_adapt_timer.c.obj)(.data*) SORT(*libosal_adapt.a:*osal_adapt_task.c.obj)(.data*) SORT(*libpm_port_ws63.a:)(.data*) SORT(*libsfc_port_ws63.a:)(.data*) SORT(*libtest_sfc.a:)(.data*)
        ASSERT(((__data_load__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
        ASSERT(((__data_begin__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > SRAM AT > PROGRAM
    .wifi_data_sram : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
       
    } > SRAM AT > PROGRAM
    .btc_data_sram : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
       
    } > SRAM AT > PROGRAM
    .bth_data_sram : ALIGN(8) {
        FILL(0xFF)
        . = ALIGN(8);
       
    } > SRAM AT > PROGRAM
    .wifi_data_cali : ALIGN(8) {
        FILL(0xFF)
       
    } > SRAM AT > PROGRAM
    .btc_data_cali : ALIGN(8) {
        FILL(0xFF)
       
    } > SRAM AT > PROGRAM
    .bth_data_cali : ALIGN(8) {
        FILL(0xFF)
       
    } > SRAM AT > PROGRAM
    .plat_data_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libc.a:)(.data*) SORT(*libuart.a:)(.data*) SORT(*libhal_reboot.a:)(.data*) SORT(*libhal_uart_synopsys.a:)(.data*) SORT(*libliteos_port.a:)(.data*) SORT(*libalgorithm.a:)(.data*) SORT(*libchip_ws63.a:)(.data*) SORT(*libmbedtls.a:)(.data*) SORT(*libuart_port_synopsys.a:)(.data*) SORT(*libat.a:)(.data*) SORT(*libcpu_utils.a:)(.data*) SORT(*libgcc.a:)(.data*) SORT(*libdfx_diag.a:)(.data*) SORT(*libdfx_exception.a:)(.data*) SORT(*libdfx_log.a:)(.data*) SORT(*libdfx_panic.a:)(.data*) SORT(*libdfx_port_ws63.a:)(.data*) SORT(*libdfx_preserve.a:)(.data*) SORT(*libbase.a:)(.data*) SORT(*libws63_liteos_app.a:)(.data*) SORT(*libnon_os.a:)(.data*) SORT(*libproc.a:)(.data*) SORT(*libcompat.a:)(.data*) SORT(*libcmsis.a:)(.data*) SORT(*libdisk.a:)(.data*) SORT(*libmulti_partition.a:)(.data*) SORT(*libinit.a:)(.data*) SORT(*libm.a:)(.data*) SORT(*libfat.a:)(.data*) SORT(*libvfs.a:)(.data*) SORT(*libsec.a:)(.data*) SORT(*libcsysdeps.a:)(.data*) SORT(*libposix.a:)(.data*) SORT(*libinterrupt.a:)(.data*) SORT(*libtargets.a:)(.data*) SORT(*libdriverbase.a:)(.data*) SORT(*liblinux.a:)(.data*) SORT(*libbcache.a:)(.data*) SORT(*libramfs.a:)(.data*) SORT(*libconsole.a:)(.data*) SORT(*libriscv.a:)(.data*) SORT(*libosal.a:)(.data*) SORT(*libosal_adapt.a:)(.data*) SORT(*liblos_iperf.a:)(.data*) SORT(*libhal_uart.a:)(.data*) SORT(*libhal_tsensor.a:)(.data*) SORT(*libuart_port.a:)(.data*) SORT(*libhal_pmp.a:)(.data*) SORT(*libpmp_cfg_ws63.a:)(.data*) SORT(*libpmp_port.a:)(.data*) SORT(*libi2c_porting.a:)(.data*) SORT(*libsfc_flash_config_ws63.a:)(.data*) SORT(*libtsensor.a:)(.data*) SORT(*libtsensor_port.a:)(.data*) SORT(*libefuse.a:)(.data*) SORT(*libefuse_port.a:)(.data*) SORT(*libhal_efuse_v151.a:)(.data*) SORT(*libsoc_port.a:)(.data*) SORT(*libplat_patch.a:)(.data*) SORT(*libnv_ws63.a:)(.data*) SORT(*libupdate_common_ws63.a:)(.data*) SORT(*libapp_init.a:)(.data*) SORT(*libplt_at.a:)(.data*) SORT(*libdiag_dfx_cmd.a:)(.data*) SORT(*libnv.a:)(.data*) SORT(*libupdate_common.a:)(.data*) SORT(*libmips.a:)(.data*) SORT(*libcpup.a:)(.data*) SORT(*libhal_gpio_v150.a:)(.data*) SORT(*libgpio_port.a:)(.data*) SORT(*libboundscheck_sscanf_s.a:)(.data*)
    } > SRAM AT > PROGRAM
    .wifi_data_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libwifi_driver_hmac.a:)(.data*) SORT(*libwifi_driver_dmac.a:)(.data*) SORT(*libwpa_supplicant.a:)(.data*) SORT(*libwifi_service.a:)(.data*) SORT(*libwifi_at.a:)(.data*) SORT(*liblwip.a:)(.data*) SORT(*liblwip_tcm.a:)(.data*) SORT(*libwifi_promisc.a:)(.data*) SORT(*libwifi_auto_adjust_freq.a:)(.data*) SORT(*libwifi_csa.a:)(.data*) SORT(*libwifi_alg_temp_protect.a:)(.data*) SORT(*libwifi_alg_txbf.a:)(.data*) SORT(*libwifi_alg_anti_interference.a:)(.data*) SORT(*libwifi_alg_edca_opt.a:)(.data*) SORT(*libwifi_roam.a:)(.data*) SORT(*libwifi_alg_cca_opt.a:)(.data*) SORT(*libwifi_tx_amsdu.a:)(.data*) SORT(*libwifi_sdp.a:)(.data*) SORT(*libwifi_blacklist.a:)(.data*) SORT(*libwifi_sr.a:)(.data*) SORT(*libwifi_frag.a:)(.data*) SORT(*libwifi_mbo.a:)(.data*) SORT(*libwifi_bsrp_nfrp.a:)(.data*) SORT(*libwifi_slp.a:)(.data*) SORT(*libwifi_radar_sensor.a:)(.data*) SORT(*libwifi_apf.a:)(.data*) SORT(*libwifi_repeater.a:)(.data*) SORT(*libwifi_csi.a:)(.data*) SORT(*libwifi_wapi.a:)(.data*) SORT(*libwifi_wps.a:)(.data*) SORT(*libwifi_psd.a:)(.data*) SORT(*libwifi_latency.a:)(.data*) SORT(*libwifi_roam.a:)(.data*) SORT(*libwifi_uapsd_sta.a:)(.data*) SORT(*libwifi_ant_sel.a:)(.data*) SORT(*libwifi_twt.a:)(.data*) SORT(*libwifi_11k.a:)(.data*) SORT(*libwifi_11v.a:)(.data*) SORT(*libwifi_11r.a:)(.data*) SORT(*libwifi_btcoex.a:)(.data*) SORT(*libwifi_uapsd_ap.a:)(.data*) SORT(*libwifi_dnb.a:)(.data*) SORT(*libwifi_m2u.a:)(.data*)
    } > SRAM AT > PROGRAM
    .radar_data_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libradar_sensing.a:)(.data*) SORT(*libradar_at.a:)(.data*)
    } > SRAM AT > PROGRAM
    .radar_ai_data_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libradar_ai.a:)(.data*)
    } > SRAM AT > PROGRAM
    .btc_data_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libbgtp.a:)(.data*)
    } > SRAM AT > PROGRAM
    .bth_data_flash : ALIGN(8) {
        FILL(0xFF)
        SORT(*libbg_common.a:)(.data*) SORT(*libbt_host.a:)(.data*) SORT(*libbth_gle.a:)(.data*) SORT(*libbth_sdk.a:)(.data*) SORT(*libbt_at.a:)(.data*) SORT(*libbt_app.a:)(.data*) SORT(*libsamples.a:)(.data*) SORT(*libtestsuite.a:)(.data*)
    } > SRAM AT > PROGRAM
    .data : ALIGN(8) {
        FILL(0xFF)
        *(.data)
        *(.data*)
        *(.got)
        *(.got*)
        *(.igot)
        *(.igot*)
        _gp_ = . + 0x800;
        *(.sdata)
        *(.sdata*)
        . = ALIGN(4);
        __data_end__ = .;
        ASSERT(((__data_end__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > SRAM AT > PROGRAM
    __data_size__ = __data_end__ - __data_begin__;
    .plat_bss_sram (NOLOAD) : ALIGN(8) {
        __bss_begin__ = .;
        SORT(*libbase.a:*los_hwi.c.obj)(.bss*) SORT(*libbase.a:*los_tick.c.obj)(.bss*) SORT(*libbase.a:*los_sched.c.obj)(.bss*) SORT(*libbase.a:*los_exc.c.obj)(.bss*) SORT(*libbase.a:*los_sortlink.c.obj)(.bss*) SORT(*libinterrupt.a:*riscv_himideerv200_plic.c.obj)(.bss*) SORT(*libriscv.a:*exception.S.obj)(.bss*) SORT(*libriscv.a:*trap.S.obj)(.bss*) SORT(*libriscv.a:*dispatch.S.obj)(.bss*) SORT(*libriscv.a:*cache.c.obj)(.bss*) SORT(*libriscv.a:*task.c.obj)(.bss*) SORT(*libcsysdeps.a:*memcpy.S.obj)(.bss*) SORT(*libcsysdeps.a:*memset.S.obj)(.bss*) SORT(*libosal.a:*osal_spinlock.c.obj)(.bss*) SORT(*libosal.a:*osal_interrupt.c.obj)(.bss*) SORT(*libosal.a:*osal_addr.c.obj)(.bss*) SORT(*libosal_adapt.a:*osal_adapt_timer.c.obj)(.bss*) SORT(*libosal_adapt.a:*osal_adapt_task.c.obj)(.bss*) SORT(*libpm_port_ws63.a:)(.bss*) SORT(*libsfc_port_ws63.a:)(.bss*) SORT(*libtest_sfc.a:)(.bss*)
        ASSERT(((__bss_begin__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > SRAM
    .wifi_bss_sram (NOLOAD) : ALIGN(8) {
       
    } > SRAM
    .btc_bss_sram (NOLOAD) : ALIGN(8) {
       
    } > SRAM
    .bth_bss_sram (NOLOAD) : ALIGN(8) {
       
    } > SRAM
    .wifi_bss_cali (NOLOAD) : ALIGN(8) {
       
    } > SRAM
    .btc_bss_cali (NOLOAD) : ALIGN(8) {
       
    } > SRAM
    .bth_bss_cali (NOLOAD) : ALIGN(8) {
       
    } > SRAM
    .plat_bss_flash (NOLOAD) : ALIGN(8) {
        SORT(*libc.a:)(.bss*) SORT(*libuart.a:)(.bss*) SORT(*libhal_reboot.a:)(.bss*) SORT(*libhal_uart_synopsys.a:)(.bss*) SORT(*libliteos_port.a:)(.bss*) SORT(*libalgorithm.a:)(.bss*) SORT(*libchip_ws63.a:)(.bss*) SORT(*libmbedtls.a:)(.bss*) SORT(*libuart_port_synopsys.a:)(.bss*) SORT(*libat.a:)(.bss*) SORT(*libcpu_utils.a:)(.bss*) SORT(*libgcc.a:)(.bss*) SORT(*libdfx_diag.a:)(.bss*) SORT(*libdfx_exception.a:)(.bss*) SORT(*libdfx_log.a:)(.bss*) SORT(*libdfx_panic.a:)(.bss*) SORT(*libdfx_port_ws63.a:)(.bss*) SORT(*libdfx_preserve.a:)(.bss*) SORT(*libbase.a:)(.bss*) SORT(*libws63_liteos_app.a:)(.bss*) SORT(*libnon_os.a:)(.bss*) SORT(*libproc.a:)(.bss*) SORT(*libcompat.a:)(.bss*) SORT(*libcmsis.a:)(.bss*) SORT(*libdisk.a:)(.bss*) SORT(*libmulti_partition.a:)(.bss*) SORT(*libinit.a:)(.bss*) SORT(*libm.a:)(.bss*) SORT(*libfat.a:)(.bss*) SORT(*libvfs.a:)(.bss*) SORT(*libsec.a:)(.bss*) SORT(*libcsysdeps.a:)(.bss*) SORT(*libposix.a:)(.bss*) SORT(*libinterrupt.a:)(.bss*) SORT(*libtargets.a:)(.bss*) SORT(*libdriverbase.a:)(.bss*) SORT(*liblinux.a:)(.bss*) SORT(*libbcache.a:)(.bss*) SORT(*libramfs.a:)(.bss*) SORT(*libconsole.a:)(.bss*) SORT(*libriscv.a:)(.bss*) SORT(*libosal.a:)(.bss*) SORT(*libosal_adapt.a:)(.bss*) SORT(*liblos_iperf.a:)(.bss*) SORT(*libhal_uart.a:)(.bss*) SORT(*libhal_tsensor.a:)(.bss*) SORT(*libuart_port.a:)(.bss*) SORT(*libhal_pmp.a:)(.bss*) SORT(*libpmp_cfg_ws63.a:)(.bss*) SORT(*libpmp_port.a:)(.bss*) SORT(*libi2c_porting.a:)(.bss*) SORT(*libsfc_flash_config_ws63.a:)(.bss*) SORT(*libtsensor.a:)(.bss*) SORT(*libtsensor_port.a:)(.bss*) SORT(*libefuse.a:)(.bss*) SORT(*libefuse_port.a:)(.bss*) SORT(*libhal_efuse_v151.a:)(.bss*) SORT(*libsoc_port.a:)(.bss*) SORT(*libplat_patch.a:)(.bss*) SORT(*libnv_ws63.a:)(.bss*) SORT(*libupdate_common_ws63.a:)(.bss*) SORT(*libapp_init.a:)(.bss*) SORT(*libplt_at.a:)(.bss*) SORT(*libdiag_dfx_cmd.a:)(.bss*) SORT(*libnv.a:)(.bss*) SORT(*libupdate_common.a:)(.bss*) SORT(*libmips.a:)(.bss*) SORT(*libcpup.a:)(.bss*) SORT(*libhal_gpio_v150.a:)(.bss*) SORT(*libgpio_port.a:)(.bss*) SORT(*libboundscheck_sscanf_s.a:)(.bss*)
    } > SRAM
    .wifi_bss_flash (NOLOAD) : ALIGN(8) {
        SORT(*libwifi_driver_hmac.a:)(.bss*) SORT(*libwifi_driver_dmac.a:)(.bss*) SORT(*libwpa_supplicant.a:)(.bss*) SORT(*libwifi_service.a:)(.bss*) SORT(*libwifi_at.a:)(.bss*) SORT(*liblwip.a:)(.bss*) SORT(*liblwip_tcm.a:)(.bss*) SORT(*libwifi_promisc.a:)(.bss*) SORT(*libwifi_auto_adjust_freq.a:)(.bss*) SORT(*libwifi_csa.a:)(.bss*) SORT(*libwifi_alg_temp_protect.a:)(.bss*) SORT(*libwifi_alg_txbf.a:)(.bss*) SORT(*libwifi_alg_anti_interference.a:)(.bss*) SORT(*libwifi_alg_edca_opt.a:)(.bss*) SORT(*libwifi_roam.a:)(.bss*) SORT(*libwifi_alg_cca_opt.a:)(.bss*) SORT(*libwifi_tx_amsdu.a:)(.bss*) SORT(*libwifi_sdp.a:)(.bss*) SORT(*libwifi_blacklist.a:)(.bss*) SORT(*libwifi_sr.a:)(.bss*) SORT(*libwifi_frag.a:)(.bss*) SORT(*libwifi_mbo.a:)(.bss*) SORT(*libwifi_bsrp_nfrp.a:)(.bss*) SORT(*libwifi_slp.a:)(.bss*) SORT(*libwifi_radar_sensor.a:)(.bss*) SORT(*libwifi_apf.a:)(.bss*) SORT(*libwifi_repeater.a:)(.bss*) SORT(*libwifi_csi.a:)(.bss*) SORT(*libwifi_wapi.a:)(.bss*) SORT(*libwifi_wps.a:)(.bss*) SORT(*libwifi_psd.a:)(.bss*) SORT(*libwifi_latency.a:)(.bss*) SORT(*libwifi_roam.a:)(.bss*) SORT(*libwifi_uapsd_sta.a:)(.bss*) SORT(*libwifi_ant_sel.a:)(.bss*) SORT(*libwifi_twt.a:)(.bss*) SORT(*libwifi_11k.a:)(.bss*) SORT(*libwifi_11v.a:)(.bss*) SORT(*libwifi_11r.a:)(.bss*) SORT(*libwifi_btcoex.a:)(.bss*) SORT(*libwifi_uapsd_ap.a:)(.bss*) SORT(*libwifi_dnb.a:)(.bss*) SORT(*libwifi_m2u.a:)(.bss*)
    } > SRAM
    .radar_bss_flash (NOLOAD) : ALIGN(8) {
        SORT(*libradar_sensing.a:)(.bss*) SORT(*libradar_at.a:)(.bss*)
    } > SRAM
    .radar_ai_bss_flash (NOLOAD) : ALIGN(8) {
        SORT(*libradar_ai.a:)(.bss*)
    } > SRAM
    .btc_bss_flash (NOLOAD) : ALIGN(8) {
        SORT(*libbgtp.a:)(.bss*)
    } > SRAM
    .bth_bss_flash (NOLOAD) : ALIGN(8) {
        SORT(*libbg_common.a:)(.bss*) SORT(*libbt_host.a:)(.bss*) SORT(*libbth_gle.a:)(.bss*) SORT(*libbth_sdk.a:)(.bss*) SORT(*libbt_at.a:)(.bss*) SORT(*libbt_app.a:)(.bss*) SORT(*libsamples.a:)(.bss*) SORT(*libtestsuite.a:)(.bss*)
    } > SRAM
    .bss (NOLOAD) : ALIGN(8) {
        . = ALIGN(16);
        PROVIDE (__int_stack_start = .);
        *(.int_stack);
        . = ALIGN(16);
        PROVIDE (__int_stack_end = .);
        . = ALIGN(8);
        *(.bss)
        *(.bss*)
        *(.sbss)
        *(.sbss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = . ;
        ASSERT(((__bss_end__ % 4) == 0), "Error: symbol value is not 4-byte aligned.");
    } > SRAM
    __bss_size__ = __bss_end__ - __bss_begin__;
}
g_ram_begin = ORIGIN(SRAM);
g_ram_size = LENGTH(SRAM);
SECTIONS {
    .preserve (NOLOAD): ALIGN(8) {
        . = ALIGN(8);
        KEEP(*(preserve))
    } > PRESERVE_SHARE_MEM
    .stacks (NOLOAD): ALIGN(16) {
        . = ALIGN(16);
        g_system_stack_begin = .;
        . += 0x800;
        . = ALIGN(16);
        __irq_stack_top = .;
        . += 0x800;
        . = ALIGN(16);
        __exc_stack_top = .;
        . += 0x400;
        . = ALIGN(16);
        __nmi_stack_top = .;
        g_system_stack_end = .;
    } > SRAM
    g_system_stack_size = g_system_stack_end - g_system_stack_begin;
    g_stack_system = g_system_stack_size;
    .heap (NOLOAD): {
        . = ALIGN(8);
        __heap_start = .;
        g_intheap_begin = ABSOLUTE(.);
        g_intheap_size = (ORIGIN(SRAM) + LENGTH(SRAM)) - g_intheap_begin;
    } > SRAM
    __int_stack_end = g_intheap_begin + g_intheap_size;
}
