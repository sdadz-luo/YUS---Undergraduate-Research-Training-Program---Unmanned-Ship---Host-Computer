#ifndef _GPT_H_
#define _GPT_H_

#include "hal_data.h"

void gpt0_init(void);
void gpt0_callback(timer_callback_args_t *p_args);

#endif