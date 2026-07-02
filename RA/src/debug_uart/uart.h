#ifndef _UART_H_
#define _UART_H_
#include "hal_data.h"
#include "stdio.h"
#include "string.h"
#include <stdbool.h>

/*============================================================================*
 * 宏定义
 *============================================================================*/
#define UART3_RX_BUF_SIZE       256         /* UART3 接收环形缓冲区大小   */
#define UART2_TX_TIMEOUT        1000        /* UART2 发送超时（循环次数） */

/*============================================================================*
 * 函数声明
 *============================================================================*/

/**
 * @brief 初始化所有 UART 模块（UART3 接收 + UART2 发送）
 */
void Uart_Init(void);

/**
 * @brief 从 UART3 环形缓冲区读取接收到的数据
 * @param buf  存放数据的目标缓冲区
 * @param len  期望读取的最大字节数
 * @return 实际读取到的字节数
 */
uint16_t Uart3_GetData(uint8_t *buf, uint16_t len);

/**
 * @brief 获取 UART3 缓冲区中当前可读数据量
 * @return 可读字节数
 */
uint16_t Uart3_DataAvailable(void);

/**
 * @brief 清空 UART3 接收缓冲区
 */
void Uart3_Flush(void);

/**
 * @brief 通过 UART2 (LoRa) 发送数据
 * @param data  待发送数据指针
 * @param len   发送长度
 */
void Uart2_SendData(const uint8_t *data, uint16_t len);

/**
 * @brief 查询 UART2 是否空闲（上一次发送已完成）
 * @return true=空闲, false=正在发送
 */
bool Uart2_IsIdle(void);

/*============================================================================*
 * FSP 回调函数声明（已在 hal_data.h 中前置声明）
 *============================================================================*/
void uart3_callback(uart_callback_args_t *p_args);
void lora_callback(uart_callback_args_t *p_args);

#endif /* _UART_H_ */