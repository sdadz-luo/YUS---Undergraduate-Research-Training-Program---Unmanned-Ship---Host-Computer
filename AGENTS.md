# AGENTS.md - 无人船上位机 (Host Computer)

## 项目性质

嵌入式 C 项目，**Renesas RA6M5 (R7FA6M5BF)** 主控 + 淘晶驰串口屏 + LoRa 无线数传，用于遥控无人船。IDE 为 **Keil MDK-ARM V5.43** (ARMClang V6.24)，外设配置依赖 **Renesas FSP V6.4.0**。

```
RA/              <- RA6M5 主项目（活跃）
STM32/           <- STM32H750 项目（未完成，不修改）
上位机/          <- 串口屏素材（HMI 工程、图片、字库）
```

GitHub 远端：`HostComputer -> https://github.com/sdadz-luo/YUS---Undergraduate-Research-Training-Program---Unmanned-Ship---Host-Computer.git`

## 关键文件

| 路径 | 说明 |
|------|------|
| `RA/FSP_Project.uvprojx` | Keil MDK 项目文件（双击打开） |
| `RA/src/hal_entry.c` | **用户主入口**（等价 `main`） |
| `RA/src/debug_*/*.{c,h}` | 用户模块代码 |
| `RA/ra_gen/` | **FSP 自动生成，勿手动编辑** |
| `RA/ra_cfg/` | FSP 配置文件（由 RASC 管理） |
| `RA/.opencode/lsp.json` | LSP 配置（clangd） |
| `first.HMI` | 串口屏 HMI 工程文件（淘晶驰编辑器打开，二进制格式） |

## 构建流程

1. 外设配置变更 -> 使用 **RASC** (RA Configurator) 修改 `RA/configuration.xml` -> 生成代码到 `ra_gen/`、`ra_cfg/`
2. 编译 -> 在 **Keil MDK** 中打开 `RA/FSP_Project.uvprojx` -> Rebuild
3. 烧录 -> 通过 JLink / Keil 下载到目标板

项目没有配置任何运行脚本，也没有 CI。

## 程序入口点

- `main()` 在 `RA/ra_gen/main.c`，自动生成，内部调用 `hal_entry()`
- **所有用户代码写进 `hal_entry()` 即可**
- `R_BSP_WarmStart()` 在 `RA/src/hal_warmstart.c`，初始化 pin 和时钟

## 模块结构

```
RA/src/
+-- hal_entry.c        # 主循环 + 帧解析 + ADC 和 LoRa 调度
+-- debug_uart/        # UART3 + DMAC 接收中断处理 + UART2 + LoRa 发送
+-- debug_gpt/         # GPT 定时器，10ms 周期，触发 ADC
+-- debug_adc/          # ADC 4 通道 + 死区判断 + 摇杆方向合成
+-- debug_lora/        # LoRa 协议（CRC8 + 组包）
```

### 模块内部规范（以下均为代码中观察到，无法确定是否为强约束）：

- 目录前缀 `debug_`
- 函数名 `PascalCase_WithUnderscore`，如 `Uart_Init()`、`Adc_ProcessAll()`
- 回调函数 `snake_case`，如 `adc_callback()`、`gpt0_callback()`
- 宏定义：`UPPER_CASE` + 模块前缀，如 `UART3_RX_BUF_SIZE`
- 头文件保护：`_MODULE_NAME_H_`，如 `_UART_H_`
- C 标准：C11（Target_1 配置，非 C99），参考 VSCode 配置

## 通信协议

所有数据包通过 UART2 (LoRa 模块) 发送：

### 屏端命令（串口屏 -> LoRa 转发）
```
EE <地址> <部件> <数据> CRC8 FF     (6 字节)
```
CRC8 校验范围：`EE ~ 数据`，4 字节

### 摇杆包（ADC -> LoRa 发送）
```
CC 01 <方向1> 02 <方向2> CRC8       (6 字节，无帧尾)
```
CRC8 校验范围：`CC ~ 方向2`，5 字节

### CRC8 算法
- 多项式 `0x31` (x8 + x5 + x4 + 1)
- 初始值 `0x00`

### 部件列表
| 编号 | 部件 | 说明 |
|------|------|------|
| 0x00 | 无操作 | ? |
| 0x01 | 灯 | 0=关, 1=开, 2=闪, 3=爆闪, 4=SOS, 5=自动, 6=导航 |
| 0x02 | 水泵 | 0=关, 1=开 |
| 0x03~0x04 | 云台上下/左右 | 占空比(hex) |
| 0x05~0x06 | 机械臂舵机/占空比 | 角度/占空比(hex) |
| 0x07 | 速度 | 0~10 |
| 0x08 | 新指令 | ? |

## ADC

- 4 通道：AN04 (P004)、AN05 (P005)、AN10 (P010)、AN12 (P014)
- 12-bit，右对齐
- 内置死区约 30%（+-614 对称）
- 每 10ms 触发一次扫描（GPT 定时器）
- 校准：8 次采样取平均作为中心值

## UART

- **UART3**：DMAC0 接收中断传输，256 字节环形缓冲
- **UART2**：LoRa 发送，阻塞等待上一包完成，中断回调知会忙状态
- UART3 支持两种接收模式：DMA 模式为主，RXI 字符中断模式为 fallback

## 引脚分配

| 引脚 | 功能 | 说明 |
|------|------|------|
| P004 | AN04 | 摇杆1 X |
| P005 | AN05 | 摇杆1 Y |
| P010 | AN10 | 摇杆2 X |
| P014 | AN12 | 摇杆2 Y |
| P706 | SCI3 RXD | UART3 接收（串口屏） |
| P707 | SCI3 TXD | UART3 发送（串口屏） |
| P301 | SCI2 RXD | UART2 LoRa 接收 |
| P302 | SCI2 TXD | UART2 LoRa 发送 |

## 注意事项

- **不要编辑 `RA/ra_gen/` 下的任何文件** - 它们是 FSP 自动生成的，会被覆盖
- 外设配置：时钟、引脚、中断优先级、DMA 均通过 **RASC** (RA Configurator) 修改
- 串口屏 UI 使用 **淘晶驰编辑器** 修改 `first.HMI`（二进制格式，无法用文本编辑器编辑）
- `.zi` 和 `.xi` 文件是字库资源/图片源文件
- 项目配置在 `.vscode/c_cpp_properties.json`，`Target_1` 为 RA6M5 ARMClang，另有 `uart_test` 为 STM32H750 ARMClang
- VSCode IntelliSense 对 ARMClang 专有关键字无法识别，已在 `c_cpp_properties.json` 的 `defines` 中用 `#define` 消隐
- `.gitignore` 排除了 `*.o`、`*.axf`、`*.hex`、`DebugConfig/`、`ra_gen/`、`ra_cfg/`、`RTE/` 等编译产物和自动生成部分
