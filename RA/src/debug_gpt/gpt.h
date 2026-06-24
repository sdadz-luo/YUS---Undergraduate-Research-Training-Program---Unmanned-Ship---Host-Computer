#ifndef _GPT_H_
#define _GPT_H_

#include "hal_data.h"
#include <stdbool.h>

/*============================================================================*
 * 函数声明
 *============================================================================*/

/**
 * @brief 初始化 GPT 定时器
 */
void Gpt_Init(void);

/**
 * @brief 启动定时器
 */
void Gpt_Start(void);

/**
 * @brief 停止定时器
 */
void Gpt_Stop(void);

/**
 * @brief 查询定时器周期性触发标志并清除
 * @return true=定时周期到
 */
bool Gpt_TimeoutCheck(void);

/*============================================================================*
 * FSP 回调函数声明
 *============================================================================*/
void gpt0_callback(timer_callback_args_t *p_args);

#endif /* _GPT_H_ */