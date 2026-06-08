/*
 * Copyright (c) 2020 - 2026 Renesas Electronics Corporation and/or its affiliates
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * =====================================================================================
 * UART API 使用说明（中文）
 * =====================================================================================
 *
 * 一、概述
 * --------
 * 本文件定义了 Renesas FSP (Flexible Software Package) 中 UART 接口的通用 API。
 * 该接口为 UART HAL 驱动程序提供了统一的通信框架，支持全双工通信、中断驱动
 * 收发、回调函数、运行时波特率更改、硬件资源锁定以及 CTS/RTS 硬件流控制等功能。
 *
 * 二、基本用法
 * -----------
 * 使用 UART API 通常需要以下几个步骤：
 *
 *   1. 声明控制块和控制结构体
 *      - 定义一个 uart_ctrl_t 类型的指针变量，作为 UART 控制块。
 *      - 定义一个 uart_cfg_t 类型的结构体变量，其中包含通道、数据位、奇偶校验、
 *        停止位、中断优先级等配置信息。
 *      - 定义 uart_instance_t 结构体，将控制块、配置和 API 接口函数集组合在一起。
 *
 *   2. 初始化 UART
 *      - 调用 g_uart.p_api->open(&g_uart.p_ctrl, &g_uart.p_cfg) 打开并初始化
 *        UART 设备，配置中断、波特率、数据格式等。
 *      - 配置回调函数（可选）以接收事件通知（如接收完成、发送完成、错误等）。
 *
 *   3. 发送数据
 *      - 调用 g_uart.p_api->write(&g_uart.p_ctrl, p_src, bytes) 发送数据。
 *      - 数据通过中断或 DMA（需配置传输实例）发送。发送完成后会调用回调函数
 *        并传入 UART_EVENT_TX_COMPLETE 事件。
 *
 *   4. 接收数据
 *      - 调用 g_uart.p_api->read(&g_uart.p_ctrl, p_dest, bytes) 启动读取操作。
 *      - 数据通过中断或 DMA（需配置传输实例）接收，读取完成后回调函数被调用
 *        并传入 UART_EVENT_RX_COMPLETE 事件。
 *      - 未启动批量读取时，接收到的单个字符会通过回调函数以 UART_EVENT_RX_CHAR
 *        事件通知用户。
 *
 *   5. 关闭 UART
 *      - 调用 g_uart.p_api->close(&g_uart.p_ctrl) 关闭 UART 设备，释放相关
 *        硬件资源。
 *
 * 三、高级功能
 * -----------
 *   - 波特率调整：运行时调用 baudSet() 可动态更改波特率。
 *   - 读取停止：调用 readStop() 可终止正在进行的读取操作。
 *   - 接收挂起/恢复：使用 receiveSuspend() 和 receiveResume() 暂停与恢复
 *     接收操作。
 *   - 通信中止：使用 communicationAbort() 中止正在进行的发送或接收。
 *   - 回调设置：使用 callbackSet() 在运行时动态更新回调函数和上下文。
 *   - 信息获取：使用 infoGet() 获取驱动特定信息（如单次最大读写字节数）。
 *
 * 四、注意事项
 * -----------
 *   1. 控制块（uart_ctrl_t）必须由用户声明，并在 open() 调用时传入地址，
 *      由驱动程序在其中存储实例状态信息。
 *   2. 配置结构体（uart_cfg_t）的所有字段必须由用户在调用 open() 之前
 *      正确填写。
 *   3. 如果未配置 p_transfer_tx（传输实例），则 write API 每次只能发送
 *      一个字节；若未配置 p_transfer_rx，则 read API 每次只能读取一个字节。
 *   4. 在 write 操作完成之前（即收到 UART_EVENT_TX_COMPLETE 回调之前），
 *      切勿修改发送缓冲区内容。
 *   5. 调用 baudSet() 会中止正在进行的发送并禁用接收，直至新波特率配置
 *      生效，需谨慎使用。
 *   6. 回调函数在中断上下文中执行，应保持简短，避免阻塞或调用可能引发
 *      重入问题的函数。
 *   7. 错误事件（奇偶校验错误、帧错误、溢出错误、中断检测）会通过回调
 *      函数通知，应用程序应根据具体需求进行相应处理。
 * =====================================================================================
 */

/*******************************************************************************************************************//**
 * @ingroup RENESAS_CONNECTIVITY_INTERFACES
 * @defgroup UART_API UART 接口
 * @brief UART 通信接口。
 *
 * @section UART_INTERFACE_SUMMARY 简介
 * UART 接口为 UART HAL 驱动程序提供通用 API。UART 接口支持以下功能：
 * - 全双工 UART 通信
 * - 中断驱动的发送/接收处理
 * - 带事件代码返回的回调函数
 * - 运行时波特率更改
 * - 通信过程中的硬件资源锁定
 * - CTS/RTS 硬件流控制支持（需配合 IOPORT 引脚）
 *
 *
 * @{
 **********************************************************************************************************************/

#ifndef R_UART_API_H
#define R_UART_API_H

/***********************************************************************************************************************
 * 包含文件
 **********************************************************************************************************************/

/* 包含与板和 MCU 相关的头文件。 */
#include "bsp_api.h"
#include "r_transfer_api.h"

/* FSP 头文件的通用宏。本文件末尾也有对应的 FSP_FOOTER 宏。 */
FSP_HEADER

/**********************************************************************************************************************
 * 宏定义
 **********************************************************************************************************************/

/**********************************************************************************************************************
 * 类型定义
 **********************************************************************************************************************/

/** UART 事件代码 */
#ifndef BSP_OVERRIDE_UART_EVENT_T
typedef enum e_sf_event
{
    UART_EVENT_RX_COMPLETE   = (1UL << 0), ///< 接收完成事件
    UART_EVENT_TX_COMPLETE   = (1UL << 1), ///< 发送完成事件
    UART_EVENT_RX_CHAR       = (1UL << 2), ///< 接收到字符
    UART_EVENT_ERR_PARITY    = (1UL << 3), ///< 奇偶校验错误事件
    UART_EVENT_ERR_FRAMING   = (1UL << 4), ///< 帧错误事件
    UART_EVENT_ERR_OVERFLOW  = (1UL << 5), ///< FIFO 溢出错误事件
    UART_EVENT_BREAK_DETECT  = (1UL << 6), ///< 中断检测错误事件
    UART_EVENT_TX_DATA_EMPTY = (1UL << 7), ///< 最后一个字节正在发送，准备接收更多数据
} uart_event_t;
#endif
#ifndef BSP_OVERRIDE_UART_DATA_BITS_T

/** UART 数据位长度定义 */
typedef enum e_uart_data_bits
{
    UART_DATA_BITS_9 = 0U,             ///< 数据位 9 位
    UART_DATA_BITS_8 = 2U,             ///< 数据位 8 位
    UART_DATA_BITS_7 = 3U,             ///< 数据位 7 位
} uart_data_bits_t;
#endif
#ifndef BSP_OVERRIDE_UART_PARITY_T

/** UART 奇偶校验定义 */
typedef enum e_uart_parity
{
    UART_PARITY_OFF  = 0U,             ///< 无校验
    UART_PARITY_ZERO = 1U,             ///< 零校验
    UART_PARITY_EVEN = 2U,             ///< 偶校验
    UART_PARITY_ODD  = 3U,             ///< 奇校验
} uart_parity_t;
#endif

/** UART 停止位定义 */
typedef enum e_uart_stop_bits
{
    UART_STOP_BITS_1 = 0U,             ///< 停止位 1 位
    UART_STOP_BITS_2 = 1U,             ///< 停止位 2 位
} uart_stop_bits_t;

/** UART 通信方向定义 */
typedef enum e_uart_dir
{
    UART_DIR_RX_TX = 3U,               ///< 同时支持接收和发送
    UART_DIR_RX    = 1U,               ///< 仅接收
    UART_DIR_TX    = 2U,               ///< 仅发送
} uart_dir_t;

/** UART 驱动特定信息 */
typedef struct st_uart_info
{
    /** 当前可写入的最大字节数。仅在 uart_cfg_t::p_transfer_tx 不为 NULL 时适用。 */
    uint32_t write_bytes_max;

    /** 当前可读取的最大字节数。仅在 uart_cfg_t::p_transfer_rx 不为 NULL 时适用。 */
    uint32_t read_bytes_max;
} uart_info_t;

/** UART 回调参数定义 */
typedef struct st_uart_callback_arg
{
    uint32_t     channel;              ///< 设备通道号
    uart_event_t event;                ///< 事件代码

    /** 包含通过 UART_EVENT_RX_CHAR、UART_EVENT_ERR_PARITY、
     * UART_EVENT_ERR_FRAMING 或 UART_EVENT_ERR_OVERFLOW 事件接收到的下一个字符。
     * 其他情况下未使用。 */
    uint32_t data;
    void   * p_context;                ///< 回调期间提供给用户的上下文
} uart_callback_args_t;

/** UART 配置 */
typedef struct st_uart_cfg
{
    /* UART 通用配置 */
    uint8_t          channel;          ///< 选择与硬件通道号对应的通道
    uart_data_bits_t data_bits;        ///< 数据位长度（8 或 7 或 9）
    uart_parity_t    parity;           ///< 奇偶校验类型（无校验/奇校验/偶校验）
    uart_stop_bits_t stop_bits;        ///< 停止位长度（1 或 2）
    uint8_t          rxi_ipl;          ///< 接收中断优先级
    IRQn_Type        rxi_irq;          ///< 接收中断 IRQ 号
    uint8_t          txi_ipl;          ///< 发送中断优先级
    IRQn_Type        txi_irq;          ///< 发送中断 IRQ 号
    uint8_t          tei_ipl;          ///< 发送结束中断优先级
    IRQn_Type        tei_irq;          ///< 发送结束中断 IRQ 号
    uint8_t          eri_ipl;          ///< 错误中断优先级
    IRQn_Type        eri_irq;          ///< 错误中断 IRQ 号

    /** 用于在没有中断的情况下接收多个字节的可选传输实例。如果不使用，设置为 NULL。
     * 如果为 NULL，则 read API 每次仅允许读取一个字节。 */
    transfer_instance_t const * p_transfer_rx;

    /** 用于在没有中断的情况下发送多个字节的可选传输实例。如果不使用，设置为 NULL。
     * 如果为 NULL，则 write API 每次仅允许发送一个字节。 */
    transfer_instance_t const * p_transfer_tx;

    /* UART 事件处理配置 */
    void (* p_callback)(uart_callback_args_t * p_args); ///< 指向回调函数的指针
    void * p_context;                                   ///< 传入回调函数的用户定义上下文

    /* 指向 UART 外设特定配置的指针 */
    void const * p_extend;                              ///< UART 硬件相关配置
} uart_cfg_t;

/** UART 控制块。分配一个实例特定的控制块，用于传入 UART API 调用。
 */
typedef void uart_ctrl_t;

/** UART 共享接口定义 */
typedef struct st_uart_api
{
    /** 打开 UART 设备。
     *
     * @param[in,out]  p_ctrl     指向 UART 控制块的指针。必须由用户声明。此处设置值。
     * @param[in]      uart_cfg_t 指向 UART 配置结构体的指针。此结构体的所有元素必须由用户设置。
     */
    fsp_err_t (* open)(uart_ctrl_t * const p_ctrl, uart_cfg_t const * const p_cfg);

    /** 从 UART 设备读取数据。读取缓冲区在读取完成前持续使用。传输完成时，
     * 回调函数被调用并传入 UART_EVENT_RX_COMPLETE 事件。
     * 在活动传输之外接收到的字节，通过回调函数以 UART_EVENT_RX_CHAR 事件接收。
     * 最大传输大小由 infoGet() 报告。
     *
     * @param[in]   p_ctrl     指向该通道 UART 控制块的指针。
     * @param[in]   p_dest     读取数据的目标地址。
     * @param[in]   bytes      读取数据长度。
     */
    fsp_err_t (* read)(uart_ctrl_t * const p_ctrl, uint8_t * const p_dest, uint32_t const bytes);

    /** 向 UART 设备写入数据。写入缓冲区在写入完成前持续使用。写入完成前不要覆盖
     * 写入缓冲区内容。写入完成时（所有字节已完全在线上传输完毕），
     * 回调函数被调用并传入 UART_EVENT_TX_COMPLETE 事件。
     * 最大传输大小由 infoGet() 报告。
     *
     * @param[in]   p_ctrl     指向 UART 控制块的指针。
     * @param[in]   p_src      要写入数据的源地址。
     * @param[in]   bytes      写入数据长度。
     */
    fsp_err_t (* write)(uart_ctrl_t * const p_ctrl, uint8_t const * const p_src, uint32_t const bytes);

    /** 更改波特率。
     * @warning 调用此 API 会中止任何正在进行的发送，并在新波特率设置生效前
     * 禁用接收功能。
     *
     *
     * @param[in]   p_ctrl          指向 UART 控制块的指针。
     * @param[in]   p_baudrate_info 指向配置波特率的模块特定信息的指针。
     */
    fsp_err_t (* baudSet)(uart_ctrl_t * const p_ctrl, void const * const p_baudrate_info);

    /** 获取驱动程序特定信息。
     *
     * @param[in]    p_ctrl     指向 UART 控制块的指针。
     * @param[out]   p_info     指向 UART 信息结构体的指针。
     */
    fsp_err_t (* infoGet)(uart_ctrl_t * const p_ctrl, uart_info_t * const p_info);

    /**
     * 中止正在进行的传输。
     *
     * @param[in]   p_ctrl                   指向 UART 控制块的指针。
     * @param[in]   communication_to_abort   中止请求的类型。
     */
    fsp_err_t (* communicationAbort)(uart_ctrl_t * const p_ctrl, uart_dir_t communication_to_abort);

    /**
     * 指定回调函数以及可选的上下文指针和工作内存指针。
     *
     * @param[in]   p_ctrl                   指向 UART 控制块的指针。
     * @param[in]   p_callback               回调函数
     * @param[in]   p_context                发送给回调函数的指针
     * @param[in]   p_callback_memory        指向可分配回调结构体的易失性内存的指针。
     *                                       在此处分配的回调参数仅在回调期间有效。
     */
    fsp_err_t (* callbackSet)(uart_ctrl_t * const p_ctrl, void (* p_callback)(uart_callback_args_t *),
                              void * const p_context, uart_callback_args_t * const p_callback_memory);

    /** 关闭 UART 设备。
     *
     * @param[in]   p_ctrl     指向 UART 控制块的指针。
     */
    fsp_err_t (* close)(uart_ctrl_t * const p_ctrl);

    /** 停止正在进行的读取操作，并返回读取操作中剩余的字节数。
     *
     * @param[in]      p_ctrl                指向 UART 控制块的指针。
     * @param[in,out]  remaining_bytes       指向存储读取剩余字节数的位置的指针。
     */
    fsp_err_t (* readStop)(uart_ctrl_t * const p_ctrl, uint32_t * remaining_bytes);

    /** 挂起 UART 设备的接收操作。
     *
     * @param[in]   p_ctrl     指向 UART 控制块的指针。
     */
    fsp_err_t (* receiveSuspend)(uart_ctrl_t * const p_ctrl);

    /** 恢复 UART 设备的接收操作。
     *
     * @param[in]   p_ctrl     指向 UART 控制块的指针。
     */
    fsp_err_t (* receiveResume)(uart_ctrl_t * const p_ctrl);
} uart_api_t;

/** 该结构体包含了使用此接口实例所需的一切。 */
typedef struct st_uart_instance
{
    uart_ctrl_t      * p_ctrl;         ///< 指向该实例控制结构体的指针
    uart_cfg_t const * p_cfg;          ///< 指向该实例配置结构体的指针
    uart_api_t const * p_api;          ///< 指向该实例 API 结构体的指针
} uart_instance_t;

/** @} (end defgroup UART_API) */

/* FSP 头文件的通用宏。本文件顶部也有对应的 FSP_HEADER 宏。 */
FSP_FOOTER

#endif
