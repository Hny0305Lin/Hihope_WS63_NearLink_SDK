/**
 * @defgroup driver Platform device
 * @ingroup linux
 */

#ifndef _LINUX_PLATFORM_DEVICE_H
#define _LINUX_PLATFORM_DEVICE_H

#include "linux/device.h"
#include "linux/kernel.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define IORESOURCE_IO       0x00000100  /* PCI/ISA I/O ports */
#define IORESOURCE_MEM      0x00000200
#define IORESOURCE_REG      0x00000300  /* Register offsets */
#define IORESOURCE_IRQ      0x00000400
#define IORESOURCE_DMA      0x00000800
#define IORESOURCE_BUS      0x00001000

typedef uintptr_t resource_size_t;

struct resource {
    resource_size_t start;
    resource_size_t end;
    const char *name;
    unsigned long flags;
    unsigned long desc;
    struct resource *parent, *sibling, *child;
};

/**
 * @ingroup driver
 * Define the structure of the parameters used for platform device creation.
 */
struct platform_device {
    const char *name;           /**< device name. */
    struct device dev;          /**< device. */
    int id;                     /**< device id. */
    UINT32 num_resources;       /**< Number of resources used by devices. */
    struct resource *resource;  /**< Resources used by the device. */
};

struct pm_message_t {
    int event;
};

/**
 * @ingroup driver
 * Define the structure of the parameters used for platform driver creation.
 */
struct platform_driver {
    int (*probe)(struct platform_device *);         /**< Called to query the existence of a specific device. */
    int (*remove)(struct platform_device *);        /**< Remove the device, and unbind this device from driver. */
    void (*shutdown)(struct platform_device *);     /**< Shut-down the device. */
    int (*suspend)(struct platform_device *);       /**< Put the device to sleep mode. */
    int (*resume)(struct platform_device *);        /**< Resume the device from sleep mode. */
    struct device_driver driver;                    /**< device driver. */
};

/**
 * @ingroup  driver
 * @brief register a driver for platform device.
 *
 * @par Description:
 * This API is used to register a driver for platform device.
 *
 * @attention
 * <ul>
 * <li> None. </li>
 * </ul>
 *
 * @param  drv   [IN] platform driver.
 *
 * @retval # 0          The driver is successfully registered.
 * @retval # EINVAL     Invalid parameter.
 * @retval # others     Failed to register the driver.
 * @par Dependency:
 * <ul>
 * <li>los_drivers.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see platform_driver_unregister
 */
extern INT32 platform_driver_register(struct platform_driver *drv);

/**
 * @ingroup  driver
 * @brief unregister a driver for platform device.
 *
 * @par Description:
 * This API is used to unregister a driver for platform device.
 *
 * @attention
 * <ul>
 * <li> None. </li>
 * </ul>
 *
 * @param  drv   [IN] platform driver.
 *
 * @retval # 0          The driver is successfully registered.
 * @retval # EINVAL     Invalid parameter.
 * @retval # others     Failed to register the driver.
 * @par Dependency:
 * <ul>
 * <li>platform_device.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see platform_driver_register
 */
extern INT32 platform_driver_unregister(struct platform_driver *drv);

/**
 * @ingroup  driver
 * @brief register a platform device.
 *
 * @par Description:
 * This API is used to register a platform device.
 *
 * @attention
 * None.
 *
 * @param  dev   [IN] platform device.
 *
 * @retval # 0          The device is successfully registered.
 * @retval # EINVAL     Invalid parameter.
 * @retval # others     Failed to register the device.
 * @par Dependency:
 * <ul>
 * <li>platform_device.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see platform_device_unregister
 */
extern INT32 platform_device_register(struct platform_device *dev);

/**
 * @ingroup  driver
 * @brief unregister a platform device.
 *
 * @par Description:
 * This API is used to unregister a platform device.
 *
 * @attention
 * None.
 *
 * @param  dev   [IN] platform device.
 *
 * @retval # None.
 * @par Dependency:
 * <ul>
 * <li>platform_device.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see platform_device_register
 */
extern VOID platform_device_unregister(struct platform_device *dev);

/**
 * @ingroup  driver
 * @brief Gets a resource for a device.
 *
 * @par Description:
 * This API is used to get a resource for a device.
 *
 * @attention
 * None.
 *
 * @param  dev   [IN] platform device.
 * @param  type  [IN] resource type.
 * @param  num   [IN] resource index.
 *
 * @retval # resource.
 * @par Dependency:
 * <ul>
 * <li>platform_device.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern struct resource *platform_get_resource(struct platform_device *dev,
                                              unsigned int type, unsigned int num);

/**
 * @ingroup  driver
 * @brief Gets an available interrupt number for the device.
 *
 * @par Description:
 * This API is used to get an available interrupt number for the device.
 *
 * @attention
 * None.
 *
 * @param  dev   [IN] platform device.
 * @param  num   [IN] interrupt number index.
 *
 * @retval # interrupt number.
 * @par Dependency:
 * <ul>
 * <li>platform_device.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
extern long platform_get_irq(struct platform_device *dev, unsigned int num);

/*
 * This is not a standard interface for linux.
 */
extern VOID *platform_ioremap_resource(struct resource *res);

/**
 * @ingroup  driver
 * @brief Obtains the address of the struct platform_driver based on drv.
 *
 * @par Description:
 * This API is used to obtain the address of the struct platform_driver based on drv.
 *
 * @attention
 * None.
 *
 * @param  drv   [IN] Point to driver.
 *
 * @retval # the address of the struct platform_driver.
 * @par Dependency:
 * <ul>
 * <li>platform_device.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define to_platform_driver(drv) (container_of((drv), struct platform_driver, driver))

/**
 * @ingroup  driver
 * @brief Obtains the address of the struct platform_device based on dev.
 *
 * @par Description:
 * This API is used to obtain the address of the struct platform_device based on dev.
 *
 * @attention
 * None.
 *
 * @param  dev   [IN] Point to device.
 *
 * @retval # the address of the struct platform_device.
 * @par Dependency:
 * <ul>
 * <li>platform_device.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see None.
 */
#define to_platform_device(x) container_of((x), struct platform_device, dev)

/**
 * @ingroup  driver
 * @brief Get the device private data.
 *
 * @par Description:
 * This API is used to get the device private data.
 *
 * @attention
 * <ul>
 * <li> The parameter passed in should be a legal pointer. </li>
 * </ul>
 *
 * @param  dev   [IN] Point to device.
 *
 * @retval # the device private data.
 * @par Dependency:
 * <ul>
 * <li>platform_device.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see platform_set_drvdata
 */
STATIC INLINE VOID *platform_get_drvdata(const struct platform_device *dev)
{
    if (dev == NULL) {
        PRINT_WARN("platform_get_drvdata :the input dev is NULL!\n");
        return NULL;
    }
    return dev_get_drvdata(&dev->dev);
}

/**
 * @ingroup  driver
 * @brief Set the device private data.
 *
 * @par Description:
 * This API is used to set the device private data.
 *
 * @attention
 * None.
 *
 * @param  dev   [IN] Point to device.
 * @param  data  [IN] Point to data.
 *
 * @retval # None.
 * @par Dependency:
 * <ul>
 * <li>platform_device.h: the header file that contains the API declaration.</li>
 * </ul>
 * @see platform_get_drvdata
 */
STATIC INLINE VOID platform_set_drvdata(struct platform_device *dev, VOID *data)
{
    if (dev == NULL) {
        PRINT_WARN("platform_set_drvdata :the input dev is NULL!\n");
        return;
    }
    dev_set_drvdata(&dev->dev, data);
}

#define __combine_1(x, y)   x##y
#define __combine(x, y)     __combine_1(x, y)

/* temporary device/list name for platform_foreach_device/platform_foreach_device_by_driver */
#define temp_dev    __combine(_dev, __LINE__)
#define temp_list   __combine(_list, __LINE__)

/*
 * This is for going through all the registered devices on the "platform".
 * The input 'dev' is struct device * device.
 * This is not a standard interface for linux.
 */
#define platform_foreach_device(dev)    \
    struct LosDevice *temp_dev;         \
    LOS_FOREACH_DEVICE_HOOK(temp_dev, (dev = container_of(temp_dev, struct device, losdevice)))

/*
 * This is for going through all the attached devices on drivers.
 * The input 'dev' is struct device * device, 'drv' is struct device_driver * driver.
 * This is not a standard interface for linux.
 */
#define platform_foreach_device_by_driver(dev, drv)                 \
    struct LosDevice *temp_dev;                                     \
    struct LOS_DL_LIST *temp_list = &(drv)->losdriver.deviceList;   \
    LOS_FOREACH_DEVICE_LIST_HOOK(temp_dev, temp_list, (dev = container_of(temp_dev, struct device, losdevice)))

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
