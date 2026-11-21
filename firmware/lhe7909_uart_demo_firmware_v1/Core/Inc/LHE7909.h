#include "main.h"


#include "stdio.h"
#include <stdbool.h>


#define ID_ADDRESS                         0x00 

#define CONFIG1_ADDRESS                    0x01 
#define CONFIG2_ADDRESS                    0x02 
#define CONFIG3_ADDRESS                    0x03 
#define LOFF_ADDRESS                       0x04


#define CH1SET_ADDRESS                     0x05
#define CH2SET_ADDRESS                     0x06
#define CH3SET_ADDRESS                     0x07
#define CH4SET_ADDRESS                     0x08
#define CH5SET_ADDRESS                     0x09
#define CH6SET_ADDRESS                     0x0A
#define CH7SET_ADDRESS                     0x0B
#define CH8SET_ADDRESS                     0x0C
#define RLD_SENSP_ADDRESS                  0x0D
#define RLD_SENSN_ADDRESS                  0x0E
/* 0x0D/0x0E 在本芯片同时是偏置求和选择寄存器 BIAS_SENSP/BIAS_SENSN（历史命名 RLD_SENSP/N
 * 与其为同一地址）。BIAS_AMP 闭环测试按 BIAS_SENSP/N 语义使用：bit0=1 将 IN1P/IN1N
 * 接入偏置求和网络。 */
#define BIAS_SENSP_ADDRESS                 RLD_SENSP_ADDRESS
#define BIAS_SENSN_ADDRESS                 RLD_SENSN_ADDRESS
#define LOFF_SENSP_ADDRESS                 0x0F
#define LOFF_SENSN_ADDRESS                 0x10
#define LOFF_FLIP_ADDRESS                  0x11

#define LOFF_STATP_ADDRESS                 0x12
#define LOFF_STATN_ADDRESS                 0x13


#define GPIO_ADDRESS                       0x14
#define PACE_ADDRESS                       0x15
#define RESP_ADDRESS                       0x16
#define CONFIG4_ADDRESS                    0x17
#define WCT1_ADDRESS                       0x18
#define WCT2_ADDRESS                       0x19
/* 扩展寄存器（地址 >= 0x40 时，读用 DUT_RTREG/写用 DUT_WTREG） */
#define DIG_ACLOFF_ENABLE_ADDRESS  0X40

#define DIG_ACLOFF_ENABLE_HIGH_ADDR  (DIG_ACLOFF_ENABLE_ADDRESS >> 8)
#define DIG_ACLOFF_ENABLE_LOW_ADDR   (DIG_ACLOFF_ENABLE_ADDRESS & 0xFF)


#define LOFF_UTH_HIGH_ADDRESS  0X41
#define LOFF_UTH_HIGH_HIGH_ADDR  (LOFF_UTH_HIGH_ADDRESS >> 8)
#define LOFF_UTH_HIGH_LOW_ADDR   (LOFF_UTH_HIGH_ADDRESS & 0xFF)

#define LOFF_UTH_LOW_ADDRESS  0X42
#define LOFF_UTH_LOW_HIGH_ADDR  (LOFF_UTH_LOW_ADDRESS >> 8)
#define LOFF_UTH_LOW_LOW_ADDR   (LOFF_UTH_LOW_ADDRESS & 0xFF)

#define LOFF_LTH_HIGH_ADDRESS  0X43
#define LOFF_LTH_HIGH_HIGH_ADDR  (LOFF_LTH_HIGH_ADDRESS >> 8)
#define LOFF_LTH_HIGH_LOW_ADDR  (LOFF_LTH_HIGH_ADDRESS & 0xFF)

#define LOFF_LTH_LOW_ADDRESS  0X44
#define LOFF_LTH_LOW_HIGH_ADDR  (LOFF_LTH_LOW_ADDRESS >> 8)
#define LOFF_LTH_LOW_LOW_ADDR   (LOFF_LTH_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH1_HIGH_ADDRESS 0x45
#define LOFF_DAT_CH1_HIGH_HIGH_ADDR  (LOFF_DAT_CH1_HIGH_ADDRESS >> 8)
#define LOFF_DAT_CH1_HIGH_LOW_ADDR   (LOFF_DAT_CH1_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH1_LOW_ADDRESS 0x46
#define LOFF_DAT_CH1_LOW_HIGH_ADDR  (LOFF_DAT_CH1_LOW_ADDRESS >> 8)
#define LOFF_DAT_CH1_LOW_LOW_ADDR   (LOFF_DAT_CH1_LOW_ADDRESS & 0xFF)



#define LOFF_DAT_CH2_HIGH_ADDRESS 0x47
#define LOFF_DAT_CH2_HIGH_HIGH_ADDR  (LOFF_DAT_CH2_HIGH_ADDRESS >> 8)
#define LOFF_DAT_CH2_HIGH_LOW_ADDR  (LOFF_DAT_CH2_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH2_LOW_ADDRESS 0x48
#define LOFF_DAT_CH2_LOW_HIGH_ADDR  (LOFF_DAT_CH2_LOW_ADDRESS >> 8)
#define LOFF_DAT_CH2_LOW_LOW_ADDR   (LOFF_DAT_CH2_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH3_HIGH_ADDRESS 0x49
#define LOFF_DAT_CH3_HIGH_HIGH_ADDR  (LOFF_DAT_CH3_HIGH_ADDRESS >> 8)
#define LOFF_DAT_CH3_HIGH_LOW_ADDR   (LOFF_DAT_CH3_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH3_LOW_ADDRESS 0x4A
#define LOFF_DAT_CH3_LOW_HIGH_ADDR  (LOFF_DAT_CH3_LOW_ADDRESS >> 8)
#define LOFF_DAT_CH3_LOW_LOW_ADDR   (LOFF_DAT_CH3_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH4_HIGH_ADDRESS 0x4B
#define LOFF_DAT_CH4_HIGH_HIGH_ADDR  (LOFF_DAT_CH4_HIGH_ADDRESS >> 8)
#define LOFF_DAT_CH4_HIGH_LOW_ADDR   (LOFF_DAT_CH4_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH4_LOW_ADDRESS 0x4C
#define LOFF_DAT_CH4_LOW_HIGH_ADDR  (LOFF_DAT_CH4_LOW_ADDRESS >> 8)
#define LOFF_DAT_CH4_LOW_LOW_ADDR   (LOFF_DAT_CH4_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH5_HIGH_ADDRESS 0x4D
#define LOFF_DAT_CH5_HIGH_HIGH_ADDR  (LOFF_DAT_CH5_HIGH_ADDRESS >> 8)
#define LOFF_DAT_CH5_HIGH_LOW_ADDR   (LOFF_DAT_CH5_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH5_LOW_ADDRESS 0x4E
#define LOFF_DAT_CH5_LOW_HIGH_ADDR  (LOFF_DAT_CH5_LOW_ADDRESS >> 8)
#define LOFF_DAT_CH5_LOW_LOW_ADDR   (LOFF_DAT_CH5_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH6_HIGH_ADDRESS 0x4F
#define LOFF_DAT_CH6_HIGH_HIGH_ADDR  (LOFF_DAT_CH6_HIGH_ADDRESS >> 8)
#define LOFF_DAT_CH6_HIGH_LOW_ADDR  (LOFF_DAT_CH6_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH6_LOW_ADDRESS 0x50
#define LOFF_DAT_CH6_LOW_HIGH_ADDR  (LOFF_DAT_CH6_LOW_ADDRESS >> 8)
#define LOFF_DAT_CH6_LOW_LOW_ADDR   (LOFF_DAT_CH6_LOW_ADDRESS & 0xFF)



#define LOFF_DAT_CH7_HIGH_ADDRESS 0x51
#define LOFF_DAT_CH7_HIGH_HIGH_ADDR  (LOFF_DAT_CH7_HIGH_ADDRESS >> 8)
#define LOFF_DAT_CH7_HIGH_LOW_ADDR   (LOFF_DAT_CH7_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH7_LOW_ADDRESS 0x52
#define LOFF_DAT_CH7_LOW_HIGH_ADDR  (LOFF_DAT_CH7_LOW_ADDRESS >> 8)
#define LOFF_DAT_CH7_LOW_LOW_ADDR   (LOFF_DAT_CH7_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH8_HIGH_ADDRESS 0x53
#define LOFF_DAT_CH8_HIGH_HIGH_ADDR  (LOFF_DAT_CH8_HIGH_ADDRESS >> 8)
#define LOFF_DAT_CH8_HIGH_LOW_ADDR   (LOFF_DAT_CH8_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH8_LOW_ADDRESS 0x54
#define LOFF_DAT_CH8_LOW_HIGH_ADDR  (LOFF_DAT_CH8_LOW_ADDRESS >> 8)
#define LOFF_DAT_CH8_LOW_LOW_ADDR   (LOFF_DAT_CH8_LOW_ADDRESS & 0xFF)


#define LOFF_CFG_ADDRESS  0X82
#define LOFF_CFG_HIGH_ADDR  (LOFF_CFG_ADDRESS >> 8)
#define LOFF_CFG_LOW_ADDR   (LOFF_CFG_ADDRESS & 0xFF)


#define LOFF_FREQ_ADDRESS  0X83
#define LOFF_FREQ_HIGH_ADDR  (LOFF_FREQ_ADDRESS >> 8)
#define LOFF_FREQ_LOW_ADDR   (LOFF_FREQ_ADDRESS & 0xFF)



#define USERKEY_ADDRESS                    0x182
#define USERKEY_HIGH_ADDR (USERKEY_ADDRESS >> 8)
#define USERKEY_LOW_ADDR  (USERKEY_ADDRESS & 0xFF)


#define LHE7909_CMD_WAKEUP                 0x02
#define LHE7909_CMD_STANDBY                0x04
#define LHE7909_CMD_RESET                  0x06
#define LHE7909_CMD_START                  0x08
#define LHE7909_CMD_STOP                   0x0A
#define LHE7909_CMD_RDATAC                 0x10
#define LHE7909_CMD_SDATAC                 0x11
#define LHE7909_CMD_RDATA                  0x12
#define LHE7909_CMD_RREG                   0x20
#define LHE7909_CMD_WREG                   0x40


#define LHE7909_DR_16K_LP                     0x00  
#define LHE7909_DR_8K_LP                      0x01
#define LHE7909_DR_4K_LP                      0x02
#define LHE7909_DR_2K_LP                      0x03
#define LHE7909_DR_1K_LP                      0x04
#define LHE7909_DR_500_LP                     0x05
#define LHE7909_DR_250_LP                     0x06


#define LHE7909_DR_32K_HR                     0x80  
#define LHE7909_DR_16K_HR                     0x81
#define LHE7909_DR_8K_HR                      0x82
#define LHE7909_DR_4K_HR                      0x83
#define LHE7909_DR_2K_HR                      0x84
#define LHE7909_DR_1K_HR                      0x85
#define LHE7909_DR_500_HR                     0x86

/* ========== 上位机二进制帧协议 ========== */
/* 帧格式 (37 字节):
 *   [0]     0xAA          帧头1
 *   [1]     0x55          帧头2
 *   [2]     0x08          通道数 (=8)
 *   [3]     帧序号        0~255 循环递增（防伪帧头误同步 + 统计真实丢帧）
 *   [4..35] CH1~CH8 各 4 字节 float mV (IEEE754 小端序)
 *   [36]    校验和 = 字节[0..35] 累加和的低 8 位
 */
#define ECG_FRAME_HEAD1                      0xAA
#define ECG_FRAME_HEAD2                      0x55
#define ECG_FRAME_CH_COUNT                   0x08
#define ECG_FRAME_LEN                        37
#define ECG_FRAME_DATA_OFFSET                4
#define ECG_FRAME_SEQ_OFFSET                 3
#define ECG_FRAME_CH_BYTES                   4

/* 1: 二进制帧输出(上位机模式)  0: printf CSV 调试输出 */
#define ECG_UART_BIN_MODE                    0

/* ========== 上位机控制命令协议（下行） ========== */
/* PC->MCU: [0xAA][0xCC][cmd][data][sum]{0x0D}  共 6B */
/* MCU->PC: [0xAA][0xDB][cmd][data][sum]{0x0D}  确认帧 */
#define ECG_CMD_HEAD1                        0xAA
#define ECG_CMD_HEAD2_PC2MCU                 0xCC
#define ECG_CMD_HEAD2_MCU2PC                 0xDB
#define ECG_CMD_LEN                          6

/* 命令码 */
#define ECG_CMD_SET_DR                       0x01   /* data=DR 值 */
#define ECG_CMD_PAUSE                        0x02   /* data=0:暂停(SDATAC+STOP) 1:继续(START+RDATAC) */
#define ECG_CMD_READ_CFG                     0x03   /* data=0:读取寄存器配置 */
#define ECG_CMD_WRITE_REG                    0x04   /* 写寄存器(7B帧): [AA][CC][04][addr][val][sum][0D] */

/* BIAS_AMP 闭环 A/B/A′ 对比测试命令（帧格式同上，仅文本调试模式 ECG_UART_BIN_MODE=0 下解析）。
 * 分组定义：A 组 CONFIG3=0x98(PD_BIAS 掉电) / B 组 0x9C(PD_BIAS 使能) / A′ 组 0x98(切回复测)。
 * 命令行为详见 LHE7909.c 的 BIAS 测试模块注释。 */
#define ECG_CMD_BIAS_ENTER                   0x10   /* data=0x00: 进入测试模式: 写公共配置(逐个回读校验)+切A组+2s稳定+配置转储 */
#define ECG_CMD_BIAS_GROUP                   0x11   /* data=0x00:切A组 0x01:切B组 0x02:切回A组(A′) — SDATAC→写CONFIG3→回读校验→START→丢弃2s数据 */
#define ECG_CMD_BIAS_ACQ                     0x12   /* data=0x00: 触发一次定长采集(2000点CH1+CH8原始码)，采满后主循环输出CSV */
#define ECG_CMD_BIAS_EXIT                    0x13   /* data=0x00: 退出测试模式，恢复进入前的初始化配置 */

/* ========== BIAS_AMP 闭环 A/B/A′ 对比测试公共配置 ========== */
/* 唯一变量是 CONFIG3 的 PD_BIAS(bit2)：A/A′=0x98 掉电、B=0x9C 使能；其余寄存器固定 */
#define BIAS_TEST_CFG1                       0x55   /* 500SPS */
#define BIAS_TEST_CFG2                       0xD3   /* 内部测试信号源(与上电初始化一致) */
#define BIAS_TEST_CFG3_A                     0x98   /* PD_REFBUF=1,BIAS_MEAS=1,BIASREF_INT=1,PD_BIAS=0(掉电) */
#define BIAS_TEST_CFG3_B                     0x9C   /* 同上,PD_BIAS=1(BIAS_AMP 使能) */
#define BIAS_TEST_LOFF                       0x03
#define BIAS_TEST_CH1SET                     0x60   /* CH1 增益12、正常电极输入(MUX=000) */
#define BIAS_TEST_CH1_GAIN                   12     /* 与 BIAS_TEST_CH1SET 高 4 位一致，进测试模式时同步给 ch1_gain */
#define BIAS_TEST_CH8SET                     0x12   /* CH8 增益1、偏置回读(MUX=010) */
#define BIAS_TEST_BIASSENSP                  0x01   /* IN1P 接入偏置求和 */
#define BIAS_TEST_BIASSENSN                  0x01   /* IN1N 接入偏置求和 */
#define BIAS_TEST_N_POINTS                   2000   /* 每组采集点数(500SPS 下 4 秒) */
#define BIAS_TEST_SETTLE_MS                  2000   /* 切组后丢弃数据的稳定等待(环路+数字滤波器) */
#define BIAS_TEST_FS_SPS                     500    /* 采样率(CSV 头 FS= 字段) */
#define BIAS_TEST_STALL_MS                   1000   /* 采集中断超过该时长无新样本→按实际点数结束并告警 */

/* 配置响应帧: [0xAA][0xDD][count][reg val]...[sum]（帧头2=0xDD，区分数据帧0x55/确认帧0xDB） */
#define ECG_CFG_HEAD2                        0xDD

/* ========== LOFF 状态字帧（MCU->PC，6 字节，随采样数据上传） ========== */
/* SPI 读回 27 字节中的前 3 字节就是 ADC 状态字；本侧只原样上传，位解析在上位机：
 *   [0]  0xAA          帧头1
 *   [1]  0xDE          帧头2（区分数据帧 0x55 / 确认帧 0xDB / 配置帧 0xDD）
 *   [2]  状态字节0     高 4bit=1100，低 4bit=LOFF_STATP[7:4]
 *   [3]  状态字节1     高 4bit=LOFF_STATP[3:0]，低 4bit=LOFF_STATN[7:4]
 *   [4]  状态字节2     高 4bit=LOFF_STATN[3:0]，低 4bit=GPIO[3:0]
 *   [5]  校验和 = 字节[0..4] 累加和的低 8 位
 * 上位机解析处：host_tool/config.py::decode_loff_status */
#define ECG_LOFF_HEAD2                       0xDE
#define ECG_LOFF_FRAME_LEN                   6
/* 状态字帧发送节拍：每 ECG_LOFF_SEND_EVERY_N 个采样帧发 1 次（1=每次采样都发）。
 * 状态变化缓慢；串口 1.5Mbps(8N1) 约 150kB/s，37B 数据帧 + 6B 状态帧在 >=4K
 * 采样率下会超出链路带宽(4K: 172kB/s)，此时可调大本值(如 8/16)降低串口占用。 */
#define ECG_LOFF_SEND_EVERY_N                1

/* ========== 运行指示灯（只在“正在上传数据”时闪烁） ========== */
/* 闪烁节奏：每 LED_BLINK_HALF_MS 翻转一次 LED（250ms → 亮/灭各 250ms，
 * 约 2 次/秒，人眼能清楚分辨；不要用帧计数分频，否则切换采样率后闪烁会变快/变慢）。 */
#define LED_BLINK_HALF_MS                    250
/* 判定“正在上传”的超时：最近该时长内成功上传过数据帧才算在传数据；
 * 暂停/停止采集超过该时长后指示灯熄灭。 */
#define LED_UPLOAD_ALIVE_MS                  300
/* 指示灯“熄灭”对应的 GPIO 电平。当前按高电平点亮（与 MX_GPIO_Init 初始
 * 写入 RESET 一致）；若硬件是低电平点亮，改成 GPIO_PIN_SET 即可。 */
#define LED_OFF_LEVEL                        GPIO_PIN_RESET




uint8_t LHE7909_read_id(void);

uint8_t LHE7909_read_register(uint8_t address);

void LHE7909_write_register(uint8_t address,uint8_t data);

void DUT_WTREG(uint8_t _value, uint8_t opcode1, uint8_t opcode2);
uint8_t DUT_RTREG(uint8_t opcode1, uint16_t opcode2);


void LHE7909_init(uint8_t DR);


void LHE7909_SDATAC(void);
void LHE7909_RDATAC(void);
void LHE7909_RDATA(void);

void LHE7909_WAKEUP(void);
void LHE7909_STANDBY(void);
void LHE7909_RESET(void);
void LHE7909_START(void);
void LHE7909_STOP(void);





void LHE7909_delay_us(uint32_t time);


void LHE7909_get_adc_data(void);

float  ADC_DataToMiniVolt(int32_t data,uint8_t gain);

void LHE7909_send_frame(void);

/* 上传 SPI 读回状态字的前 3 字节(原始 hex)，供上位机解析 LOFF_STATP/LOFF_STATN */
void LHE7909_send_loff_status(void);

/* 读取关键寄存器配置并通过 UART 返回（仅暂停/SDATAC 状态下调用） */
void LHE7909_read_config(void);

/* 上位机命令: 写单个寄存器（SDATAC 窗口内完成 SPI 写并发送 ack） */
void LHE7909_write_reg_cmd(uint8_t addr, uint8_t value);

/* ========== 采样率控制（上位机命令） ========== */
/* 设置 LHE7909 采样率: SDATAC->写CONFIG1->START，返回新 DR
 * （采集用 DRDY 中断 + RDATA 单次读，不能进 RDATAC 连续读模式） */
uint8_t LHE7909_set_dr(uint8_t dr);

/* 运行指示灯任务：在上传数据时按 LED_BLINK_HALF_MS 翻转 LED，
 * 停止上传超过 LED_UPLOAD_ALIVE_MS 后熄灭。由 main 主循环周期调用。 */
void LHE7909_led_task(void);

/* 解析上位机命令帧（在 main 主循环轮询），返回是否处理了命令。
 * 二进制模式(ECG_UART_BIN_MODE=1)：采样率/暂停/读配置/写寄存器命令；
 * 文本调试模式(ECG_UART_BIN_MODE=0)：BIAS_AMP 闭环 A/B/A′ 对比测试命令
 * (ENTER/GROUP/ACQ/EXIT，见 ECG_CMD_BIAS_*)。 */
uint8_t LHE7909_process_command(void);

/* BIAS 测试任务：主循环周期调用（仅文本调试模式有效）。采集完成(或采集中断
 * 停滞超时)后输出定长采集 CSV：起始行 #GROUP,<A|B|A2>,CONFIG3=0x98|0x9C,N=2000,
 * FS=500,GAIN=12；数据行 序号,CH1原始码,CH1电压mV,CH8原始码；结束行 #END,<实际点数>。 */
void LHE7909_bias_task(void);






