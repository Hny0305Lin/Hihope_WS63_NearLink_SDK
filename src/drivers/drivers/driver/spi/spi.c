/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides spi driver source \n
 *
 * History: \n
 * 2022-09-16, Create file. \n
 */
#include "soc_osal.h"
#include "common_def.h"
#include "spi_porting.h"
#include "spi.h"
#if defined(CONFIG_SPI_SUPPORT_MASTER) || defined(CONFIG_SPI_SUPPORT_SLAVE)
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
#include "dma_porting.h"
#include "dma.h"
#include "hal_dma.h"
#include "securec.h"

#define DMA_SPI_TRANSFER_WIDTH_WORD 4
#define DMA_SPI_TRANSFER_WIDTH_HALF_WORD 2

typedef struct spi_dma_trans_inf {
    bool trans_succ;
#if !defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH)
    bool is_enable;
#endif  /* NOT CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
    uint8_t channel;
    uint8_t reserved;
    osal_semaphore dma_sem;
} spi_dma_trans_inf_t;

static spi_dma_trans_inf_t g_dma_trans_tx[SPI_BUS_MAX_NUM] = { 0 };
static spi_dma_trans_inf_t g_dma_trans_rx[SPI_BUS_MAX_NUM] = { 0 };
#if !defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH)
static spi_dma_config_t g_dma_cfg[SPI_BUS_MAX_NUM] = { 0 };
#endif  /* NOT CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
#endif  /* CONFIG_SPI_SUPPORT_DMA */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
#define SPI_MAX_NUMBER_OF_FRAGMENTS 4

/**
 * @brief  A fragment of data that is to be transmitted.
 */
typedef struct spi_tx_fragment {
    uint8_t *data;
    void *params;
    uint32_t data_length;
} spi_tx_fragment_t;

/**
 * @brief  The SPI Transmission configuration parameters.
 */
typedef struct spi_tx_state {
    spi_tx_fragment_t *current_tx_fragment;         /*!< Current TX fragment being transmitted. */
    spi_tx_fragment_t *free_tx_fragment;            /*!< The unused TX fragment admin blocks available
                                                         for re-use/freeing. */
    uint16_t fragments_to_process;                  /*!< Number of fragments to process including the current one. */
    uint32_t current_tx_fragment_pos;               /*!< Index of the current position of the next byte to be
                                                         transmitted in the current TX fragment
                                                         current_tx_fragment_pos == 0 means
                                                         the first byte is yet to be sent for transmission */
    spi_tx_fragment_t fragment_buffer[SPI_MAX_NUMBER_OF_FRAGMENTS]; /*!< Fragments buffer pointer. */
    spi_tx_callback_t tx_callback;
} spi_tx_state_t;

/**
 * @brief  The SPI Reception configuration parameters.
 */
typedef struct spi_rx_state {
    uint32_t rx_buffer_size;                        /*!< The size of the receive buffer. */
    spi_rx_callback_t rx_callback;                  /*!< The RX callback to make when the condition is met. */
    uint32_t new_rx_pos;                            /*!< Index to the position in the RX buffer that is where new data
                                                         should be put if (new_rx_pos == 0) the buffer is empty. */
    uint8_t *rx_buffer;                             /*!< The RX data buffer. */
} spi_rx_state_t;

static bool g_spi_tx_int_flag[SPI_BUS_MAX_NUM];
static bool g_spi_rx_int_flag[SPI_BUS_MAX_NUM];
/**
 * @brief  Internal SPI TX configuration.
 */
static spi_tx_state_t g_spi_tx_state[SPI_BUS_MAX_NUM];
/**
 * @brief  Internal SPI RX configuration.
 */
static spi_rx_state_t g_spi_rx_state[SPI_BUS_MAX_NUM];

#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
#endif  /* CONFIG_SPI_SUPPORT_MASTER || CONFIG_SPI_SUPPORT_SLAVE */

#define SPI_TX_FIFO_BUSY_TIMEOUT 3200000
#define hal_spi_frame_size_trans_to_frame_bytes(x)  (((x) + 1) >> 0x03)

static bool g_spi_is_initialised[SPI_BUS_MAX_NUM] = { false };
#if defined(CONFIG_SPI_SUPPORT_CONCURRENCY) && (CONFIG_SPI_SUPPORT_CONCURRENCY == 1)
static osal_mutex g_spi_mutex[SPI_BUS_MAX_NUM] = { NULL };
#endif  /* CONFIG_SPI_SUPPORT_CONCURRENCY */

#if defined(CONFIG_SPI_SUPPORT_CONCURRENCY) && (CONFIG_SPI_SUPPORT_CONCURRENCY == 1)
static bool spi_mutex_lock(spi_bus_t bus)
{
    if (osal_mutex_lock_timeout(&g_spi_mutex[bus], OSAL_MUTEX_WAIT_FOREVER) != OSAL_SUCCESS) {
        return false;
    }
    return true;
}
#endif  /* CONFIG_SPI_SUPPORT_CONCURRENCY */

static void spi_mutex_unlock(spi_bus_t bus)
{
#if defined(CONFIG_SPI_SUPPORT_CONCURRENCY) && (CONFIG_SPI_SUPPORT_CONCURRENCY == 1)
    osal_mutex_unlock(&g_spi_mutex[bus]);
#endif  /* CONFIG_SPI_SUPPORT_CONCURRENCY */
    unused(bus);
}

#if defined(CONFIG_SPI_SUPPORT_MASTER) || defined(CONFIG_SPI_SUPPORT_SLAVE)
static uint32_t spi_get_attr_tmod(spi_bus_t bus)
{
    spi_attr_t attr; /* uapi_spi_get_attr will copy attr from hal, no need init to 0. */

    if (uapi_spi_get_attr(bus, &attr) != ERRCODE_SUCC) {
        return 0;
    }
    return attr.tmod;
}

static errcode_t spi_param_check(spi_bus_t bus, spi_mode_t spi_mode, hal_spi_trans_mode_t trans_mode)
{
    if (spi_porting_get_device_mode(bus) != spi_mode) {
        return ERRCODE_SPI_MODE_MISMATCH;
    }

    if (spi_get_attr_tmod(bus) == trans_mode) {
        return ERRCODE_SPI_INVALID_TMODE;
    }

    return ERRCODE_SUCC;
}

static errcode_t spi_fifo_check(spi_bus_t bus)
{
#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (!g_spi_tx_int_flag[bus]) {
        if (hal_spi_ctrl(bus, SPI_CTRL_CHECK_FIFO_BUSY, (uintptr_t)SPI_TX_FIFO_BUSY_TIMEOUT) != ERRCODE_SUCC) {
            return ERRCODE_SPI_TIMEOUT;
        }
    }
#else
    if (hal_spi_ctrl(bus, SPI_CTRL_CHECK_FIFO_BUSY, (uintptr_t)SPI_TX_FIFO_BUSY_TIMEOUT) != ERRCODE_SUCC) {
        return ERRCODE_SPI_TIMEOUT;
    }
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
    return ERRCODE_SUCC;
}

static errcode_t spi_param_check_read(spi_bus_t bus, spi_mode_t spi_mode, hal_spi_trans_mode_t trans_mode,
                                      const spi_xfer_data_t *data, bool check_fifo)
{
    if (bus >= SPI_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }

    if (data == NULL || data->rx_buff == NULL || data->rx_bytes == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    errcode_t ret = spi_param_check(bus, spi_mode, trans_mode);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }
    if (check_fifo) {
        if (spi_fifo_check(bus) != ERRCODE_SUCC) {
            return ERRCODE_SPI_TIMEOUT;
        }
    }
#if defined(CONFIG_SPI_SUPPORT_CONCURRENCY) && (CONFIG_SPI_SUPPORT_CONCURRENCY == 1)
    if (!spi_mutex_lock(bus)) {
        return ERRCODE_SPI_TIMEOUT;
    }
#endif  /* CONFIG_SPI_SUPPORT_CONCURRENCY */

    return ERRCODE_SUCC;
}

static errcode_t spi_param_check_write(spi_bus_t bus, spi_mode_t spi_mode, hal_spi_trans_mode_t trans_mode,
                                       const spi_xfer_data_t *data, bool check_fifo)
{
    /* data->tx_buff maybe NULL and data->tx_bytes maybe 0, as flash will write cmd only in master mode. */
    if (bus >= SPI_BUS_MAX_NUM || data == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    errcode_t ret = spi_param_check(bus, spi_mode, trans_mode);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    if (check_fifo) {
        if (spi_fifo_check(bus) != ERRCODE_SUCC) {
            return ERRCODE_SPI_TIMEOUT;
        }
    }
#if defined(CONFIG_SPI_SUPPORT_CONCURRENCY) && (CONFIG_SPI_SUPPORT_CONCURRENCY == 1)
    if (!spi_mutex_lock(bus)) {
        return ERRCODE_SPI_TIMEOUT;
    }
#endif  /* CONFIG_SPI_SUPPORT_CONCURRENCY */
    return ERRCODE_SUCC;
}

#endif  /* CONFIG_SPI_SUPPORT_MASTER || CONFIG_SPI_SUPPORT_SLAVE */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
static bool spi_helper_add_fragment(spi_bus_t bus, const void *buffer, uint32_t length)
{
    spi_tx_fragment_t *fragment = NULL;

    if (g_spi_tx_state[bus].fragments_to_process >= SPI_MAX_NUMBER_OF_FRAGMENTS) {
        return false;
    } else {
        /* If we have fragments left add it.
           Put it on the queue and populate the fragment. */
        fragment = g_spi_tx_state[bus].free_tx_fragment;
        fragment->data = (uint8_t *)buffer;
        fragment->data_length = length;

        /* Update the counters */
        g_spi_tx_state[bus].free_tx_fragment++;
        if (g_spi_tx_state[bus].free_tx_fragment >=
            g_spi_tx_state[bus].fragment_buffer  + SPI_MAX_NUMBER_OF_FRAGMENTS) {
            g_spi_tx_state[bus].free_tx_fragment = g_spi_tx_state[bus].fragment_buffer;  /* wrapping */
        }
        g_spi_tx_state[bus].fragments_to_process++;
    }
    return true;
}

static inline bool spi_helper_is_the_current_fragment_the_last_to_process(spi_bus_t bus)
{
    return (g_spi_tx_state[bus].fragments_to_process == 1);
}

static inline bool spi_helper_are_there_fragments_to_process(spi_bus_t bus)
{
    return (g_spi_tx_state[bus].fragments_to_process > 0);
}

static bool spi_helper_send_next_char(spi_bus_t bus, uint32_t frame_bytes)
{
    spi_tx_fragment_t *current_fragment;
    uint32_t current_fragment_pos;
    spi_xfer_data_t data;

    current_fragment = g_spi_tx_state[bus].current_tx_fragment;
    current_fragment_pos = g_spi_tx_state[bus].current_tx_fragment_pos;

    data.tx_buff = &current_fragment->data[current_fragment_pos];
    data.tx_bytes = frame_bytes;
    hal_spi_write(bus, &data, CONFIG_SPI_MAX_TIMEOUT);
    /* update the counters */
    g_spi_tx_state[bus].current_tx_fragment_pos += frame_bytes;

    return (g_spi_tx_state[bus].current_tx_fragment_pos >= current_fragment->data_length);
}

static void spi_helper_invoke_current_fragment_callback(spi_bus_t bus)
{
    spi_tx_fragment_t *current_fragment = g_spi_tx_state[bus].current_tx_fragment;
    /* Call any TX data release call-back */
    if (g_spi_tx_state[bus].tx_callback != NULL) {
        g_spi_tx_state[bus].tx_callback(current_fragment->data, current_fragment->data_length);
    }
}

static void spi_helper_move_to_next_fragment(spi_bus_t bus)
{
    /* Move onto the next fragment and re-set the position to zero */
    g_spi_tx_state[bus].current_tx_fragment++;
    if (g_spi_tx_state[bus].current_tx_fragment >=
        g_spi_tx_state[bus].fragment_buffer + SPI_MAX_NUMBER_OF_FRAGMENTS) {
        g_spi_tx_state[bus].current_tx_fragment = g_spi_tx_state[bus].fragment_buffer;  /* wrapping */
    }
    g_spi_tx_state[bus].current_tx_fragment_pos = 0;  /* reset the current fragment */
    g_spi_tx_state[bus].fragments_to_process--;       /* one fragment less to process */
}

static void spi_tx_isr(spi_bus_t bus)
{
    bool tx_fifo_full = false;
    hal_spi_attr_t attr; /* will copy attr from hal, no need init to 0. */

    hal_spi_ctrl(bus, SPI_CTRL_GET_ATTR, (uintptr_t)&attr);
    uint32_t frame_bytes = hal_spi_frame_size_trans_to_frame_bytes(attr.frame_size);

    /* if there are fragments to process do it */
    if (!spi_helper_are_there_fragments_to_process(bus)) {
        /* No data to transmit so disable the TX interrupt */
        hal_spi_ctrl(bus, SPI_CTRL_EN_TXEI_INT, 0);
        return;
    }

    if (g_spi_tx_state[bus].current_tx_fragment_pos == 0) {
        if (hal_spi_ctrl(bus, SPI_CTRL_CHECK_FIFO_BUSY, (uintptr_t)SPI_TX_FIFO_BUSY_TIMEOUT) != ERRCODE_SUCC) {
            return;
        }
    }

    hal_spi_ctrl(bus, SPI_CTRL_CHECK_TX_FIFO_FULL, (uintptr_t)&tx_fifo_full);
    /* Populate the SPI TX FIFO if there is data to send */
    while (!tx_fifo_full) {
        /* There is some data to transmit so provide another byte to the SPI */
        bool end_of_fragment = spi_helper_send_next_char(bus, frame_bytes);
        if (end_of_fragment) {
            /* If it is the end of the fragment invoke the callback and move to the next one */
            spi_helper_invoke_current_fragment_callback(bus);
            spi_helper_move_to_next_fragment(bus);
            /* If it was the last fragment disable the TX interrupts and leave */
            if (spi_helper_are_there_fragments_to_process(bus) == false) {
                /* No data to transmit so disable the TX interrupt */
                hal_spi_ctrl(bus, SPI_CTRL_EN_TXEI_INT, 0);
                break;
            }
        }

        hal_spi_ctrl(bus, SPI_CTRL_CHECK_TX_FIFO_FULL, (uintptr_t)&tx_fifo_full);
    }
}

static inline void spi_rx_buffer_release(spi_bus_t bus)
{
    g_spi_rx_state[bus].new_rx_pos = 0;
}

static inline bool spi_rx_buffer_has_free_space(spi_bus_t bus)
{
    return (g_spi_rx_state[bus].new_rx_pos < g_spi_rx_state[bus].rx_buffer_size);
}

static void spi_rx_finish_clear_ndf(spi_bus_t bus)
{
    spi_attr_t attr; /* uapi_spi_get_attr will copy attr from hal, no need init to 0. */
    if (uapi_spi_get_attr(bus, &attr) != ERRCODE_SUCC) {
        return;
    }
    attr.ndf = 1;
    if (uapi_spi_set_attr(bus, &attr) != ERRCODE_SUCC) {
        return;
    }
}

static void spi_rx_isr(spi_bus_t bus)
{
    if (g_spi_rx_state[bus].rx_callback == NULL) {
        return;
    }
    spi_xfer_data_t data;

    data.rx_bytes =  g_spi_rx_state[bus].rx_buffer_size - g_spi_rx_state[bus].new_rx_pos;
    data.rx_buff = g_spi_rx_state[bus].rx_buffer + g_spi_rx_state[bus].new_rx_pos;

    if (hal_spi_read(bus, &data, 0) != ERRCODE_SUCC) {
        return;
    }
    g_spi_rx_state[bus].new_rx_pos = g_spi_rx_state[bus].rx_buffer_size - data.rx_bytes;

    if (!spi_rx_buffer_has_free_space(bus)) {
        hal_spi_ctrl(bus, SPI_CTRL_EN_RXFI_INT, 0);
        g_spi_rx_state[bus].rx_callback(g_spi_rx_state[bus].rx_buffer, g_spi_rx_state[bus].new_rx_pos, false);
        spi_rx_buffer_release(bus);
        spi_rx_finish_clear_ndf(bus);
    }
}

static errcode_t spi_evt_callback(spi_bus_t bus, hal_spi_evt_id_t evt, uintptr_t param)
{
    unused(param);
    unused(bus);

    switch (evt) {
        case SPI_EVT_TX_EMPTY_ISR:
            spi_tx_isr(bus);
            break;
        case SPI_EVT_TX_OVERFLOW_ISR:
            break;
        case SPI_EVT_RX_FULL_ISR:
            spi_rx_isr(bus);
            break;
        case SPI_EVT_RX_OVERFLOW_ISR:
            break;
        case SPI_EVT_RX_UNDERFLOW_ISR:
            break;
        case SPI_EVT_MULTI_MASTER_ISR:
            break;
    }
    return ERRCODE_SUCC;
}

static void spi_config_tx_state(spi_bus_t bus)
{
    /* Configure TX state structure */
    g_spi_tx_state[bus].current_tx_fragment = g_spi_tx_state[bus].fragment_buffer;  /* the queue is empty */
    g_spi_tx_state[bus].free_tx_fragment = g_spi_tx_state[bus].fragment_buffer;     /* the queue is empty */
}
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */

static void spi_mutex_init(spi_bus_t bus)
{
#if defined(CONFIG_SPI_SUPPORT_CONCURRENCY) && (CONFIG_SPI_SUPPORT_CONCURRENCY == 1)
    osal_mutex_init(&g_spi_mutex[bus]);
#endif  /* CONFIG_SPI_SUPPORT_CONCURRENCY */
    unused(bus);
}

static void spi_mutex_deinit(spi_bus_t bus)
{
#if defined(CONFIG_SPI_SUPPORT_CONCURRENCY) && (CONFIG_SPI_SUPPORT_CONCURRENCY == 1)
    osal_mutex_destroy(&g_spi_mutex[bus]);
#endif  /* CONFIG_SPI_SUPPORT_CONCURRENCY */
    unused(bus);
}

static void spi_int_mode_init(spi_bus_t bus)
{
#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    spi_port_register_irq(bus);
    spi_config_tx_state(bus);
#else
    unused(bus);
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
}

errcode_t uapi_spi_init(spi_bus_t bus, spi_attr_t *attr, spi_extra_attr_t *extra_attr)
{
    errcode_t ret = ERRCODE_SUCC;

    if (bus >= SPI_BUS_MAX_NUM || attr == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    if (g_spi_is_initialised[bus]) {
        return ret;
    }

#if defined(CONFIG_SPI_SUPPORT_LPC)
    spi_port_clock_enable(bus, true);
#endif

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    (void)osal_sem_init(&(g_dma_trans_tx[bus].dma_sem), 0);
    (void)osal_sem_init(&(g_dma_trans_rx[bus].dma_sem), 0);
#endif  /* CONFIG_SPI_SUPPORT_DMA */

    spi_mutex_init(bus);

    spi_int_mode_init(bus);

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    ret = hal_spi_init(bus, (hal_spi_attr_t *)attr, (hal_spi_extra_attr_t *)extra_attr, spi_evt_callback);
#else  /* CONFIG_SPI_SUPPORT_INTERRUPT */
    ret = hal_spi_init(bus, (hal_spi_attr_t *)attr, (hal_spi_extra_attr_t *)extra_attr, NULL);
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
    if (ret == ERRCODE_SUCC) {
        g_spi_is_initialised[bus] = true;
    }

    return ret;
}

errcode_t uapi_spi_deinit(spi_bus_t bus)
{
    if (bus >= SPI_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }

    if (!g_spi_is_initialised[bus]) {
        return ERRCODE_SUCC;
    }

    errcode_t ret = hal_spi_deinit(bus);

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    osal_sem_destroy(&(g_dma_trans_tx[bus].dma_sem));
    osal_sem_destroy(&(g_dma_trans_rx[bus].dma_sem));
#endif  /* CONFIG_SPI_SUPPORT_DMA */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    spi_port_unregister_irq(bus);
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */

#if defined(CONFIG_SPI_SUPPORT_LPC)
    spi_port_clock_enable(bus, false);
#endif

    spi_mutex_deinit(bus);

    g_spi_is_initialised[bus] = false;

    return ret;
}

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
errcode_t uapi_spi_set_dma_mode(spi_bus_t bus, bool en, const spi_dma_config_t *dma_cfg)
{
    if (bus >= SPI_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    hal_spi_dma_cfg_param_t data;
#if !defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH)
#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (g_spi_rx_int_flag[bus] || g_spi_tx_int_flag[bus]) {
        return ERRCODE_SPI_DMA_IRQ_MODE_MUTEX;
    }
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
#endif  /* NOT CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

    uint32_t irq_sts = spi_porting_lock(bus);
    if (en) {
        data.dma_tx_level = spi_port_tx_data_level_get(bus);
        data.dma_rx_level = spi_port_rx_data_level_get(bus);
#if !defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH)
        g_dma_cfg[bus].src_width = dma_cfg->src_width;
        g_dma_cfg[bus].dest_width = dma_cfg->dest_width;
        g_dma_cfg[bus].burst_length = dma_cfg->burst_length;
        g_dma_cfg[bus].priority = dma_cfg->priority;
#else
        unused(dma_cfg);
#endif  /* NOT CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
    } else {
        data.dma_tx_level = 0;
        data.dma_rx_level = 0;
    }

    data.is_enable = en;
    hal_spi_ctrl(bus, SPI_CTRL_SET_DMA_CFG, (uintptr_t)&data);
#if !defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH)
    g_dma_trans_tx[bus].is_enable = en;
    g_dma_trans_rx[bus].is_enable = en;
#endif  /* NOT CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
    spi_porting_unlock(bus, irq_sts);

    return ERRCODE_SUCC;
}

static void spi_dma_isr(uint8_t int_type, uint8_t ch, uintptr_t arg)
{
    uint8_t bus = SPI_BUS_MAX_NUM;
    spi_dma_trans_inf_t *dma_trans = NULL;

    if ((uint32_t)arg == 0) {
        dma_trans = g_dma_trans_tx;
    } else {
        dma_trans = g_dma_trans_rx;
    }

    for (uint8_t i = SPI_BUS_0; i < SPI_BUS_MAX_NUM; i++) {
        /* channel default value is 0, means not used. channel > 0 means used.
           So ch + 1 will not misjudgment with channel value 0. */
        if (dma_trans[i].channel == ch + 1) {
            bus = i;
            break;
        }
    }

    if (bus != SPI_BUS_MAX_NUM) {
        if (int_type == 0) {
            dma_trans[bus].trans_succ = true;
        }
        osal_sem_up(&(dma_trans[bus].dma_sem));
    }
}

#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
static inline bool spi_dma_align_check_word(uint32_t data_buf, uint32_t data_len, uint32_t align_data)
{
    bool ret1 = (data_buf % align_data) == 0 ? true : false;
    bool ret2 = (data_len % align_data) == 0 ? true : false;
    return ret1 && ret2;
}

static hal_dma_data_width_t spi_dma_get_mem_width(uint32_t buff, uint32_t bytes)
{
    if (spi_dma_align_check_word(buff, bytes, (uint32_t)DMA_SPI_TRANSFER_WIDTH_WORD)) {
        return HAL_DMA_TRANSFER_WIDTH_32;
    }
    if (spi_dma_align_check_word(buff, bytes, (uint32_t)DMA_SPI_TRANSFER_WIDTH_HALF_WORD)) {
        return HAL_DMA_TRANSFER_WIDTH_16;
    }
    return HAL_DMA_TRANSFER_WIDTH_8;
}

static hal_dma_data_width_t spi_dma_get_spi_width(spi_bus_t bus)
{
    hal_spi_attr_t attr; /* will copy attr from hal, no need init to 0. */

    hal_spi_ctrl(bus, SPI_CTRL_GET_ATTR, (uintptr_t)&attr);
    uint32_t frame_bytes = hal_spi_frame_size_trans_to_frame_bytes(attr.frame_size);

    if (frame_bytes == (uint32_t)DMA_SPI_TRANSFER_WIDTH_WORD) {
        return HAL_DMA_TRANSFER_WIDTH_32;
    } else if (frame_bytes == (uint32_t)DMA_SPI_TRANSFER_WIDTH_HALF_WORD) {
        return HAL_DMA_TRANSFER_WIDTH_16;
    }
    return HAL_DMA_TRANSFER_WIDTH_8;
}
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

#if !defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH)
static void spi_dma_common_config(dma_ch_user_peripheral_config_t *transfer_config, const spi_dma_config_t *dma_cfg)
{
    transfer_config->src_width = dma_cfg->src_width;
    transfer_config->dest_width = dma_cfg->dest_width;
    transfer_config->burst_length = dma_cfg->burst_length;
    transfer_config->priority = dma_cfg->priority;
}
#endif  /* NOT CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

static void spi_dma_tx_config(dma_ch_user_peripheral_config_t *transfer_config, spi_bus_t bus)
{
    uint32_t data_addr = 0;
    hal_spi_ctrl(bus, SPI_CTRL_GET_DMA_DATA_ADDR, (uintptr_t)(&data_addr));

#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    transfer_config->dest_width = spi_dma_get_spi_width(bus);
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
    transfer_config->trans_type = HAL_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA;
    transfer_config->trans_dir = HAL_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL;
    transfer_config->src_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config->dest_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    transfer_config->protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;
    transfer_config->src_handshaking =  0;
    transfer_config->dest = data_addr;
}

static errcode_t spi_write_dma(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    dma_ch_user_peripheral_config_t transfer_config = { 0 };
    uint8_t channel = DMA_CHANNEL_NONE;
    if (data->tx_buff == NULL || data->tx_bytes == 0) {
        return ERRCODE_INVALID_PARAM;
    }
    transfer_config.src = (uint32_t)(uintptr_t)data->tx_buff;
#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    transfer_config.src_width = spi_dma_get_mem_width(transfer_config.src, data->tx_bytes);
    transfer_config.transfer_num = (uint16_t)data->tx_bytes >> transfer_config.src_width;
    transfer_config.burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_1;
    transfer_config.priority = HAL_DMA_CH_PRIORITY_0;
#else
    if (data->tx_bytes % bit(g_dma_cfg[bus].src_width) == 0) {
        transfer_config.transfer_num = (uint16_t)data->tx_bytes >> g_dma_cfg[bus].src_width;
    } else {
        return ERRCODE_INVALID_PARAM;
    }

    spi_dma_common_config(&transfer_config, &(g_dma_cfg[bus]));
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
    spi_dma_tx_config(&transfer_config, bus);

    transfer_config.dest_handshaking = spi_port_get_dma_trans_dest_handshaking(bus);
    if (transfer_config.dest_handshaking == HAL_DMA_HANDSHAKING_MAX_NUM) {
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (uapi_dma_configure_peripheral_transfer_single(&transfer_config, &channel, spi_dma_isr, 0) != ERRCODE_SUCC) {
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    g_dma_trans_tx[bus].channel = channel + 1;
    g_dma_trans_tx[bus].trans_succ = false;

    if (uapi_dma_start_transfer(channel) != ERRCODE_SUCC) {
        g_dma_trans_tx[bus].channel = 0;
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (osal_sem_down_timeout(&(g_dma_trans_tx[bus].dma_sem), timeout) != OSAL_SUCCESS) {
        uapi_dma_end_transfer(channel);
        g_dma_trans_tx[bus].channel = 0;
        return ERRCODE_SPI_DMA_TRANSFER_ERROR;
    }

    uapi_dma_end_transfer(channel);
    g_dma_trans_tx[bus].channel = 0;

    if (!g_dma_trans_tx[bus].trans_succ) {
        return ERRCODE_SPI_DMA_TRANSFER_ERROR;
    }

    return ERRCODE_SUCC;
}

static void spi_dma_rx_config(dma_ch_user_peripheral_config_t *transfer_config, spi_bus_t bus)
{
    uint32_t data_addr = 0;
    hal_spi_ctrl(bus, SPI_CTRL_GET_DMA_DATA_ADDR, (uintptr_t)(&data_addr));

#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    transfer_config->src_width = spi_dma_get_spi_width(bus);
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
    transfer_config->trans_type = HAL_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    transfer_config->trans_dir = HAL_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    transfer_config->src_increment = HAL_DMA_ADDRESS_INC_NO_CHANGE;
    transfer_config->dest_increment = HAL_DMA_ADDRESS_INC_INCREMENT;
    transfer_config->protection = HAL_DMA_PROTECTION_CONTROL_BUFFERABLE;
    transfer_config->dest_handshaking =  0;
    transfer_config->src = data_addr;
}

static errcode_t spi_read_dma(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    dma_ch_user_peripheral_config_t transfer_config = { 0 };
    uint8_t channel = DMA_CHANNEL_NONE;

    spi_dma_rx_config(&transfer_config, bus);
    transfer_config.dest = (uint32_t)(uintptr_t)data->rx_buff;
#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    transfer_config.dest_width = spi_dma_get_mem_width(transfer_config.dest, data->rx_bytes);
    transfer_config.burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_1;
    transfer_config.priority = HAL_DMA_CH_PRIORITY_0;
    transfer_config.transfer_num = (uint16_t)data->rx_bytes >> transfer_config.src_width;
#else
    if (data->rx_bytes % bit(g_dma_cfg[bus].src_width) == 0) {
        transfer_config.transfer_num = (uint16_t)data->rx_bytes >> g_dma_cfg[bus].src_width;
    } else {
        return ERRCODE_INVALID_PARAM;
    }

    spi_dma_common_config(&transfer_config, &(g_dma_cfg[bus]));
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

    transfer_config.src_handshaking = spi_port_get_dma_trans_src_handshaking(bus);
    if (transfer_config.src_handshaking == HAL_DMA_HANDSHAKING_MAX_NUM) {
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (uapi_dma_configure_peripheral_transfer_single(&transfer_config, &channel, spi_dma_isr, 1) != ERRCODE_SUCC) {
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    g_dma_trans_rx[bus].channel = channel + 1;
    g_dma_trans_rx[bus].trans_succ = false;

    if (uapi_dma_start_transfer(channel) != ERRCODE_SUCC) {
        g_dma_trans_rx[bus].channel = 0;
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (osal_sem_down_timeout(&(g_dma_trans_rx[bus].dma_sem), timeout) != OSAL_SUCCESS) {
        uapi_dma_end_transfer(channel);
        g_dma_trans_rx[bus].channel = 0;
        return ERRCODE_SPI_DMA_TRANSFER_ERROR;
    }

    uapi_dma_end_transfer(channel);
    g_dma_trans_rx[bus].channel = 0;

    if (!g_dma_trans_rx[bus].trans_succ) {
        return ERRCODE_SPI_DMA_TRANSFER_ERROR;
    }

    return ERRCODE_SUCC;
}

static errcode_t spi_read_dma_config(spi_bus_t bus, uint8_t *ch, const spi_xfer_data_t *data)
{
    dma_ch_user_peripheral_config_t transfer_config = { 0 };
    uint8_t channel = DMA_CHANNEL_NONE;

    spi_dma_rx_config(&transfer_config, bus);
    transfer_config.dest = (uint32_t)(uintptr_t)data->rx_buff;
#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    transfer_config.dest_width = spi_dma_get_mem_width(transfer_config.dest, data->rx_bytes);
    transfer_config.burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_1;
    transfer_config.priority = HAL_DMA_CH_PRIORITY_0;
    transfer_config.transfer_num = (uint16_t)data->rx_bytes >> transfer_config.src_width;
#else
    if (data->rx_bytes % bit(g_dma_cfg[bus].src_width) == 0) {
        transfer_config.transfer_num = (uint16_t)data->rx_bytes >> g_dma_cfg[bus].src_width;
    } else {
        return ERRCODE_INVALID_PARAM;
    }

    spi_dma_common_config(&transfer_config, &(g_dma_cfg[bus]));
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

    transfer_config.src_handshaking = spi_port_get_dma_trans_src_handshaking(bus);
    if (transfer_config.src_handshaking == HAL_DMA_HANDSHAKING_MAX_NUM) {
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (uapi_dma_configure_peripheral_transfer_single(&transfer_config, &channel, spi_dma_isr, 1) != ERRCODE_SUCC) {
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    g_dma_trans_rx[bus].channel = channel + 1;
    g_dma_trans_rx[bus].trans_succ = false;
    *ch = channel;

    return ERRCODE_SUCC;
}

static errcode_t spi_write_dma_config(spi_bus_t bus, uint8_t *ch, const spi_xfer_data_t *data)
{
    dma_ch_user_peripheral_config_t transfer_config = { 0 };
    uint8_t channel = DMA_CHANNEL_NONE;

    transfer_config.src = (uint32_t)(uintptr_t)data->tx_buff;
#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    transfer_config.src_width = spi_dma_get_mem_width(transfer_config.src, data->tx_bytes);
    transfer_config.transfer_num = (uint16_t)data->tx_bytes >> transfer_config.src_width;
    transfer_config.burst_length = HAL_DMA_BURST_TRANSACTION_LENGTH_1;
    transfer_config.priority = HAL_DMA_CH_PRIORITY_0;
#else
    if (data->tx_bytes % bit(g_dma_cfg[bus].src_width) == 0) {
        transfer_config.transfer_num = (uint16_t)data->tx_bytes >> g_dma_cfg[bus].src_width;
    } else {
        return ERRCODE_INVALID_PARAM;
    }

    spi_dma_common_config(&transfer_config, &(g_dma_cfg[bus]));
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
    spi_dma_tx_config(&transfer_config, bus);

    transfer_config.dest_handshaking = spi_port_get_dma_trans_dest_handshaking(bus);
    if (transfer_config.dest_handshaking == HAL_DMA_HANDSHAKING_MAX_NUM) {
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (uapi_dma_configure_peripheral_transfer_single(&transfer_config, &channel, spi_dma_isr, 0) != ERRCODE_SUCC) {
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    g_dma_trans_tx[bus].channel = channel + 1;
    g_dma_trans_tx[bus].trans_succ = false;
    *ch = channel;

    return ERRCODE_SUCC;
}

static void spi_writeread_dma_clear_trans(spi_bus_t bus, uint8_t channel_tx, uint8_t channel_rx)
{
    g_dma_trans_tx[bus].channel = 0;
    g_dma_trans_rx[bus].channel = 0;
    uapi_dma_end_transfer(channel_rx);
    uapi_dma_end_transfer(channel_tx);
}

static errcode_t spi_writeread_dma(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    uint8_t channel_rx;
    uint8_t channel_tx;

    if (data->tx_buff == NULL || data->tx_bytes == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    if (spi_read_dma_config(bus, &channel_rx, data) != ERRCODE_SUCC) {
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (uapi_dma_start_transfer(channel_rx) != ERRCODE_SUCC) {
        g_dma_trans_rx[bus].channel = 0;
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (spi_write_dma_config(bus, &channel_tx, data) != ERRCODE_SUCC) {
        uapi_dma_end_transfer(channel_rx);
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (uapi_dma_start_transfer(channel_tx) != ERRCODE_SUCC) {
        uapi_dma_end_transfer(channel_rx);
        g_dma_trans_tx[bus].channel = 0;
        return ERRCODE_SPI_DMA_CONFIG_ERROR;
    }

    if (osal_sem_down_timeout(&(g_dma_trans_tx[bus].dma_sem), timeout) != OSAL_SUCCESS) {
        spi_writeread_dma_clear_trans(bus, channel_tx, channel_rx);
        return ERRCODE_SPI_DMA_TRANSFER_ERROR;
    }

    if (osal_sem_down_timeout(&(g_dma_trans_rx[bus].dma_sem), timeout) != OSAL_SUCCESS) {
        spi_writeread_dma_clear_trans(bus, channel_tx, channel_rx);
        return ERRCODE_SPI_DMA_TRANSFER_ERROR;
    }

    spi_writeread_dma_clear_trans(bus, channel_tx, channel_rx);

    if ((!g_dma_trans_tx[bus].trans_succ) || (!g_dma_trans_rx[bus].trans_succ)) {
        return ERRCODE_SPI_DMA_TRANSFER_ERROR;
    }

    return ERRCODE_SUCC;
}
#endif  /* CONFIG_SPI_SUPPORT_DMA */

errcode_t uapi_spi_set_tmod(spi_bus_t bus, hal_spi_trans_mode_t tmod, uint8_t data_frame_num)
{
    if (bus >= SPI_BUS_MAX_NUM || tmod >= HAL_SPI_TRANS_MODE_MAX) {
        return ERRCODE_INVALID_PARAM;
    }
    spi_attr_t attr;
    attr.ndf = data_frame_num;
    attr.tmod = tmod;
    return hal_spi_ctrl(bus, SPI_CTRL_SET_TMOD, (uintptr_t)&attr);
}

errcode_t uapi_spi_set_attr(spi_bus_t bus, spi_attr_t *attr)
{
    if (bus >= SPI_BUS_MAX_NUM || attr == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    return hal_spi_ctrl(bus, SPI_CTRL_SET_ATTR, (uintptr_t)attr);
}

errcode_t uapi_spi_get_attr(spi_bus_t bus, spi_attr_t *attr)
{
    if (bus >= SPI_BUS_MAX_NUM || attr == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    return hal_spi_ctrl(bus, SPI_CTRL_GET_ATTR, (uintptr_t)attr);
}

errcode_t uapi_spi_set_extra_attr(spi_bus_t bus, spi_extra_attr_t *extra_attr)
{
    if (bus >= SPI_BUS_MAX_NUM || extra_attr == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    return hal_spi_ctrl(bus, SPI_CTRL_SET_EXTRA_ATTR, (uintptr_t)extra_attr);
}

errcode_t uapi_spi_get_extra_attr(spi_bus_t bus, spi_extra_attr_t *extra_attr)
{
    if (bus >= SPI_BUS_MAX_NUM || extra_attr == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

    return hal_spi_ctrl(bus, SPI_CTRL_GET_EXTRA_ATTR, (uintptr_t)extra_attr);
}

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
errcode_t uapi_spi_set_irq_mode(spi_bus_t bus, bool irq_en, spi_rx_callback_t rx_callback,
                                spi_tx_callback_t tx_callback)
{
    if (bus >= SPI_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    if (g_dma_trans_tx[bus].is_enable || g_dma_trans_rx[bus].is_enable) {
        return ERRCODE_SPI_DMA_IRQ_MODE_MUTEX;
    }
#endif  /* CONFIG_SPI_SUPPORT_DMA */
    uint32_t irq_sts = spi_porting_lock(bus);
    if (irq_en) {
        g_spi_rx_state[bus].rx_callback = rx_callback;
        g_spi_tx_state[bus].tx_callback = tx_callback;
    } else {
        g_spi_rx_state[bus].rx_callback = NULL;
        g_spi_tx_state[bus].tx_callback = NULL;
    }

    g_spi_rx_int_flag[bus] = irq_en;
    g_spi_tx_int_flag[bus] = irq_en;
    spi_porting_unlock(bus, irq_sts);

    return ERRCODE_SUCC;
}

static void spi_data_send(spi_bus_t bus)
{
    bool tx_fifo_full = true;
    hal_spi_attr_t attr; /* will copy attr from hal, no need init to 0. */

    hal_spi_ctrl(bus, SPI_CTRL_GET_ATTR, (uintptr_t)&attr);
    uint32_t frame_bytes = hal_spi_frame_size_trans_to_frame_bytes(attr.frame_size);

    if (g_spi_tx_state[bus].current_tx_fragment_pos == 0) {
        if (hal_spi_ctrl(bus, SPI_CTRL_CHECK_FIFO_BUSY, (uintptr_t)SPI_TX_FIFO_BUSY_TIMEOUT) != ERRCODE_SUCC) {
            return;
        }
    }

    hal_spi_ctrl(bus, SPI_CTRL_CHECK_TX_FIFO_FULL, (uintptr_t)&tx_fifo_full);
    /* Populate the SPI TX FIFO if there is data to send */
    while (!tx_fifo_full) {
        /* There is some data to transmit so provide another byte to the SPI */
        bool end_of_fragment = spi_helper_send_next_char(bus, frame_bytes);
        if (end_of_fragment) {
            /* If it is the end of the fragment invoke the callback and move to the next one */
            spi_helper_invoke_current_fragment_callback(bus);
            spi_helper_move_to_next_fragment(bus);
            /* As it was the only fragment leave */
            break;
        }

        hal_spi_ctrl(bus, SPI_CTRL_CHECK_TX_FIFO_FULL, (uintptr_t)&tx_fifo_full);
    }
}

static errcode_t spi_write_int(spi_bus_t bus, const spi_xfer_data_t *data)
{
    if (data->tx_buff == NULL || data->tx_bytes == 0) {
        return ERRCODE_INVALID_PARAM;
    }
    uint32_t irq_sts = spi_porting_lock(bus);

    if (!spi_helper_add_fragment(bus, data->tx_buff, data->tx_bytes)) {
        spi_porting_unlock(bus, irq_sts);
        return ERRCODE_SPI_ADD_QUEUE_FAIL;
    }
    /* If it is the first on the list process it */
    /* No other fragments require transmission so start the transmission */
    if (spi_helper_is_the_current_fragment_the_last_to_process(bus)) {
        spi_data_send(bus);
        /* if we have not finished transmitting it enable the interrupts */
        if (spi_helper_are_there_fragments_to_process(bus)) {  /* if it is not finished transmitting it */
            hal_spi_ctrl(bus, SPI_CTRL_EN_TXEI_INT, 1);
        }
    }
    spi_porting_unlock(bus, irq_sts);
    return ERRCODE_SUCC;
}

static errcode_t spi_register_rx_callback(spi_bus_t bus, const spi_xfer_data_t *data)
{
    spi_attr_t attr; /* uapi_spi_get_attr will copy attr from hal, no need init to 0. */
    errcode_t ret = ERRCODE_SUCC;
    if (uapi_spi_get_attr(bus, &attr) != ERRCODE_SUCC) {
        return ERRCODE_SPI_CONFIG_FAIL;
    }
    uint32_t irq_sts = spi_porting_lock(bus);
    g_spi_rx_state[bus].rx_buffer = data->rx_buff;
    g_spi_rx_state[bus].rx_buffer_size = data->rx_bytes;
    hal_spi_ctrl(bus, SPI_CTRL_EN_RXFI_INT, 1);

    if (spi_porting_get_device_mode(bus) == SPI_MODE_MASTER && attr.tmod == HAL_SPI_TRANS_MODE_RX) {
        uint32_t frame_bytes = hal_spi_frame_size_trans_to_frame_bytes(attr.frame_size);
        if (frame_bytes == 0) {
            return ERRCODE_SPI_CONFIG_FAIL;
        }
        attr.ndf = g_spi_rx_state[bus].rx_buffer_size / frame_bytes;
        if (uapi_spi_set_attr(bus, &attr) != ERRCODE_SUCC) {
            return ERRCODE_SPI_CONFIG_FAIL;
        }
        uint32_t data_tx = 0;
        spi_xfer_data_t data_write = { 0 };
        data_write.tx_buff = (uint8_t *)(uintptr_t)&data_tx;
        data_write.tx_bytes = frame_bytes;
        ret = hal_spi_write(bus, (hal_spi_xfer_data_t *)&data_write, CONFIG_SPI_MAX_TIMEOUT);
    }
    spi_porting_unlock(bus, irq_sts);

    return ret;
}
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */

#if defined(CONFIG_SPI_SUPPORT_MASTER) && (CONFIG_SPI_SUPPORT_MASTER == 1)
errcode_t uapi_spi_select_slave(spi_bus_t bus, spi_slave_t cs)
{
    errcode_t ret;

    if (bus >= SPI_BUS_MAX_NUM || cs >= SPI_SLAVE_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    if (spi_porting_get_device_mode(bus) != SPI_MODE_MASTER) {
        return ERRCODE_SPI_MODE_MISMATCH;
    }

    uint32_t irq_sts = spi_porting_lock(bus);

    ret = hal_spi_ctrl(bus, SPI_CTRL_SELECT_SLAVE, (uintptr_t)cs);
    spi_porting_unlock(bus, irq_sts);

    return ret;
}

errcode_t uapi_spi_master_write(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    errcode_t ret;
    uint32_t timeout_tmp = timeout == 0 ? CONFIG_SPI_MAX_TIMEOUT : timeout;

    ret = spi_param_check_write(bus, SPI_MODE_MASTER, HAL_SPI_TRANS_MODE_RX, data, true);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (data->tx_bytes > CONFIG_SPI_AUTO_SWITCH_DMA_THRESHOLD) {
        uapi_spi_set_dma_mode(bus, true, NULL);
        ret = spi_write_dma(bus, data, timeout_tmp);
        if (hal_spi_ctrl(bus, SPI_CTRL_CHECK_FIFO_BUSY, (uintptr_t)SPI_TX_FIFO_BUSY_TIMEOUT) != ERRCODE_SUCC) {
            spi_mutex_unlock(bus);
            return ERRCODE_SPI_TIMEOUT;
        }
        spi_mutex_unlock(bus);
        return ret;
    }
    uapi_spi_set_dma_mode(bus, false, NULL);
#else
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    if (g_dma_trans_tx[bus].is_enable) {
        ret = spi_write_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_DMA */
#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (g_spi_tx_int_flag[bus]) {
        ret = spi_write_int(bus, data);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

    uint32_t irq_sts = spi_porting_lock(bus);
    ret = hal_spi_write(bus, (hal_spi_xfer_data_t *)data, timeout_tmp);
    spi_porting_unlock(bus, irq_sts);
    spi_mutex_unlock(bus);

    return ret;
}

#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
static errcode_t spi_read_by_writeread(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    spi_attr_t attr; /* uapi_spi_get_attr will copy attr from hal, no need init to 0. */
    uint32_t data_tx = 0;
    spi_xfer_data_t data_writeread = { 0 };

    uapi_spi_get_attr(bus, &attr);
    uint32_t ndf_before = attr.ndf;
    uint32_t frame_bytes = hal_spi_frame_size_trans_to_frame_bytes(attr.frame_size);
    if (frame_bytes == 0) {
        return ERRCODE_SPI_CONFIG_FAIL;
    }
    attr.ndf = data->rx_bytes / frame_bytes;
    if (uapi_spi_set_attr(bus, &attr) != ERRCODE_SUCC) {
        return ERRCODE_SPI_CONFIG_FAIL;
    }
    data_writeread.rx_buff = data->rx_buff;
    data_writeread.rx_bytes = data->rx_bytes;
    data_writeread.tx_buff = (uint8_t *)(uintptr_t)&data_tx;
    data_writeread.tx_bytes = frame_bytes;

    errcode_t ret = uapi_spi_master_writeread(bus, &data_writeread, timeout);

    attr.ndf = ndf_before;
    if (uapi_spi_set_attr(bus, &attr) != ERRCODE_SUCC) {
        return ERRCODE_SPI_CONFIG_FAIL;
    }

    return ret;
}

#if !defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH)
static errcode_t spi_master_read_dma(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    if (spi_get_attr_tmod(bus) == HAL_SPI_TRANS_MODE_RX) {
        return spi_read_by_writeread(bus, data, timeout);
    }
    return spi_read_dma(bus, data, timeout);
}
#endif  /* NOT CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */
#endif  /* CONFIG_SPI_SUPPORT_DMA */

errcode_t uapi_spi_master_read(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    errcode_t ret;
    uint32_t timeout_tmp = timeout == 0 ? CONFIG_SPI_MAX_TIMEOUT : timeout;

    ret = spi_param_check_read(bus, SPI_MODE_MASTER, HAL_SPI_TRANS_MODE_TX, data, true);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (data->rx_bytes > CONFIG_SPI_AUTO_SWITCH_DMA_THRESHOLD) {
        /* When the threshold is exceeded, the writeread interface should be used to write data and then read data. */
        ret = spi_read_by_writeread(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
    uapi_spi_set_dma_mode(bus, false, NULL);
#else
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    if (g_dma_trans_rx[bus].is_enable) {
        ret = spi_master_read_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_DMA */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (g_spi_rx_int_flag[bus]) {
        ret = spi_register_rx_callback(bus, data);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

    uint32_t irq_sts = spi_porting_lock(bus);
    ret = hal_spi_read(bus, (hal_spi_xfer_data_t *)data, timeout_tmp);
    spi_porting_unlock(bus, irq_sts);
    spi_mutex_unlock(bus);

    return ret;
}

errcode_t uapi_spi_master_writeread(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    errcode_t ret;
    uint32_t timeout_tmp = timeout == 0 ? CONFIG_SPI_MAX_TIMEOUT : timeout;

    ret = spi_param_check_read(bus, SPI_MODE_MASTER, HAL_SPI_TRANS_MODE_TX, data, true);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (data->rx_bytes > CONFIG_SPI_AUTO_SWITCH_DMA_THRESHOLD ||
        data->tx_bytes > CONFIG_SPI_AUTO_SWITCH_DMA_THRESHOLD) {
        uapi_spi_set_dma_mode(bus, true, NULL);
        ret = spi_writeread_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
    uapi_spi_set_dma_mode(bus, false, NULL);
#else
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    if (g_dma_trans_tx[bus].is_enable && g_dma_trans_rx[bus].is_enable) {
        ret = spi_writeread_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_DMA */
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

    uint32_t irq_sts = spi_porting_lock(bus);
    ret = hal_spi_write(bus, (hal_spi_xfer_data_t *)data, timeout_tmp);
    ret = hal_spi_read(bus, (hal_spi_xfer_data_t *)data, timeout_tmp);
    spi_porting_unlock(bus, irq_sts);
    spi_mutex_unlock(bus);
    return ret;
}
#endif  /* CONFIG_SPI_SUPPORT_MASTER */

#if defined(CONFIG_SPI_SUPPORT_SLAVE) && (CONFIG_SPI_SUPPORT_SLAVE == 1)
errcode_t uapi_spi_slave_write(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    errcode_t ret;
    uint32_t timeout_tmp = timeout == 0 ? CONFIG_SPI_MAX_TIMEOUT : timeout;

    if (data == NULL || data->tx_buff == NULL || data->tx_bytes == 0) {
        return ERRCODE_INVALID_PARAM;
    }

    ret = spi_param_check_write(bus, SPI_MODE_SLAVE, HAL_SPI_TRANS_MODE_RX, data, true);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (data->tx_bytes > CONFIG_SPI_AUTO_SWITCH_DMA_THRESHOLD) {
        uapi_spi_set_dma_mode(bus, true, NULL);
        ret = spi_write_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
    uapi_spi_set_dma_mode(bus, false, NULL);
#else
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    if (g_dma_trans_tx[bus].is_enable) {
        ret = spi_write_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_DMA */
#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (g_spi_tx_int_flag[bus]) {
        ret = spi_write_int(bus, data);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

    uint32_t irq_sts = spi_porting_lock(bus);
    ret = hal_spi_write(bus, (hal_spi_xfer_data_t *)data, timeout_tmp);
    spi_porting_unlock(bus, irq_sts);
    spi_mutex_unlock(bus);

    return ret;
}

errcode_t uapi_spi_slave_read(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    errcode_t ret;
    uint32_t timeout_tmp = timeout == 0 ? CONFIG_SPI_MAX_TIMEOUT : timeout;

    ret = spi_param_check_read(bus, SPI_MODE_SLAVE, HAL_SPI_TRANS_MODE_TX, data, false);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (data->rx_bytes > CONFIG_SPI_AUTO_SWITCH_DMA_THRESHOLD) {
        uapi_spi_set_dma_mode(bus, true, NULL);
        ret = spi_read_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
    uapi_spi_set_dma_mode(bus, false, NULL);
#else
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    if (g_dma_trans_rx[bus].is_enable) {
        ret = spi_read_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_DMA */

#if defined(CONFIG_SPI_SUPPORT_INTERRUPT) && (CONFIG_SPI_SUPPORT_INTERRUPT == 1)
    if (g_spi_rx_int_flag[bus]) {
        ret = spi_register_rx_callback(bus, data);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_INTERRUPT */
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

    uint32_t irq_sts = spi_porting_lock(bus);
    ret = hal_spi_read(bus, (hal_spi_xfer_data_t *)data, timeout_tmp);
    spi_porting_unlock(bus, irq_sts);
    spi_mutex_unlock(bus);

    return ret;
}

errcode_t uapi_spi_slave_writeread(spi_bus_t bus, const spi_xfer_data_t *data, uint32_t timeout)
{
    errcode_t ret;
    uint32_t timeout_tmp = timeout == 0 ? CONFIG_SPI_MAX_TIMEOUT : timeout;

    ret = spi_param_check_read(bus, SPI_MODE_SLAVE, HAL_SPI_TRANS_MODE_TX, data, false);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

#if defined(CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH) && (CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH == 1)
    if (data->rx_bytes > CONFIG_SPI_AUTO_SWITCH_DMA_THRESHOLD ||
        data->tx_bytes > CONFIG_SPI_AUTO_SWITCH_DMA_THRESHOLD) {
        uapi_spi_set_dma_mode(bus, true, NULL);
        ret = spi_writeread_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
    uapi_spi_set_dma_mode(bus, false, NULL);
#else
#if defined(CONFIG_SPI_SUPPORT_DMA) && (CONFIG_SPI_SUPPORT_DMA == 1)
    if (g_dma_trans_tx[bus].is_enable && g_dma_trans_rx[bus].is_enable) {
        ret = spi_writeread_dma(bus, data, timeout_tmp);
        spi_mutex_unlock(bus);
        return ret;
    }
#endif  /* CONFIG_SPI_SUPPORT_DMA */
#endif  /* CONFIG_SPI_SUPPORT_POLL_AND_DMA_AUTO_SWITCH */

    uint32_t irq_sts = spi_porting_lock(bus);
    ret = hal_spi_write(bus, (hal_spi_xfer_data_t *)data, timeout_tmp);
    ret = hal_spi_read(bus, (hal_spi_xfer_data_t *)data, timeout_tmp);
    hal_spi_ctrl(bus, SPI_CTRL_CHECK_FIFO_BUSY, timeout_tmp);
    spi_porting_unlock(bus, irq_sts);
    spi_mutex_unlock(bus);
    return ret;
}
#endif  /* CONFIG_SPI_SUPPORT_SLAVE */

#if defined(CONFIG_SPI_SUPPORT_LOOPBACK)
errcode_t uapi_spi_set_loop_back_mode(spi_bus_t bus, bool loopback_en)
{
    unused(bus);
    unused(loopback_en);
    return ERRCODE_SUCC;
}
#endif  /* CONFIG_SPI_SUPPORT_LOOPBACK */

#if defined(CONFIG_SPI_SUPPORT_CRC)
errcode_t uapi_spi_set_crc_mode(spi_bus_t bus, const spi_crc_config_t *crc_config, spi_crc_err_callback_t cb)
{
    unused(bus);
    unused(crc_config);
    unused(cb);
    return ERRCODE_SUCC;
}
#endif

#if defined(CONFIG_SPI_SUPPORT_LPM)
errcode_t uapi_spi_suspend(uintptr_t arg)
{
    if (!g_spi_is_initialised[arg]) {
        return ERRCODE_SUCC;
    }
    if (hal_spi_ctrl(arg, SPI_CTRL_SUSPEND, 0) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }
#if defined(CONFIG_SPI_SUPPORT_LPC)
    spi_port_clock_enable(arg, false);
#endif
#if defined(CONFIG_SPI_SUPPORT_DMA)
    uapi_dma_suspend(arg);
#endif

    return ERRCODE_SUCC;
}

errcode_t uapi_spi_resume(uintptr_t arg)
{
    if (!g_spi_is_initialised[arg]) {
        return ERRCODE_SUCC;
    }
#if defined(CONFIG_SPI_SUPPORT_LPC)
    spi_port_clock_enable(arg, true);
#endif

    if (hal_spi_ctrl(arg, SPI_CTRL_RESUME, 0) != ERRCODE_SUCC) {
        return ERRCODE_FAIL;
    }

#if defined(CONFIG_SPI_SUPPORT_DMA)
    uapi_dma_resume(arg);
#endif
    return ERRCODE_SUCC;
}
#endif