#include "main.h"


#include "stdio.h"
#include <stdbool.h>

//==============LOFF  BEGIN=======================
typedef union {
    struct {
        unsigned char FLEAD_OFF      : 2;    // [1:0]   导联脱落频率/模式
        unsigned char ILEAD_OFF      : 2;    // [3:2]   导联脱落电流幅度
        unsigned char VLEAD_OFF_EN   : 1;    // [4]     检测模式：0=电流源 1=电阻模式
        unsigned char COMP_TH        : 3;    // [7:5]   比较器阈值
    } bit;

    // 整字节访问（直接写入寄存器）
    unsigned char reg;

} LOFF_TypeDef;

// ==========================
// COMP_TH[2:0] 比较器阈值
// ==========================
// 正极侧
#define COMP_TH_P_95        0x00
#define COMP_TH_P_92_5      0x01
#define COMP_TH_P_90        0x02
#define COMP_TH_P_87_5      0x03
#define COMP_TH_P_85        0x04
#define COMP_TH_P_80        0x05
#define COMP_TH_P_75        0x06
#define COMP_TH_P_70        0x07

// 负极侧
#define COMP_TH_N_5         0x00
#define COMP_TH_N_7_5       0x01
#define COMP_TH_N_10        0x02
#define COMP_TH_N_12_5      0x03
#define COMP_TH_N_15        0x04
#define COMP_TH_N_20        0x05
#define COMP_TH_N_25        0x06
#define COMP_TH_N_30        0x07

// ==========================
// VLEAD_OFF_EN 模式
// ==========================
#define LOFF_MODE_CURRENT       0   // 0: 电流源模式（默认）
#define LOFF_MODE_RESISTOR      1   // 1: 上拉/下拉电阻模式

// ==========================
// ILEAD_OFF[1:0] 电流大小
// ==========================
#define ILEAD_OFF_0NA        0x00
#define ILEAD_OFF_2_5NA      0x01
#define ILEAD_OFF_5NA        0x02
#define ILEAD_OFF_7_5NA      0x03

#define ILEAD_OFF_10NA        0x00
#define ILEAD_OFF_12_5NA      0x01
#define ILEAD_OFF_15NA     	  0x02
#define ILEAD_OFF_17_5NA      0x03

#define ILEAD_OFF_20NA      0x02
#define ILEAD_OFF_30NA      0x03

#define ILEAD_OFF_40NA      0x00
#define ILEAD_OFF_50NA      0x01
#define ILEAD_OFF_60NA      0x02
#define ILEAD_OFF_70NA      0x03
// ==========================
// FLEAD_OFF[1:0] 检测频率/模式
// ==========================
#define FLEAD_OFF_OFF           0x00    // 关闭
#define FLEAD_OFF_AC			0x01    // AC 交流
#define FLEAD_OFF_RESERVED      0x02    // 禁止使用
#define FLEAD_OFF_DC            0x03    // DC 直流检测
//================================LOFF END======================================
//================================LOFF_FRQ BEGIN================================
typedef union {
    // 位域定义（低比特在前，高比特在后，严格匹配手册）
    struct {
        unsigned char AC_FREQ     : 3;    // [2:0]  导联脱落激励频率
        unsigned char RESERVED    : 5;    // [7:3]  保留位，必须写 0
    } bit;

    // 字节整体访问（直接写入寄存器）
    unsigned char reg;

} LOFF_FREQ_TypeDef;

#define LOFF_FREQ_500HZ    0x00
#define LOFF_FREQ_1KHZ     0x01  
#define LOFF_FREQ_2KHZ     0x02
#define LOFF_FREQ_4KHZ     0x03  
#define LOFF_FREQ_8KHZ     0x04 
#define LOFF_FREQ_16KHZ    0x05  

#define LOFF_FREQ_7_8HZ    0x06  
#define LOFF_FREQ_31_2HZ   0x07
//================================LOFF_FRQ END================================
//================================LOFF_CFG BEGIN================================

typedef union {
    // 位域：低比特在前，高比特在后（严格匹配 BIT0 ~ BIT7）
    struct {
        unsigned char RESERVED0        : 1;  // [0]   保留，复位1，必须写0
        unsigned char ILOFF_BST_EN     : 1;  // [1]   DC电流倍增使能
        unsigned char RLD_COMP_EN      : 1;  // [2]   RLD比较器使能
        unsigned char LOFF_DEGLITCH_BYP: 1;  // [3]   去毛刺旁路
        unsigned char RESERVED7_4      : 4;  // [7:4] 保留，必须写0
    } bit;

    // 整字节访问（直接写入寄存器）
    unsigned char reg;

} LOFF_CFG_TypeDef;
// ==========================
// LOFF_CFG 位定义
// ==========================
#define LOFF_DEGLITCH_NORMAL        0U    // 带deglitch（默认）
#define LOFF_DEGLITCH_BYPASS        1U    // 旁路deglitch

#define RLD_COMP_DISABLE            0U    // 比较器关闭
#define RLD_COMP_ENABLE             1U    // 比较器使能（默认）

#define ILOFF_BST_NORMAL           0U    // 电流不倍增 step=2.5nA
#define ILOFF_BST_BOOST            1U    // 电流倍增     step=10nA

//================================LOFF_CFG END================================

//================================CONFIG1 BEGIN================================

typedef union {
    struct {
        unsigned char DR             : 3;   // [2:0] 数据速率
        unsigned char RESERVED_4_3    : 2;   // [4:3] 保留位，必须写 0
        unsigned char CLK_EN         : 1;   // [5] 时钟输出使能
        unsigned char DAISY_EN        : 1;   // [6] 菊花链/多读回模式
        unsigned char HR             : 1;   // [7] 高分辨率/低功耗模式
    } bit;

    unsigned char reg;
} CONFIG1_TypeDef;

// ==========================
// CONFIG1 宏定义
// ==========================
// HR[7] 工作模式
#define CONFIG1_HR_LP          0U    // 低功耗模式
#define CONFIG1_HR_HR          1U    // 高分辨率模式

// DAISY_EN[6] 菊花链 / 多读回
#define CONFIG1_DAISY          0U    // 菊花链模式
#define CONFIG1_MULTI_READ     1U    // 多读回模式

// CLK_EN[5] 时钟输出
#define CONFIG1_CLK_OUT_DIS    0U    // 禁用振荡器时钟输出
#define CONFIG1_CLK_OUT_EN     1U    // 启用振荡器时钟输出

// DR[2:0] 数据速率
#define HR_DR_32KSPS         0U
#define HR_DR_16KSPS         1U
#define HR_DR_8KSPS         2U
#define HR_DR_4KSPS         3U
#define HR_DR_2KSPS         4U
#define HR_DR_1KSPS         5U
#define HR_DR_500SPS         6U    // 默认
#define HR_DR_64KSPS         7U

#define LP_DR_16KSPS         0U
#define LP_DR_8KSPS         1U
#define LP_DR_4KSPS         2U
#define LP_DR_2KSPS         3U
#define LP_DR_1KSPS         4U
#define LP_DR_500SPS         5U
#define LP_DR_250SPS         6U    // 默认
#define LP_DR_32KSPS         7U


//================================CONFIG1 END================================
//================================CONFIG2 BEGIN================================

typedef union {
    // 位序严格对应 BIT0 ~ BIT7（低比特在前）
    struct {
        unsigned char TEST_FREQ     : 2;  // [1:0]   测试信号频率
        unsigned char TEST_AMP      : 1;  // [2]     测试信号幅度
        unsigned char RESERVED3     : 1;  // [3]     保留，必须写 0
        unsigned char INT_TEST      : 1;  // [4]     内部测试信号源
        unsigned char WCT_CHOP      : 1;  // [5]     WCT斩波模式
        unsigned char RESERVED7_6   : 2;  // [7:6]   保留，必须写 3 (11)
    } bit;

    // 整字节访问
    unsigned char reg;
} CONFIG2_TypeDef;
// ==========================
// CONFIG2 宏定义（无重名版）
// ==========================

// RESERVED7_6 [7:6] 必须写 11 (0x3)
#define CONFIG2_RESERVED_76       0x3U

// WCT_CHOP [5] WCT斩波模式
#define WCT_CHOP_VARIABLE         0U      // 可变频率
#define WCT_CHOP_CONSTANT         1U      // 固定 fMOD/16

// INT_TEST [4] 测试信号源
#define TEST_SOURCE_EXTERNAL      0U      // 外部驱动
#define TEST_SOURCE_INTERNAL      1U      // 内部生成

// TEST_AMP [2] 测试幅度（手册值一致）
#define TEST_AMP_1X               0U
#define TEST_AMP_2X               1U

// TEST_FREQ [1:0] 测试频率
#define TEST_FREQ1         0U      // 00: fCLK/221
#define TEST_FREQ2         1U      // 01: fCLK/220
#define TEST_FREQ_UNUSED          2U      // 10: 未使用
#define TEST_FREQ_DC              3U      // 11: 直流

//================================CONFIG2 END================================


//================================CONFIG3 BEGIN================================

typedef union {
    // 位域：低比特在前，高比特在后（严格匹配 BIT0 ~ BIT7）
    struct {
        unsigned char RLD_STAT        : 1;  // [0]  R  只读：RLD脱落状态
        unsigned char RLD_LOFF_SENS   : 1;  // [1]  R/W RLD感应使能
        unsigned char PD_RLD          : 1;  // [2]  R/W RLD缓冲器电源控制
        unsigned char RLDREF_INT      : 1;  // [3]  R/W RLDREF来源：内部/外部
        unsigned char RLD_MEAS        : 1;  // [4]  R/W RLD测量路由使能
        unsigned char VREF_4V         : 1;  // [5]  R/VREFP电压：2.4V/4V
        unsigned char ILEAD_EN        : 1;  // [6]  R/W ILEADOFF大电流使能
        unsigned char PD_REFBUF       : 1;  // [7]  R/W 内部基准缓冲器使能
    } bit;

    // 整字节访问（直接写入寄存器）
    unsigned char reg;

} CONFIG3_TypeDef;

// ==========================
// CONFIG3 位功能宏定义
// ==========================
// PD_REFBUF[7] 基准缓冲器
#define REFBUF_DISABLE           0U    // 关断（默认）
#define REFBUF_ENABLE            1U    // 启用

// ILEAD_EN[6] ILEADOFF大电流
#define ILEAD_DISABLE            0U    // 关闭（默认）
#define ILEAD_ENABLE             1U    // 开启

// VREF_4V[5] 基准电压
#define VREF_V2V4                0U    // 2.4V
#define VREF_V4V                 1U    // 4V


// RLD_MEAS[4] RLD测量
#define RLD_MEAS_DISABLE         0U    // 开路（默认）
#define RLD_MEAS_ENABLE          1U    // 路由至通道MUX 010

// RLDREF_INT[3] RLDREF来源
#define RLDREF_INT_INTERNAL    1U  // 内部
#define RLDREF_INT_EXTERNAL    0U  // 外部


// PD_RLD[2] RLD缓冲器
#define RLD_POWER_DOWN           0U    // 断电（默认）
#define RLD_POWER_ON             1U    // 启用

// RLD_LOFF_SENS[1] RLD感应
#define RLD_LOFF_SENS_DISABLE    0U    // 禁用（默认）
#define RLD_LOFF_SENS_ENABLE     1U    // 启用

// RLD_STAT[0] 只读状态
#define RLD_CONNECTED            0U    // 已连接
#define RLD_DISCONNECTED         1U    // 未连接

//================================CONFIG3 END================================
 //================================CHnSET BEGIN===============================

 typedef union {
	 // 位序严格对应 BIT0 ~ BIT7（低比特在前）
	 struct {
		 unsigned char MUXn 	   : 3;   // [2:0] 通道输入选择
		 unsigned char RESERVED3   : 1;   // [3]   保留位，必须写 0
		 unsigned char GAINn	   : 3;   // [6:4] PGA 增益设置
		 unsigned char PDn		   : 1;   // [7]   通道断电控制
	 } bit;
 
	 // 整字节访问（直接写入寄存器）
	 unsigned char reg;
 
 } CHnSET_TypeDef;
 // ==========================
// CHnSET 宏定义
// ==========================

// PDn [7] 通道电源状态
#define CH_POWER_NORMAL          0U    // 正常运行
#define CH_POWER_DOWN            1U    // 通道断电

// GAINn [6:4] PGA 增益设置
#define GAIN_6                   0U    // 000
#define GAIN_1                   1U    // 001
#define GAIN_2                   2U    // 010
#define GAIN_3                   3U    // 011
#define GAIN_4                   4U    // 100
#define GAIN_8                   5U    // 101
#define GAIN_12                  6U    // 110
#define GAIN_24                  7U    // 111

/*NOTE:
对于通道 1、2、5、6、7 和 8，(MVDDP – MVDDN) = [0.5 × (AVDD – AVSS)] 
对于通道 3 和 4，(MVDDP – MVDDN) = DVDD/4。
为避免在测量电源时使 PGA 饱和，请将增益设置为 1。
*/
// MUXn [2:0] 通道输入选择
#define MUX_NORMAL_ELECTRODE     0U    // 000 正常电极输入
#define MUX_INPUT_SHORT          1U    // 001 输入短路（偏移校准）
#define MUX_RLD_MEAS             2U    // 010 RLD 测量
#define MUX_MVDD                 3U    // 011 电源测量
#define MUX_TEMP_SENSOR          4U    // 100 温度传感器
#define MUX_TEST_SIGNAL          5U    // 101 测试信号
#define MUX_RLD_DRP              6U    // 110 RLD_DRP
#define MUX_RLD_DRN              7U    // 111 RLD_DRN
 //================================CHnSET END================================
 //================================CONFIG4 BEGIN================================
 typedef union {
	 // 位域：低比特在前，高比特在后（严格匹配 BIT0 ~ BIT7）
	 struct {
		 unsigned char SINC4_EN 	   : 1;  // [0] R/W SINC4滤波器使能
		 unsigned char PD_LOFF_COMP    : 1;  // [1] R/W 导联脱落比较器断电控制
		 unsigned char WCT_TO_RLD	   : 1;  // [2] R/W WCT与RLD连接使能
		 unsigned char SINGLE_SHOT	   : 1;  // [3] R/W 单次/连续转换模式
		 unsigned char RESERVED 	   : 4;  // [7:4] R/W 保留位，必须写0
	 } bit;
 
	 // 整字节访问（直接写入寄存器）
	 unsigned char reg;
 
 } CONFIG4_TypeDef;
 // ==========================
// CONFIG4 位功能宏定义
// ==========================
// SINC4_EN[0] SINC4滤波器使能
#define SINC4_DISABLE           0U    // 关闭SINC4
#define SINC4_ENABLE            1U    // 开启SINC4（默认）

// PD_LOFF_COMP[1] 导联脱落比较器断电
#define LOFF_COMP_POWER_ON      1U    // 启用导联脱落比较器
#define LOFF_COMP_POWER_DOWN    0U    // 禁用导联脱落比较器（默认）

// WCT_TO_RLD[2] WCT连接到RLD
#define WCT_RLD_DISCONNECT      0U    // WCT到RLD连接关闭（默认）
#define WCT_RLD_CONNECT         1U    // WCT到RLD连接开启

// SINGLE_SHOT[3] 转换模式
#define CONV_MODE_CONTINUOUS    0U    // 连续转换模式（默认）
#define CONV_MODE_SINGLE_SHOT   1U    // 单次转换模式
 //================================CONFIG4 END================================
 //================================LOFF_SENSP BEGIN================================
typedef union {
    // 位域：低比特在前，高比特在后（严格匹配 BIT0 ~ BIT7）
    struct {
        unsigned char LOFF1P    : 1;  // [0]  CH1 正极脱落检测使能
        unsigned char LOFF2P    : 1;  // [1]  CH2 正极脱落检测使能
        unsigned char LOFF3P    : 1;  // [2]  CH3 正极脱落检测使能
        unsigned char LOFF4P    : 1;  // [3]  CH4 正极脱落检测使能
        unsigned char LOFF5P    : 1;  // [4]  CH5 正极脱落检测使能
        unsigned char LOFF6P    : 1;  // [5]  CH6 正极脱落检测使能
        unsigned char LOFF7P    : 1;  // [6]  CH7 正极脱落检测使能
        unsigned char LOFF8P    : 1;  // [7]  CH8 正极脱落检测使能
    } bit;

    // 整字节访问（直接写入寄存器）
    unsigned char reg;

} LOFF_SENSP_TypeDef;
// ==========================
// LOFF_SENSP 通道使能宏
// ==========================
#define LOFF_CH_DISABLE          0U    // 禁用导联脱落检测
#define LOFF_CH_ENABLE           1U    // 启用导联脱落检测

//================================LOFF_SENSP END================================
//================================LOFF_SENSN BEGIN================================
typedef union {
    // 位域：低比特在前，高比特在后（严格匹配 BIT0 ~ BIT7）
    struct {
        unsigned char LOFF1N    : 1;  // [0]  CH1 负极脱落检测使能
        unsigned char LOFF2N    : 1;  // [1]  CH2 负极脱落检测使能
        unsigned char LOFF3N    : 1;  // [2]  CH3 负极脱落检测使能
        unsigned char LOFF4N    : 1;  // [3]  CH4 负极脱落检测使能
        unsigned char LOFF5N    : 1;  // [4]  CH5 负极脱落检测使能
        unsigned char LOFF6N    : 1;  // [5]  CH6 负极脱落检测使能
        unsigned char LOFF7N    : 1;  // [6]  CH7 负极脱落检测使能
        unsigned char LOFF8N    : 1;  // [7]  CH8 负极脱落检测使能
    } bit;

    // 整字节访问（直接写入寄存器）
    unsigned char reg;

} LOFF_SENSN_TypeDef;

// ==========================
// LOFF_SENSN 通道使能宏
// ==========================
#define LOFF_CH_DISABLE          0U    // 禁用导联脱落检测
#define LOFF_CH_ENABLE           1U    // 启用导联脱落检测
//================================LOFF_SENSN END================================
//===============================LOFF_FLIP BEGIN================================
/*******************************************************************************
 * LOFF_FLIP：导联脱落极性翻转寄存器
 * 地址：0x11
 * 复位值：0x00
 * 功能：翻转通道1~8 导联脱落检测电流/电阻的上拉/下拉极性
 ******************************************************************************/
typedef union {
    // 位序严格对应 BIT0 ~ BIT7（低比特在前）
    struct {
        unsigned char LOFF_FLIP1    : 1;  // [0] 通道1 极性翻转
        unsigned char LOFF_FLIP2    : 1;  // [1] 通道2 极性翻转
        unsigned char LOFF_FLIP3    : 1;  // [2] 通道3 极性翻转
        unsigned char LOFF_FLIP4    : 1;  // [3] 通道4 极性翻转
        unsigned char LOFF_FLIP5    : 1;  // [4] 通道5 极性翻转
        unsigned char LOFF_FLIP6    : 1;  // [5] 通道6 极性翻转
        unsigned char LOFF_FLIP7    : 1;  // [6] 通道7 极性翻转
        unsigned char LOFF_FLIP8    : 1;  // [7] 通道8 极性翻转
    } bit;

    // 整字节访问（直接写入寄存器）
    unsigned char reg;

} LOFF_FLIP_TypeDef;
// ==========================
// LOFF_FLIP 极性控制宏
// ==========================
#define LOFF_FLIP_NORMAL        0U    // 不翻转极性（默认）
#define LOFF_FLIP_REVERSE       1U    // 翻转极性
//===============================LOFF_FLIP END================================

//================================DIG_ACLOFF_ENABLE BEGIN================================
typedef union {
    // 位序严格对应 BIT0 ~ BIT7（低比特在前）
    struct {
        unsigned char CH1EN    : 1;  // [0] CH1 AC LOFF 使能
        unsigned char CH2EN    : 1;  // [1] CH2 AC LOFF 使能
        unsigned char CH3EN    : 1;  // [2] CH3 AC LOFF 使能
        unsigned char CH4EN    : 1;  // [3] CH4 AC LOFF 使能
        unsigned char CH5EN    : 1;  // [4] CH5 AC LOFF 使能
        unsigned char CH6EN    : 1;  // [5] CH6 AC LOFF 使能
        unsigned char CH7EN    : 1;  // [6] CH7 AC LOFF 使能
        unsigned char CH8EN    : 1;  // [7] CH8 AC LOFF 使能
    } bit;

    // 整字节访问（直接写入寄存器）
    unsigned char reg;

} DIG_ACLOFF_ENABLE_TypeDef;

// ==========================
// DIG_ACLOFF_ENABLE 宏定义
// ==========================
#define AC_LOFF_CH_DISABLE      0U    // 禁用通道 AC 脱落检测
#define AC_LOFF_CH_ENABLE       1U    // 使能通道 AC 脱落检测

//================================DIG_ACLOFF_ENABLE END================================

//==============================DC_LOFF_OSEL  BEGIN================================
/*******************************************************************************
 * DC_LOFF_OSEL：DC LOFF 导联脱落状态 GPIO 输出选择寄存器
 * 地址：0x1D
 * 复位值：0x00
 * 受 UserKey 保护，需先解锁
 * 功能：配置导联脱落状态从指定 GPIO 输出
 ******************************************************************************/
typedef union {
    // 位序严格对应 BIT0 ~ BIT7（低比特在前）
    struct {
        unsigned char P_SEL      : 3;   // [2:0] 正极脱落 GPIO 选择
        unsigned char N_SEL      : 3;   // [5:3] 负极脱落 GPIO 选择
        unsigned char RESERVED   : 2;   // [7:6] 保留位，必须写 0
    } bit;

    // 整字节访问
    unsigned char reg;

} DC_LOFF_OSEL_TypeDef;

// ==========================
// DC_LOFF_OSEL 宏定义
// ==========================
// 保留位值
#define DC_LOFF_OSEL_RESERVED_VAL    0U

// P_SEL[2:0] / N_SEL[2:0] 输出选择
#define LOFF_GPIO_SEL_NONE       0U    // 000~011: 不起作用
#define LOFF_GPIO_SEL_GPIO1      4U    // 100: GPIO1 输出
#define LOFF_GPIO_SEL_GPIO2      5U    // 101: GPIO2 输出
#define LOFF_GPIO_SEL_GPIO3      6U    // 110: GPIO3 输出
#define LOFF_GPIO_SEL_GPIO4      7U    // 111: GPIO4 输出

//==============================DC_LOFF_OSEL  END================================

//==============用户寄存器地址宏定义============
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

#define DC_LOFF_OSEL_ADDRESS               0x1D

//==============高地址寄存器地址宏定义============
#define DIG_ACLOFF_ENABLE_ADDRESS  0X40
// 高8位地址指针
#define DIG_ACLOFF_ENABLE_HIGH_ADDR  (DIG_ACLOFF_ENABLE_ADDRESS >> 8)
// 低8位地址指针
#define DIG_ACLOFF_ENABLE_LOW_ADDR   (DIG_ACLOFF_ENABLE_ADDRESS & 0xFF)



#define LOFF_UTH_HIGH_ADDRESS  0X41
// 高8位地址指针
#define LOFF_UTH_HIGH_HIGH_ADDR  (LOFF_UTH_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_UTH_HIGH_LOW_ADDR   (LOFF_UTH_HIGH_ADDRESS & 0xFF)

#define LOFF_UTH_LOW_ADDRESS  0X42
// 高8位地址指针
#define LOFF_UTH_LOW_HIGH_ADDR  (LOFF_UTH_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_UTH_LOW_LOW_ADDR   (LOFF_UTH_LOW_ADDRESS & 0xFF)

#define LOFF_LTH_HIGH_ADDRESS  0X43
// 高8位地址指针
#define LOFF_LTH_HIGH_HIGH_ADDR  (LOFF_LTH_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_LTH_HIGH_LOW_ADDR  (LOFF_LTH_HIGH_ADDRESS & 0xFF)

#define LOFF_LTH_LOW_ADDRESS  0X44
// 高8位地址指针
#define LOFF_LTH_LOW_HIGH_ADDR  (LOFF_LTH_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_LTH_LOW_LOW_ADDR   (LOFF_LTH_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH1_HIGH_ADDRESS 0x45
// 高8位地址指针
#define LOFF_DAT_CH1_HIGH_HIGH_ADDR  (LOFF_DAT_CH1_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH1_HIGH_LOW_ADDR   (LOFF_DAT_CH1_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH1_LOW_ADDRESS 0x46
// 高8位地址指针
#define LOFF_DAT_CH1_LOW_HIGH_ADDR  (LOFF_DAT_CH1_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH1_LOW_LOW_ADDR   (LOFF_DAT_CH1_LOW_ADDRESS & 0xFF)



#define LOFF_DAT_CH2_HIGH_ADDRESS 0x47
// 高8位地址指针
#define LOFF_DAT_CH2_HIGH_HIGH_ADDR  (LOFF_DAT_CH2_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH2_HIGH_LOW_ADDR  (LOFF_DAT_CH2_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH2_LOW_ADDRESS 0x48
// 高8位地址指针
#define LOFF_DAT_CH2_LOW_HIGH_ADDR  (LOFF_DAT_CH2_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH2_LOW_LOW_ADDR   (LOFF_DAT_CH2_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH3_HIGH_ADDRESS 0x49
// 高8位地址指针
#define LOFF_DAT_CH3_HIGH_HIGH_ADDR  (LOFF_DAT_CH3_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH3_HIGH_LOW_ADDR   (LOFF_DAT_CH3_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH3_LOW_ADDRESS 0x4A
// 高8位地址指针
#define LOFF_DAT_CH3_LOW_HIGH_ADDR  (LOFF_DAT_CH3_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH3_LOW_LOW_ADDR   (LOFF_DAT_CH3_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH4_HIGH_ADDRESS 0x4B
// 高8位地址指针
#define LOFF_DAT_CH4_HIGH_HIGH_ADDR  (LOFF_DAT_CH4_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH4_HIGH_LOW_ADDR   (LOFF_DAT_CH4_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH4_LOW_ADDRESS 0x4C
// 高8位地址指针
#define LOFF_DAT_CH4_LOW_HIGH_ADDR  (LOFF_DAT_CH4_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH4_LOW_LOW_ADDR   (LOFF_DAT_CH4_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH5_HIGH_ADDRESS 0x4D
// 高8位地址指针
#define LOFF_DAT_CH5_HIGH_HIGH_ADDR  (LOFF_DAT_CH5_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH5_HIGH_LOW_ADDR   (LOFF_DAT_CH5_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH5_LOW_ADDRESS 0x4E
// 高8位地址指针
#define LOFF_DAT_CH5_LOW_HIGH_ADDR  (LOFF_DAT_CH5_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH5_LOW_LOW_ADDR   (LOFF_DAT_CH5_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH6_HIGH_ADDRESS 0x4F
// 高8位地址指针
#define LOFF_DAT_CH6_HIGH_HIGH_ADDR  (LOFF_DAT_CH6_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH6_HIGH_LOW_ADDR  (LOFF_DAT_CH6_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH6_LOW_ADDRESS 0x50
// 高8位地址指针
#define LOFF_DAT_CH6_LOW_HIGH_ADDR  (LOFF_DAT_CH6_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH6_LOW_LOW_ADDR   (LOFF_DAT_CH6_LOW_ADDRESS & 0xFF)



#define LOFF_DAT_CH7_HIGH_ADDRESS 0x51
// 高8位地址指针
#define LOFF_DAT_CH7_HIGH_HIGH_ADDR  (LOFF_DAT_CH7_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH7_HIGH_LOW_ADDR   (LOFF_DAT_CH7_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH7_LOW_ADDRESS 0x52
// 高8位地址指针
#define LOFF_DAT_CH7_LOW_HIGH_ADDR  (LOFF_DAT_CH7_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH7_LOW_LOW_ADDR   (LOFF_DAT_CH7_LOW_ADDRESS & 0xFF)


#define LOFF_DAT_CH8_HIGH_ADDRESS 0x53
// 高8位地址指针
#define LOFF_DAT_CH8_HIGH_HIGH_ADDR  (LOFF_DAT_CH8_HIGH_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH8_HIGH_LOW_ADDR   (LOFF_DAT_CH8_HIGH_ADDRESS & 0xFF)

#define LOFF_DAT_CH8_LOW_ADDRESS 0x54
// 高8位地址指针
#define LOFF_DAT_CH8_LOW_HIGH_ADDR  (LOFF_DAT_CH8_LOW_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_DAT_CH8_LOW_LOW_ADDR   (LOFF_DAT_CH8_LOW_ADDRESS & 0xFF)


#define LOFF_CFG_ADDRESS  0X82
// 高8位地址指针
#define LOFF_CFG_HIGH_ADDR  (LOFF_CFG_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_CFG_LOW_ADDR   (LOFF_CFG_ADDRESS & 0xFF)


#define LOFF_FREQ_ADDRESS  0X83
// 高8位地址指针
#define LOFF_FREQ_HIGH_ADDR  (LOFF_FREQ_ADDRESS >> 8)
// 低8位地址指针
#define LOFF_FREQ_LOW_ADDR   (LOFF_FREQ_ADDRESS & 0xFF)



#define USERKEY_ADDRESS                    0x182
// 高8位地址指针
#define USERKEY_HIGH_ADDR (USERKEY_ADDRESS >> 8)
// 低8位地址指针
#define USERKEY_LOW_ADDR  (USERKEY_ADDRESS & 0xFF)
//=============================================================	


//===============================================
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


#define LHE7909_USERKEY_VALUE              0xAC




uint8_t LHE7909_read_id(void);

uint8_t LHE7909_read_register(uint8_t address);

void LHE7909_write_register(uint8_t address,uint8_t data);


void LHE7909_init(void);


void LHE7909_SDATAC(void);
void LHE7909_RDATAC(void);
void LHE7909_RDATA(void);

void LHE7909_WAKEUP(void);
void LHE7909_STANDBY(void);
void LHE7909_RESET(void);
void LHE7909_START(void);
void LHE7909_STOP(void);





void LHE7909_delay_us(uint32_t time);
void LHE7909_get_ACLO_data(void);


void LHE7909_get_adc_data(void);

float  ADC_DataToMiniVolt(int32_t data);

uint8_t DUT_RTREG(uint8_t opcode1,uint16_t opcode2);
void DUT_WTREG(uint8_t _value, uint8_t opcode1,uint8_t opcode2);




