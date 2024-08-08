/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2022-2022. All rights reserved.
 *
 * Description: Provides uart driver source \n
 *
 * History: \n
 * 2022-06-09, Create file. \n
 */
#include "uart.h"
#include <stdbool.h>
#include "common_def.h"
#include "soc_osal.h"
#include "securec.h"
#if defined(CONFIG_UART_SUPPORT_DMA)
#include "dma_porting.h"
#include "dma.h"
#endif

/**
 * @brief  Maximum number of fragments per uart for transmission.
 * Configurable field that specifies the maximum numbers of transmissions the driver can queue
 * before it returns false on the write requests.
 */
#define UART_MAX_NUMBER_OF_FRAGMENTS 4

/**
 * @brief  Uart parity error bit mask
 */
#define UART_PARITY_ERROR_MASK BIT(8)

/**
 * @brief  Uart frame error bit mask.
 */
#define UART_FRAME_ERROR_MASK BIT(7)

#if defined(CONFIG_UART_SUPPORT_TX)
/**
 * @brief  A fragment of data that is to be transmitted.
 */
typedef struct {
    uint8_t *data;
    void *params;
    uart_tx_callback_t release_func;
    uint32_t data_length;
} uart_tx_fragment_t;

/**
 * @brief  The UART transmission configuration parameters.
 */
typedef struct {
    uart_tx_fragment_t *current_tx_fragment;        /*!< Current TX fragment being transmitted. */
    uart_tx_fragment_t *free_tx_fragment;           /*!< The unused TX fragment admin blocks available
                                                         for re-use/freeing. */
    uint16_t fragments_to_process;                  /*!< Number of fragments to process including the current one. */
    uint16_t current_tx_fragment_pos;               /*!< Index of the current position of the next byte to be
                                                         transmitted in the current TX fragment
                                                         current_tx_fragment_pos == 0 means
                                                         the first byte is yet to be sent for transmission */
    uart_tx_fragment_t fragment_buffer[UART_MAX_NUMBER_OF_FRAGMENTS]; /*!< Fragments buffer pointer. */
} uart_tx_state_t;

/**
 * @brief  Internal UART TX configuration.
 */
#if defined(CONFIG_UART_SUPPORT_TX_INT)
static uart_tx_state_t g_uart_tx_state_array[UART_BUS_MAX_NUM];
#endif

#if defined(CONFIG_UART_SUPPORT_DMA)
#define DMA_UART_TRANSFER_TIMEOUT_MS 1000
#define UART_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA 1
#define UART_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA 2
#define UART_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL 0
#define UART_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM 1
#define UART_DMA_ADDRESS_INC_INCREMENT 0
#define UART_DMA_ADDRESS_INC_NO_CHANGE 2
#define UART_DMA_PROTECTION_CONTROL_BUFFERABLE 1

typedef struct uart_dma_trans_inf {
    bool inited;
    bool trans_succ;
    uint8_t channel;
    uint8_t reserved;
    osal_semaphore dma_sem;
} uart_dma_trans_inf_t;

static uart_dma_trans_inf_t g_dma_trans[UART_BUS_MAX_NUM] = { 0 };

static void uart_dma_set_config(uart_bus_t bus, const uart_extra_attr_t *extra_attr);
#endif  /* CONFIG_UART_SUPPORT_DMA */

#endif  /* CONFIG_UART_SUPPORT_TX */

#if defined(CONFIG_UART_SUPPORT_RX)
/**
 * @brief  The UART reception configuration parameters.
 */
typedef struct {
    uart_rx_callback_t rx_callback;                 /*!< The RX callback to make when the condition is met. */
    uart_error_callback_t parity_error_callback;    /*!< The parity error callback. */
    uart_error_callback_t frame_error_callback;     /*!< The frame error callback. */
    uint8_t *rx_buffer;                             /*!< The RX data buffer. */
    uint16_t rx_buffer_size;                        /*!< The size of the receive buffer. */
    uint16_t rx_condition_size;                     /*!< The size relating the condition. */
    uint16_t new_rx_pos;                            /*!< Index to the position in the RX buffer that is where new data
                                                         should be put if (new_rx_pos == 0) the buffer is empty. */
    uart_rx_condition_t rx_condition;               /*!< The condition under which an RX callback is made. */
} uart_rx_state_t;

/**
 * @brief  Internal UART RX configuration.
 */
static uart_rx_state_t g_uart_rx_state_array[UART_BUS_MAX_NUM];

#endif  /* CONFIG_UART_SUPPORT_RX */

static bool g_uart_inited[UART_BUS_MAX_NUM] = { false };

#if defined(CONFIG_UART_SUPPORT_RX)
static bool uart_config_rx_state(uart_bus_t bus, const uart_buffer_config_t *uart_buffer_config);
#endif  /* CONFIG_UART_SUPPORT_RX */

#if defined(CONFIG_UART_SUPPORT_TX)
#if defined(CONFIG_UART_SUPPORT_TX_INT)
static void uart_config_tx_state(uart_bus_t bus);
#endif  /* CONFIG_UART_SUPPORT_TX_INIT */
#endif  /* CONFIG_UART_SUPPORT_TX */

#if defined(CONFIG_UART_SUPPORT_RX) || defined(CONFIG_UART_SUPPORT_TX)
static void uart_deconfig_state(uart_bus_t bus);
#endif  /* defined(CONFIG_UART_SUPPORT_RX) || defined(CONFIG_UART_SUPPORT_TX) */

#if defined(CONFIG_UART_SUPPORT_TX)
#if defined(CONFIG_UART_SUPPORT_TX_INT)
static bool uart_helper_add_fragment(uart_bus_t bus, const uint8_t *buffer,
                                     uint32_t length, void *params,
                                     uart_tx_callback_t finished_with_buffer_func);
static inline bool uart_helper_is_the_current_fragment_the_last_to_process(uart_bus_t bus);

static inline bool uart_helper_are_there_fragments_to_process(uart_bus_t bus);

static inline bool uart_helper_send_next_char(uart_bus_t bus);

static inline void uart_helper_invoke_current_fragment_callback(uart_bus_t bus);

static inline void uart_helper_move_to_next_fragment(uart_bus_t bus);
#endif  /* CONFIG_UART_SUPPORT_TX_INIT */
#endif  /* CONFIG_UART_SUPPORT_TX */

#if defined(CONFIG_UART_SUPPORT_RX)
static inline void uart_rx_buffer_release(uart_bus_t bus);

static inline bool uart_rx_buffer_has_free_space(uart_bus_t bus);

static inline uint16_t uart_rx_buffer_data_available(uart_bus_t bus);
#endif  /* CONFIG_UART_SUPPORT_RX */

static int32_t uart_check_params_attr(const uart_attr_t *attr);

static int32_t uart_init_check_params(uart_bus_t bus, const uart_pin_config_t *pins, const uart_attr_t *attr);

static void uart_claim_pins(uart_bus_t bus, const uart_pin_config_t *pins);

static void uart_release_pins(uart_bus_t bus);

#if defined(CONFIG_UART_SUPPORT_RX)
static void uart_idle_isr(uart_bus_t bus);

static void uart_rx_isr(uart_bus_t bus);

static void uart_error_isr(uart_bus_t bus);
#endif  /* CONFIG_UART_SUPPORT_RX */

#if defined(CONFIG_UART_SUPPORT_TX_INT)
static void uart_tx_isr(uart_bus_t bus);
#endif  /* CONFIG_UART_SUPPORT_TX_INIT */

#if defined(CONFIG_UART_SUPPORT_LPM)
static bool g_uart_suspend_flag[UART_BUS_MAX_NUM] = { false };
static uart_pin_config_t g_uart_pins[UART_BUS_MAX_NUM] = { 0 };
static uart_attr_t g_uart_attr[UART_BUS_MAX_NUM] = { 0 };
static uart_extra_attr_t g_uart_extra_attr[UART_BUS_MAX_NUM] = { 0 };
static uart_buffer_config_t g_uart_buffer_config[UART_BUS_MAX_NUM] = { 0 };
static uart_rx_condition_t g_uart_condition[UART_BUS_MAX_NUM] = { 0 };
static uint32_t g_uart_size[UART_BUS_MAX_NUM] = { 0 };
static uart_rx_callback_t g_uart_callback[UART_BUS_MAX_NUM] = { 0 };
#endif  /* CONFIG_UART_SUPPORT_LPM */

static errcode_t uart_evt_callback(uart_bus_t bus, hal_uart_evt_id_t evt, uintptr_t param);

errcode_t uapi_uart_init(uart_bus_t bus, const uart_pin_config_t *pins, const uart_attr_t *attr,
                         const uart_extra_attr_t *extra_attr, uart_buffer_config_t *uart_buffer_config)
{
    unused(uart_buffer_config);
    if (uart_init_check_params(bus, pins, attr) != 0) {
        return ERRCODE_INVALID_PARAM;
    }
    if (g_uart_inited[bus]) { return ERRCODE_SUCC; }
#if defined(CONFIG_UART_SUPPORT_LPM)
    if (g_uart_suspend_flag[bus] == false) {
        (void)memcpy_s(&g_uart_pins[bus], sizeof(uart_pin_config_t), pins, sizeof(uart_pin_config_t));
        (void)memcpy_s(&g_uart_attr[bus], sizeof(uart_attr_t), attr, sizeof(uart_attr_t));
        (void)memcpy_s(&g_uart_extra_attr[bus], sizeof(uart_extra_attr_t), extra_attr, sizeof(uart_extra_attr_t));
        (void)memcpy_s(&g_uart_buffer_config[bus], sizeof(uart_buffer_config_t), uart_buffer_config,
            sizeof(uart_buffer_config_t));
    }
#endif  /* CONFIG_UART_SUPPORT_LPM */
#if defined(CONFIG_UART_SUPPORT_LPC)
    uart_port_clock_enable(bus, true);
#endif
    uart_claim_pins(bus, pins);

#if defined(CONFIG_UART_SUPPORT_RX)
    if (uart_config_rx_state(bus, uart_buffer_config) == false) {
        return ERRCODE_UART_INIT_TRX_STATE_FAIL;
    }
#endif  /* CONFIG_UART_SUPPORT_RX */
#if defined(CONFIG_UART_SUPPORT_TX)
#if defined(CONFIG_UART_SUPPORT_TX_INT)
    uart_config_tx_state(bus);
#endif  /* CONFIG_UART_SUPPORT_TX_INIT */
#endif  /* CONFIG_UART_SUPPORT_TX */
    uint8_t flow_ctrl = UART_FLOW_CTRL_SOFT;
#if defined(CONFIG_UART_SUPPORT_FLOW_CTRL)
    flow_ctrl = attr->flow_ctrl;
#endif  /* CONFIG_UART_SUPPORT_FLOW_CTRL */
    errcode_t ret = hal_uart_init(bus, uart_evt_callback, (hal_uart_pin_config_t *)pins, (hal_uart_attr_t *)attr,
                                  (hal_uart_flow_ctrl_t)flow_ctrl);
    if (ret != ERRCODE_SUCC) { return ret; }

#if defined(CONFIG_UART_SUPPORT_DMA)
    if ((extra_attr != NULL) && (extra_attr->tx_dma_enable || extra_attr->rx_dma_enable)) {
        uart_dma_set_config(bus, extra_attr);
    }
#else
    unused(extra_attr);
#endif  /* CONFIG_UART_SUPPORT_DMA */
    g_uart_inited[bus] = true;
    uart_port_register_irq(bus);
    return ret;
}

errcode_t uapi_uart_deinit(uart_bus_t bus)
{
    errcode_t ret = ERRCODE_FAIL;
    if (bus >= UART_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    if (!g_uart_inited[bus]) {
        return ERRCODE_SUCC;
    }
    ret = hal_uart_deinit(bus);

    uart_port_unregister_irq(bus);

#if defined(CONFIG_UART_SUPPORT_RX) || defined(CONFIG_UART_SUPPORT_TX)
    uart_deconfig_state(bus);
#endif  /* defined(CONFIG_UART_SUPPORT_RX) || defined(CONFIG_UART_SUPPORT_TX) */

    uart_release_pins(bus);

#if defined(CONFIG_UART_SUPPORT_DMA)
    if (g_dma_trans[bus].inited) {
        osal_sem_destroy(&(g_dma_trans[bus].dma_sem));
        g_dma_trans[bus].inited = false;
    }
#endif  /* CONFIG_UART_SUPPORT_DMA */
#if defined(CONFIG_UART_SUPPORT_LPC)
    uart_port_clock_enable(bus, false);
#endif
    g_uart_inited[bus] = false;
    return ret;
}

errcode_t uapi_uart_set_attr(uart_bus_t bus, const uart_attr_t *attr)
{
    if (bus >= UART_BUS_MAX_NUM) {
        return ERRCODE_INVALID_PARAM;
    }
    if ((uart_check_params_attr(attr)) != 0) {
        return ERRCODE_INVALID_PARAM;
    }
    uint32_t irq_sts = uart_porting_lock(bus);
    errcode_t ret = hal_uart_ctrl(bus, UART_CTRL_SET_ATTR, (uintptr_t)attr);
    uart_porting_unlock(bus, irq_sts);
    return ret;
}

errcode_t uapi_uart_get_attr(uart_bus_t bus, const uart_attr_t *attr)
{
    if (bus >= UART_BUS_MAX_NUM || attr == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    uint32_t irq_sts = uart_porting_lock(bus);
    errcode_t ret = hal_uart_ctrl(bus, UART_CTRL_GET_ATTR, (uintptr_t)attr);
    uart_porting_unlock(bus, irq_sts);
    return ret;
}

#if defined(CONFIG_UART_SUPPORT_RX)
errcode_t uapi_uart_register_rx_callback(uart_bus_t bus, uart_rx_condition_t condition,
                                         uint32_t size, uart_rx_callback_t callback)
{
    errcode_t ret = ERRCODE_FAIL;

    if (bus >= UART_BUS_MAX_NUM || callback == NULL) {
        return ERRCODE_INVALID_PARAM;
    }

#if defined(CONFIG_UART_SUPPORT_LPM)
    if (g_uart_suspend_flag[bus] == false) {
        memcpy_s(&g_uart_condition[bus], sizeof(uart_rx_condition_t), &condition, sizeof(uart_rx_condition_t));
        memcpy_s(&g_uart_size[bus], sizeof(uint32_t), &size, sizeof(uint32_t));
        memcpy_s(&g_uart_callback[bus], sizeof(uart_rx_callback_t), &callback, sizeof(uart_rx_callback_t));
    }
#endif  /* CONFIG_UART_SUPPORT_LPM */

    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    uint32_t irq_sts = uart_porting_lock(bus);
    rx_state->rx_callback = callback;
    rx_state->rx_condition = condition;
#if !defined(CONFIG_UART_NOT_SUPPORT_RX_CONDITON_SIZE_OPTIMIZE)
    uint32_t uart_rx_fifo_thresh = 0;
    ret = hal_uart_ctrl(bus, UART_CTRL_GET_RX_FIFO_THRESHOLD, (uintptr_t)&uart_rx_fifo_thresh);
    size = size > uart_rx_fifo_thresh ? uart_rx_fifo_thresh : size;
#endif
    rx_state->rx_condition_size = (uint16_t)size;
    ret = hal_uart_ctrl(bus, UART_CTRL_EN_RX_INT, 1);
    ret = hal_uart_ctrl(bus, UART_CTRL_EN_FRAME_ERR_INT, 1);
    ret = hal_uart_ctrl(bus, UART_CTRL_EN_PARITY_ERR_INT, 1);
    ret = hal_uart_ctrl(bus, UART_CTRL_EN_IDLE_INT, 1);
    uart_porting_unlock(bus, irq_sts);

    return ret;
}

errcode_t uapi_uart_register_parity_error_callback(uart_bus_t bus, uart_error_callback_t callback)
{
    errcode_t ret = ERRCODE_FAIL;

    if (bus >= UART_BUS_MAX_NUM || callback == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    uint32_t irq_sts = uart_porting_lock(bus);
    rx_state->parity_error_callback = callback;
    ret = hal_uart_ctrl(bus, UART_CTRL_EN_PARITY_ERR_INT, 0);
    uart_porting_unlock(bus, irq_sts);

    return ret;
}

errcode_t uapi_uart_register_frame_error_callback(uart_bus_t bus, uart_error_callback_t callback)
{
    errcode_t ret = ERRCODE_FAIL;

    if (bus >= UART_BUS_MAX_NUM || callback == NULL) {
        return ERRCODE_INVALID_PARAM;
    }
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    uint32_t irq_sts = uart_porting_lock(bus);
    rx_state->frame_error_callback = callback;
    ret = hal_uart_ctrl(bus, UART_CTRL_EN_FRAME_ERR_INT, 0);
    uart_porting_unlock(bus, irq_sts);

    return ret;
}
#endif  /* CONFIG_UART_SUPPORT_RX */

static int32_t uapi_uart_param_check(uart_bus_t bus, const uint8_t *buffer, uint32_t length)
{
    if (bus >= UART_BUS_MAX_NUM || buffer == NULL || length == 0) {
        return ERRCODE_INVALID_PARAM;
    }
    if (!g_uart_inited[bus]) {
        return ERRCODE_UART_NOT_INIT;
    }

    return ERRCODE_SUCC;
}

#if defined(CONFIG_UART_SUPPORT_TX)
int32_t uapi_uart_write(uart_bus_t bus, const uint8_t *buffer, uint32_t length, uint32_t timeout)
{
    unused(timeout);
    bool tx_fifo_full = false;
    uint8_t *data_buffer = (uint8_t *)buffer;
    int32_t write_count = 0;
    uint32_t len = length;

    int32_t ret = uapi_uart_param_check(bus, buffer, length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uint32_t irq_sts = uart_porting_lock(bus);
    while (len > 0) {
        hal_uart_ctrl(bus, UART_CTRL_CHECK_TX_FIFO_FULL, (uintptr_t)&tx_fifo_full);
        if (tx_fifo_full == false) {
            hal_uart_write(bus, data_buffer++, 1);
            len--;
            write_count++;
        }
    }
    uart_porting_unlock(bus, irq_sts);

    return write_count;
}

#if defined(CONFIG_UART_SUPPORT_TX_INT)
static void uapi_uart_data_send(uart_bus_t bus)
{
    bool tx_fifo_full = false;
    hal_uart_ctrl(bus, UART_CTRL_CHECK_TX_FIFO_FULL, (uintptr_t)&tx_fifo_full);

    /* Populate the UART TX FIFO if there is data to send */
    while (tx_fifo_full == false) {
        /* There is some data to transmit so provide another byte to the UART */
        bool end_of_fragment = uart_helper_send_next_char(bus);
        if (end_of_fragment) {
            /* If it is the end of the fragment invoke the callback and move to the next one */
            uart_helper_invoke_current_fragment_callback(bus);
            uart_helper_move_to_next_fragment(bus);
            /* As it was the only fragment leave */
            break;
        }

        hal_uart_ctrl(bus, UART_CTRL_CHECK_TX_FIFO_FULL, (uintptr_t)&tx_fifo_full);
    }
}

errcode_t uapi_uart_write_int(uart_bus_t bus, const uint8_t *buffer, uint32_t length,
                              void *params, uart_tx_callback_t finished_with_buffer_func)
{
    errcode_t ret = (errcode_t)uapi_uart_param_check(bus, buffer, length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    uint32_t irq_sts = uart_porting_lock(bus);
    if (uart_helper_are_there_fragments_to_process(bus) == true) {
        uapi_uart_data_send(bus);
    }

    bool fragment_added = uart_helper_add_fragment(bus, buffer, length, params, finished_with_buffer_func);
    if (!fragment_added) {
        uart_porting_unlock(bus, irq_sts);
        return ERRCODE_UART_ADD_QUEUE_FAIL;
    }
    /* If it is the first on the list process it */
    if (uart_helper_is_the_current_fragment_the_last_to_process(bus) ==
        true) {  /* No other fragments require transmission so start the transmission */
        uapi_uart_data_send(bus);
        /* if we have not finished transmitting it enable the interrupts */
        if (uart_helper_are_there_fragments_to_process(bus) == true) {  /* if it is not finished transmitting it */
            hal_uart_ctrl(bus, UART_CTRL_EN_TX_INT, true);
        }
    }
    uart_porting_unlock(bus, irq_sts);
    return ERRCODE_SUCC;
}
#endif

#if defined(CONFIG_UART_SUPPORT_DMA)
static void uart_dma_set_config(uart_bus_t bus, const uart_extra_attr_t *extra_attr)
{
    hal_uart_set_dma_config(bus, (hal_uart_extra_attr_t *)extra_attr);
    (void)memset_s(&(g_dma_trans[bus].dma_sem), sizeof(g_dma_trans[bus].dma_sem), 0, sizeof(g_dma_trans[bus].dma_sem));
    (void)osal_sem_init(&(g_dma_trans[bus].dma_sem), 0);
    g_dma_trans[bus].inited = true;
}

static void uart_dma_isr(uint8_t int_type, uint8_t ch, uintptr_t arg)
{
    unused(arg);
    uint8_t bus = UART_BUS_MAX_NUM;
    for (uint8_t i = UART_BUS_0; i < UART_BUS_MAX_NUM; i++) {
        /* channel default value is 0, means not used. channel > 0 means used.
           So ch + 1 will not misjudgment with channel value 0. */
        if (g_dma_trans[i].channel == ch + 1) {
            bus = i;
            break;
        }
    }

    if (bus != UART_BUS_MAX_NUM) {
        if (int_type == 0) {
            g_dma_trans[bus].trans_succ = true;
        }
        osal_sem_up(&(g_dma_trans[bus].dma_sem));
    }
}

static int32_t uart_write_by_dma_config(uart_bus_t bus, const void *buffer, uint32_t length,
                                        uart_write_dma_config_t *dma_cfg,
                                        dma_ch_user_peripheral_config_t *user_cfg)
{
    uint32_t uart_data_addr = 0;
    errcode_t ret = hal_uart_ctrl(bus, UART_CTRL_GET_DMA_DATA_ADDR, (uintptr_t)&uart_data_addr);
    if (ret != ERRCODE_SUCC) {
        return -1;
    }
    user_cfg->src = (uint32_t)(uintptr_t)buffer;
    user_cfg->dest = uart_data_addr;
    user_cfg->transfer_num = (uint16_t)(length >> dma_cfg->src_width);
    user_cfg->src_handshaking = 0;
    user_cfg->trans_type = UART_DMA_TRANS_MEMORY_TO_PERIPHERAL_DMA;
    user_cfg->trans_dir = UART_DMA_TRANSFER_DIR_MEM_TO_PERIPHERAL;
    user_cfg->priority = dma_cfg->priority;
    user_cfg->src_width = dma_cfg->src_width;
    user_cfg->dest_width = dma_cfg->dest_width;
    user_cfg->burst_length = dma_cfg->burst_length;
    user_cfg->src_increment = UART_DMA_ADDRESS_INC_INCREMENT;
    user_cfg->dest_increment = UART_DMA_ADDRESS_INC_NO_CHANGE;
    user_cfg->protection = UART_DMA_PROTECTION_CONTROL_BUFFERABLE;
    user_cfg->dest_handshaking = uart_port_get_dma_trans_dest_handshaking(bus);
    return ERRCODE_SUCC;
}

static int32_t uapi_uart_dma_check(uart_bus_t bus, const void *buffer, uint32_t length,
                                   const uart_write_dma_config_t *dma_cfg)
{
    if ((bus >= UART_BUS_MAX_NUM) || (dma_cfg == NULL)) {
        return UART_DMA_CFG_PARAM_INVALID;
    }
    if ((buffer == NULL) || (length == 0)) {
        return UART_DMA_BUFF_NULL;
    }
    if (length % bit(dma_cfg->src_width) != 0) {
        return UART_DMA_CFG_PARAM_INVALID;
    }
    return 0;
}

int32_t uapi_uart_write_by_dma(uart_bus_t bus, const void *buffer, uint32_t length, uart_write_dma_config_t *dma_cfg)
{
    int32_t ret = uapi_uart_dma_check(bus, buffer, length, dma_cfg);
    if (ret != 0) {
        return ret;
    }

    dma_ch_user_peripheral_config_t user_cfg = {0};

    ret = uart_write_by_dma_config(bus, buffer, length, dma_cfg, &user_cfg);
    if (ret != ERRCODE_SUCC || user_cfg.dest_handshaking == HAL_DMA_HANDSHAKING_MAX_NUM) {
        return UART_DMA_SHAKING_INVALID_OR_UART_FUNCS_NULL;
    }

    uint8_t dma_ch;
    if (uapi_dma_configure_peripheral_transfer_single(&user_cfg, &dma_ch,
        uart_dma_isr, (uintptr_t)NULL) != ERRCODE_SUCC) {
        return UART_DMA_CONFIGURE_FAIL;
    }

    g_dma_trans[bus].channel = dma_ch + 1;
    g_dma_trans[bus].trans_succ = false;

    if (uapi_dma_start_transfer(dma_ch) != ERRCODE_SUCC) {
        g_dma_trans[bus].channel = 0;
        return UART_DMA_START_TRANSFER_FAIL;
    }

    if (osal_sem_down_timeout(&(g_dma_trans[bus].dma_sem), DMA_UART_TRANSFER_TIMEOUT_MS) != OSAL_SUCCESS) {
        g_dma_trans[bus].channel = 0;
        return UART_DMA_TRANSFER_TIMEOUT;
    }

    g_dma_trans[bus].channel = 0;

    if (!g_dma_trans[bus].trans_succ) {
        return UART_DMA_TRANSFER_ERROR;
    }

    return (int32_t)uapi_dma_get_block_ts(dma_ch);
}

static int32_t uart_read_by_dma_config(uart_bus_t bus, const void *buffer, uint32_t length,
                                       uart_write_dma_config_t *dma_cfg,
                                       dma_ch_user_peripheral_config_t *user_cfg)
{
    uint32_t uart_data_addr = 0;
    errcode_t ret = hal_uart_ctrl(bus, UART_CTRL_GET_DMA_DATA_ADDR, (uintptr_t)&uart_data_addr);
    if (ret != ERRCODE_SUCC) {
        return -1;
    }
    user_cfg->src = uart_data_addr;
    user_cfg->dest = (uint32_t)(uintptr_t)buffer;
    user_cfg->transfer_num = (uint16_t)(length >> dma_cfg->src_width);
    user_cfg->dest_handshaking = 0;
    user_cfg->trans_type = UART_DMA_TRANS_PERIPHERAL_TO_MEMORY_DMA;
    user_cfg->trans_dir = UART_DMA_TRANSFER_DIR_PERIPHERAL_TO_MEM;
    user_cfg->priority = dma_cfg->priority;
    user_cfg->src_width = dma_cfg->src_width;
    user_cfg->dest_width = dma_cfg->dest_width;
    user_cfg->burst_length = dma_cfg->burst_length;
    user_cfg->src_increment = UART_DMA_ADDRESS_INC_NO_CHANGE;
    user_cfg->dest_increment = UART_DMA_ADDRESS_INC_INCREMENT;
    user_cfg->protection = UART_DMA_PROTECTION_CONTROL_BUFFERABLE;
    user_cfg->src_handshaking = uart_port_get_dma_trans_src_handshaking(bus);
    return ERRCODE_SUCC;
}

int32_t uapi_uart_read_by_dma(uart_bus_t bus, const void *buffer, uint32_t length, uart_write_dma_config_t *dma_cfg)
{
    int32_t ret = uapi_uart_dma_check(bus, buffer, length, dma_cfg);
    if (ret != 0) {
        return ret;
    }

    dma_ch_user_peripheral_config_t user_cfg = {0};
    uint8_t dma_ch;

    ret = uart_read_by_dma_config(bus, buffer, length, dma_cfg, &user_cfg);
    if (ret != ERRCODE_SUCC || user_cfg.src_handshaking == HAL_DMA_HANDSHAKING_MAX_NUM) {
        return -1;
    }

    if (uapi_dma_configure_peripheral_transfer_single(&user_cfg, &dma_ch,
        uart_dma_isr, (uintptr_t)NULL) != ERRCODE_SUCC) {
        return -1;
    }

    g_dma_trans[bus].channel = dma_ch + 1;
    g_dma_trans[bus].trans_succ = false;

    if (uapi_dma_start_transfer(dma_ch) != ERRCODE_SUCC) {
        g_dma_trans[bus].channel = 0;
        return -1;
    }

    if (osal_sem_down(&(g_dma_trans[bus].dma_sem)) != OSAL_SUCCESS) {
        g_dma_trans[bus].channel = 0;
        return -1;
    }

    g_dma_trans[bus].channel = 0;

    if (!g_dma_trans[bus].trans_succ) {
        return -1;
    }

    return (int32_t)uapi_dma_get_block_ts(dma_ch);
}
#endif  /* CONFIG_UART_SUPPORT_DMA */
#endif  /* CONFIG_UART_SUPPORT_TX */

#if defined(CONFIG_UART_SUPPORT_RX)
int32_t uapi_uart_read(uart_bus_t bus, const uint8_t *buffer, uint32_t length, uint32_t timeout)
{
    int32_t ret = uapi_uart_param_check(bus, buffer, length);
    if (ret != ERRCODE_SUCC) {
        return ret;
    }

    unused(timeout);
    bool rx_fifo_empty = false;
    uint8_t *data_buffer = (uint8_t *)buffer;
    int32_t read_count = 0;
    uint32_t len = length;

    uint32_t irq_sts = uart_porting_lock(bus);
    while (len > 0) {
        hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
        if (rx_fifo_empty == false) {
            hal_uart_read(bus, data_buffer++, 1);
            len--;
            read_count++;
        }
    }
    uart_porting_unlock(bus, irq_sts);

    return read_count;
}
#endif  /* CONFIG_UART_SUPPORT_RX */

#if defined(CONFIG_UART_SUPPORT_RX)
static bool uart_config_rx_state(uart_bus_t bus, const uart_buffer_config_t *uart_buffer_config)
{
    if ((uart_buffer_config == NULL) ||
        (uart_buffer_config->rx_buffer == NULL) ||
        (uart_buffer_config->rx_buffer_size == 0)) {  /* No RX buffer specified */
        return false;
    }
    /* Configure RX state structure */
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    rx_state->rx_buffer = uart_buffer_config->rx_buffer;
    rx_state->rx_buffer_size = (uint16_t)uart_buffer_config->rx_buffer_size;

    return true;
}
#endif  /* CONFIG_UART_SUPPORT_RX */

#if defined(CONFIG_UART_SUPPORT_TX)
#if defined(CONFIG_UART_SUPPORT_TX_INT)
static void uart_config_tx_state(uart_bus_t bus)
{
    /* Configure TX state structure */
    uart_tx_state_t *tx_state = &g_uart_tx_state_array[bus];
    tx_state->current_tx_fragment = tx_state->fragment_buffer;  /* the queue is empty */
    tx_state->free_tx_fragment = tx_state->fragment_buffer;     /* the queue is empty */
}
#endif  /* CONFIG_UART_SUPPORT_TX_INIT */
#endif  /* CONFIG_UART_SUPPORT_TX */

#if defined(CONFIG_UART_SUPPORT_RX) || (CONFIG_UART_SUPPORT_TX)
static void uart_deconfig_state(uart_bus_t bus)
{
    unused(bus);
#if defined(CONFIG_UART_SUPPORT_RX)
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    (void)memset_s(rx_state, sizeof(uart_rx_state_t), 0, sizeof(uart_rx_state_t));
#endif  /* CONFIG_UART_SUPPORT_RX */
#if defined(CONFIG_UART_SUPPORT_TX)
#if defined(CONFIG_UART_SUPPORT_TX_INT)
    uart_tx_state_t *tx_state = &g_uart_tx_state_array[bus];
    (void)memset_s(tx_state, sizeof(uart_tx_state_t), 0, sizeof(uart_tx_state_t));
#endif  /* CONFIG_UART_SUPPORT_TX_INIT */
#endif  /* CONFIG_UART_SUPPORT_TX */
}
#endif  /* defined(CONFIG_UART_SUPPORT_RX) || (CONFIG_UART_SUPPORT_TX) */

#if defined(CONFIG_UART_SUPPORT_TX)
#if defined(CONFIG_UART_SUPPORT_TX_INT)
static bool uart_helper_add_fragment(uart_bus_t bus, const uint8_t *buffer,
                                     uint32_t length, void *params,
                                     uart_tx_callback_t finished_with_buffer_func)
{
    uart_tx_fragment_t *fragment;

    uart_tx_state_t *tx_state = &g_uart_tx_state_array[bus];
    /* If we have fragments left add it */
    if (tx_state->fragments_to_process >= UART_MAX_NUMBER_OF_FRAGMENTS) {
        return false;
    }

    /* Put it on the queue */
    fragment = tx_state->free_tx_fragment;
    /* Populate the fragment */
    fragment->data = (uint8_t *)buffer;
    fragment->params = params;
    fragment->data_length = length;
    fragment->release_func = finished_with_buffer_func;

    /* Update the counters */
    tx_state->free_tx_fragment++;
    if (tx_state->free_tx_fragment >=
        tx_state->fragment_buffer + UART_MAX_NUMBER_OF_FRAGMENTS) {
        tx_state->free_tx_fragment = tx_state->fragment_buffer;  /* wrapping */
    }
    tx_state->fragments_to_process++;
    return true;
}

static inline bool uart_helper_is_the_current_fragment_the_last_to_process(uart_bus_t bus)
{
    uart_tx_state_t *tx_state = &g_uart_tx_state_array[bus];
    return (tx_state->fragments_to_process == 1);
}

static inline bool uart_helper_are_there_fragments_to_process(uart_bus_t bus)
{
    uart_tx_state_t *tx_state = &g_uart_tx_state_array[bus];
    return (tx_state->fragments_to_process > 0);
}

static bool uart_helper_send_next_char(uart_bus_t bus)
{
    uart_tx_fragment_t *current_fragment;
    uint16_t current_fragment_pos;

    uart_tx_state_t *tx_state = &g_uart_tx_state_array[bus];
    current_fragment = tx_state->current_tx_fragment;
    current_fragment_pos = tx_state->current_tx_fragment_pos;
    hal_uart_write(bus, &current_fragment->data[current_fragment_pos], 1);
    /* update the counters */
    tx_state->current_tx_fragment_pos++;

    return (tx_state->current_tx_fragment_pos >= current_fragment->data_length);
}

static inline void uart_helper_invoke_current_fragment_callback(uart_bus_t bus)
{
    uart_tx_fragment_t *current_fragment;
    uart_tx_state_t *tx_state = &g_uart_tx_state_array[bus];
    current_fragment = tx_state->current_tx_fragment;
    /* Call any TX data release call-back */
    if (current_fragment->release_func != NULL) {
        current_fragment->release_func(current_fragment->data, current_fragment->data_length, current_fragment->params);
    }
}

static inline void uart_helper_move_to_next_fragment(uart_bus_t bus)
{
    /* Move onto the next fragment and re-set the position to zero */
    uart_tx_state_t *tx_state = &g_uart_tx_state_array[bus];
    tx_state->current_tx_fragment++;
    if (tx_state->current_tx_fragment >=
        tx_state->fragment_buffer + UART_MAX_NUMBER_OF_FRAGMENTS) {
        tx_state->current_tx_fragment = tx_state->fragment_buffer;  /* wrapping */
    }
    tx_state->current_tx_fragment_pos = 0;  /* reset the current fragment */
    tx_state->fragments_to_process--;       /* one fragment less to process */
}
#endif   /* CONFIG_UART_SUPPORT_TX_INIT */
#endif  /* CONFIG_UART_SUPPORT_TX */

#if defined(CONFIG_UART_SUPPORT_RX)
static inline void uart_rx_buffer_release(uart_bus_t bus)
{
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    rx_state->new_rx_pos = 0;
}

static inline bool uart_rx_buffer_has_free_space(uart_bus_t bus)
{
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    return (rx_state->new_rx_pos < rx_state->rx_buffer_size);
}

static inline uint16_t uart_rx_buffer_data_available(uart_bus_t bus)
{
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    return rx_state->new_rx_pos;
}
#endif  /* CONFIG_UART_SUPPORT_RX */

#if defined(CONFIG_UART_SUPPORT_LPM)
errcode_t uapi_uart_suspend(uintptr_t arg)
{
    errcode_t ret = ERRCODE_SUCC;
    unused(arg);
    for (uint32_t i = 0; i < UART_BUS_MAX_NUM; i++) {
        if (g_uart_inited[i] == false) {
            continue;
        }
        g_uart_suspend_flag[i] = true;
#if defined(CONFIG_UART_SUPPORT_DMA)
        uapi_dma_suspend(arg);
#endif
    }
    return ret;
}

errcode_t uapi_uart_resume(uintptr_t arg)
{
    errcode_t ret = ERRCODE_SUCC;
    unused(arg);
    for (uint32_t i = 0; i < UART_BUS_MAX_NUM; i++) {
        if (g_uart_suspend_flag[i] == false) {
            continue;
        }
        ret |= uapi_uart_deinit(i);
        ret |= uapi_uart_init(i, &g_uart_pins[i], &g_uart_attr[i], &g_uart_extra_attr[i], &g_uart_buffer_config[i]);
        if (g_uart_callback[i] != NULL) {
            ret |= uapi_uart_register_rx_callback(i, g_uart_condition[i], g_uart_size[i], g_uart_callback[i]);
        }
#if defined(CONFIG_UART_SUPPORT_DMA)
        uapi_dma_resume(arg);
#endif
        g_uart_suspend_flag[i] = false;
    }
    return ret;
}
#endif  /* CONFIG_UART_SUPPORT_LPM */

static int32_t uart_check_params_attr(const uart_attr_t *attr)
{
    if (attr == NULL || attr->data_bits > UART_DATA_BIT_8) {
        return -1;
    }

    if (attr->parity > UART_PARITY_EVEN) {
        return -1;
    }

    if (attr->stop_bits != UART_STOP_BIT_1 && attr->stop_bits != UART_STOP_BIT_2) {
        return -1;
    }

    return 0;
}

static int32_t uart_init_check_params(uart_bus_t bus, const uart_pin_config_t *pins, const uart_attr_t *attr)
{
    if (bus >= UART_BUS_MAX_NUM || pins == NULL) {
        return -1;
    }

#if defined(CONFIG_UART_SUPPORT_RX)
    if (pins->rx_pin >= PIN_NONE) {
        return -1;
    }
#endif
#if defined(CONFIG_UART_SUPPORT_TX)
    if (pins->tx_pin >= PIN_NONE) {
        return -1;
    }
#endif

    return uart_check_params_attr(attr);
}

static void uart_claim_pins(uart_bus_t bus, const uart_pin_config_t *pins)
{
    unused(pins);
    uart_port_config_pinmux(bus);
}

static void uart_release_pins(uart_bus_t bus)
{
    unused(bus);
}

#if defined(CONFIG_UART_SUPPORT_RX)
static void uart_idle_isr(uart_bus_t bus)
{
    uint16_t char_recv_cnt = 0;
    uint16_t uart_rx_isr_available = 0;
    uint8_t uart_rx_isr_data = 0 ;
    bool rx_fifo_empty = false;

    hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    while (rx_fifo_empty != true) {
        /* Read the data out of the UART to clear the interrupt */
        /* using a volatile variable to ensure the read always happens */
        hal_uart_read(bus, &uart_rx_isr_data, 1);
        char_recv_cnt++;
        /* Only bother to try and record UART data it there is an RX callback registered */
        if (rx_state->rx_callback == NULL) {
            hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
            continue;
        }
        /* There is some space in the RX buffer so put the data in and move the pointers */
        rx_state->rx_buffer[rx_state->new_rx_pos] = uart_rx_isr_data;
        rx_state->new_rx_pos++;
        /* When the rx buffer is full, callback should be invoked */
        if (uart_rx_buffer_has_free_space(bus) == false) {
            if (rx_state->rx_callback != NULL) {
                uart_rx_isr_available = uart_rx_buffer_data_available(bus);
                rx_state->rx_callback(rx_state->rx_buffer, uart_rx_isr_available, false);
            }
            uart_rx_buffer_release(bus);
        }

        hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
    }
    /**
     * Is the RX callback an exact size condition or
     * it has already been determined that a callback must be made.
     */
    uart_rx_isr_available = uart_rx_buffer_data_available(bus);
    if (uart_rx_isr_available > 0 &&
        ((((uint8_t)rx_state->rx_condition & UART_RX_CONDITION_MASK_IDLE) != 0) ||
        (((uint8_t)rx_state->rx_condition & UART_RX_CONDITION_MASK_SUFFICIENT_DATA) != 0 &&
        char_recv_cnt >= rx_state->rx_condition_size))) {
        if (rx_state->rx_callback != NULL) {
            rx_state->rx_callback(rx_state->rx_buffer, uart_rx_isr_available, false);
        }
        uart_rx_buffer_release(bus);
    }
}

static void uart_rx_isr(uart_bus_t bus)
{
    uint16_t uart_rx_isr_available;
    uint8_t uart_rx_isr_data = 0;
    bool rx_fifo_empty = false;
    uint16_t char_recv_cnt = 0;

    hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    /* Check that the UART is opened */
    while (rx_fifo_empty != true) {
        /* Read the data out of the UART to clear the interrupt */
        /* Using a volatile variable to ensure the read always happens */
        hal_uart_read(bus, &uart_rx_isr_data, 1);
        char_recv_cnt++;
        /* Only bother to try and record UART data it there is an RX callback registered */
        if (rx_state->rx_callback == NULL) {
            hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
            continue;
        }
        /* There is some space in the RX buffer so put the data in and move the pointers */
        rx_state->rx_buffer[rx_state->new_rx_pos] = uart_rx_isr_data;
        rx_state->new_rx_pos++;
        /* When the rx buffer is full, callback should be invoked */
        if (uart_rx_buffer_has_free_space(bus) == false) {
            uart_rx_isr_available = uart_rx_buffer_data_available(bus);
            rx_state->rx_callback(rx_state->rx_buffer, uart_rx_isr_available, false);
            uart_rx_buffer_release(bus);
        }

        hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
    }
    /* Check to see if the callback should be invoked */
    uart_rx_isr_available = uart_rx_buffer_data_available(bus);
    if (uart_rx_isr_available > 0 &&
        (((uint8_t)rx_state->rx_condition & UART_RX_CONDITION_MASK_SUFFICIENT_DATA) != 0 &&
        char_recv_cnt >= rx_state->rx_condition_size)) {
        if (rx_state->rx_callback != NULL) {
            rx_state->rx_callback(rx_state->rx_buffer, uart_rx_isr_available, false);
        }
        uart_rx_buffer_release(bus);
    }
}

static void uart_error_isr(uart_bus_t bus)
{
    uint16_t uart_rx_isr_available;
    uint8_t uart_rx_isr_data = 0;
    bool rx_fifo_empty = false;

    hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    while (rx_fifo_empty != true) {
        /* Read the data out of the UART FIFO */
        hal_uart_read(bus, &uart_rx_isr_data, 1);
        /* There is some space in the RX buffer so put the data in and move the pointers */
        rx_state->rx_buffer[rx_state->new_rx_pos] = (uint8_t)uart_rx_isr_data;
        rx_state->new_rx_pos++;
        /* Only bother to try and record UART data if there is an RX callback registered */
        if (uart_rx_buffer_has_free_space(bus) == false) {
            if (rx_state->rx_callback != NULL) {
                /* Calculate the amount of available data */
                uart_rx_isr_available = uart_rx_buffer_data_available(bus);
                /* Callback should be invoked in any case */
                rx_state->rx_callback(rx_state->rx_buffer, uart_rx_isr_available, true);
            }
            uart_rx_buffer_release(bus);
        }
        hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
    }
    uart_rx_isr_available = uart_rx_buffer_data_available(bus);
    if (uart_rx_isr_available > 0 && rx_state->rx_callback != NULL) {
        rx_state->rx_callback(rx_state->rx_buffer, uart_rx_isr_available, true);
        uart_rx_buffer_release(bus);
    }
}
#endif  /* CONFIG_UART_SUPPORT_RX */

#if defined(CONFIG_UART_SUPPORT_TX_INT)
static void uart_tx_isr(uart_bus_t bus)
{
    bool tx_fifo_full = false;

    /* if there are fragments to process do it */
    if (!uart_helper_are_there_fragments_to_process(bus)) {
        /* No data to transmit so disable the TX interrupt */
        hal_uart_ctrl(bus, UART_CTRL_EN_TX_INT, false);
        return;
    }

    hal_uart_ctrl(bus, UART_CTRL_CHECK_TX_FIFO_FULL, (uintptr_t)&tx_fifo_full);
    /* Populate the UART TX FIFO if there is data to send */
    while (tx_fifo_full != true) {
        /* There is some data to transmit so provide another byte to the UART */
        bool end_of_fragment = uart_helper_send_next_char(bus);
        if (end_of_fragment) {
            /* If it is the end of the fragment invoke the callback and move to the next one */
            uart_helper_invoke_current_fragment_callback(bus);
            uart_helper_move_to_next_fragment(bus);
            /* If it was the last fragment disable the TX interrupts and leave */
            if (uart_helper_are_there_fragments_to_process(bus) == false) {
                /* No data to transmit so disable the TX interrupt */
                hal_uart_ctrl(bus, UART_CTRL_EN_TX_INT, false);
                break;
            }
        }

        hal_uart_ctrl(bus, UART_CTRL_CHECK_TX_FIFO_FULL, (uintptr_t)&tx_fifo_full);
    }
}
#endif  /* CONFIG_UART_SUPPORT_TX_INIT */

static errcode_t uart_evt_callback(uart_bus_t bus, hal_uart_evt_id_t evt, uintptr_t param)
{
    unused(param);
    unused(bus);
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    switch (evt) {
#if defined(CONFIG_UART_SUPPORT_TX_INT)
        case UART_EVT_TX_ISR:
            uart_tx_isr(bus);
            break;
#endif  /* CONFIG_UART_SUPPORT_TX_INIT */

#if defined(CONFIG_UART_SUPPORT_RX)
        case UART_EVT_RX_ISR:
            uart_rx_isr(bus);
            break;

        case UART_EVT_IDLE_ISR:
            uart_idle_isr(bus);
            break;

        case UART_EVT_PARITY_ERR_ISR:
            if (rx_state->parity_error_callback != NULL) {
                rx_state->parity_error_callback(NULL, 0);
            }
            uart_error_isr(bus);
            break;

        case UART_EVT_FRAME_ERR_ISR:
            if (rx_state->frame_error_callback != NULL) {
                rx_state->frame_error_callback(NULL, 0);
            }
            uart_error_isr(bus);
            break;

        case UART_EVT_BREAK_ERR_ISR:
            uart_error_isr(bus);
            break;

#endif  /* CONFIG_UART_SUPPORT_RX */
        default :
/* 为保证UT覆盖到default分支，UART_EVT_OVERRUN_ERR_ISR分支与default分支合并 */
#if defined(CONFIG_UART_SUPPORT_RX)
            uart_error_isr(bus);
#endif  /* CONFIG_UART_SUPPORT_RX */
            break;
    }
    return ERRCODE_SUCC;
}

bool uapi_uart_has_pending_transmissions(uart_bus_t bus)
{
    if (bus >= UART_BUS_MAX_NUM) {
        return false;
    }
    if (!g_uart_inited[bus]) {
        return false;
    }

    bool currentstate = false;

    hal_uart_ctrl(bus, UART_CTRL_CHECK_UART_BUSY, (uintptr_t)&currentstate);

#if defined(CONFIG_UART_SUPPORT_TX)
#if defined(CONFIG_UART_SUPPORT_TX_INT)
    uart_tx_state_t *tx_state = &g_uart_tx_state_array[bus];
    return ((tx_state->fragments_to_process > 0) || currentstate);
#else
     return currentstate;
#endif /* CONFIG_UART_SUPPORT_TX_INIT */
#else
    return currentstate;
#endif  /* CONFIG_UART_SUPPORT_TX */
}

bool uapi_uart_rx_fifo_is_empty(uart_bus_t bus)
{
    if (bus >= UART_BUS_MAX_NUM) {
        return false;
    }
    if (!g_uart_inited[bus]) {
        return false;
    }

    bool currentstate = false;

    hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&currentstate);

    return currentstate;
}

bool uapi_uart_tx_fifo_is_empty(uart_bus_t bus)
{
    if (bus >= UART_BUS_MAX_NUM) {
        return false;
    }
    if (!g_uart_inited[bus]) {
        return false;
    }

    bool currentstate = false;

    hal_uart_ctrl(bus, UART_CTRL_CHECK_TX_BUSY, (uintptr_t)&currentstate);

    return currentstate;
}

void uapi_uart_unregister_rx_callback(uart_bus_t bus)
{
    bool rx_fifo_empty = false;
    uint8_t uart_rx_isr_data;
    uint32_t fifo_depth = CONFIG_UART_FIFO_DEPTH;
    if (bus >= UART_BUS_MAX_NUM) {
        return;
    }
    uint32_t irq_sts = uart_porting_lock(bus);
    uart_rx_state_t *rx_state = &g_uart_rx_state_array[bus];
    rx_state->rx_callback = NULL;
    hal_uart_ctrl(bus, UART_CTRL_EN_RX_INT, 0);
    hal_uart_ctrl(bus, UART_CTRL_EN_FRAME_ERR_INT, 0);
    hal_uart_ctrl(bus, UART_CTRL_EN_PARITY_ERR_INT, 0);
    hal_uart_ctrl(bus, UART_CTRL_EN_IDLE_INT, 0);
    /* Flush the data on the RX FIFO */
    while (fifo_depth > 0) {
        hal_uart_ctrl(bus, UART_CTRL_CHECK_RX_FIFO_EMPTY, (uintptr_t)&rx_fifo_empty);
        if (rx_fifo_empty) {
            break;
        }
        hal_uart_read(bus, &uart_rx_isr_data, 1);
        fifo_depth--;
        unused(uart_rx_isr_data);
    }
    uart_porting_unlock(bus, irq_sts);
}