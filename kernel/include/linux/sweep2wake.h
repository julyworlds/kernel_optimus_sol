/* include/linux/sweep2wake.h */

#ifndef _LINUX_SWEEP2WAKE_H
#define _LINUX_SWEEP2WAKE_H
#include <linux/input.h>

void sw_unlock(u16 state);

void sw_lock(u16 state);

void setInputDev(struct input_dev *input_dev);

#endif
