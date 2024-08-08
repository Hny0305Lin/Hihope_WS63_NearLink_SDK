#ifndef _LINUX_SUSPEND_H
#define _LINUX_SUSPEND_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef int suspend_state_t;

#define PM_SUSPEND_ON           ((suspend_state_t)0)
#define PM_SUSPEND_FREEZE       ((suspend_state_t)1)
#define PM_SUSPEND_STANDBY      ((suspend_state_t)2)
#define PM_SUSPEND_MEM          ((suspend_state_t)3)
#define PM_SUSPEND_MIN          PM_SUSPEND_FREEZE
#define PM_SUSPEND_MAX          ((suspend_state_t)4)

int pm_suspend(suspend_state_t state);
void pm_resume(void);

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#endif
