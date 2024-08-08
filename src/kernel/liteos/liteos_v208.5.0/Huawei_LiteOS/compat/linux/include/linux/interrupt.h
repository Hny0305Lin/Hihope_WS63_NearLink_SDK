/**
 * @defgroup interrupt Interrupt mechanism
 * @ingroup linux
 */
#ifndef _LINUX_INTERRUPT_H
#define _LINUX_INTERRUPT_H

#include "linux/kernel.h"
#include "los_base.h"
#include "linux/workqueue.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IRQ_RETVAL(x)    ((x) != IRQ_NONE)

/**
 * These correspond to the IORESOURCE_IRQ_* defines in
 * linux/ioport.h to select the interrupt line behaviour.
 */
#define IRQF_TRIGGER_LOW      0x00000008
#define IRQF_TRIGGER_HIGH     0x00000004
#define IRQF_TRIGGER_FALLING  0x00000002
#define IRQF_TRIGGER_RISING   0x00000001
#define IRQF_TRIGGER_NONE     0x00000000
#define IRQF_TRIGGER_MASK     (IRQF_TRIGGER_HIGH | IRQF_TRIGGER_LOW | \
                               IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING)
#define IRQF_PROBE_SHARED     0x00000100

#ifdef LOSCFG_ARCH_RISCV32
#define IRQ_DEFAULT_PRIORITY 0x1
#else
#define IRQ_DEFAULT_PRIORITY 0x0
#endif

typedef enum irqreturn {
    IRQ_NONE        = (0U << 0),  /* interrupt was not from this device. */
    IRQ_HANDLED     = (1U << 0),  /* interrupt was handled by this device. */
    IRQ_WAKE_THREAD = (1U << 1)   /* handler requests to wake the handler thread. */
} irqreturn_t;

typedef irqreturn_t (*irq_handler_t)(int, void *);

typedef struct irq_args {
    int         iIrq;
    void        *pDevId;
    const char  *pName;
} irq_args;

/**
 * @ingroup interrupt
 * @brief Request an interrupt.
 *
 * @par Description:
 * This API is used to request the interrupt that has a specified interrupt ID and register an interrupt handler.
 * @attention
 * <ul>
 * <li>The maximum number of interrupts supported by the OS Kernel is pre-configured
 * (for details on how to pre-configure an appropriate upper threshold, see the reference document provided by
 * the chip supplier). If the maximum number of interrupts is exceeded, the request fails.</li>
 * <li>The value of registered interrupt handler must not be null.
 * Devices that share a same interrupt ID must each be assigned a unique value of the dev parameter.</li>
 * <li>The input parameter dev must be valid, otherwise, the system may be abnormal.</li>
 * <li>The return value is inconsistent with the Linux standard interface definition.</li>
 * </ul>
 *
 * @param irq     [IN] ID of the interrupt to be requested. [OS_USER_HWI_MIN, OS_USER_HWI_MAX].
 * @param handler [IN] Interrupt handler to be registered.
 * @param flags   [IN] Attributes of the interrupt processing, NOT used.
 * @param name    [IN] Interrupt name.
 * @param dev     [IN] Input parameter of the interrupt handler.
 *
 * @retval #LOS_ERRNO_HWI_PROC_FUNC_NULL    The hardware interrupt processing function is null.
 * @retval #LOS_ERRNO_HWI_NUM_INVALID       The interrupt ID is invalid.
 * @retval #LOS_ERRNO_HWI_NO_MEMORY         The memory is insufficient for creating a hardware interrupt.
 * @retval #LOS_ERRNO_HWI_ALREADY_CREATED   The interrupt being created has already been created.
 * @retval #LOS_ERRNO_HWI_SHARED_ERROR      The interrupt can not be shared.
 * @retval #LOS_OK                          The interrupt request is accepted.
 * @par Dependency:
 * <ul><li>interrupt.h: the header file that contains the API declaration.</li></ul>
 * @see free_irq
 */
int request_irq(unsigned int irq, irq_handler_t handler, unsigned long flags,
                const char *name, void *dev);

/**
 * @ingroup interrupt
 * @brief Delete an interrupt.
 *
 * @par Description:
 * This API is used to delete the interrupt handler that has a specified input parameter.
 * @attention
 * <ul>
 * <li>The maximum number of interrupts supported by the OS Kernel is pre-configured
 * (for details on how to pre-configure an appropriate upper threshold, see the reference document provided by
 * the chip supplier). If the maximum number of interrupts is exceeded, the interrupt fails to be deleted.</li>
 * <li>Please make sure that the parameters irq and dev_id is that specified by calling request_irq(),
 * or else delete an interrupt would be fails.</li>
 * </ul>
 *
 * @param irq   [IN] ID of the interrupt to be deleted. [OS_USER_HWI_MIN, OS_USER_HWI_MAX].
 * @param dev_id   [IN] Input parameter of the interrupt handler to be deleted.
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>interrupt.h: the header file that contains the API declaration.</li></ul>
 * @see request_irq
 */
void free_irq(unsigned int irq, void *dev_id);

/**
 * @ingroup interrupt
 * @brief Enable an interrupt.
 *
 * @par Description:
 * This API is used to enable the interrupt that has a specified interrupt ID.
 * @attention
 * <ul>
 * <li>The maximum number of interrupts supported by the OS Kernel is pre-configured
 * (for details on how to pre-configure an appropriate upper threshold, see the reference document provided by
 * the chip supplier). If the maximum number of interrupts is exceeded, the interrupt fails to be enabled.</li>
 * <li>Please do make sure the specified irq has corresponding interrupt handler, otherwise,
 * the system would do nothing  but only respond the specified interrupt if the specified interrupt is pending.</li>
 * </ul>
 *
 * @param irq   [IN] ID of the interrupt to be enabled. [OS_USER_HWI_MIN, OS_USER_HWI_MAX].
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>interrupt.h: the header file that contains the API declaration.</li></ul>
 * @see disable_irq
 */
void enable_irq(unsigned int irq);

/**
 * @ingroup interrupt
 * @brief Disable an interrupt.
 *
 * @par Description:
 * This API is used to disable the interrupt that has a specified interrupt ID.
 * @attention
 * <ul>
 * <li>The maximum number of interrupts supported by the OS Kernel is pre-configured
 * (for details on how to pre-configure an appropriate upper threshold, see the reference document provided by
 * the chip supplier). If the maximum number of interrupts is exceeded, the interrupt fails to be disabled.</li>
 * </ul>
 *
 * @param irq   [IN] ID of the interrupt to be disabled. [OS_USER_HWI_MIN, OS_USER_HWI_MAX].
 *
 * @retval None.
 * @par Dependency:
 * <ul><li>interrupt.h: the header file that contains the API declaration.</li></ul>
 * @see enable_irq
 */
void disable_irq(unsigned int irq);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_INTERRUPT_H */
