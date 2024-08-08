#ifndef _LINUX_MTD_LIST_H
#define _LINUX_MTD_LIST_H

#include "linux/mtd/mtd.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct Node {
    char *type;           /* flash type */
    int status;           /* flash status */
    struct mtd_info *mtd; /* mtd_info struct */
    struct Node *next;    /* next mtd_info struct */
} Lnode, *linklist;

extern linklist head;
struct mtd_info* get_mtd(const char *type);
int get_mtd_info(const char *type);
int free_mtd(struct mtd_info *mtd);
int del_mtd_list(struct mtd_info *mtd);
void add_mtd_list(char *type, struct mtd_info *mtd);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _LINUX_MTD_LIST_H */
