# LHE7909 心电采集项目

这是一个基于 **LHE7909 芯片** 的 8 通道心电（ECG）采集系统。
采集板负责把人体心电信号采下来、通过 USB 串口上传，支持在线切换采样率、暂停采集与读写寄存器。

## 这套系统有什么

- 📡 **采集板固件**（STM32 + LHE7909，USB 串口）：实时采集、打包上传，支持采样率/暂停/寄存器配置命令
- 📐 **硬件设计**：原理图、装配图、MCU 核心板资料

## 目录结构（挑重要的看）

```
lhe7909/
├── firmware/
│   ├── lhe7909_uart_demo_firmware_v1/    ← USB 串口采集固件（主固件）
│   └── lhe7909_ACLOTest_firmware_v1/     ← ACLO（导联脱落）测试固件
├── hardware/                             ← 硬件设计文档（原理图/装配图 PDF）
└── doc/                                  ← 芯片资料
    └── LHLX-3-DS-135 LHE7909_Datasheet_CN_Rev.A.docx
```

## 怎么跑起来

1. 用 Keil MDK 打开 `firmware/lhe7909_uart_demo_firmware_v1/MDK-ARM/` 下的工程并编译
2. 烧录固件到 STM32H750 采集板
3. 采集板通过 USB 串口连接主机（默认波特率 1500000）

## 新同事先看这里

- 想了解硬件 → 看 `hardware/` 下的原理图/装配图 PDF
- 想改固件 → `firmware/lhe7909_uart_demo_firmware_v1/`（Keil MDK 工程在 `MDK-ARM/`）
- 芯片细节 → `doc/LHLX-3-DS-135 LHE7909_Datasheet_CN_Rev.A.docx`

## 说明

- 本仓库由 LHE7908 项目派生，当前已完成产品名称/界面/工程文件的 LHE7909 改名；
  芯片相关的采样率档位、寄存器默认值等细节请以
  `doc/LHLX-3-DS-135 LHE7909_Datasheet_CN_Rev.A.docx` 为准，并在驱动移植时逐项核对。
- 蓝牙穿戴版（nRF52832）不在本仓库范围内。