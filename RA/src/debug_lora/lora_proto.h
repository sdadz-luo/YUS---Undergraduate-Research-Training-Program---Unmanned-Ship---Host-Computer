#ifndef _LORA_PROTO_H_
#define _LORA_PROTO_H_

#include "hal_data.h"
#include <stdbool.h>

/*============================================================================*
 * 协议格式定义
 *============================================================================*
 *
 * 屏包（串口屏组件命令）:
 *   EE <船编号> <组件编号> <组件数据> <CRC8> FF
 *   长度: 6 字节
 *
 * 摇杆包（行驶方向控制）:
 *   CC 01 <小白方向> 02 <小黑方向> <CRC8> 33
 *   长度: 7 字节
 *============================================================================*/

#define LORA_CRC_POLY           0x31        /* CRC-8 多项式               */

/* 屏包定义 ---------------------------------------------------------------- */
#define SCR_PKT_HEADER          0xEE        /* 帧头                       */
#define SCR_PKT_TAIL            0xFF        /* 帧尾                       */
#define SCR_PKT_SIZE            6           /* 屏包长度                   */
#define SCR_PKT_CRC_LEN         4           /* CRC8 校验范围（EE~数据）  */

/* 摇杆包定义 -------------------------------------------------------------- */
#define JOY_PKT_HEADER          0xCC        /* 控制帧头                   */
#define JOY_PKT_TAIL            0x33        /* 控制帧尾                   */
#define JOY_PKT_BOAT1_ID        0x01        /* 小白标识                   */
#define JOY_PKT_BOAT2_ID        0x02        /* 小黑标识                   */
#define JOY_PKT_SIZE            7           /* 摇杆包长度                 */
#define JOY_PKT_CRC_LEN         5           /* CRC8 校验范围（CC~方向）  */

/* 行驶方向宏（可按需修改）----------------------------------------------- */
#define LORA_DIR_STOP           0x00        /* 停止                       */
#define LORA_DIR_FORWARD        0x01        /* 前进                       */
#define LORA_DIR_BACKWARD       0x02        /* 后退                       */
#define LORA_DIR_LEFT           0x03        /* 左转                       */
#define LORA_DIR_RIGHT          0x03        /* 右转（暂与左相同，可改）   */

/* 摇杆方向(0~4) 转 协议行驶方向 ------------------------------------------ */
#define JOYDIR_TO_LORA(dir)  ((dir) == 0 ? LORA_DIR_STOP    :     \
                              (dir) == 1 ? LORA_DIR_FORWARD  :     \
                              (dir) == 2 ? LORA_DIR_BACKWARD :     \
                              (dir) == 3 ? LORA_DIR_LEFT     :     \
                                           LORA_DIR_RIGHT)

/*============================================================================*
 * 函数声明
 *============================================================================*/

/**
 * @brief 计算 CRC-8 校验（多项式 0x31）
 */
uint8_t Lora_CalcCRC8(const uint8_t *data, uint16_t len);

/**
 * @brief 构建屏包：EE <船号> <组件> <数据> CRC8 FF
 * @param boat      船编号（1=小白, 2=小黑）
 * @param comp      组件编号
 * @param comp_data 组件数据
 * @param packet    输出缓冲区（至少 SCR_PKT_SIZE 字节）
 */
void Lora_BuildScrPacket(uint8_t boat, uint8_t comp, uint8_t comp_data,
                         uint8_t *packet);

/**
 * @brief 构建摇杆包：CC 01 <小白方向> 02 <小黑方向> CRC8 33
 * @param dir1  小白方向（0=停,1=前,2=后,3=左,4=右）
 * @param dir2  小黑方向
 * @param packet 输出缓冲区（至少 JOY_PKT_SIZE 字节）
 */
void Lora_BuildJoyPacket(uint8_t dir1, uint8_t dir2, uint8_t *packet);

/**
 * @brief 发送数据包（通过 UART2）
 * @param packet 数据包指针
 * @param len    包长度
 */
void Lora_SendPacket(const uint8_t *packet, uint16_t len);

#endif /* _LORA_PROTO_H_ */
