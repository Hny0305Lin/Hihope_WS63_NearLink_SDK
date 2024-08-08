#ifndef _LINUX_PM_H
#define _LINUX_PM_H

#include "linux/list.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct pm_message {
    int event;
} dev_pm_message_t;

struct dev_pm_info {
    dev_pm_message_t power_state;
    LOS_DL_LIST      entry;
};

#define OS_EVENT_SUSPEND_FLAG        0x0002
#define OS_EVENT_RESUME_FLAG         0x0010

#define OS_SUSPEND_INIT    ((struct pm_message){ .event = OS_EVENT_SUSPEND_FLAG, })
#define OS_RESUME_INIT     ((struct pm_message){ .event = OS_EVENT_RESUME_FLAG, })

int dpm_suspend_start(dev_pm_message_t state);
void dpm_resume_end(dev_pm_message_t state);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

