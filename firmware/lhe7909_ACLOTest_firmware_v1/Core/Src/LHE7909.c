#include "LHE7909.h"

#include "main.h"


#include "stdio.h"
#include <stdbool.h>
extern SPI_HandleTypeDef hspi2;

extern UART_HandleTypeDef huart2;

uint8_t LHE7909_read_register(uint8_t address)
{
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	
	uint8_t tx_data_spi[3];
	uint8_t rx_data_spi[3];
	
	rx_data_spi[0]=0;
	rx_data_spi[1]=0;
	rx_data_spi[2]=0;
	
	
	tx_data_spi[0] = LHE7909_CMD_RREG+address;	//001r rrrr，其中 r rrrr 是起始寄存器地址
	tx_data_spi[1] = 0x00;	//000n nnnn，其中 n nnnn 是要读取的寄存器数量 – 1
	tx_data_spi[2] = 0x00;/* Send out NOP to initiate SCLK */
	
	HAL_SPI_TransmitReceive(&hspi2,tx_data_spi,rx_data_spi,3,HAL_MAX_DELAY);

	LHE7909_delay_us(5);
	
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);
  return rx_data_spi[2];

}
void LHE7909_write_register(uint8_t address,uint8_t data)
{
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	uint8_t tx_data_spi[3];
	
	tx_data_spi[0] = LHE7909_CMD_WREG+address;	//
	tx_data_spi[1] = 0x00;	//write 1 registers starting at address 
	tx_data_spi[2] = data;	//
	HAL_SPI_Transmit(&hspi2,  &tx_data_spi[0], 1, 1000);
	LHE7909_delay_us(5);
  HAL_SPI_Transmit(&hspi2,  &tx_data_spi[1], 1, 1000);	
	LHE7909_delay_us(5);
  HAL_SPI_Transmit(&hspi2,  &tx_data_spi[2], 1, 1000);
	LHE7909_delay_us(5);
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);

}

//读取一个用户寄存器:address-寄存器地址
uint8_t LHE7909_read_one_register(uint8_t address)
{
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	
	uint8_t tx_data_spi[3];
	uint8_t rx_data_spi[3];
	
	rx_data_spi[0]=0;
	rx_data_spi[1]=0;
	rx_data_spi[2]=0;
	
	
	tx_data_spi[0] = LHE7909_CMD_RREG+address;	//001r rrrr，其中 r rrrr 是起始寄存器地址
	tx_data_spi[1] = 0x00;	//000n nnnn，其中 n nnnn 是要读取的寄存器数量 – 1
	tx_data_spi[2] = 0x00;/* Send out NOP to initiate SCLK */
	
	HAL_SPI_TransmitReceive(&hspi2,tx_data_spi,rx_data_spi,3,HAL_MAX_DELAY);

	LHE7909_delay_us(5);
	
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);
  return rx_data_spi[2];

}

//写一个用户寄存器:address-寄存器地址；  data-要写入的数据
void LHE7909_write_one_register(uint8_t address,uint8_t data)
{
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	uint8_t tx_data_spi[3];
	
	tx_data_spi[0] = LHE7909_CMD_WREG+address;	//
	tx_data_spi[1] = 0x00;	//write 1 registers starting at address 
	tx_data_spi[2] = data;	//
	HAL_SPI_Transmit(&hspi2,  &tx_data_spi[0], 1, 1000);
	LHE7909_delay_us(5);
  HAL_SPI_Transmit(&hspi2,  &tx_data_spi[1], 1, 1000);	
	LHE7909_delay_us(5);
  HAL_SPI_Transmit(&hspi2,  &tx_data_spi[2], 1, 1000);
	LHE7909_delay_us(5);
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);

}

//写高地址寄存器:value:要写入的数据 opcode1地址高字节 opcode2地址低字节；
void DUT_WTREG(uint8_t _value, uint8_t opcode1,uint8_t opcode2)
{
	uint8_t wtreg_data[4];
	uint8_t sp1_rx,i;

  wtreg_data[0]=  opcode1 | 0xA0;
	wtreg_data[1]=  opcode2;
	wtreg_data[2]=  0x00;//write one reg.
	wtreg_data[3] = _value;//
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
  for(i=0;i<4;i++)
		{
			HAL_SPI_TransmitReceive(&hspi2, &wtreg_data[i], &sp1_rx, 1,1000);
			LHE7909_delay_us(20);
		}
	LHE7909_delay_us(20);
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);
}


//读取高地址寄存器:opcode1地址高字节 opcode2地址低字节；
uint8_t DUT_RTREG(uint8_t opcode1,uint16_t opcode2)
{
    uint8_t i;

	uint8_t spi1_rx[4];

	uint8_t spi1_tx[4];

	spi1_rx[0]=0;
	spi1_rx[1]=0;
	spi1_rx[2]=0;
	spi1_rx[3]=0;
	
	spi1_tx[0] = opcode1 | 0x80;
	spi1_tx[1] = opcode2;//
	spi1_tx[2] = 0x00;//长度
	spi1_tx[3] = 0x00;//读回数据
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	for(i=0;i<4;i++)
		{
			HAL_SPI_TransmitReceive(&hspi2, &spi1_tx[i], &spi1_rx[i], 1,1000);
			LHE7909_delay_us(20);
		}
	LHE7909_delay_us(20);
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);
	return spi1_rx[3];
}

//读取芯片ID
void read_LHE7909_ID(void)
{
	 uint8_t reg_data=0;
	reg_data=LHE7909_read_one_register(ID_ADDRESS);
	printf("ID: 0x%x\r\n",reg_data);
	
}

//userkye解锁:USERKEY寄存器写入0xAC
void userkey_unlock(void)
{
	DUT_WTREG(LHE7909_USERKEY_VALUE,USERKEY_HIGH_ADDR,USERKEY_LOW_ADDR);//USERKEY UNLOCK
}

//
void LHE7909_get_ACLO_data(void)
{
	uint8_t reg_data,LOFF_DAT_CHn_HIGH,LOFF_DAT_CHn_LOW;
	
	uint16_t  LOFF_DAT_CHn=0;
	uint16_t  Loff_dat[8];
	
	LOFF_DAT_CHn_HIGH=DUT_RTREG(LOFF_DAT_CH1_HIGH_HIGH_ADDR,LOFF_DAT_CH1_HIGH_LOW_ADDR);//LOFF_DATA高字节
    LOFF_DAT_CHn_LOW=DUT_RTREG(LOFF_DAT_CH1_LOW_HIGH_ADDR,LOFF_DAT_CH1_LOW_LOW_ADDR);//LOFF_DATA低字节
    LOFF_DAT_CHn=  ((LOFF_DAT_CHn_HIGH&0xFF)<<8)|((LOFF_DAT_CHn_LOW&0xFF)<<0);	
//	printf("CH1:%d\r\n",LOFF_DAT_CHn);
	  Loff_dat[0] = LOFF_DAT_CHn;
	
	
	LOFF_DAT_CHn_HIGH=DUT_RTREG(LOFF_DAT_CH2_HIGH_HIGH_ADDR,LOFF_DAT_CH2_HIGH_LOW_ADDR);//LOFF_DATA高字节
    LOFF_DAT_CHn_LOW=DUT_RTREG(LOFF_DAT_CH2_LOW_HIGH_ADDR,LOFF_DAT_CH2_LOW_LOW_ADDR);//LOFF_DATA低字节
    LOFF_DAT_CHn=  ((LOFF_DAT_CHn_HIGH&0xFF)<<8)|((LOFF_DAT_CHn_LOW&0xFF)<<0);	
//	printf("CH2:%d\r\n",LOFF_DAT_CHn);
	  Loff_dat[1] = LOFF_DAT_CHn;
	
	
	LOFF_DAT_CHn_HIGH=DUT_RTREG(LOFF_DAT_CH3_HIGH_HIGH_ADDR,LOFF_DAT_CH3_HIGH_LOW_ADDR);//LOFF_DATA高字节
    LOFF_DAT_CHn_LOW=DUT_RTREG(LOFF_DAT_CH3_LOW_HIGH_ADDR,LOFF_DAT_CH3_LOW_LOW_ADDR);//LOFF_DATA低字节
    LOFF_DAT_CHn=  ((LOFF_DAT_CHn_HIGH&0xFF)<<8)|((LOFF_DAT_CHn_LOW&0xFF)<<0);	
//	printf("CH3:%d\r\n",LOFF_DAT_CHn);
	  Loff_dat[2] = LOFF_DAT_CHn;
	
	
	LOFF_DAT_CHn_HIGH=DUT_RTREG(LOFF_DAT_CH4_HIGH_HIGH_ADDR,LOFF_DAT_CH4_HIGH_LOW_ADDR);//LOFF_DATA高字节
    LOFF_DAT_CHn_LOW=DUT_RTREG(LOFF_DAT_CH4_LOW_HIGH_ADDR,LOFF_DAT_CH4_LOW_LOW_ADDR);//LOFF_DATA低字节
    LOFF_DAT_CHn=  ((LOFF_DAT_CHn_HIGH&0xFF)<<8)|((LOFF_DAT_CHn_LOW&0xFF)<<0);	
//	printf("CH4:%d\r\n",LOFF_DAT_CHn);
	  Loff_dat[3] = LOFF_DAT_CHn;
	
	LOFF_DAT_CHn_HIGH=DUT_RTREG(LOFF_DAT_CH5_HIGH_HIGH_ADDR,LOFF_DAT_CH5_HIGH_LOW_ADDR);//LOFF_DATA高字节
    LOFF_DAT_CHn_LOW=DUT_RTREG(LOFF_DAT_CH5_LOW_HIGH_ADDR,LOFF_DAT_CH5_LOW_LOW_ADDR);//LOFF_DATA低字节
    LOFF_DAT_CHn=  ((LOFF_DAT_CHn_HIGH&0xFF)<<8)|((LOFF_DAT_CHn_LOW&0xFF)<<0);	
//	printf("CH5:%d\r\n",LOFF_DAT_CHn);
	  Loff_dat[4] = LOFF_DAT_CHn;
	
	LOFF_DAT_CHn_HIGH=DUT_RTREG(LOFF_DAT_CH6_HIGH_HIGH_ADDR,LOFF_DAT_CH6_HIGH_LOW_ADDR);//LOFF_DATA高字节
    LOFF_DAT_CHn_LOW=DUT_RTREG(LOFF_DAT_CH6_LOW_HIGH_ADDR,LOFF_DAT_CH6_LOW_LOW_ADDR);//LOFF_DATA低字节
    LOFF_DAT_CHn=  ((LOFF_DAT_CHn_HIGH&0xFF)<<8)|((LOFF_DAT_CHn_LOW&0xFF)<<0);	
//	printf("CH6:%d\r\n",LOFF_DAT_CHn);
	  Loff_dat[5] = LOFF_DAT_CHn;
	
	LOFF_DAT_CHn_HIGH=DUT_RTREG(LOFF_DAT_CH7_HIGH_HIGH_ADDR,LOFF_DAT_CH7_HIGH_LOW_ADDR);//LOFF_DATA高字节
    LOFF_DAT_CHn_LOW=DUT_RTREG(LOFF_DAT_CH7_LOW_HIGH_ADDR,LOFF_DAT_CH7_LOW_LOW_ADDR);//LOFF_DATA低字节
    LOFF_DAT_CHn=  ((LOFF_DAT_CHn_HIGH&0xFF)<<8)|((LOFF_DAT_CHn_LOW&0xFF)<<0);	
//	printf("CH7:%d\r\n",LOFF_DAT_CHn);
	  Loff_dat[6] = LOFF_DAT_CHn;
	
	LOFF_DAT_CHn_HIGH=DUT_RTREG(LOFF_DAT_CH8_HIGH_HIGH_ADDR,LOFF_DAT_CH8_HIGH_LOW_ADDR);//LOFF_DATA高字节
    LOFF_DAT_CHn_LOW=DUT_RTREG(LOFF_DAT_CH8_LOW_HIGH_ADDR,LOFF_DAT_CH8_LOW_LOW_ADDR);//LOFF_DATA低字节
    LOFF_DAT_CHn=  ((LOFF_DAT_CHn_HIGH&0xFF)<<8)|((LOFF_DAT_CHn_LOW&0xFF)<<0);	
//	printf("CH8:%d\r\n",LOFF_DAT_CHn);
	  Loff_dat[7] = LOFF_DAT_CHn;
		
		//printf("AC_LO: %d\n",Loff_dat[0]);
		printf("AC_LO: %d,%d,%d,%d,%d,%d,%d,%d\n",Loff_dat[0],Loff_dat[1],Loff_dat[2],Loff_dat[3],Loff_dat[4],Loff_dat[5],Loff_dat[6],Loff_dat[7]);
   //reg_data=LHE7909_read_one_register(LOFF_STATP_ADDRESS);
   //printf("0x%x\r\n",reg_data);
	
}
//初始化LHE7909，循环读取AC激励处理模块处理结果
void LHE7909_init(void)
{
uint8_t reg_data=0;
	
uint8_t CHn_PDn,GAINn,MUXn;

HAL_GPIO_WritePin(GPIOA, LHE7909_DAISY_IN_Pin, GPIO_PIN_RESET);//set DAISY_IN Pin is 0
HAL_GPIO_WritePin(GPIOB, LHE7909_nPWDN_Pin, GPIO_PIN_SET);//set PWDN PIN is 1
HAL_GPIO_WritePin(GPIOB, LHE7909_START_Pin, GPIO_PIN_RESET);//set START PIN is 0

//reset lhe7909 by reset pin
HAL_GPIO_WritePin(GPIOB, LHE7909_nRESET_Pin, GPIO_PIN_SET);
HAL_Delay(100);
HAL_GPIO_WritePin(GPIOB, LHE7909_nRESET_Pin, GPIO_PIN_RESET);
HAL_Delay(10);
HAL_GPIO_WritePin(GPIOB, LHE7909_nRESET_Pin, GPIO_PIN_SET);
HAL_Delay(200);
	
/*
NOTE:
如果器件处于 RDATAC 模式，则必须先发出 SDATAC命令，然后才能将任何其他命令发送到器件中。
即在RDATAC 模式下,芯片只能接收到SDATAC命令或START RESET PWDN引脚控制
芯片复位后处于RDATAC模式
*/
LHE7909_SDATAC();
HAL_Delay(200);

read_LHE7909_ID();
userkey_unlock();
	//============
CONFIG1_TypeDef config1;

config1.bit.HR			  = CONFIG1_HR_HR;		  //高分辨率模式
config1.bit.DAISY_EN	  = CONFIG1_MULTI_READ; 	  // 菊花链
config1.bit.CLK_EN		  = CONFIG1_CLK_OUT_DIS;  // 时钟输出禁用
config1.bit.RESERVED_4_3  = 0;					 // 保留位必须写0
config1.bit.DR			  = HR_DR_500SPS;	  // 采样率500SPS
LHE7909_write_one_register(CONFIG1_ADDRESS, config1.reg);
reg_data=LHE7909_read_one_register(CONFIG1_ADDRESS);
printf("CONFIG1: 0x%x\r\n",reg_data);
//============
CONFIG2_TypeDef config2;
config2.bit.RESERVED7_6   = CONFIG2_RESERVED_76;	// 11 必须
config2.bit.WCT_CHOP	  = WCT_CHOP_VARIABLE;		 // 0
config2.bit.INT_TEST	  = TEST_SOURCE_EXTERNAL;	 // 0
config2.bit.RESERVED3	  = 0;						 // 必须 0
config2.bit.TEST_AMP	  = TEST_AMP_1X;			 // 0
config2.bit.TEST_FREQ	  = TEST_FREQ1; 	 // 00
LHE7909_write_one_register(CONFIG2_ADDRESS, config2.reg);
reg_data=LHE7909_read_one_register(CONFIG2_ADDRESS);
printf("CONFIG2: 0x%x\r\n",reg_data);

//==========================
CONFIG3_TypeDef config3;
// 常用ECG配置示例
config3.bit.PD_REFBUF      = REFBUF_ENABLE;     // 使能基准缓冲器
config3.bit.ILEAD_EN       = ILEAD_ENABLE;      // 使能大电流
config3.bit.VREF_4V        = VREF_V2V4;          // 2.4V基准
config3.bit.RLD_MEAS       = RLD_MEAS_DISABLE;  // 关闭RLD测量
config3.bit.RLDREF_INT     = RLDREF_INT_INTERNAL;        //选择内部中点(AVDD+AVSS)/2作为RLD运放的正向输入
config3.bit.PD_RLD         = RLD_POWER_DOWN;      // 关闭RLD缓冲器
config3.bit.RLD_LOFF_SENS  = RLD_LOFF_SENS_DISABLE; // 开启RLD感应

LHE7909_write_one_register(CONFIG3_ADDRESS,config3.reg);
reg_data=LHE7909_read_one_register(CONFIG3_ADDRESS);
printf("CONFIG3: 0x%x\r\n",reg_data);
//===========================
CONFIG4_TypeDef config4;

// 常用ECG配置示例
config4.bit.SINC4_EN		= SINC4_ENABLE; 	   // 开启SINC4滤波器
config4.bit.PD_LOFF_COMP	= LOFF_COMP_POWER_DOWN; // 启用导联脱落比较器
config4.bit.WCT_TO_RLD		= WCT_RLD_DISCONNECT; // 断开WCT与RLD
config4.bit.SINGLE_SHOT 	= CONV_MODE_CONTINUOUS;// 连续转换模式
config4.bit.RESERVED		= 0;				  // 保留位写0

LHE7909_write_one_register(CONFIG4_ADDRESS, config4.reg);
reg_data=LHE7909_read_one_register(CONFIG4_ADDRESS);
printf("CONFIG4: 0x%x\r\n",reg_data);
//==================
CHn_PDn=CH_POWER_NORMAL;
GAINn=GAIN_2;
MUXn=MUX_NORMAL_ELECTRODE;
//==============
CHnSET_TypeDef ch1_set;

// 配置 CH1：正常供电、增益、正常电极输入
ch1_set.bit.PDn 	  = CHn_PDn;	// 正常工作
ch1_set.bit.GAINn	  = GAINn;			// 设置增益
ch1_set.bit.RESERVED3 = 0;					// 保留位必须写 0
ch1_set.bit.MUXn	  = MUXn;  //输入选择

LHE7909_write_one_register(CH1SET_ADDRESS,ch1_set.reg);
reg_data=LHE7909_read_one_register(CH1SET_ADDRESS);
printf("CH1SET: 0x%x\r\n",reg_data);

//===================
CHnSET_TypeDef ch2_set;

// 配置 CH2：正常供电、增益、正常电极输入
ch2_set.bit.PDn 	  = CHn_PDn;	// 正常工作
ch2_set.bit.GAINn	  = GAINn;			// 设置增益 
ch2_set.bit.RESERVED3 = 0;					// 保留位必须写 0
ch2_set.bit.MUXn	  = MUXn;  //输入选择

LHE7909_write_one_register(CH2SET_ADDRESS,ch2_set.reg);
reg_data=LHE7909_read_one_register(CH2SET_ADDRESS);
printf("CH2SET: 0x%x\r\n",reg_data);

//===================
CHnSET_TypeDef ch3_set;

// 配置 CH3：正常供电、增益、正常电极输入
ch3_set.bit.PDn 	  = CHn_PDn;	// 正常工作
ch3_set.bit.GAINn	  = GAINn;			// 设置增益 
ch3_set.bit.RESERVED3 = 0;					// 保留位必须写 0
ch3_set.bit.MUXn	  = MUXn;  //输入选择

LHE7909_write_one_register(CH3SET_ADDRESS,ch3_set.reg);
reg_data=LHE7909_read_one_register(CH3SET_ADDRESS);
printf("CH3SET: 0x%x\r\n",reg_data);
//===================
CHnSET_TypeDef ch4_set;
// 配置 CH4：正常供电、增益、正常电极输入
ch4_set.bit.PDn 	  = CHn_PDn;	// 正常工作
ch4_set.bit.GAINn	  = GAINn;			// 设置增益 
ch4_set.bit.RESERVED3 = 0;					// 保留位必须写 0
ch4_set.bit.MUXn	  = MUXn;  //输入选择

LHE7909_write_one_register(CH4SET_ADDRESS,ch4_set.reg);
reg_data=LHE7909_read_one_register(CH4SET_ADDRESS);
printf("CH4SET: 0x%x\r\n",reg_data);
//===================
CHnSET_TypeDef ch5_set;
// 配置 CH5：正常供电、增益、正常电极输入
ch5_set.bit.PDn 	  = CHn_PDn;	// 正常工作
ch5_set.bit.GAINn	  = GAINn;			// 设置增益 
ch5_set.bit.RESERVED3 = 0;					// 保留位必须写 0
ch5_set.bit.MUXn	  = MUXn;  //输入选择

LHE7909_write_one_register(CH5SET_ADDRESS,ch5_set.reg);
reg_data=LHE7909_read_one_register(CH5SET_ADDRESS);
printf("CH5SET: 0x%x\r\n",reg_data);
//===================
CHnSET_TypeDef ch6_set;
// 配置 CH6：正常供电、增益、正常电极输入
ch6_set.bit.PDn 	  = CHn_PDn;	// 正常工作
ch6_set.bit.GAINn	  = GAINn;			// 设置增益 
ch6_set.bit.RESERVED3 = 0;					// 保留位必须写 0
ch6_set.bit.MUXn	  = MUXn;  //输入选择

LHE7909_write_one_register(CH6SET_ADDRESS,ch6_set.reg);
reg_data=LHE7909_read_one_register(CH6SET_ADDRESS);
printf("CH6SET: 0x%x\r\n",reg_data);
//===================
CHnSET_TypeDef ch7_set;
// 配置 CH7：正常供电、增益、正常电极输入
ch7_set.bit.PDn 	  = CHn_PDn;	// 正常工作
ch7_set.bit.GAINn	  = GAINn;			// 设置增益 
ch7_set.bit.RESERVED3 = 0;					// 保留位必须写 0
ch7_set.bit.MUXn	  = MUXn;  //输入选择

LHE7909_write_one_register(CH7SET_ADDRESS,ch7_set.reg);
reg_data=LHE7909_read_one_register(CH7SET_ADDRESS);
printf("CH7SET: 0x%x\r\n",reg_data);
//==================
CHnSET_TypeDef ch8_set;

// 配置 CH8：正常供电、增益、正常电极输入
ch8_set.bit.PDn 	  = CHn_PDn;	// 正常工作
ch8_set.bit.GAINn	  = GAINn;			// 设置增益 
ch8_set.bit.RESERVED3 = 0;					// 保留位必须写 0
ch8_set.bit.MUXn	  = MUXn;  //输入选择

LHE7909_write_one_register(CH8SET_ADDRESS,ch8_set.reg);
reg_data=LHE7909_read_one_register(CH8SET_ADDRESS);
printf("CH8SET: 0x%x\r\n",reg_data);
//================LEAD OFF SET BEGIN=====================================

//=========================

LOFF_CFG_TypeDef loff_cfg;
loff_cfg.bit.RESERVED0        = 0;
loff_cfg.bit.ILOFF_BST_EN     = ILOFF_BST_BOOST;//leadoff电流倍增，step=10nA
loff_cfg.bit.RLD_COMP_EN      = RLD_COMP_DISABLE;
loff_cfg.bit.LOFF_DEGLITCH_BYP= LOFF_DEGLITCH_NORMAL;
loff_cfg.bit.RESERVED7_4      = 0;

DUT_WTREG(loff_cfg.reg,LOFF_CFG_HIGH_ADDR,LOFF_CFG_LOW_ADDR);
reg_data=DUT_RTREG(LOFF_CFG_HIGH_ADDR,LOFF_CFG_LOW_ADDR);
printf("LOFF_CFG: 0x%x\r\n",reg_data);
//===================

LOFF_TypeDef loff_reg;

// 配置示例：
loff_reg.bit.COMP_TH      = COMP_TH_N_30;        // 比较器 90% (DC导联脱落阈值设置)
loff_reg.bit.VLEAD_OFF_EN = LOFF_MODE_CURRENT;   // 电流源模式(DC可以选择电阻模式或电流模式;AC只有电流模式) 
/*
NOTE:
config3.bit.ILEAD_EN=ILEAD_DISABLE,loff_cfg.bit.ILOFF_BST_EN=ILOFF_BST_NORMAL:0na,2.5na,5na,7.5na
//=========
config3.bit.ILEAD_EN=ILEAD_ENABLE, loff_cfg.bit.ILOFF_BST_EN=ILOFF_BST_NORMAL:10na,12.5na,15na,17.5na
//========
config3.bit.ILEAD_EN=ILEAD_DISABLE, loff_cfg.bit.ILOFF_BST_EN=ILOFF_BST_BOOST:0na,20na,30na
//========
config3.bit.ILEAD_EN=ILEAD_ENABLE, loff_cfg.bit.ILOFF_BST_EN=ILOFF_BST_BOOST:40na,50na,60na,70na
*/
loff_reg.bit.ILEAD_OFF    = ILEAD_OFF_50NA; //DC或AC激励电流值
loff_reg.bit.FLEAD_OFF    = FLEAD_OFF_AC;   //FLEAD_OFF_AC: AC 检测; FLEAD_OFF_DC:DC检测

// 整体写入寄存器
LHE7909_write_one_register(LOFF_ADDRESS, loff_reg.reg);
reg_data=LHE7909_read_one_register(LOFF_ADDRESS);
printf("LOFF: 0x%x\r\n",reg_data);
//===================================
// 定义寄存器变量
LOFF_FREQ_TypeDef loff_freq;

// 配置频率：1KHz（默认）
loff_freq.bit.RESERVED = 0;        // 保留位必须写 0
loff_freq.bit.AC_FREQ = LOFF_FREQ_1KHZ;

DUT_WTREG(loff_freq.reg,LOFF_FREQ_HIGH_ADDR,LOFF_FREQ_LOW_ADDR);
reg_data=DUT_RTREG(LOFF_FREQ_HIGH_ADDR,LOFF_FREQ_LOW_ADDR);
printf("LOFF_FREQ: 0x%x\r\n",reg_data);
//=======================
//DUT_WTREG(0x01,LOFF_UTH_HIGH_HIGH_ADDR,LOFF_UTH_HIGH_LOW_ADDR);
reg_data=DUT_RTREG(LOFF_UTH_HIGH_HIGH_ADDR,LOFF_UTH_HIGH_LOW_ADDR);
printf("LOFF_UTH_HIGH: 0x%x\r\n",reg_data);

//DUT_WTREG(0x00,LOFF_UTH_LOW_HIGH_ADDR,LOFF_UTH_LOW_LOW_ADDR);
reg_data=DUT_RTREG(LOFF_UTH_LOW_HIGH_ADDR,LOFF_UTH_LOW_LOW_ADDR);
printf("LOFF_UTH_LOW: 0x%x\r\n",reg_data);

// DUT_WTREG(0x00,LOFF_LTH_HIGH_HIGH_ADDR,LOFF_LTH_HIGH_LOW_ADDR);
reg_data=DUT_RTREG(LOFF_LTH_HIGH_HIGH_ADDR,LOFF_LTH_HIGH_LOW_ADDR);
printf("LOFF_LTH_HIGH: 0x%x\r\n",reg_data);

//DUT_WTREG(0x10,LOFF_LTH_LOW_HIGH_ADDR,LOFF_LTH_LOW_LOW_ADDR);
reg_data=DUT_RTREG(LOFF_LTH_LOW_HIGH_ADDR,LOFF_LTH_LOW_LOW_ADDR);
printf("LOFF_LTH_LOW: 0x%x\r\n",reg_data);
//===============

LOFF_SENSP_TypeDef loff_sensp;


loff_sensp.bit.LOFF1P = LOFF_CH_ENABLE;// 使能 CH1 正极脱落检测 
loff_sensp.bit.LOFF2P = LOFF_CH_ENABLE;
loff_sensp.bit.LOFF3P = LOFF_CH_ENABLE;
loff_sensp.bit.LOFF4P = LOFF_CH_ENABLE;

loff_sensp.bit.LOFF5P = LOFF_CH_ENABLE;
loff_sensp.bit.LOFF6P = LOFF_CH_ENABLE;
loff_sensp.bit.LOFF7P = LOFF_CH_ENABLE;
loff_sensp.bit.LOFF8P = LOFF_CH_ENABLE;

LHE7909_write_one_register(LOFF_SENSP_ADDRESS, loff_sensp.reg);
reg_data=LHE7909_read_one_register(LOFF_SENSP_ADDRESS);
printf("LOFF_SENSP: 0x%x\r\n",reg_data);
//=====================
LOFF_SENSN_TypeDef loff_sensn;


loff_sensn.bit.LOFF1N = LOFF_CH_ENABLE;// 使能 CH1负极脱落检测
loff_sensn.bit.LOFF2N = LOFF_CH_ENABLE;
loff_sensn.bit.LOFF3N = LOFF_CH_ENABLE;
loff_sensn.bit.LOFF4N = LOFF_CH_ENABLE;

loff_sensn.bit.LOFF5N = LOFF_CH_ENABLE;
loff_sensn.bit.LOFF6N = LOFF_CH_ENABLE;
loff_sensn.bit.LOFF7N = LOFF_CH_ENABLE;
loff_sensn.bit.LOFF8N = LOFF_CH_ENABLE;

LHE7909_write_one_register(LOFF_SENSN_ADDRESS, loff_sensn.reg);
reg_data=LHE7909_read_one_register(LOFF_SENSN_ADDRESS);
printf("LOFF_SENSN: 0x%x\r\n",reg_data);
//=================
  DIG_ACLOFF_ENABLE_TypeDef acloff_en;
   
acloff_en.bit.CH1EN = AC_LOFF_CH_ENABLE;
acloff_en.bit.CH2EN = AC_LOFF_CH_ENABLE;
acloff_en.bit.CH3EN = AC_LOFF_CH_ENABLE;
acloff_en.bit.CH4EN = AC_LOFF_CH_ENABLE;

acloff_en.bit.CH5EN = AC_LOFF_CH_ENABLE;
acloff_en.bit.CH6EN = AC_LOFF_CH_ENABLE;
acloff_en.bit.CH7EN = AC_LOFF_CH_ENABLE;
acloff_en.bit.CH8EN = AC_LOFF_CH_ENABLE;
 DUT_WTREG(acloff_en.reg,DIG_ACLOFF_ENABLE_HIGH_ADDR,DIG_ACLOFF_ENABLE_LOW_ADDR);
 reg_data=DUT_RTREG(DIG_ACLOFF_ENABLE_HIGH_ADDR,DIG_ACLOFF_ENABLE_LOW_ADDR);
 printf("DIG_ACLOFF_ENABLE: 0x%x\r\n",reg_data);
//=============
//=================
LOFF_FLIP_TypeDef loff_flip;

// 配置：CH1~CH4 极性翻转，CH5~CH8 保持默认
loff_flip.bit.LOFF_FLIP1 = LOFF_FLIP_NORMAL;
loff_flip.bit.LOFF_FLIP2 = LOFF_FLIP_NORMAL;
loff_flip.bit.LOFF_FLIP3 = LOFF_FLIP_NORMAL;
loff_flip.bit.LOFF_FLIP4 = LOFF_FLIP_NORMAL;

loff_flip.bit.LOFF_FLIP5 = LOFF_FLIP_NORMAL;
loff_flip.bit.LOFF_FLIP6 = LOFF_FLIP_NORMAL;
loff_flip.bit.LOFF_FLIP7 = LOFF_FLIP_NORMAL;
loff_flip.bit.LOFF_FLIP8 = LOFF_FLIP_NORMAL;

LHE7909_write_one_register(LOFF_FLIP_ADDRESS, loff_flip.reg);
reg_data=LHE7909_read_one_register(LOFF_FLIP_ADDRESS);
printf("LOFF_FLIP_ADDRESS: 0x%x\r\n",reg_data);

//=============================
DC_LOFF_OSEL_TypeDef dc_loff_insel;

dc_loff_insel.bit.P_SEL      = LOFF_GPIO_SEL_GPIO1;
dc_loff_insel.bit.RESERVED   = DC_LOFF_OSEL_RESERVED_VAL;  // 必须写 0

LHE7909_write_one_register(DC_LOFF_OSEL_ADDRESS, dc_loff_insel.reg);
reg_data=LHE7909_read_one_register(DC_LOFF_OSEL_ADDRESS);
printf("DC_LOFF_OSEL_ADDRESS: 0x%x\r\n",reg_data);

//================LEAD OFF SET END=========================
   /*NOTE:
  	RDATAC：连续读取数据命令设置在不发送操作码的情况下连续读取数据的器件模式
   */
	//LHE7909_RDATAC();
  	//=========
	/*
	NOTE:
	1)将START引脚拉为高电平并至少持续 2个 tCLK周期，或者发送 START命令以开始转换。
	2)使用 START 操作码开始转换时，将 START 引脚保持为低电平。
	3)当START引脚被拉为高电平或发送 START命令时，器件 ADC会转换输入信号并且 DRDY被拉为高电平。DRDY的下一个下降沿指示数据是可读的。
	*/
	/*
	NOTE:
	开始转换：通过START命令或START引脚拉高，如果使用START命令，需要芯片处于非连续读模式
	*/
	LHE7909_START();
	//LHE7909_RDATAC();
	//HAL_GPIO_WritePin(GPIOB, LHE7909_START_Pin, GPIO_PIN_SET);//set START PIN is 1
//=============================
#if 1
while(1)
	{
		LHE7909_get_ACLO_data();
		HAL_Delay(100);
	}
	#endif
	
}





void LHE7909_SDATAC(void)
{
	uint8_t tx_data_spi;
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	tx_data_spi = LHE7909_CMD_SDATAC;
  HAL_SPI_Transmit(&hspi2, &tx_data_spi, 1, 1000);
	LHE7909_delay_us(5);
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);		
}


void LHE7909_RDATAC(void)
{
	uint8_t tx_data_spi;
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	tx_data_spi = LHE7909_CMD_RDATAC;
  HAL_SPI_Transmit(&hspi2, &tx_data_spi, 1, 1000);
	LHE7909_delay_us(5);
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);	
}
void LHE7909_RDATA(void)
{
	uint8_t tx_data_spi;
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	tx_data_spi = LHE7909_CMD_RDATA;
  HAL_SPI_Transmit(&hspi2, &tx_data_spi, 1, 1000);
	LHE7909_delay_us(5);
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);	
}

void LHE7909_WAKEUP(void)
{
	uint8_t tx_data_spi;
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	tx_data_spi = LHE7909_CMD_WAKEUP;
  HAL_SPI_Transmit(&hspi2, &tx_data_spi, 1, 1000);
	LHE7909_delay_us(5);
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);	
}
void LHE7909_STANDBY(void)
{
	uint8_t tx_data_spi;
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	tx_data_spi = LHE7909_CMD_STANDBY;
  HAL_SPI_Transmit(&hspi2, &tx_data_spi, 1, 1000);
	LHE7909_delay_us(5);
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);	
}
void LHE7909_RESET(void)
{
	uint8_t tx_data_spi;
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	tx_data_spi = LHE7909_CMD_RESET;
  HAL_SPI_Transmit(&hspi2, &tx_data_spi, 1, 1000);
	LHE7909_delay_us(5);
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);	
}
void LHE7909_START(void)
{
	uint8_t tx_data_spi;
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	tx_data_spi = LHE7909_CMD_START;
  HAL_SPI_Transmit(&hspi2, &tx_data_spi, 1, 1000);
	LHE7909_delay_us(5);
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);	
}
void LHE7909_STOP(void)
{
	uint8_t tx_data_spi;
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	tx_data_spi = LHE7909_CMD_STOP;
  HAL_SPI_Transmit(&hspi2, &tx_data_spi, 1, 1000);
	LHE7909_delay_us(5);
  HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);	
}





void LHE7909_delay_us(uint32_t time)
{
  for(uint32_t i=0;i<10*time;i++)
	{
		__NOP();
	}
}



extern uint8_t LHE7909_adc_data[27];
int32_t ch1_hex,ch2_hex,ch3_hex,ch4_hex,ch5_hex,ch6_hex,ch7_hex,ch8_hex;
float   ch1_voltage,ch2_voltage,ch3_voltage,ch4_voltage,ch5_voltage,ch6_voltage,ch7_voltage,ch8_voltage;
int32_t ch1_hex_buf_size[128000];
uint32_t ch1_hex_buf_count=0;
uint8_t  adc_cov_finish=0;
void LHE7909_get_adc_data(void)
{
	  uint8_t x1,x2,x3,x4,x5,x6,x7,x8,x9,x10,x11,x12,x13,x14,x15,x16;
		HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Receive(&hspi2,LHE7909_adc_data,27,1000);
    if((LHE7909_adc_data[0]&0xC0)==0xC0)
		{
				ch1_hex = (int32_t)(((((LHE7909_adc_data[3] & 0x80) ? (0xFF) :(0x00)) ) << 24) |
				                                            ((LHE7909_adc_data[3]&0xFF)<<16)|
				                                            ((LHE7909_adc_data[4]&0xFF)<<8)|
				                                            ((LHE7909_adc_data[5]&0xFF)<<0));
				ch2_hex = (int32_t)(((((LHE7909_adc_data[6] & 0x80) ? (0xFF) :(0x00)) ) << 24) |
				                                            ((LHE7909_adc_data[6]&0xFF)<<16)|
				                                            ((LHE7909_adc_data[7]&0xFF)<<8)|
				                                            ((LHE7909_adc_data[8]&0xFF)<<0));						
				ch3_hex = (int32_t)(((((LHE7909_adc_data[9] & 0x80) ? (0xFF) :(0x00)) ) << 24) |
				                                            ((LHE7909_adc_data[9]&0xFF)<<16)|
				                                            ((LHE7909_adc_data[10]&0xFF)<<8)|
				                                            ((LHE7909_adc_data[11]&0xFF)<<0));	
			 ch4_hex = (int32_t)(((((LHE7909_adc_data[12] & 0x80) ? (0xFF) :(0x00)) ) << 24) |
				                                            ((LHE7909_adc_data[12]&0xFF)<<16)|
				                                            ((LHE7909_adc_data[13]&0xFF)<<8)|
				                                            ((LHE7909_adc_data[14]&0xFF)<<0));	
			 ch5_hex = (int32_t)(((((LHE7909_adc_data[15] & 0x80) ? (0xFF) :(0x00)) ) << 24) |
				                                            ((LHE7909_adc_data[15]&0xFF)<<16)|
				                                            ((LHE7909_adc_data[16]&0xFF)<<8)|
				                                            ((LHE7909_adc_data[17]&0xFF)<<0));	
				ch6_hex = (int32_t)(((((LHE7909_adc_data[18] & 0x80) ? (0xFF) :(0x00)) ) << 24) |
				                                            ((LHE7909_adc_data[18]&0xFF)<<16)|
				                                            ((LHE7909_adc_data[19]&0xFF)<<8)|
				                                            ((LHE7909_adc_data[20]&0xFF)<<0));	
				ch7_hex = (int32_t)(((((LHE7909_adc_data[21] & 0x80) ? (0xFF) :(0x00)) ) << 24) |
				                                            ((LHE7909_adc_data[21]&0xFF)<<16)|
				                                            ((LHE7909_adc_data[22]&0xFF)<<8)|
				                                            ((LHE7909_adc_data[23]&0xFF)<<0));	
				ch8_hex = (int32_t)(((((LHE7909_adc_data[24] & 0x80) ? (0xFF) :(0x00)) ) << 24) |
				                                            ((LHE7909_adc_data[24]&0xFF)<<16)|
				                                            ((LHE7909_adc_data[25]&0xFF)<<8)|
				                                            ((LHE7909_adc_data[26]&0xFF)<<0));
				ch1_hex_buf_size[ch1_hex_buf_count++]	=	ch1_hex;

			//  HAL_UART_Transmit(&huart2, (uint8_t *)&LHE7909_adc_data[0], 3, 200);				 
				ch1_voltage =ADC_DataToMiniVolt(ch1_hex);
				//ch2_voltage =ADC_DataToMiniVolt(ch2_hex);
				//ch3_voltage =ADC_DataToMiniVolt(ch3_hex);
				//ch4_voltage =ADC_DataToMiniVolt(ch4_hex);
				//ch5_voltage =ADC_DataToMiniVolt(ch5_hex);
				//ch6_voltage =ADC_DataToMiniVolt(ch6_hex);
				//ch7_voltage =ADC_DataToMiniVolt(ch7_hex);
				//ch8_voltage =ADC_DataToMiniVolt(ch8_hex);//LEAD1,LEAD2,V2,V3,V4,V5,V1,V6
		// printf("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",ch2_voltage,ch3_voltage,ch4_voltage,ch5_voltage,ch6_voltage,ch7_voltage,ch8_voltage,ch1_voltage);
		 printf("%.3f\r\n",ch1_voltage);
		    if(ch1_hex_buf_count>=1000)
	      {
		      //LHE7909_SDATAC();
	          //LHE7909_STOP();
			//or
			
			  HAL_GPIO_WritePin(GPIOB, LHE7909_START_Pin, GPIO_PIN_RESET);//set START PIN is 0
					adc_cov_finish=1;
					ch1_hex_buf_count=0;
					
	      }
		}
		HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);
   // HAL_GPIO_TogglePin(LHE7909_GPIO1_GPIO_Port, LHE7909_GPIO1_Pin);
}
float  ADC_DataToMiniVolt(int32_t data)
{
	float mv;
	  mv = data*2.0*2400.0/((1<<24));
	return mv;
}












