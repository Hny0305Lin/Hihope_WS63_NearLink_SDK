/**
 * Copyright (c) HiSilicon (Shanghai) Technologies Co., Ltd. 2023-2023. All rights reserved.
 *
 * Description: Provides v151 uart register \n
 *
 * History: \n
 * 2023-02-17, Create file. \n
 */
#ifndef HAL_UART_V151_REGS_DEF_H
#define HAL_UART_V151_REGS_DEF_H

#include <stdint.h>
#include "uart_porting.h"

#define UART_SHADOW_RECIEVE_BUFFER_REG_LEN     16
#define UART_SHADOW_TRANSMIT_HOLDING_REG_LEN   16
#define UART_LCR_EXT_RESERVE_LEN               9

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

/**
 * @defgroup drivers_hal_uart_v151_regs_def UART V151 Regs Definition
 * @ingroup  drivers_hal_uart
 * @{
 */

typedef union uart_id {
    uint32_t d32;
    struct {
        uint32_t uart_id : 8; /* !< RO, UART ID */
    } uart_id;
} uart_id_t;

typedef union data {
    uint32_t d32;
    struct {
        uint32_t data : 8; /* !< RW, For Write(WO):
                                   If the FIFOs are enabled, data written to this location is pushed onto the transmit
                              FIFO;If the FIFOs are not enabled, data is stored in the transmitter holding register (the
                              bottom word of the transmit FIFO); For Read(RO): This register contains the data byte
                              received on the serial input port (sin).If FIFOs are disabled, the data in the RX_DATA
                              must be read before the next data arrives, otherwise it will be overwritten, resulting in
                              an over-run error. If FIFOs are enabled, this register accesses the head of the receive
                              FIFO. If the receive FIFO is full and this register is not read before the next data
                              character arrives, then the data already in the FIFO will be preserved but any incoming
                              data will be lost and an over-run error occurs. */
    } data;
} data_t;

typedef union uart_ctl {
    uint32_t d32;
    struct {
        uint32_t div_en : 1; /* !< RW, UART Divisor enable register.
                                   0x0 (DISABLED): DIV_L/DIV_H register is writable only when UART Not BUSY
                                   0x1 (ENABLED):  Divisor Latch register is always readable and writable */
        uint32_t xbreak : 1; /* !< RW, Break Control Bit
                                   0x0 (DISABLED): Serial output is released for data transmission
                                   0x1 (ENABLED): Serial output is forced to spacing state */
        uint32_t dlen : 2;   /* !< RW, Data Length
                                     0x0 : 5 data bits per character
                                     0x1 : 6 data bits per character
                                     0x2 : 7 data bits per character
                                     0x3 : 8 data bits per character */
        uint32_t eps : 1;    /* !< RW, Even Parity Select
                                    0x0 : an odd parity is transmitted or checked
                                    0x1 : an even parity is transmitted or checked */
        uint32_t pen : 1;    /* !< RW, Parity Enable
                                    0x0 (DISABLED): disable parity
                                    0x1 (ENABLED) : enable parity */
        uint32_t sps : 1;    /* !< RW, Stick Parity Select
                                    0x0 (DISABLED): Stick parity disabled
                                    0x1 (ENABLED) : Stick parity enabled */
        uint32_t stp : 1;    /* !< RW, Number of stop bits.
                                    0x0 : 1 stop bit
                                    0x1 : 1.5 stop bits when DLEN is zero, else 2 stop bit */
    } uart_ctl;
} uart_ctl_t;

typedef union div_h {
    uint32_t d32;
    struct {
        uint32_t div_h : 8; /* !< RW, Divisor Latch register that contains the baud rate divisor for the UART. This
                               register can be accessed only when the DIV_EN bit (CTL[7]) is set and the UART is not
                               busy  */
    } div_h;
} div_h_t;

typedef union div_l {
    uint32_t d32;
    struct {
        uint32_t div_l : 8; /* !< RW, Divisor Latch register that contains the baud rate divisor for the UART. This
                               register can be accessed only when the DIV_EN bit (CTL[7]) is set and the UART is not
                               busy  */
    } div_l;
} div_l_t;

typedef union div_fra {
    uint32_t d32;
    struct {
        uint32_t div_fra : 6; /* !< RW, Fractional part of divisor.The fractional value is added to integer value set by
                                 DIV_L,DIV_H. Fractional value is determined by (Divisor Fraction value)/(2^6). */
    } div_fra;
} div_fra_t;

typedef union intr_en {
    uint32_t d32;
    struct {
        uint32_t rece_line_stat_intr_en : 1; /* !< RW, Receiver Line Status Interrupt enable.
                                                   0x0: Disable Receiver Line Status Interrupt
                                                   0x1: Enable Receiver Line Status Interrupt */
        uint32_t modem_intr_en : 1;          /* !< RW, Enable Modem Status Interrupt.
                                                      0x0 (DISABLED): Disable Modem Status Interrupt
                                                      0x1 (ENABLED): Enable Modem Status Interrupt */
        uint32_t rece_data_intr_en : 1;      /* !< RW, Enable Received Data Available Interrupt.
                                                      0x0: Disable Receive data Interrupt
                                                      0x1: Enable Receive data Interrupt */
        uint32_t tran_em_intr_en : 1;        /* !< RW, Transmit empty interrupt enable.
                                                    0x0: Disable Transmit empty interrupt
                                                    0x1: Enable Transmit empty interrupt */
        uint32_t ptim_en : 1;                /* !< RW, Programmable THRE Interrupt Mode Enable
                                                    0x0: Disable Programmable THRE Interrupt Mode
                                                    0x1: Enable Programmable THRE Interrupt Mode */
    } intr_en;
} intr_en_t;

typedef union intr_status {
    uint32_t d32;
    struct {
        uint32_t busy_det_intr : 1;             /* !< RO, busy detect */
        uint32_t modem_intr_status : 1;         /* !< RO, modem status */
        uint32_t thre_intr_status : 1;          /* !< RO, THR empty */
        uint32_t char_to_intr_status : 1;       /* !< RO, character timeout */
        uint32_t data_avail_intr_status : 1;    /* !< RO, received data available */
        uint32_t line_intr_status : 1;          /* !< RO, receiver line status */
    } intr_status;
} intr_status_t;

typedef union intr_id {
    uint32_t d32;
    struct {
        uint32_t intr_id : 4;   /* !< RO, Interrupt ID.
                                       Values:
                                       0x0: modem status
                                       0x1: no interrupt pending
                                       0x2: THR empty
                                       0x4: received data available
                                       0x6: receiver line status
                                       0xc: character timeout */
        uint32_t fifo_en_s : 1; /* !< RO, FIFO Enable
                                         0x0: FIFO disabled
                                         0x1: FIFO enabled */
    } intr_id;
} intr_id_t;

typedef union fifo_ctl {
    uint32_t d32;
    struct {
        uint32_t tx_empty_trig : 2; /* !< WO, TX Empty Trigger
                                             0x0: FIFO Empty
                                             0x1: 2 characters in FIFO
                                             0x2: FIFO 1/4 full
                                             0x3: FIFO 1/2 full */
        uint32_t rx_empty_trig : 2; /* !< WO, RCVR Trigger
                                             0x0: 1 character in FIFO
                                             0x1: FIFO 1/4 full
                                             0x2: FIFO 1/2 full
                                             0x3: FIFO 2 less than full */
        uint32_t fifo_en : 1;       /* !< WO, FIFO Enable
                                           0x0: FIFO disabled
                                           0x1: FIFO enabled */
        uint32_t tx_fifo_rst : 1;   /* !< WO, Transmit FIFO Reset.
                                           Values:
                                           0x1 (RESET): Transmit FIFO reset */
        uint32_t rx_fifo_rst : 1;   /* !< WO, Receive FIFO Reset.
                                           Values:
                                           0x1 (RESET): Receive FIFO reset */
#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
        uint32_t rx_empty_trig_h : 2;   /* !< WO, RX Empty HIGH Trigger
                                             0x0: 1 character in FIFO
                                             0x1: FIFO 1/4 full
                                             0x2: FIFO 1/2 full
                                             0x3: FIFO 2 less than full */
#endif
    } fifo_ctl;
} fifo_ctl_t;

typedef union far {
    uint32_t d32;
    struct {
        uint32_t far : 1; /* !< RW, This register is use to enable a FIFO access mode for testing, so that the receive
                             FIFO can be written by the master and the transmit FIFO can be read by the master when
                             FIFO's are implemented and enabled. 0x0 (DISABLED): FIFO access mode disabled 0x1
                             (ENABLED): FIFO access mode enabled */
    } far;
} far_t;

typedef union modem_ctl {
    uint32_t d32;
    struct {
        uint32_t afc_en : 1;  /* !< RW, Auto Flow Control Enable
                                    0x0 (DISABLED): Auto Flow Control Mode disabled
                                    0x1 (ENABLED): Auto Flow Control Mode enabled */
        uint32_t lb_mode : 1; /* !< RW, LoopBack Mode
                                     0x0 (DISABLED): Loopback mode disabled
                                     0x1 (ENABLED) : Loopback mode enabled */
        uint32_t rts : 1;     /* !< RW, Request to Send
                                     0x0 (INACTIVE): dtr_n de-asserted (logic1)
                                     0x1 (ACTIVE): dtr_n asserted (logic 0) */
        uint32_t dtr : 1;     /* !< RW, Data Terminal Ready.Values:
                                     0x0 (INACTIVE): dtr_n de-asserted (logic1)
                                     0x1 (ACTIVE):   dtr_n asserted (logic 0) */
        uint32_t out1 : 1;    /* !< RW, Values:
                                      0x0 (OUT1_0): out1_n de-asserted (logic 1)
                                      0x1 (OUT1_1): out1_n asserted (logic 0) */
        uint32_t out2 : 1;    /* !< RW, Values:
                                      0x0 (OUT2_0): out2_n de-asserted (logic 1)
                                      0x1 (OUT2_1): out2_n asserted (logic 0) */
    } modem_ctl;
} modem_ctl_t;

typedef union modem_status {
    uint32_t d32;
    struct {
        uint32_t dcts : 1; /* !< RO, This is used to indicate that the modem control line cts_n has changed since the
                              last time the MODEM_STATUS was read. Values: 0x0 (NO_CHANGE): no change on cts_n since
                              last read of MSR 0x1 (CHANGE): change on cts_n since last read of MSR */
        uint32_t cts : 1;  /* !< RO, CTS status
                                  Values:
                                  0x0 (DEASSERTED): cts_n input is de-asserted (logic 1)
                                  0x1 (ASSERTED): cts_n input is asserted (logic 0) */
        uint32_t ddcd : 1; /* !< RO, Values:
                                   0x0 (NO_CHANGE): No change on dcd_n since last read of MSR
                                   0x1 (CHANGE): change on dcd_n since last read of MSR */
        uint32_t dcd : 1;  /* !< RO, Values:
                                  0x0 (DEASSERTED): dcd_n input is de-asserted (logic 1)
                                  0x1 (ASSERTED): dcd_n input is asserted (logic 0) */
        uint32_t teri : 1; /* !< RO, Values:
                                   0x0 (NO_CHANGE): no change on ri_n since last read of MSR
                                   0x1 (CHANGE): change on ri_n since last read of MSR */
        uint32_t ri : 1;   /* !< RO, Values:
                                 0x0 (DEASSERTED): ri_n input is de-asserted (logic 1)
                                 0x1 (ASSERTED): ri_n input is asserted (logic 0) */
        uint32_t ddsr : 1; /* !< RO, Values:
                                   0x0 (NO_CHANGE): no change on dsr_n since last read of MSR
                                   0x1 (CHANGE): change on dsr_n since last read of MSR */
        uint32_t dsr : 1;  /* !< RO, Values:
                                  0x0 (DEASSERTED): dsr_n input is de-asserted (logic 1)
                                  0x1 (ASSERTED): dsr_n input is asserted (logic 0) */
    } modem_status;
} modem_status_t;

typedef union line_status {
    uint32_t d32;
    struct {
        uint32_t rx_fifo_err : 1;    /* !< RO, Receiver FIFO Error bit.This bit is only relevant when FIFO's are enabled
                                        (FIFO_CTL[4] set to one). This bit is cleared when the LSR is read and the
                                        character with the error is at the top of the receiver FIFO and there are no
                                        subsequent errors in the FIFO. Values:
                                        0x0 (NO_RX_FIFO_ERROR): No error in RX FIFO
                                        0x1 (RX_FIFO_ERROR): Error in RX FIFO */
        uint32_t frame_err : 1;      /* !< RO, Frame error bit.Reading the LINE_STATUS clears this bit.
                                              Values:
                                              0x0 (NO_FRAMING_ERROR): no framing error
                                              0x1 (FRAMING_ERROR): framing error */
        uint32_t parity_err : 1;     /* !< RO, Parity Error bit.Reading the LINE_STATUS clears this bit.
                                           Values:
                                           0x0 (NO_PARITY_ERROR): no parity error
                                           0x1 (PARITY_ERROR): parity error */
        uint32_t overrun_err : 1;    /* !< RO, Overrun error bit.Reading the LINE_STATUS clears this bit.
                                            Values:
                                            0x0 (NO_PARITY_ERROR): no parity error
                                            0x1 (PARITY_ERROR): parity error */
        uint32_t break_intr : 1;     /* !< RO, This is used to indicate the detection of a break sequence on
                                           the serial input data.
                                           Values:
                                           0x0 (NO_BREAK): No break sequence detected
                                           0x1 (BREAK): Break sequence detected */
        uint32_t data_available : 1; /* !< RO, This is used to indicate that the receiver contains at least one
                                           character in the RBR or the receiver FIFO. This bit is cleared
                                           when the RBR is read in the non-FIFO mode, or when the
                                           receiver FIFO is empty, in the FIFO mode.
                                           Values:
                                           0x0 (NOT_READY): data not ready
                                           0x1 (READY): data ready */
        uint32_t thre_s : 1;     /* !< RO, Transmit Holding Register Empty bit.If INTR_EN[4] is enabled, then if FIFO
                                    FIFO_CTL[4] is enabled, this bit will be asserted when the TX FIFO is full. If
                                    FIFO_CTL[4] is not enabled, it will be asserted then the THR register is empty.
                                    Otherwise it will be asserted when THR empty is asserted.
                                    Values: 0x0 (DISABLED): THRE interrupt control is disabled
                                            0x1 (ENABLED): THRE interrupt control is enabled */
        uint32_t tx_empty_s : 1; /* !< RO, Transmitter Empty bit.
                                       If in FIFO mode and FIFO's enabled , this bit is set whenever the Transmitter
                                    Shift Register and the FIFO are both empty. If in the non-FIFO mode or FIFO's are
                                    disabled, this bit is set whenever the Transmitter Holding Register and the
                                    Transmitter Shift Register are both empty. Reset Value: 0x1 Values: 0x0 (DISABLED):
                                    Transmitter not empty 0x1 (ENABLED): Transmitter empty */
    } line_status;
} line_status_t;

typedef union uart_gp_reg {
    uint32_t d32;
    struct {
        uint32_t uart_gp_reg : 8; /* !< RW, This register is for programmers to use as a temporary storage space. */
    } uart_gp_reg;
} uart_gp_reg_t;

typedef union tx_fifo_read {
    uint32_t d32;
    struct {
        uint32_t tx_fifo_read : 8; /* !< RO, When FIFO's are implemented and enabled, reading this register gives the
                                      data at the top of the transmit FIFO. Each consecutive read pops the transmit FIFO
                                      and gives the next data value that is currently at the top of the FIFO. When
                                      FIFO's are not implemented or not enabled, reading this register gives the data in
                                      the DR. */
    } tx_fifo_read;
} tx_fifo_read_t;

typedef union rx_fifo_write {
    uint32_t d32;
    struct {
        uint32_t rx_fifo_write : 8; /* !< WO, Receive FIFO Write Data.
                                             These bits are only valid when FIFO access mode is enabled. When FIFO's are
                                       implemented and enabled, the data that is written to the rx_fifo_write is pushed
                                       into the receive FIFO. Each consecutive write pushes the new data to the next
                                       write location in the receive FIFO. When FIFO's are not implemented or not
                                       enabled, the data that is written to the rx_fifo_write is pushed into the DR. */
        uint32_t rx_fifo_pe : 1;    /* !< WO, Receive FIFO Parity Error. */
        uint32_t rx_fifo_fe : 1;    /* !< WO, Receive FIFO Framing Error. */
    } rx_fifo_write;
} rx_fifo_write_t;

typedef union fifo_status {
    uint32_t d32;
    struct {
        uint32_t tx_fifo_full : 1;  /* !< RO, Transmit FIFO Full.
                                            Values:
                                            0x0 (NOT_FULL): Transmit FIFO is not full
                                            0x1 (FULL): Transmit FIFO is full */
        uint32_t tx_fifo_empty : 1; /* !< RO, Transmit FIFO Empty.
                                             Values:
                                             0x0 (NOT_EMPTY): Transmit FIFO is not empty
                                             0x1 (EMPTY): Transmit FIFO is empty */
        uint32_t rx_fifo_full : 1;  /* !< RO, Receive FIFO Not Empty.
                                            Values:
                                            0x0 (EMPTY): Receive FIFO is empty
                                            0x1 (NOT_EMPTY): Receive FIFO is not empty */
        uint32_t rx_fifo_empty : 1; /* !< RO, Receive FIFO Full.
                                             Values:
                                             0x0 (NOT_FULL): Receive FIFO not full
                                             0x1 (FULL): Receive FIFO full */
#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
        uint32_t uart_busy : 1;     /* !< RO, Uart busy.
                                             Values:
                                             0x0 (IDLE): uart is idle or inactive
                                             0x1 (BUSY): uart is busy (actively transferring data */
#endif
    } fifo_status;
} fifo_status_t;

typedef union tx_fifo_cnt {
    uint32_t d32;
    struct {
        uint32_t tx_fifo_level : 7; /* !< RO, Transmit FIFO Data Counter. This indicates the number of data entries in
                                       the transmit FIFO.The width depends on the Transmit FIFO depth.(current width 7
                                       for TX FIFO 64) */
    } tx_fifo_cnt;
} tx_fifo_cnt_t;

typedef union rx_fifo_cnt {
    uint32_t d32;
    struct {
        uint32_t rx_fifo_level : 7; /* !< RO, Receive FIFO Data Counter. This indicates the number of data entries in
                                       the receive FIFO.The width depends on the receive FIFO depth.(current width 7 for
                                       RX FIFO 64) */
    } rx_fifo_cnt;
} rx_fifo_cnt_t;

typedef union halt_tx {
    uint32_t d32;
    struct {
        uint32_t halt_tx : 1; /* !< RW, Halt TX.
                                     Values:
                                     0x0 (DISABLED): Halt Transmission disabled
                                     0x1 (ENABLED): Halt Transmission enabled */
    } halt_tx;
} halt_tx_t;

typedef union dma_sw_ack {
    uint32_t d32;
    struct {
        uint32_t dma_sw_ack : 1; /* !< WC, DMA Software Acknowledge.
                                       Values:
                                       0x1 (SOFT_ACK): DMA software acknowledge */
    } dma_sw_ack;
} dma_sw_ack_t;

typedef union baud_ctl {
    uint32_t d32;
    struct {
        uint32_t baud_div : 4;     /* !< RW, Sample baud rate
                                           Values:
                                           0x7:baud rate is 8;
                                           0xF:baud rate is 16; */
        uint32_t sample_phase : 4; /* !< RW, RX sample phase */
    } baud_ctl;
} baud_ctl_t;

typedef union stp_ctl {
    uint32_t d32;
    struct {
        uint32_t rx_sp : 1;    /* !< RW, Rx width control.
                                        Only when stp_mode is asserted, this bit is valid.
                                        Values:
                                        0:1 stop bit.
                                        1:1.5 stop bits when DLEN is zero, else 2 stop bit. */
        uint32_t tx_sp : 1;    /* !< RW, Tx width control.
                                        Only when stp_mode is asserted, this bit is valid.
                                        Values:
                                        0:1 stop bit.
                                        1:1.5 stop bits when DLEN is zero, else 2 stop bit. */
        uint32_t stp_mode : 1; /* !< RW, Stop mode.When stp_mode is asserted,the stop width of  TX and RX  is controled
                                  by CTL[7]. Else, the stop width of  TX is controled by tx_sp, the stop width of  RX is
                                  controled by rx_sp. Value: 0:the stop width of  TX and RX  is controled by CTL[7].
                                       1:the stop width of  TX is controled by tx_sp, the stop width of  RX is controled
                                  by rx_sp. */
    } stp_ctl;
} stp_ctl_t;

typedef union uart_parameter {
    uint32_t d32;
    struct {
        uint32_t fifo_depth : 8;     /* !< RO, UART TX/RX FIFO DEPTH.Values:
                                           0x0 (FIFO_MODE_0): FIFO mode is 0
                                           0x1 (FIFO_MODE_16): FIFO mode is 16
                                           0x2 (FIFO_MODE_32): FIFO mode is 32
                                           0x4 (FIFO_MODE_64): FIFO mode is 64
                                           0x8 (FIFO_MODE_128): FIFO mode is 128
                                           0x10 (FIFO_MODE_256): FIFO mode is 256
                                           0x20 (FIFO_MODE_512): FIFO mode is 512
                                           0x40 (FIFO_MODE_1024): FIFO mode is 1024
                                           0x80 (FIFO_MODE_2048): FIFO mode is 2048 */
        uint32_t apb_data_width : 2; /* !< RO, Encoding of APB_DATA_WIDTH configuration parameter
                                           value.
                                           Values:
                                           0x0 : APB data width is 8 bits
                                           0x1 : APB data width is 16 bits
                                           0x2 : APB data width is 32 bits */
        uint32_t afce_mode : 1;      /* !< RO, Encoding of AFCE_MODE configuration parameter value.
                                              Values:
                                              0x0 (DISABLED): AFCE mode disabled
                                              0x1 (ENABLED): AFCE mode enabled */
        uint32_t dma_mode : 1;       /* !< RO, DMA_EXTRA
                                             Values:
                                             0x0 (DISABLED): DMA_EXTRA disabled
                                             0x1 (ENABLED): DMA_EXTRA enabled */
        uint32_t shadow : 1;         /* !< RO, Encoding of SHADOW configuration parameter value.
                                           Values:
                                           0x0 (DISABLED): SHADOW disabled
                                           0x1 (ENABLED): SHADOW enabled */
    } uart_parameter;
} uart_parameter_t;

typedef union lp_ctl {
    uint32_t d32;
    struct {
        uint32_t lp_req_en : 1;      /* !< RW, Low power request enable.If this bit is set to 1,
                                           uart_autocg_bypass must be set to 0; If this bit is
                                           set to 0,uart_autocg_bypass must be set to 1.
                                           Values:
                                           0x0 (DISABLED): low power request disable
                                           0x1 (ENABLED): low power request enable */
    } lp_ctl;
} lp_ctl_t;

typedef union receive_ctl {
    uint32_t d32;
    struct {
        uint32_t receive_enable : 1;      /* !< RW, This register is use to data reception while
                                                FIFO_ACCESS equals to zero.Configrate this register
                                                in the end of configuration process.
                                                Values:
                                                0x0 (DISABLED): data reception disable
                                                0x1 (ENABLED): data reception enable */
    } receive_ctl;
} receive_ctl_t;

typedef struct uart_regs {
    volatile uint32_t intr_id;
    volatile uint32_t data;
    volatile uint32_t uart_ctl;
    volatile uint32_t div_h;
    volatile uint32_t div_l;
    volatile uint32_t div_fra;
    volatile uint32_t intr_en;
    volatile uint32_t intr_status;
    volatile uint32_t reserved_20;
    volatile uint32_t fifo_ctl;
    volatile uint32_t far;
    volatile uint32_t modem_ctl;
    volatile uint32_t modem_status;
    volatile uint32_t line_status;
    volatile uint32_t uart_gp_reg;
    volatile uint32_t tx_fifo_read;
    volatile uint32_t rx_fifo_write;
    volatile uint32_t fifo_status;
    volatile uint32_t tx_fifo_cnt;
    volatile uint32_t rx_fifo_cnt;
    volatile uint32_t halt_tx;
    volatile uint32_t dma_sw_ack;
    volatile uint32_t baud_ctl;
    volatile uint32_t stp_ctl;
    volatile uint32_t uart_parameter;
#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
    volatile uint32_t lp_ctl;
    volatile uint32_t receive_ctl;
#endif
} uart_reg_t;


typedef struct uart_regs1 {
    volatile intr_id_t intr_id;
    volatile data_t data;
    volatile uart_ctl_t uart_ctl;
    volatile div_h_t div_h;
    volatile div_l_t div_l;
    volatile div_fra_t div_fra;
    volatile intr_en_t intr_en;
    volatile intr_status_t intr_status;
    volatile uint32_t reserved_20;
    volatile fifo_ctl_t fifo_ctl;
    volatile far_t far;
    volatile modem_ctl_t modem_ctl;
    volatile modem_status_t modem_status;
    volatile line_status_t line_status;
    volatile uart_gp_reg_t uart_gp_reg;
    volatile tx_fifo_read_t tx_fifo_read;
    volatile rx_fifo_write_t rx_fifo_write;
    volatile fifo_status_t fifo_status;
    volatile tx_fifo_cnt_t tx_fifo_cnt;
    volatile rx_fifo_cnt_t rx_fifo_cnt;
    volatile halt_tx_t halt_tx;
    volatile dma_sw_ack_t dma_sw_ack;
    volatile baud_ctl_t baud_ctl;
    volatile stp_ctl_t stp_ctl;
    volatile uart_parameter_t uart_parameter;
#if defined(CONFIG_UART_IP_VERSION_V151_PRO)
    volatile lp_ctl_t lp_ctl;
    volatile receive_ctl_t receive_ctl;
#endif
} uart_reg_1_t;


extern uart_reg_t *g_hal_uart_reg[UART_BUS_MAX_NUMBER];

/**
 * @brief  Resets the uarts on the protocol and apps cores using the RESET_REG register
 * The security core does not have that register, this function does nothing on
 * the security core
 * @param  uart index
 */
void hal_uart_specific_reset_uart(uart_bus_t uart);

/**
 * @brief  Specific priority of IRQ associated with a UART
 * @param  uart Uart index
 */
void hal_uart_specific_priority_irq(uart_bus_t uart);

/**
 * @brief  Enable IRQ associated with a UART
 * @param  uart Uart index
 */
void hal_uart_specific_enable_irq(uart_bus_t uart);

/**
 * @brief  Disable IRQ associated with a UART
 * @param  uart Uart index
 */
void hal_uart_specific_disable_irq(uart_bus_t uart);

/**
 * @brief  Set IRQ associated with a UART pending
 * @param  uart Uart index
 */
void hal_uart_specific_set_pending_irq(uart_bus_t uart);

/**
 * @} end of group DRIVER_HAL_UART
 */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif
