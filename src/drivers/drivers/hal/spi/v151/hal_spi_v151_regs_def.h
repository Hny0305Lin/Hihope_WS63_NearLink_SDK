/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides V151 ssi register \n
 *
 * History: \n
 * 2022-08-08, Create file. \n
 */
#ifndef HAL_SPI_V151_REGS_DEF_H
#define HAL_SPI_V151_REGS_DEF_H

#include <stdint.h>

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_spi_v151_regs_def SPI V151 Regs Definition
 * @ingroup  drivers_hal_spi
 * @{
 */

#define HAL_SPI_DRNM_REG_SIZE 36

/**
 * @brief  This union represents the bit fields in the Control Register 0. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_ctra_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t soe            :   1;
        uint32_t srlt           :   1;
        uint32_t ssn_te         :   1;
        uint32_t scph           :   1;
        uint32_t scpol          :   1;
        uint32_t cfs16          :   4;
        uint32_t dfs16          :   4;
        uint32_t dfs32          :   5;
        uint32_t trsm           :   2;
        uint32_t enhff          :   2;
        uint32_t prs            :   2;
        uint32_t reserved24_31  :   6;
    } b;                                /*!< Register bits. */
} spi_ctra_data_t;

/**
 * @brief  This union represents the bit fields in the Control Register 1. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_ctrb_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t nrdf           :   16;
    } b;                                /*!< Register bits. */
} spi_ctrb_data_t;

/**
 * @brief  This union represents the bit fields in the SPI Enable Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_er_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t start_en       :   1;
    } b;                                /*!< Register bits. */
} spi_er_data_t;

/**
 * @brief  This union represents the bit fields in the Microwire Control Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_mcr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t mh             :   1;
        uint32_t mss            :   1;
        uint32_t mtrc           :   1;
    } b;                                /*!< Register bits. */
} spi_mcr_data_t;

/**
 * @brief  This union represents the bit fields in the Slave Enable Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_slenr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t ssef           :   16;
    } b;                                /*!< Register bits. */
} spi_slenr_data_t;

/**
 * @brief  This union represents the bit fields in the Slave Enable Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_brs_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t frdv           :   16;
    } b;                                /*!< Register bits. */
} spi_brs_data_t;

/**
 * @brief  This union represents the bit fields in the Transmit FIFO Threshold Level. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_twlr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t twl;
    } b;                                /*!< Register bits. */
} spi_twlr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Threshold Level. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_rwlr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rwl;
    } b;                                /*!< Register bits. */
} spi_rwlr_data_t;

/**
 * @brief  This union represents the bit fields in the Transmit FIFO Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_tlr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t tfl;                   /*!< <b>Transmit FIFO Level.</b> \n
                                             Contains the number of valid data entries in the transmit FIFO. */
    } b;                                /*!< Register bits. */
} spi_tlr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_rlr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rfl;                 /*!< <b>Receive FIFO Level.</b> \n
                                             Contains the number of valid data entries in the receive FIFO. */
    } b;                                /*!< Register bits. */
} spi_rlr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_wsr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dcfe_tfee      :   1;
        uint32_t reserved1_3    :   3;
        uint32_t rfne           :   1;
        uint32_t rffe           :   1;
        uint32_t reserved6_10   :   5;
        uint32_t tfnf           :   1;
        uint32_t tfe            :   1;
        uint32_t reserved13_14  :   2;
        uint32_t sbf            :   1;
        uint32_t reserved16_31;
    } b;                                /*!< Register bits. */
} spi_wsr_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt Registers. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
union spi_v151_interrupt_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rffis          :   1;
        uint32_t rfofis         :   1;
        uint32_t rfufis         :   1;
        uint32_t reserved3_6    :   4;
        uint32_t tfeis          :   1;
        uint32_t tfofis         :   1;
        uint32_t reserved9_14   :   6;
        uint32_t mmcris         :   1;
    } b;                                /*!< Register bits. */
};

/**
 * @brief  This union represents the bit fields in the Interrupt Mask Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_v151_interrupt_data spi_inmar_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt Status Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_v151_interrupt_data spi_insr_data_t;

/**
 * @brief  This union represents the bit fields in the Raw Interrupt Status Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_v151_interrupt_data spi_rainsr_data_t;

/**
 * @brief  This union represents the bit fields in the Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_icr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t cis            :   1;
        uint32_t reserved1_31   :   31;
    } b;                                /*!< Register bits. */
} spi_icr_data_t;

/**
 * @brief  This union represents the bit fields in the Transmit FIFO Overflow Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_icr_data spi_tfoficr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Overflow Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_icr_data spi_rxoicr_data_t;

/**
 * @brief  This union represents the bit fields in the Receive FIFO Underflow Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_icr_data spi_rfficr_data_t;

/**
 * @brief  This union represents the bit fields in the Multi-Master Interrupt Clear Register Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_icr_data spi_msticr_data_t;

/**
 * @brief  This union represents the bit fields in the DMA Control Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_dcr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t tden           :   1;
        uint32_t rden           :   1;
        uint32_t reserved2_31   :   30;
    } b;                                /*!< Register bits. */
} spi_dcr_data_t;

/**
 * @brief  This union represents the bit fields in the DMA Data Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
union spi_v151_dmadlr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t dl;
    } b;                                /*!< Register bits. */
};

/**
 * @brief  This union represents the bit fields in the DMA Transmit Data Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_v151_dmadlr_data spi_dtdl_data_t;

/**
 * @brief  This union represents the bit fields in the DMA Receive Data Level Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_v151_dmadlr_data spi_drdl_data_t;

/**
 * @brief  This union represents the bit fields in the  coreKit version ID Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_id_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t id;
    } b;                                /*!< Register bits. */
} spi_id_data_t;

/**
 * @brief  This union represents the bit fields in the Data Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_drnm_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t datar;
    } b;                                /*!< Register bits. */
} spi_drnm_data_t;

/**
 * @brief  This union represents the bit fields in the RX Sample Delay Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_rsdr_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t rxdsd          :   8;
        uint32_t reserved8_31   :   24;
    } b;                                /*!< Register bits. */
} spi_rsdr_data_t;

/**
 * @brief  This union represents the bit fields in the SPI Control 0 Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_enhctl_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t waitnum        :   5;
        uint32_t aaitf          :   2;
        uint32_t rdsen          :   1;
        uint32_t addrlen        :   4;
        uint32_t ilen           :   2;
        uint32_t iddren         :   1;
        uint32_t ddren          :   1;
        uint32_t reserved16_31  :   18;
    } b;                                /*!< Register bits. */
} spi_enhctl_data_t;

/**
 * @brief  This union represents the bit fields in the Transmit Drive Edge Register. \n
 *         Read the register into the <i>d32</i> member then
 *         set/clear the bits using the <i>b</i> elements.
 */
typedef union spi_tder_data {
    uint32_t d32;                       /*!< Raw register data. */
    struct {
        uint32_t txdde          :   8;
        uint32_t reserved8_31   :   24;
    } b;                                /*!< Register bits. */
} spi_tder_data_t;

/**
 * @brief  Registers associated with spi.
 */
typedef struct spi_v151_regs {
    volatile uint32_t spi_er;
    volatile uint32_t spi_ctra;
    volatile uint32_t spi_ctrb;
    volatile uint32_t spi_enhctl;
    volatile uint32_t spi_mcr;
    volatile uint32_t spi_brs;
    volatile uint32_t spi_dcr;
    volatile uint32_t spi_drdl;
    volatile uint32_t spi_dtdl;
    volatile uint32_t spi_rsdr;
    volatile uint32_t spi_tder;
    volatile uint32_t spi_drnm[HAL_SPI_DRNM_REG_SIZE];
    volatile uint32_t spi_rainsr;
    volatile uint32_t spi_insr;
    volatile uint32_t spi_inmar;
    volatile uint32_t spi_slenr;
    volatile uint32_t spi_twlr;
    volatile uint32_t spi_tlr;
    volatile uint32_t spi_tfoficr;
    volatile uint32_t spi_rwlr;
    volatile uint32_t spi_rlr;
    volatile uint32_t spi_rfficr;
    volatile uint32_t spi_wsr;
    volatile uint32_t spi_msticr;
    volatile uint32_t spi_id;
    volatile uint32_t spi_rsvd;
    volatile uint32_t spi_rxoicr;
    volatile uint32_t spi_icr;
} spi_v151_regs_t;

/**
 * @}
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif