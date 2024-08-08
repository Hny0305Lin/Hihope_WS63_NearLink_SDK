/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides sfc port template \n
 *
 * History: \n
 * 2022-11-30， Create file. \n
 */
#ifndef SFC_PROTECT_H
#define SFC_PROTECT_H

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


#define SPI_CMD_RDSR_1 0x05
#define SPI_CMD_RDSR_2 0x35
#define SPI_CMD_RDSR_3 0x15

/**
 * @brief  读取状态寄存器的值。
 * @param  [in] cmd 命令字可以是：05H/35H/15H。
 *                   05H: 读 SR1
 *                   35H: 读 SR2
 *                   15H: 读 SR3
 * @retval 读取到的SR中的值。
 * @endif
 */
uint32_t sfc_port_read_sr(uint32_t cmd);

#define SPI_CMD_WRSR_1 0x01
#define SPI_CMD_WRSR_2 0x31
#define SPI_CMD_WRSR_3 0x11

/**
 * @brief  写状态寄存器。
 * @param  [in]  is_volatile 是否是易失性写入。
 *                True: 易失性写入, 掉电后配置丢失。
 *                False: 非易失性写入, 掉电后配置不丢失。
 * @param  [in]  cmd 命令字可以是：01H/31H/11H。
 *                    01H: 写 SR1
 *                    31H: 写 SR2
 *                    11H: 写 SR3
 * @param  [in]  data 待写入的SR寄存器的数据。
 * @endif
 */
void sfc_port_write_sr(bool is_volatile, uint8_t cmd, uint8_t data);

/**
 * @brief  修复状态寄存器。
 * @retval ERRCODE_SUCC 成功。
 * @retval Other        失败，参考 @ref errcode_t 。
 * @endif
 */
errcode_t sfc_port_fix_sr(void);

/**
 * @if Eng
 * @brief  sfc lock with addr.
 * @retval lock status.
 * @else
 * @brief  SFC上锁，同时根据地址信息，放开对应区域的擦、写权限。
 * @retval 锁状态。
 * @endif
 */
uint32_t sfc_port_write_lock(uint32_t start_addr, uint32_t end_addr);

/**
 * @if Eng
 * @brief  sfc unlock.
 * @param [in] lock_sts lock status.
 * @else
 * @brief  SFC解锁。
 * @param [in] lock_sts 锁状态，传入的为由sfc_port_write_lock接口返回的值
 * @endif
 */
void sfc_port_write_unlock(uint32_t lock_sts);

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
