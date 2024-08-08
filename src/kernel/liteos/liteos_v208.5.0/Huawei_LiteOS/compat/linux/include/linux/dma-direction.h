#ifndef _LINUX_DMA_DIRECTION_H
#define _LINUX_DMA_DIRECTION_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*
 * These definitions mirror those in pci.h, so they can be used
 * interchangeably with their PCI_ counterparts.
 */
#define    DMA_BIDIRECTIONAL  0
#define    DMA_TO_DEVICE      1
#define    DMA_FROM_DEVICE    2
#define    DMA_NONE           3

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_DMA_DIRECTION_H */
