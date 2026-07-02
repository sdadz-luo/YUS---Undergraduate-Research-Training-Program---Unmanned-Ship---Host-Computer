#ifndef _ADC_H_
#define _ADC_H_

#include "hal_data.h"
#include <stdbool.h>

/*============================================================================*
 * 宏定义
 *============================================================================*/
#define ADC_CHANNEL_COUNT       4           /* 使用的 ADC 通道数           */

/* 物理通道号映射（与 pin_data.c 的模拟引脚配置一致）
 *   g_adc_results[0] ← AN04 (P004)
 *   g_adc_results[1] ← AN05 (P005)
 *   g_adc_results[2] ← AN10 (P010)
 *   g_adc_results[3] ← AN12 (P014) */
#define ADC_PHYS_CH_0           4
#define ADC_PHYS_CH_1           5
#define ADC_PHYS_CH_2           10
#define ADC_PHYS_CH_3           12

/* 数字量阈值与迟滞（死区）------------------------------------------ */
#define ADC_FULL_RANGE          4095        /* 12位 ADC 满量程           */
#define ADC_HALF_RANGE          2048        /* 半量程（中点到端点）      */
#define ADC_DEAD_ZONE_HALF      614         /* 死区半宽 = 30% × 半量程  */
#define ADC_DEAD_ZONE_PCT       30          /* 死区占半量程百分比        */

/*============================================================================*
 * 函数声明
 *============================================================================*/

/**
 * @brief 初始化 ADC 模块
 */
void Adc_Init(void);

/**
 * @brief 启动一次单次扫描（扫描 AN04/AN05/AN10/AN12）
 */
void Adc_StartScan(void);

/**
 * @brief 读取指定通道的最新转换结果
 * @param channel  逻辑通道号 (0 ~ ADC_CHANNEL_COUNT-1)
 * @return 12位 ADC 原始值
 */
uint16_t Adc_GetResult(uint8_t channel);

/**
 * @brief 查询上次扫描是否完成
 * @return true=完成, false=扫描中
 */
bool Adc_IsScanComplete(void);

/**
 * @brief 获取扫描完成标志并清除
 * @return true=扫描刚完成
 */
bool Adc_ScanCompleteCheck(void);

/*============================================================================*
 * 数字量转换（带迟滞比较）
 *============================================================================*/

/**
 * @brief 设置指定通道的比较阈值
 * @param channel   逻辑通道号
 * @param threshold 阈值 (0~4095)，死区范围为 threshold ± 15%
 */
void Adc_SetThreshold(uint8_t channel, uint16_t threshold);

/**
 * @brief 校准摇杆中心：多次采样取平均，设为各通道阈值
 *
 * 调用前提：已启动 ADC 扫描并等待完成（在校准前确保至少完成一次扫描）。
 * 建议上电后摇杆在静止位置时调用。
 *
 * @param samples  采样次数
 */
void Adc_CalibrateCenter(uint8_t samples);

/**
 * @brief 处理所有通道：将 ADC 原始值转换为方向态 0/1/2
 *
 * 判断规则：
 *   ADC值 > (threshold + 15%) → 输出 1（正向）
 *   ADC值 < (threshold - 15%) → 输出 2（反向）
 *   死区内 (threshold ± 15%) → 输出 0（原点）
 *
 * @note 死区宽度固定为 30% 满量程，对称分布在阈值两侧。
 */
void Adc_ProcessAll(void);

/**
 * @brief 获取指定通道的方向态
 * @param channel 逻辑通道号
 * @return 0=原点, 1=正向, 2=反向
 */
uint8_t Adc_GetDirection(uint8_t channel);

/**
 * @brief 获取指定通道的当前阈值
 * @param channel 逻辑通道号
 * @return 阈值
 */
uint16_t Adc_GetThreshold(uint8_t channel);

/*============================================================================*
 * 摇杆方向合成
 *============================================================================*/

/**
 * @brief 根据 X/Y 轴方向合成摇杆方向
 *
 * 两轴同时动作时，比较偏离中心的幅度，取偏离更大的轴。
 *
 * @param dir_x  X轴方向 (0=原点, 1=正向, 2=反向)
 * @param dir_y  Y轴方向
 * @param adc_x  X轴 ADC 原始值（用于偏差比较）
 * @param adc_y  Y轴 ADC 原始值
 * @param th_x   X轴阈值
 * @param th_y   Y轴阈值
 * @return 0=中间, 1=上, 2=下, 3=左, 4=右
 */
uint8_t Adc_JoystickDir(uint8_t dir_x, uint8_t dir_y,
                        uint16_t adc_x, uint16_t adc_y,
                        uint16_t th_x, uint16_t th_y);

/*============================================================================*
 * FSP 回调函数声明
 *============================================================================*/
void adc_callback(adc_callback_args_t *p_args);

#endif /* _ADC_H_ */