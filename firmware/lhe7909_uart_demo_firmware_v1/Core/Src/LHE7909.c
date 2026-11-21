#include "LHE7909.h"

#include "main.h"


#include "stdio.h"
#include <stdbool.h>
extern SPI_HandleTypeDef hspi2;

#define GAIN 0X00//0X10:1 0X20:2 0x00:6 0x70:24  0x60:12 0x80:power down

#define HR_MODE 0x80
#define LP_MODE 0x00

uint8_t counter;
float VrefVolt;

uint8_t ch1_gain,ch2_gain,ch3_gain,ch4_gain,ch5_gain,ch6_gain,ch7_gain,ch8_gain;

/* 回读校验：写后立即读回并与期望值比对，只报告、不改变流程。
 * 用于 WCT1/WCT2/LOFF 这类“写入值是否被硅片保持”需要实测确认的寄存器。 */
static uint8_t LHE7909_check_reg(uint8_t address, uint8_t expect, const char *name)
{
	uint8_t value = LHE7909_read_register(address);

	if (value == expect)
	{
		printf("%s: 0x%x OK\r\n", name, value);
	}
	else
	{
		printf("%s: 0x%x (expect 0x%x) MISMATCH\r\n", name, value, expect);
	}
	return value;
}

void LHE7909_init(uint8_t DR)
{
	uint8_t x1=0;
	
    HAL_GPIO_WritePin(GPIOA, LHE7909_DAISY_IN_Pin, GPIO_PIN_RESET);
  
    HAL_GPIO_WritePin(GPIOB, LHE7909_nPWDN_Pin, GPIO_PIN_SET);
  
		HAL_GPIO_WritePin(GPIOB, LHE7909_nRESET_Pin, GPIO_PIN_SET);
	
		HAL_GPIO_WritePin(GPIOB, LHE7909_START_Pin, GPIO_PIN_RESET);

     
	/* 上电等待 tPOR：手册给 32~100ms，取上限 ≥100ms，同时覆盖 VCAP1 充到
	 * 1.1V 的时间（VCAP1 用 100µF 时更久），宁长勿短。 */
	HAL_Delay(100);

	LHE7909_RESET();//先发RESET命令
	/* 复位后至少等 18 tCLK（2.048MHz 下约 8.8µs）再发任何命令，这里给 20µs */
	LHE7909_delay_us(20);

	LHE7909_SDATAC();
		
	DUT_WTREG(0xac,0x01,0x82);//USERKEY UNLOCK;

	/* ID 校验：DEV_ID[7:5] 必须为 101b。bit4 手册两节不一致（8 通道可能回读
	 * A0h 或 B0h），故只校验高 3 位；不符时报警并停止配置，不带病继续。 */
	x1 = LHE7909_read_register(ID_ADDRESS);
	printf("ID_ADDRESS: %x\r\n", x1);
	if ((x1 & 0xE0) != 0xA0)
	{
		printf("ID check FAIL: DEV_ID[7:5]=%d, expect 5(101b)\r\n", x1 >> 5);
		return;
	}
	printf("ID check OK\r\n");

	counter=0;

	/* 手册无 HR 位（bit7 保留恒 0、bit4 只读恒 1）；DR[2:0]=101 才是 500 SPS。
	 * bit6=1 保持多重回读模式。实测写 C6h 会读回 56h（DR=110，即 250 SPS），不能沿用。 */
	LHE7909_write_register(CONFIG1_ADDRESS,0x55);
	x1=LHE7909_read_register(CONFIG1_ADDRESS);
	printf("CONFIG1_ADDRESS: %x\r\n",x1);

	
	/* 内部测试信号源：INT_TEST=1、CAL_FREQ=11（直流）、CAL_AMP=0（1 倍），bit7:6=11。
	 * 测试信号幅度 = (VREFP-VREFN)/1000 ≈ 2.4mV（输入端起，再乘 PGA 增益送 ADC）。 */
	LHE7909_write_register(CONFIG2_ADDRESS,0xD3);
	x1=LHE7909_read_register(CONFIG2_ADDRESS);
	printf("CONFIG2_ADDRESS: %x\r\n",x1);
 
 
 
 /* 0x9C：闭环测试 BIAS_AMP 负反馈与偏置回读：
	 * PD_REFBUF=1(bit7, 2.4V内部基准缓冲开)、ILEAD_EN=0(bit6)、VREF_4V=0(bit5)、
	 * BIAS_MEAS=1(bit4, 开启偏置回读)、BIASREF_INT=1(bit3, 内部(AVDD+AVSS)/2中点)、
	 * PD_BIAS=1(bit2, BIAS_AMP使能)。 */
 LHE7909_write_register(CONFIG3_ADDRESS,0x9C);
	LHE7909_check_reg(CONFIG3_ADDRESS, 0x9C, "CONFIG3_ADDRESS");
	VrefVolt=2400.0;//参考电压为2400mV

	/* 内部基准建立时间约 150ms（电气特性表）：写完 CONFIG3 使能基准缓冲后，
	 * 先等够再写其余寄存器、更不要采数。改用外部基准时可删掉本次等待。 */
	HAL_Delay(150);
	
  /* 导联脱落检测已整体关闭（CONFIG4 比较器关、LOFF_SENSP/N 全 0），这里只清掉
	 * bit4 保留位、不使能激励电流。实测写 13h 会读回 03h（bit4 只读 0），故直接写 03h。 */
  LHE7909_write_register(LOFF_ADDRESS,0x03);
	LHE7909_check_reg(LOFF_ADDRESS, 0x03, "LOFF_ADDRESS");
	
   //============CHnSET Begin==============================================
   /* 偏置闭环与回读测试：CH1 设为 Gain=1、正常电极输入(MUX=000，编码 0x10)；
	 * CH2~CH7 保持 Gain=6、内部短接(0x01)；CH8 设为 Gain=1、偏置测量模式(MUX=010，编码 0x12)。 */
   LHE7909_write_register(CH1SET_ADDRESS,0x10);
   LHE7909_check_reg(CH1SET_ADDRESS, 0x10, "CH1SET_ADDRESS");
   LHE7909_write_register(CH2SET_ADDRESS,0X01);
   LHE7909_write_register(CH3SET_ADDRESS,0X01);
   LHE7909_write_register(CH4SET_ADDRESS,0X01);
   LHE7909_write_register(CH5SET_ADDRESS,0X01);
   LHE7909_write_register(CH6SET_ADDRESS,0X01);
   LHE7909_write_register(CH7SET_ADDRESS,0X01);
   LHE7909_write_register(CH8SET_ADDRESS,0x12);
   LHE7909_check_reg(CH8SET_ADDRESS, 0x12, "CH8SET_ADDRESS");
	 ch1_gain=1;
	 ch2_gain=6;
	 ch3_gain=6;
	 ch4_gain=6;
	 ch5_gain=6;
	 ch6_gain=6;
	 ch7_gain=6;
	 ch8_gain=1;
	
	LHE7909_write_register(RLD_SENSP_ADDRESS,0x00);//RLD_SENSP=0x00，断开 CH1~CH8 P 端偏置求和选择
	LHE7909_check_reg(RLD_SENSP_ADDRESS, 0x00, "RLD_SENSP_ADDRESS");
	
	LHE7909_write_register(RLD_SENSN_ADDRESS,0x00);//RLD_SENSN=0x00，断开 CH1~CH8 N 端偏置求和选择
	LHE7909_check_reg(RLD_SENSN_ADDRESS, 0x00, "RLD_SENSN_ADDRESS");


	LHE7909_write_register(LOFF_SENSP_ADDRESS,0x00);//关闭 IN1P~IN8P 导联脱落检测（未启用激励电流）
	x1=LHE7909_read_register(LOFF_SENSP_ADDRESS);
	printf("LOFF_SENSP_ADDRESS: %x\r\n",x1);


	LHE7909_write_register(LOFF_SENSN_ADDRESS,0x00);//关闭 IN1N~IN8N 导联脱落检测（未启用激励电流）
	x1=LHE7909_read_register(LOFF_SENSN_ADDRESS);
	printf("LOFF_SENSN_ADDRESS: %x\r\n",x1);

	LHE7909_write_register(LOFF_FLIP_ADDRESS,0x00);//
	x1=LHE7909_read_register(LOFF_FLIP_ADDRESS);
	printf("LOFF_FLIP_ADDRESS: %x\r\n",x1);
	
	x1=LHE7909_read_register(LOFF_STATP_ADDRESS);
	printf("LOFF_STATP_ADDRESS: %x\r\n",x1);
	
	x1=LHE7909_read_register(LOFF_STATN_ADDRESS);
	printf("LOFF_STATN_ADDRESS: %x\r\n",x1);
	
	
	x1=LHE7909_read_register(GPIO_ADDRESS);
	printf("GPIO_ADDRESS: %x\r\n",x1);
	
	x1=LHE7909_read_register(PACE_ADDRESS);
	printf("PACE_ADDRESS: %x\r\n",x1);
	
	
	LHE7909_write_register(CONFIG4_ADDRESS,0x01);//连续转换；导联脱落比较器关闭；bit0 固定写 1
	x1=LHE7909_read_register(CONFIG4_ADDRESS);
	printf("CONFIG4_ADDRESS: %x\r\n",x1);



	/* WCT1/WCT2(18h/19h)：手册未定义这两个地址，实测写入也不保持
	 * （0Bh→03h、D4h→14h），故不再配置 WCT，胸导不使用 WCT 作为参考。 */

		
	x1=DUT_RTREG(LOFF_CFG_HIGH_ADDR,LOFF_CFG_LOW_ADDR);
	printf("LOFF_CFG_ADDRESS: %x\r\n",x1);
	
	x1=DUT_RTREG(LOFF_FREQ_HIGH_ADDR,LOFF_FREQ_LOW_ADDR);
	printf("LOFF_FREQ_ADDRESS: %x\r\n",x1);

	#if 1//adc  not work
	LHE7909_START();
	//LHE7909_RDATAC();
	#endif
}


uint8_t LHE7909_read_register(uint8_t address)
{
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	LHE7909_delay_us(5);
	uint8_t tx_data_spi[3];
	uint8_t rx_data_spi[7];
	
	tx_data_spi[0] = LHE7909_CMD_RREG+address;	//
	tx_data_spi[1] = 0x00;	//Read 1 registers starting at address 
	HAL_SPI_Transmit(&hspi2,  &tx_data_spi[0], 1, 1000);	
	LHE7909_delay_us(5);
	HAL_SPI_Transmit(&hspi2,  &tx_data_spi[1], 1, 1000);
	LHE7909_delay_us(5);
	HAL_SPI_Receive(&hspi2,  rx_data_spi, 1, 1000);
	LHE7909_delay_us(5);
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);
  return rx_data_spi[0];

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
//=======写大于或等于地址为0x40的寄存器
void DUT_WTREG(uint8_t _value, uint8_t opcode1,uint8_t opcode2)
{
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);

	uint8_t wtreg_data[4];
	uint8_t sp1_rx,i;

    wtreg_data[0]=  opcode1 | 0xA0;
	wtreg_data[1]=  opcode2;
	wtreg_data[2]=  0x00;//write one reg.
	wtreg_data[3] = _value;//
	
  for(i=0;i<4;i++)
		{
			HAL_SPI_TransmitReceive(&hspi2, &wtreg_data[i], &sp1_rx, 1,1000);
			LHE7909_delay_us(5);
		}
	LHE7909_delay_us(5);
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);
}

//读取大于等于地址为0x40的寄存器:opcode1地址高字节 opcode2地址低字节；并将读出的数据通过串口返回。
uint8_t DUT_RTREG(uint8_t opcode1,uint16_t opcode2)
{
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);

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
	
	for(i=0;i<4;i++)
		{
			HAL_SPI_TransmitReceive(&hspi2, &spi1_tx[i], &spi1_rx[i], 1,1000);
			LHE7909_delay_us(5);
		}
	LHE7909_delay_us(5);
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);

	return spi1_rx[3];
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





/* 微秒级延时：用 DWT 周期计数器实现，替代原来靠 __NOP 空循环“凑”的写法，
 * 保证 18 tCLK 复位等待、tSDECODE 字节间隔等最小时间有真实裕量。
 * CYCCNT 随 CPU 时钟递增，cycles_per_us 由 HAL_RCC_GetSysClockFreq() 换算。 */
void LHE7909_delay_us(uint32_t time)
{
	static uint32_t cycles_per_us = 0;
	uint32_t start;

	if (cycles_per_us == 0U)
	{
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0U;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
		cycles_per_us = HAL_RCC_GetSysClockFreq() / 1000000U;
		if (cycles_per_us == 0U)
		{
			cycles_per_us = 1U;
		}
	}

	start = DWT->CYCCNT;
	while ((DWT->CYCCNT - start) < (time * cycles_per_us))
	{
	}
}

/* 标准 24 位二进制补码符号扩展为 32 位有符号整数（无符号拼接避免有符号左移 UB，不减 0x800000） */
static int32_t LHE7909_sign_extend_24bit(uint8_t b2, uint8_t b1, uint8_t b0)
{
	uint32_t raw = ((uint32_t)b2 << 16) | ((uint32_t)b1 << 8) | (uint32_t)b0;
	if ((raw & 0x00800000U) != 0U)
	{
		raw |= 0xFF000000U;
	}
	return (int32_t)raw;
}

extern uint8_t LHE7909_adc_data[27];
int32_t ch1_hex,ch2_hex,ch3_hex,ch4_hex,ch5_hex,ch6_hex,ch7_hex,ch8_hex;
float   ch1_voltage,ch2_voltage,ch3_voltage,ch4_voltage,ch5_voltage,ch6_voltage,ch7_voltage,ch8_voltage;

/* ===== BIAS_AMP 闭环 A/B/A′ 对比测试：DRDY 中断侧（命令侧实现见文末 BIAS 测试模块） =====
 * 测试模式下中断不再逐帧 printf（每帧约 450µs，必丢点，且会与定长采集/CSV 输出混杂）；
 * 采集请求有效时把 CH1/CH8 原始码存入下面的静态缓冲，采满置位后由主循环统一输出。 */
#if (ECG_UART_BIN_MODE == 0)
volatile uint8_t  g_bias_test_active = 0;            /* 1=已进入 BIAS 测试模式（DRDY 中断静默） */
static volatile uint8_t  bias_capture_request = 0;   /* 主循环→中断：开始定长采集 */
static volatile uint8_t  bias_capture_done = 0;      /* 中断→主循环：本次采集已结束 */
static volatile uint16_t bias_capture_count = 0;     /* 已存入缓冲的样本数 */
static volatile uint32_t bias_last_sample_tick = 0;  /* 最近一个样本的时刻（停滞超时用） */
static int32_t bias_ch1_buf[BIAS_TEST_N_POINTS];     /* CH1 原始码缓冲（2000 点） */
static int32_t bias_ch8_buf[BIAS_TEST_N_POINTS];     /* CH8 原始码缓冲（2000 点） */

/* DRDY 中断上下文调用：只做拷贝与计数，绝不 printf。采满 2000 点置 bias_capture_done，
 * 由主循环 LHE7909_bias_task() 检测并输出 CSV。 */
static void LHE7909_bias_capture_store_isr(void)
{
	if ((bias_capture_request != 0U) && (bias_capture_count < BIAS_TEST_N_POINTS))
	{
		bias_ch1_buf[bias_capture_count] = ch1_hex;
		bias_ch8_buf[bias_capture_count] = ch8_hex;
		bias_capture_count++;
		bias_last_sample_tick = HAL_GetTick();
		if (bias_capture_count >= BIAS_TEST_N_POINTS)
		{
			bias_capture_request = 0U;
			bias_capture_done = 1U;
		}
	}
}
#endif /* (ECG_UART_BIN_MODE == 0) */

void LHE7909_get_adc_data(void)
{

    uint8_t x1;
		
	
	HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_RESET);
	
	
	uint8_t tx_data_spi;
	tx_data_spi = LHE7909_CMD_RDATA;
  HAL_SPI_Transmit(&hspi2, &tx_data_spi, 1, 1000);
	
    HAL_SPI_Receive(&hspi2,LHE7909_adc_data,27,1000);
    if((LHE7909_adc_data[0]&0xC0)==0xC0)
		{
		       #if 1//打印ECG通道数据
				ch1_hex = LHE7909_sign_extend_24bit(LHE7909_adc_data[3],  LHE7909_adc_data[4],  LHE7909_adc_data[5]);
				ch2_hex = LHE7909_sign_extend_24bit(LHE7909_adc_data[6],  LHE7909_adc_data[7],  LHE7909_adc_data[8]);
				ch3_hex = LHE7909_sign_extend_24bit(LHE7909_adc_data[9],  LHE7909_adc_data[10], LHE7909_adc_data[11]);
				ch4_hex = LHE7909_sign_extend_24bit(LHE7909_adc_data[12], LHE7909_adc_data[13], LHE7909_adc_data[14]);
				ch5_hex = LHE7909_sign_extend_24bit(LHE7909_adc_data[15], LHE7909_adc_data[16], LHE7909_adc_data[17]);
				ch6_hex = LHE7909_sign_extend_24bit(LHE7909_adc_data[18], LHE7909_adc_data[19], LHE7909_adc_data[20]);
				ch7_hex = LHE7909_sign_extend_24bit(LHE7909_adc_data[21], LHE7909_adc_data[22], LHE7909_adc_data[23]);
				ch8_hex = LHE7909_sign_extend_24bit(LHE7909_adc_data[24], LHE7909_adc_data[25], LHE7909_adc_data[26]);
				ch1_voltage =ADC_DataToMiniVolt(ch1_hex,ch1_gain);
				ch2_voltage =ADC_DataToMiniVolt(ch2_hex,ch2_gain);
				ch3_voltage =ADC_DataToMiniVolt(ch3_hex,ch3_gain);
				ch4_voltage =ADC_DataToMiniVolt(ch4_hex,ch4_gain);
				ch5_voltage =ADC_DataToMiniVolt(ch5_hex,ch5_gain);
				ch6_voltage =ADC_DataToMiniVolt(ch6_hex,ch6_gain);
				ch7_voltage =ADC_DataToMiniVolt(ch7_hex,ch7_gain);
				ch8_voltage =ADC_DataToMiniVolt(ch8_hex,ch8_gain);//LEAD1,LEAD2,V2,V3,V4,V5,V1,V6
#if ECG_UART_BIN_MODE
			LHE7909_send_frame();
			/* 状态字前 3 字节(原始 hex)紧随数据帧上传，上位机解析导联脱落状态 */
			LHE7909_send_loff_status();
#else
		 /* BIAS 测试模式：不做逐帧调试打印（避免与定长采集/CSV 输出混杂并丢点）；
		  * 采集请求有效时把 CH1/CH8 原始码存入静态缓冲，其余样本静默丢弃 */
		 if (g_bias_test_active)
		 {
			 LHE7909_bias_capture_store_isr();
		 }
		 else
		 {
			 /* 调试输出：CH1~CH8 按顺序输出电压(mV，已除 PGA 增益，折算到输入端)
			  * 及 24 位原始采集码(0xXXXXXX) */
			 printf("CH1:%.3f(0x%06lX),CH2:%.3f(0x%06lX),CH3:%.3f(0x%06lX),CH4:%.3f(0x%06lX),CH5:%.3f(0x%06lX),CH6:%.3f(0x%06lX),CH7:%.3f(0x%06lX),CH8:%.3f(0x%06lX)\r\n",
			        ch1_voltage, (unsigned long)((uint32_t)ch1_hex & 0x00FFFFFFUL),
			        ch2_voltage, (unsigned long)((uint32_t)ch2_hex & 0x00FFFFFFUL),
			        ch3_voltage, (unsigned long)((uint32_t)ch3_hex & 0x00FFFFFFUL),
			        ch4_voltage, (unsigned long)((uint32_t)ch4_hex & 0x00FFFFFFUL),
			        ch5_voltage, (unsigned long)((uint32_t)ch5_hex & 0x00FFFFFFUL),
			        ch6_voltage, (unsigned long)((uint32_t)ch6_hex & 0x00FFFFFFUL),
			        ch7_voltage, (unsigned long)((uint32_t)ch7_hex & 0x00FFFFFFUL),
			        ch8_voltage, (unsigned long)((uint32_t)ch8_hex & 0x00FFFFFFUL));
		 }
#endif
		 #endif
		// printf("%.3f\r\n",ch1_voltage);
             
		    //==========
		    #if 0//只打印状态字
			
			if(counter  >= 10)
			{
					counter = 0;
		 		x1=LHE7909_adc_data[0];
				printf("stauts 1: %x\r\n",x1);

				x1=LHE7909_adc_data[1];
				printf("stauts 2: %x\r\n",x1);
	
				x1=LHE7909_adc_data[2];
				printf("stauts 3: %x\r\n",x1);

			}
			else
				{
				 	 counter ++;
				}
			#endif
			//V1---IN8P V2---IN4P V3---IN5P V4--IN6P    V5---IN7P   V6---IN1P LA--IN2P  LL--IN3P
			//RA---IN3N 
			//===========
		}
		HAL_GPIO_WritePin(LHE7909_SPI2_CS_GPIO_Port, LHE7909_SPI2_CS_Pin, GPIO_PIN_SET);
    HAL_GPIO_TogglePin(LHE7909_GPIO1_GPIO_Port, LHE7909_GPIO1_Pin);
}
float  ADC_DataToMiniVolt(int32_t data,uint8_t gain)
{
	float mv;
	if (gain == 0U)
	{
		gain = 1U;   /* 增益档 0x80(掉电)对应的 0 值按 1 处理，避免除 0 */
	}
	/* 修正：mV 必须除以 PGA 增益折算到输入端（此前未除，输出的是 ADC 输出端电压）。
	 * gain 由各通道的 chX_gain 传入，与各自 CHnSET 配置保持一致，不写死。 */
	mv = data*2.0*VrefVolt/((1<<24))/gain;
	return mv;
}

/* ============ 上位机二进制帧发送 ============ */
extern UART_HandleTypeDef huart2;

/* 已上传的数据帧计数（DRDY 中断里递增，主循环指示灯任务读取） */
volatile uint32_t g_ecg_tx_frame_count = 0;

void LHE7909_send_frame(void)
{
	/* 帧格式 (37 字节):
	 *   [0]     0xAA            帧头1
	 *   [1]     0x55            帧头2
	 *   [2]     0x08            通道数 (=8)
	 *   [3]     帧序号          0~255 循环递增
	 *   [4..35] CH1~CH8 各 4 字节 float mV (IEEE754 小端序)
	 *   [36]    校验和 = 字节[0..35] 累加和的低 8 位
	 */
	static uint8_t frame[ECG_FRAME_LEN];
	static uint8_t seq = 0;
	uint8_t i;
	uint8_t ch;
	uint8_t sum = 0;

	frame[0] = ECG_FRAME_HEAD1;
	frame[1] = ECG_FRAME_HEAD2;
	frame[2] = ECG_FRAME_CH_COUNT;
	frame[ECG_FRAME_SEQ_OFFSET] = seq;   /* 帧序号递增 */
	seq++;

	{
		const float *voltage[ECG_FRAME_CH_COUNT] = {
			&ch1_voltage, &ch2_voltage, &ch3_voltage, &ch4_voltage,
			&ch5_voltage, &ch6_voltage, &ch7_voltage, &ch8_voltage
		};
		union {
			float f;
			uint8_t b[4];
		} conv;

		for (ch = 0; ch < ECG_FRAME_CH_COUNT; ch++)
		{
			conv.f = *voltage[ch];
			frame[ECG_FRAME_DATA_OFFSET + ch * ECG_FRAME_CH_BYTES + 0] = conv.b[0];
			frame[ECG_FRAME_DATA_OFFSET + ch * ECG_FRAME_CH_BYTES + 1] = conv.b[1];
			frame[ECG_FRAME_DATA_OFFSET + ch * ECG_FRAME_CH_BYTES + 2] = conv.b[2];
			frame[ECG_FRAME_DATA_OFFSET + ch * ECG_FRAME_CH_BYTES + 3] = conv.b[3];
		}
	}

	for (i = 0; i < (ECG_FRAME_LEN - 1); i++)
	{
		sum += frame[i];
	}
	frame[ECG_FRAME_LEN - 1] = sum;

	HAL_UART_Transmit(&huart2, frame, ECG_FRAME_LEN, 1000);

	/* 上传成功计数：主循环的指示灯任务据此判断“当前是否在上传数据”
	 * （只做 +1，中断上下文里也是原子的 32bit 读改写，无需关中断） */
	g_ecg_tx_frame_count++;
}

/* ============ 运行指示灯（只在上传数据时闪烁） ============ */
/* g_ecg_tx_frame_count 由 LHE7909_send_frame() 在 DRDY 中断里每发一帧 +1；
 * 主循环调用 LHE7909_led_task() 观察该计数：
 *   - 最近 LED_UPLOAD_ALIVE_MS 内计数有变化 → 正在上传 → 按 LED_BLINK_HALF_MS 翻转 LED；
 *   - 超过该时长没有新帧（暂停/停止采集）→ 熄灭 LED。
 * 用“时间”而不是“帧数分频”控制闪烁，切换 500Hz~16K 采样率时闪烁节奏不变。 */
void LHE7909_led_task(void)
{
	static uint32_t last_count = 0;          /* 上次观察到的上传帧计数 */
	static uint32_t last_change_tick = 0;    /* 最近一次“有新帧上传”的时刻 */
	static uint32_t last_toggle_tick = 0;    /* 上次翻转 LED 的时刻 */
	static uint8_t  has_uploaded = 0;        /* 上电后是否已经上传过数据 */
	static uint8_t  led_is_on = 0;           /* LED 当前逻辑状态（1=亮） */
	uint32_t now = HAL_GetTick();
	uint32_t count = g_ecg_tx_frame_count;

	/* 计数变化 → 说明刚上传过数据帧 */
	if (count != last_count)
	{
		last_count = count;
		last_change_tick = now;
		has_uploaded = 1;
	}

	if (has_uploaded &&
	    (uint32_t)(now - last_change_tick) <= LED_UPLOAD_ALIVE_MS)
	{
		/* 正在上传：亮/灭各 LED_BLINK_HALF_MS，人眼可分辨 */
		if ((uint32_t)(now - last_toggle_tick) >= LED_BLINK_HALF_MS)
		{
			last_toggle_tick = now;
			led_is_on = (uint8_t)(!led_is_on);
			HAL_GPIO_TogglePin(MCU_LED_RUN_GPIO_Port, MCU_LED_RUN_Pin);
		}
	}
	else if (led_is_on)
	{
		/* 停止上传：主动熄灭，避免恰好停在“亮”的状态 */
		led_is_on = 0;
		HAL_GPIO_WritePin(MCU_LED_RUN_GPIO_Port, MCU_LED_RUN_Pin,
		                  LED_OFF_LEVEL);
		last_toggle_tick = now;
	}
}

/* ============ LOFF 状态字帧上传（上位机解析导联脱落状态） ============ */
/* 目的：把 LHE7909_get_adc_data() 中 HAL_SPI_Receive 读回缓冲区的前 3 字节(ADC 状态字)
 *       原样上传；本侧不做位解析，LOFF_STATP/LOFF_STATN 的提取由上位机完成。
 * 帧格式(6 字节，定义见 LHE7909.h)：[0xAA][0xDE][状态字节0][状态字节1][状态字节2][校验和]，
 *       校验和 = 前 5 字节累加和的低 8 位。
 * 输入：LHE7909_adc_data[0..2]；来源：Core/Src/main.c 的 uint8_t LHE7909_adc_data[27]，
 *       由本文件 LHE7909_get_adc_data() 经 HAL_SPI_Receive 填满(上方已有 extern 声明)。
 * 副作用：在 DRDY(EXTI) 中断上下文内阻塞发送 6 字节(1.5Mbps 约 40us)；发送节拍由
 *       ECG_LOFF_SEND_EVERY_N 控制(默认 1 = 每次采样都发；调用点在 ECG_UART_BIN_MODE
 *       分支内，非二进制模式不编译)。
 * 错误处理：HAL_UART_Transmit 超时/失败时丢弃本帧，不重试(中断内不做恢复，
 *       上位机收不到该帧时按“状态未更新”处理)。 */
void LHE7909_send_loff_status(void)
{
	static uint8_t loff_skip = 0;    /* 剩余跳过帧数，用于 ECG_LOFF_SEND_EVERY_N 节拍 */
	uint8_t frame[ECG_LOFF_FRAME_LEN];
	uint8_t i;
	uint8_t sum = 0;

	if (loff_skip > 0)
	{
		loff_skip--;                 /* 未到发送节拍，本次不发 */
		return;
	}
	loff_skip = (uint8_t)(ECG_LOFF_SEND_EVERY_N - 1);   /* 重新装载节拍 */

	frame[0] = ECG_CMD_HEAD1;        /* 0xAA 帧头1 */
	frame[1] = ECG_LOFF_HEAD2;       /* 0xDE 帧头2(状态字帧) */
	frame[2] = LHE7909_adc_data[0];  /* 状态字节0：高 4bit=1100 */
	frame[3] = LHE7909_adc_data[1];  /* 状态字节1 */
	frame[4] = LHE7909_adc_data[2];  /* 状态字节2 */
	for (i = 0; i < (ECG_LOFF_FRAME_LEN - 1); i++)
	{
		sum = (uint8_t)(sum + frame[i]);
	}
	frame[ECG_LOFF_FRAME_LEN - 1] = sum;   /* 校验和 */

	(void)HAL_UART_Transmit(&huart2, frame, ECG_LOFF_FRAME_LEN, 1000);
}

/* ============ 采样率控制（上位机命令） ============ */
/* 设置 LHE7909 采样率：
 * 关闭中断 -> 双重SDATAC退出连续读模式 -> 写CONFIG1 ->
 * 关中断窗口内发确认帧 -> START确保转换继续 -> 开中断。返回新 DR。
 *
 * 注意：本固件的采集中断(LHE7909_get_adc_data)用 RDATA 单次读命令取数，
 * 芯片必须保持在命令模式；这里绝不能调用 RDATAC 进入连续读模式，否则
 * DRDY 中断里再发 RDATA 会与连续输出冲突，导致数据帧停止（必须复位才能恢复）。 */
uint8_t LHE7909_set_dr(uint8_t dr)
{
#if ECG_UART_BIN_MODE
	uint8_t readback = 0;
	volatile uint16_t wait = 0;
	uint8_t i;

	/* 禁止所有中断，避免 DRDY(EXTI0) 中的 SPI 读取与本次 SPI 写并发冲突、
	 * UART 数据帧插队确认帧。 */
	__disable_irq();

	/* ② 双重 SDATAC：停止连续输出模式。重复两次提高成功率，
	 *    至少一次在芯片空闲窗口内被识别。 */
	for (i = 0; i < 2; i++)
	{
		LHE7909_SDATAC();
	}

	/* ④ SPI 完全可控后写采样率 */
	LHE7909_write_register(CONFIG1_ADDRESS, dr);


	/* 在关中断窗口内直接发送确认帧，确保不被 DRDY 数据帧中断插队：
	 * [0xAA][0xDB][cmd][data][sum]{0x0D} */
	{
		uint8_t ack[ECG_CMD_LEN];
		ack[0] = ECG_CMD_HEAD1;
		ack[1] = ECG_CMD_HEAD2_MCU2PC;
		ack[2] = ECG_CMD_SET_DR;
		ack[3] = dr;
		ack[4] = (uint8_t)(ack[0] + ack[1] + ack[2] + ack[3]);
		ack[5] = 0x0D;
		HAL_UART_Transmit(&huart2, ack, ECG_CMD_LEN, 20);
	}
	
	/* ⑥ 保持命令模式：本固件用 DRDY 中断 + RDATA 单次读采集，
	 *    只需确保转换在运行，不能进入 RDATAC 连续读模式 */
	LHE7909_START();
	LHE7909_delay_us(20);

	__enable_irq();
#endif
	return dr;
}

/* 读取关键寄存器并通过 UART 返回（仅限暂停/SDATAC 状态下调用）：
 * 帧格式: [0xAA][0xDD][count][reg1][val1]...[regN][valN][sum]
 * 关中断窗口内逐寄存器读取，避免与 DRDY 中断并发 SPI。 */
void LHE7909_read_config(void)
{
#if ECG_UART_BIN_MODE
	uint8_t buf[58];                       /* 帧头2 + count + 27*2 + sum */
	const uint8_t regs[27] = {
		ID_ADDRESS,      CONFIG1_ADDRESS,  CONFIG2_ADDRESS,  CONFIG3_ADDRESS,
		LOFF_ADDRESS,    CH1SET_ADDRESS,   CH2SET_ADDRESS,   CH3SET_ADDRESS,
		CH4SET_ADDRESS,  CH5SET_ADDRESS,   CH6SET_ADDRESS,   CH7SET_ADDRESS,
		CH8SET_ADDRESS,  RLD_SENSP_ADDRESS,RLD_SENSN_ADDRESS,LOFF_SENSP_ADDRESS,
		LOFF_SENSN_ADDRESS,LOFF_FLIP_ADDRESS,LOFF_STATP_ADDRESS,LOFF_STATN_ADDRESS,
		GPIO_ADDRESS,    CONFIG4_ADDRESS,  WCT1_ADDRESS,     WCT2_ADDRESS,
		DIG_ACLOFF_ENABLE_ADDRESS,LOFF_CFG_ADDRESS,LOFF_FREQ_ADDRESS
	};
	uint8_t i, sum = 0, idx = 0;

	__disable_irq();

	/* 保险：确保已退出连续输出模式，读寄存器才可靠 */
	LHE7909_SDATAC();

	buf[idx] = ECG_CMD_HEAD1;   sum = (uint8_t)(sum + buf[idx]); idx++;
	buf[idx] = ECG_CFG_HEAD2;   sum = (uint8_t)(sum + buf[idx]); idx++;
	buf[idx] = sizeof(regs);    sum = (uint8_t)(sum + buf[idx]); idx++;

	for (i = 0; i < sizeof(regs); i++)
	{
		buf[idx] = regs[i];
		sum = (uint8_t)(sum + buf[idx]); idx++;
		/* 扩展寄存器(>=0x40)读用 DUT_RTREG，普通寄存器用 LHE7909_read_register */
		if (regs[i] >= 0x40)
		{
			buf[idx] = DUT_RTREG(0x00, regs[i]);
		}
		else
		{
			buf[idx] = LHE7909_read_register(regs[i]);
		}
		sum = (uint8_t)(sum + buf[idx]); idx++;
	}
	buf[idx++] = sum;

	HAL_UART_Transmit(&huart2, buf, idx, 100);
	__enable_irq();
#endif
}

/* 上位机命令: 写单个寄存器（仅暂停/SDATAC 状态下调用）
 * 双重 SDATAC 保险退出连续输出 → SPI 写寄存器 → 发确认帧。
 * ack: [0xAA][0xDB][0x04][addr][sum]{0x0D} */
void LHE7909_write_reg_cmd(uint8_t addr, uint8_t value)
{
#if ECG_UART_BIN_MODE
	uint8_t i;
	uint8_t ack[ECG_CMD_LEN];

	__disable_irq();

	/* 双重 SDATAC：退出连续输出模式后写寄存器才可靠 */
	for (i = 0; i < 2; i++)
	{
		LHE7909_SDATAC();
	}
	/* 扩展寄存器(>=0x40)写用 DUT_WTREG，普通寄存器用 LHE7909_write_register */
	if (addr >= 0x40)
	{
		DUT_WTREG(value, 0x00, addr);
	}
	else
	{
		LHE7909_write_register(addr, value);
	}

	/* 关中断窗口内发确认帧，避免被 DRDY 数据帧插队 */
	ack[0] = ECG_CMD_HEAD1;
	ack[1] = ECG_CMD_HEAD2_MCU2PC;
	ack[2] = ECG_CMD_WRITE_REG;
	ack[3] = addr;
	ack[4] = (uint8_t)(ack[0] + ack[1] + ack[2] + ack[3]);
	ack[5] = 0x0D;
	HAL_UART_Transmit(&huart2, ack, ECG_CMD_LEN, 20);

	__enable_irq();
#endif
}

/* ====================================================================
 * BIAS_AMP 闭环 A/B/A′ 对比测试模块（仅文本调试模式 ECG_UART_BIN_MODE=0 编译）
 * ====================================================================
 * 目的：验证 LHE7909 片内 BIAS_AMP 闭环对共模干扰的抑制能力。
 *   A  组 CONFIG3=0x98（PD_BIAS=0，BIAS_AMP 掉电）
 *   B  组 CONFIG3=0x9C（PD_BIAS=1，BIAS_AMP 使能）
 *   A′ 组 CONFIG3=0x98（切回 A 复测，寄存器值与 A 相同，仅 CSV 头标注 A2）
 * 公共配置（宏定义见 LHE7909.h BIAS_TEST_*）：CONFIG1=0x55(500SPS)、CONFIG2=0xD3、
 *   LOFF=0x03、CH1SET=0x60(增益12,正常输入)、CH8SET=0x12(增益1,偏置回读MUX=010)、
 *   BIAS_SENSP=0x01、BIAS_SENSN=0x01；CONFIG3 是唯一变量。
 * 硬件侧（由测试者搭建，固件不参与）：信号源向共模节点 Node_CM 注入
 *   50Hz/1Vpp/+2.5V 偏置，IN1P 支路串 51kΩ 制造电极不平衡，示波器监测 Node_CM。
 * 命令（帧格式沿用上位机协议 [0xAA][0xCC][cmd][data][sum]{0x0D}）：
 *   0x10 ENTER / 0x11 GROUP / 0x12 ACQ / 0x13 EXIT，见 LHE7909.h。
 * 采集：DRDY 中断只把 CH1/CH8 原始码存入静态缓冲（中断内 printf 每帧约 450µs，
 *   必丢点），采满 2000 点(500SPS 下 4 秒)后由主循环 LHE7909_bias_task() 输出 CSV。 */

#if (ECG_UART_BIN_MODE == 0)
static uint8_t  bias_group = 0;              /* 当前组 0=A 1=B 2=A′（采集输出标注用） */
static uint8_t  bias_capture_gain = 12;      /* 本次采集的 CH1 增益（ACQ 时锁定，mV 换算与 CSV 头 GAIN= 用） */
static volatile uint32_t bias_settle_until_tick = 0;  /* 切组后丢弃窗口截止时刻(2s 内禁止采集) */

/* 文本模式命令接收：直接轮询 USART2 RXNE 并读 RDR 取 1 字节（最多等 1ms）。
 * 绕开 HAL_UART_Receive 的原因：该函数全程持有 HAL 全局锁(huart->Lock)，会把
 * DRDY 中断里的 printf(HAL_UART_Transmit) 全部挤成 HAL_BUSY 丢字节；直读寄存器
 * 不经过 HAL 锁，且 RX FIFO(16 字节已使能)可缓冲中断发送窗口内到达的命令字节。 */
static uint8_t LHE7909_uart_rx_byte(uint8_t *out)
{
	uint32_t start = HAL_GetTick();

	while ((HAL_GetTick() - start) < 1U)
	{
		uint32_t isr_flags = READ_REG(huart2.Instance->ISR);
		if ((isr_flags & USART_ISR_RXNE_RXFNE) != 0U)
		{
			if ((isr_flags & USART_ISR_ORE) != 0U)
			{
				/* 清 RX 溢出标志（被挤掉的命令字节按丢失处理，后续帧重新同步） */
				WRITE_REG(huart2.Instance->ICR, USART_ICR_ORECF);
			}
			*out = (uint8_t)(READ_REG(huart2.Instance->RDR) & 0xFFU);
			return 1;
		}
	}
	return 0;
}

/* 组号 → 输出标注：A′ 与 A 的寄存器值相同，仅 CSV 头标注不同 */
static const char *LHE7909_bias_group_label(uint8_t group)
{
	static const char *const label[3] = {"A", "B", "A2"};
	return label[(group <= 2U) ? group : 0U];
}

/* 组号 → CONFIG3 期望值：A/A′=0x98(PD_BIAS 掉电)，B=0x9C(PD_BIAS 使能) */
static uint8_t LHE7909_bias_group_cfg3(uint8_t group)
{
	return (group == 1U) ? BIAS_TEST_CFG3_B : BIAS_TEST_CFG3_A;
}

static uint8_t LHE7909_bias_capture_busy(void)
{
	return (uint8_t)((bias_capture_request != 0U) || (bias_capture_done != 0U));
}

/* 打印 BIAS 测试配置转储（必须在 SDATAC 窗口内调用，回读才可靠） */
static void LHE7909_bias_config_dump(void)
{
	printf("---- BIAS TEST CONFIG DUMP ----\r\n");
	printf("CONFIG1=0x%02X\r\n", LHE7909_read_register(CONFIG1_ADDRESS));
	printf("CONFIG2=0x%02X\r\n", LHE7909_read_register(CONFIG2_ADDRESS));
	printf("CONFIG3=0x%02X\r\n", LHE7909_read_register(CONFIG3_ADDRESS));
	printf("LOFF=0x%02X\r\n", LHE7909_read_register(LOFF_ADDRESS));
	printf("CH1SET=0x%02X\r\n", LHE7909_read_register(CH1SET_ADDRESS));
	printf("CH8SET=0x%02X\r\n", LHE7909_read_register(CH8SET_ADDRESS));
	printf("BIAS_SENSP=0x%02X\r\n", LHE7909_read_register(BIAS_SENSP_ADDRESS));
	printf("BIAS_SENSN=0x%02X\r\n", LHE7909_read_register(BIAS_SENSN_ADDRESS));
	printf("-------------------------------\r\n");
}

/* 进入 BIAS 测试模式（命令 0x10）：应用公共配置 + 切 A 组 + 配置转储。
 * 所有寄存器写入都在关中断窗口内完成（双重 SDATAC 后 SPI 独占，DRDY 不并发），
 * 每个寄存器写后用 LHE7909_check_reg() 回读校验（不一致打印 MISMATCH 报错），
 * START 前打印一份配置转储。启动转换后丢弃 2 秒数据才允许采集。 */
static void LHE7909_bias_enter(void)
{
	printf("BIAS TEST ENTER: applying common config...\r\n");

	__disable_irq();

	/* ① 双重 SDATAC：退出连续读模式（沿用现有代码习惯），寄存器读写才可靠 */
	LHE7909_SDATAC();
	LHE7909_SDATAC();

	/* ② 公共配置（CONFIG3 之外的固定项），逐个写 + 回读校验 */
	LHE7909_write_register(CONFIG1_ADDRESS, BIAS_TEST_CFG1);
	LHE7909_check_reg(CONFIG1_ADDRESS, BIAS_TEST_CFG1, "CONFIG1(BIAS)");
	LHE7909_write_register(CONFIG2_ADDRESS, BIAS_TEST_CFG2);
	LHE7909_check_reg(CONFIG2_ADDRESS, BIAS_TEST_CFG2, "CONFIG2(BIAS)");
	LHE7909_write_register(LOFF_ADDRESS, BIAS_TEST_LOFF);
	LHE7909_check_reg(LOFF_ADDRESS, BIAS_TEST_LOFF, "LOFF(BIAS)");
	LHE7909_write_register(CH1SET_ADDRESS, BIAS_TEST_CH1SET);
	LHE7909_check_reg(CH1SET_ADDRESS, BIAS_TEST_CH1SET, "CH1SET(BIAS)");
	LHE7909_write_register(CH8SET_ADDRESS, BIAS_TEST_CH8SET);
	LHE7909_check_reg(CH8SET_ADDRESS, BIAS_TEST_CH8SET, "CH8SET(BIAS)");
	LHE7909_write_register(BIAS_SENSP_ADDRESS, BIAS_TEST_BIASSENSP);
	LHE7909_check_reg(BIAS_SENSP_ADDRESS, BIAS_TEST_BIASSENSP, "BIAS_SENSP(BIAS)");
	LHE7909_write_register(BIAS_SENSN_ADDRESS, BIAS_TEST_BIASSENSN);
	LHE7909_check_reg(BIAS_SENSN_ADDRESS, BIAS_TEST_BIASSENSN, "BIAS_SENSN(BIAS)");

	/* ③ 增益变量随 CHnSET 同步（ADC_DataToMiniVolt 按各自增益折算到输入端）；
	 * CH2~CH7 本测试不改写，仍保持上电初始化的 0x01(增益6,内部短接) */
	ch1_gain = BIAS_TEST_CH1_GAIN;   /* CH1SET=0x60 → 12 */
	ch8_gain = 1U;                   /* CH8SET=0x12 → 1 */

	/* ④ 切 A 组：CONFIG3=0x98（PD_BIAS 掉电）——本测试唯一变量 */
	LHE7909_write_register(CONFIG3_ADDRESS, BIAS_TEST_CFG3_A);
	LHE7909_check_reg(CONFIG3_ADDRESS, BIAS_TEST_CFG3_A, "CONFIG3(BIAS)");
	bias_group = 0U;

	/* ⑤ 清残留采集状态 + 配置转储（仍在 SDATAC 窗口内，回读可靠） */
	bias_capture_request = 0U;
	bias_capture_done = 0U;
	bias_capture_count = 0U;
	LHE7909_bias_config_dump();

	/* ⑥ 重新启动转换：沿用现有代码的启动方式（START 命令，不能进 RDATAC——
	 *    本固件采集中断用 RDATA 单次读取数，进连续读模式会取数冲突） */
	LHE7909_START();
	LHE7909_delay_us(20);

	__enable_irq();

	/* ⑦ 置测试模式标志 → DRDY 中断停止逐帧打印；丢弃切换后 2 秒内的数据
	 *    （环路与数字滤波器稳定），期间样本静默丢弃，之后才允许 ACQ */
	g_bias_test_active = 1U;
	bias_settle_until_tick = HAL_GetTick() + BIAS_TEST_SETTLE_MS;
	printf("BIAS TEST ENTER: GROUP A settling %u ms (data discarded)...\r\n",
	       (unsigned)BIAS_TEST_SETTLE_MS);
	HAL_Delay(BIAS_TEST_SETTLE_MS);
	printf("BIAS TEST ENTER OK: GROUP A (CONFIG3=0x98) ready, CH1 GAIN=%u\r\n",
	       (unsigned)ch1_gain);
}

/* 切换测试分组（命令 0x11，data=0x00:A 0x01:B 0x02:A′）。严格按序：
 * 关中断窗口内 发 SDATAC 退出连续读模式 → 写 CONFIG3 → LHE7909_check_reg()
 * 回读校验（不一致打印 MISMATCH 报错，只报告不改流程）→ START 重新启动转换
 * （沿用现有代码的启动方式）→ 丢弃切换后 2 秒内的数据（环路与数字滤波器稳定）
 * → 才允许采集（LHE7909_bias_acq_start() 的 settle 窗口检查兜底）。 */
static void LHE7909_bias_switch_group(uint8_t group)
{
	uint8_t cfg3 = LHE7909_bias_group_cfg3(group);

	__disable_irq();
	LHE7909_SDATAC();
	LHE7909_SDATAC();
	LHE7909_write_register(CONFIG3_ADDRESS, cfg3);
	LHE7909_check_reg(CONFIG3_ADDRESS, cfg3, "CONFIG3(BIAS)");
	LHE7909_START();
	LHE7909_delay_us(20);
	__enable_irq();

	/* 后续采集输出的 GROUP 标注与 CONFIG3 回显按本组 */
	bias_group = group;
	bias_settle_until_tick = HAL_GetTick() + BIAS_TEST_SETTLE_MS;
	printf("BIAS GROUP %s: settling %u ms (data discarded)...\r\n",
	       LHE7909_bias_group_label(group), (unsigned)BIAS_TEST_SETTLE_MS);
	HAL_Delay(BIAS_TEST_SETTLE_MS);
	printf("BIAS GROUP %s OK: CONFIG3=0x%02X, ready for ACQ\r\n",
	       LHE7909_bias_group_label(group), cfg3);
}

/* 触发一次定长采集（命令 0x12）：DRDY 中断把 CH1/CH8 原始码写入静态缓冲，
 * 采满 2000 点（500SPS 下 4 秒）后由主循环 LHE7909_bias_task() 统一输出 CSV。 */
static void LHE7909_bias_acq_start(void)
{
	uint32_t now = HAL_GetTick();
	const char *label = LHE7909_bias_group_label(bias_group);

	if (LHE7909_bias_capture_busy())
	{
		printf("BIAS ACQ BUSY: previous capture running, wait for #END\r\n");
		return;
	}
	/* 切组后 2 秒丢弃窗口未过不允许采集（切组命令自身已等 2 秒，此为兜底） */
	if ((int32_t)(now - bias_settle_until_tick) < 0)
	{
		printf("BIAS ACQ FAIL: %u ms settle window after group switch not elapsed\r\n",
		       (unsigned)BIAS_TEST_SETTLE_MS);
		return;
	}

	/* 锁定本次采集的 CH1 增益（12）：CSV 头 GAIN= 与 mV 换算都用它 */
	bias_capture_gain = ch1_gain;

	__disable_irq();
	bias_capture_count = 0U;
	bias_capture_done = 0U;
	bias_last_sample_tick = now;
	bias_capture_request = 1U;   /* ISR 从下一个 DRDY 开始存点 */
	__enable_irq();

	printf("BIAS ACQ START: GROUP %s, %u points @%uSPS (~%u s), GAIN=%u\r\n",
	       label, (unsigned)BIAS_TEST_N_POINTS, (unsigned)BIAS_TEST_FS_SPS,
	       (unsigned)(BIAS_TEST_N_POINTS / BIAS_TEST_FS_SPS), (unsigned)bias_capture_gain);
}

/* 输出一次定长采集结果（主循环上下文，DRDY 中断只存缓冲不打印）：
 * 起始行  #GROUP,<A|B|A2>,CONFIG3=0x98|0x9C,N=2000,FS=500,GAIN=12
 * 数据行  序号,CH1原始码(有符号十进制),CH1电压mV,CH8原始码(有符号十进制)  ×每样本一行
 * 结束行  #END,<实际点数>（实际点数 ≠ 2000 时先打印 #WARN 警告，如实报告） */
static void LHE7909_bias_print_capture(void)
{
	uint16_t n = bias_capture_count;
	uint16_t i;
	const char *label = LHE7909_bias_group_label(bias_group);
	uint8_t cfg3 = LHE7909_bias_group_cfg3(bias_group);

	printf("#GROUP,%s,CONFIG3=0x%02X,N=%u,FS=%u,GAIN=%u\r\n",
	       label, cfg3, (unsigned)BIAS_TEST_N_POINTS, (unsigned)BIAS_TEST_FS_SPS,
	       (unsigned)bias_capture_gain);

	for (i = 0U; i < n; i++)
	{
		float mv = ADC_DataToMiniVolt(bias_ch1_buf[i], bias_capture_gain);
		printf("%u,%ld,%.3f,%ld\r\n",
		       (unsigned)(i + 1U), (long)bias_ch1_buf[i], mv, (long)bias_ch8_buf[i]);
	}

	if (n != BIAS_TEST_N_POINTS)
	{
		printf("#WARN: expected %u points, got %u (DRDY stall?)\r\n",
		       (unsigned)BIAS_TEST_N_POINTS, (unsigned)n);
	}
	printf("#END,%u\r\n", (unsigned)n);
}

/* 退出 BIAS 测试模式（命令 0x13）：恢复进入前的初始化配置。
 * 恢复值与 LHE7909_init() 完全一致（若 init 调整配置请同步修改此处）：
 * CONFIG1=0x55、CONFIG2=0xD3、CONFIG3=0x9C、LOFF=0x03、CH1SET=0x10、
 * CH2SET~CH7SET=0x01、CH8SET=0x12、RLD_SENSP/N(=BIAS_SENSP/N)=0x00，
 * 增益变量同步恢复，VrefVolt 恢复 2400mV。逐寄存器写 + 回读校验。 */
static void LHE7909_bias_exit(void)
{
	uint16_t aborted = 0;

	__disable_irq();
	if (bias_capture_request != 0U)
	{
		/* 采集中退出：中止本次采集，不输出 */
		aborted = bias_capture_count;
		bias_capture_request = 0U;
		bias_capture_done = 0U;
		bias_capture_count = 0U;
	}
	/* 已完成待输出的采集(done=1)不清理：主循环同一轮内 bias_task 随后照常输出 */
	g_bias_test_active = 0U;

	LHE7909_SDATAC();
	LHE7909_SDATAC();

	/* ---- 以下恢复值与 LHE7909_init() 保持一致 ---- */
	LHE7909_write_register(CONFIG1_ADDRESS, 0x55);
	LHE7909_check_reg(CONFIG1_ADDRESS, 0x55, "CONFIG1(RESTORE)");
	LHE7909_write_register(CONFIG2_ADDRESS, 0xD3);
	LHE7909_check_reg(CONFIG2_ADDRESS, 0xD3, "CONFIG2(RESTORE)");
	LHE7909_write_register(CONFIG3_ADDRESS, 0x9C);
	LHE7909_check_reg(CONFIG3_ADDRESS, 0x9C, "CONFIG3(RESTORE)");
	LHE7909_write_register(LOFF_ADDRESS, 0x03);
	LHE7909_check_reg(LOFF_ADDRESS, 0x03, "LOFF(RESTORE)");
	LHE7909_write_register(CH1SET_ADDRESS, 0x10);
	LHE7909_check_reg(CH1SET_ADDRESS, 0x10, "CH1SET(RESTORE)");
	LHE7909_write_register(CH2SET_ADDRESS, 0x01);
	LHE7909_write_register(CH3SET_ADDRESS, 0x01);
	LHE7909_write_register(CH4SET_ADDRESS, 0x01);
	LHE7909_write_register(CH5SET_ADDRESS, 0x01);
	LHE7909_write_register(CH6SET_ADDRESS, 0x01);
	LHE7909_write_register(CH7SET_ADDRESS, 0x01);
	LHE7909_write_register(CH8SET_ADDRESS, 0x12);
	LHE7909_check_reg(CH8SET_ADDRESS, 0x12, "CH8SET(RESTORE)");
	LHE7909_write_register(RLD_SENSP_ADDRESS, 0x00);
	LHE7909_check_reg(RLD_SENSP_ADDRESS, 0x00, "RLD_SENSP(RESTORE)");
	LHE7909_write_register(RLD_SENSN_ADDRESS, 0x00);
	LHE7909_check_reg(RLD_SENSN_ADDRESS, 0x00, "RLD_SENSN(RESTORE)");

	/* 增益变量随 CHnSET 恢复（与 LHE7909_init 一致） */
	ch1_gain = 1U;
	ch2_gain = 6U;
	ch3_gain = 6U;
	ch4_gain = 6U;
	ch5_gain = 6U;
	ch6_gain = 6U;
	ch7_gain = 6U;
	ch8_gain = 1U;
	VrefVolt = 2400.0;

	LHE7909_START();
	LHE7909_delay_us(20);
	__enable_irq();

	if (aborted != 0U)
	{
		printf("BIAS EXIT: capture aborted (%u/%u points discarded)\r\n",
		       (unsigned)aborted, (unsigned)BIAS_TEST_N_POINTS);
	}
	printf("BIAS EXIT OK: init config restored\r\n");
}

/* BIAS 测试任务（主循环周期调用，仅文本模式）：
 * 1) 采集中停滞超时监测：DRDY 停止等异常导致 BIAS_TEST_STALL_MS 内无新样本时
 *    结束本次采集，按实际点数输出并告警；
 * 2) 采集完成后输出定长采集 CSV（起始行/数据行/结束行）。 */
void LHE7909_bias_task(void)
{
	if (bias_capture_request != 0U)
	{
		if ((HAL_GetTick() - bias_last_sample_tick) > BIAS_TEST_STALL_MS)
		{
			bias_capture_request = 0U;
			bias_capture_done = 1U;
		}
	}
	if (bias_capture_done != 0U)
	{
		bias_capture_done = 0U;
		LHE7909_bias_print_capture();
	}
}
#endif /* (ECG_UART_BIN_MODE == 0) */

/* 外部 UART 句柄（定义于 main.c，本文件 LHE7909_send_frame 之前已声明） */

/* 解析上位机命令帧（主循环轮询调用）：
 * 普通命令: [0xAA][0xCC][cmd][data][sum]{0x0D}      PC->MCU (6B)
 * 写寄存器: [0xAA][0xCC][0x04][addr][val][sum]{0x0D}  PC->MCU (7B)
 * 成功处理返回 1，无命令/校验失败返回 0 */
uint8_t LHE7909_process_command(void)
{
#if ECG_UART_BIN_MODE
	uint8_t rx[ECG_CMD_LEN],i;
	uint16_t timeout = 10;
	uint8_t gain;

	/* ============ 二进制模式：原有上位机命令（行为不变） ============ */

	/* 轮询接收 1 字节，直到凑满 ECG_CMD_LEN 或超时 */
	while (timeout > 0)
	{
		if (HAL_UART_Receive(&huart2, &rx[0], 1, 1) == HAL_OK)
		{
			if (rx[0] != ECG_CMD_HEAD1)
			{
				continue;   /* 等待帧头 */
			}
			break;
		}
		timeout--;
	}
	if (timeout == 0)
	{
		return 0;   /* 无命令 */
	}

	/* 已收到帧头，继续接收剩余 5 字节 */
	if (HAL_UART_Receive(&huart2, &rx[1], ECG_CMD_LEN - 1, 20) != HAL_OK)
	{
		return 0;
	}

	/* 校验帧头2 */
	if (rx[1] != ECG_CMD_HEAD2_PC2MCU)
	{
		return 0;
	}

	/* 写寄存器命令(7B 帧)特殊处理：前 6B 已收 [AA CC 04 addr val sum]，
	 * 校验和覆盖前 5 字节，再读尾部 0x0D。 */
	if (rx[2] == ECG_CMD_WRITE_REG)
	{
		uint8_t calc = (uint8_t)(rx[0] + rx[1] + rx[2] + rx[3] + rx[4]);
		uint8_t tail;
		if (calc != rx[5])
		{
			return 0;
		}
		if (HAL_UART_Receive(&huart2, &tail, 1, 10) != HAL_OK || tail != 0x0D)
		{
			return 0;
		}
		LHE7909_write_reg_cmd(rx[3], rx[4]);
		//========每个通道增益修改========
	
			switch(rx[3])
			{
					case 0x03://config3
						if((rx[4]&0x20)==0x20)
						{
								VrefVolt=4000.0;
						}
						else
						{
								VrefVolt=2400.0;
						}
							break;
					case 0x05://ch1
						gain=rx[4]&0xF0;
						switch(gain)
						{
								case 0x00:
										ch1_gain = 6;
										break;
								case 0x10:
										ch1_gain = 1;
										break;
								case 0x20:
										ch1_gain = 2;
										break;
								case 0x30:
										ch1_gain = 3;
										break;
								case 0x40:
										ch1_gain = 4;
										break;
								case 0x50:
										ch1_gain = 8;
										break;
								case 0x60:
										ch1_gain = 12;
										break;
								case 0x70:
										ch1_gain = 24;
										break;
								default:
										break;
						}	
						break;
						case 0x06://ch2
						gain=rx[4]&0xF0;
						switch(gain)
						{
								case 0x00:
										ch2_gain = 6;
										break;
								case 0x10:
										ch2_gain = 1;
										break;
								case 0x20:
										ch2_gain = 2;
										break;
								case 0x30:
										ch2_gain = 3;
										break;
								case 0x40:
										ch2_gain = 4;
										break;
								case 0x50:
										ch2_gain = 8;
										break;
								case 0x60:
										ch2_gain = 12;
										break;
								case 0x70:
										ch2_gain = 24;
										break;
								default:
										break;
						}
							break;
						case 0x07://ch3
						gain=rx[4]&0xF0;
						switch(gain)
						{
								case 0x00:
										ch3_gain = 6;
										break;
								case 0x10:
										ch3_gain = 1;
										break;
								case 0x20:
										ch3_gain = 2;
										break;
								case 0x30:
										ch3_gain = 3;
										break;
								case 0x40:
										ch3_gain = 4;
										break;
								case 0x50:
										ch3_gain = 8;
										break;
								case 0x60:
										ch3_gain = 12;
										break;
								case 0x70:
										ch3_gain = 24;
										break;
								default:
										break;
						}
							break;
						case 0x08://ch4
							gain=rx[4]&0xF0;
							switch(gain)
							{
									case 0x00:
											ch4_gain = 6;
											break;
									case 0x10:
											ch4_gain = 1;
											break;
									case 0x20:
											ch4_gain = 2;
											break;
									case 0x30:
											ch4_gain = 3;
											break;
									case 0x40:
											ch4_gain = 4;
											break;
									case 0x50:
											ch4_gain = 8;
											break;
									case 0x60:
											ch4_gain = 12;
											break;
									case 0x70:
											ch4_gain = 24;
											break;
									default:
											break;
							}
							break;
						case 0x09://ch5
							gain=rx[4]&0xF0;
							switch(gain)
							{
									case 0x00:
											ch5_gain = 6;
											break;
									case 0x10:
											ch5_gain = 1;
											break;
									case 0x20:
											ch5_gain = 2;
											break;
									case 0x30:
											ch5_gain = 3;
											break;
									case 0x40:
											ch5_gain = 4;
											break;
									case 0x50:
											ch5_gain = 8;
											break;
									case 0x60:
											ch5_gain = 12;
											break;
									case 0x70:
											ch5_gain = 24;
											break;
									default:
											break;
							}
							break;
						case 0x0a://ch6
							gain=rx[4]&0xF0;
							switch(gain)
							{
									case 0x00:
											ch6_gain = 6;
											break;
									case 0x10:
											ch6_gain = 1;
											break;
									case 0x20:
											ch6_gain = 2;
											break;
									case 0x30:
											ch6_gain = 3;
											break;
									case 0x40:
											ch6_gain = 4;
											break;
									case 0x50:
											ch6_gain = 8;
											break;
									case 0x60:
											ch6_gain = 12;
											break;
									case 0x70:
											ch6_gain = 24;
											break;
									default:
											break;
							}
							break;
						case 0x0b://ch7
							gain=rx[4]&0xF0;
							switch(gain)
							{
									case 0x00:
											ch7_gain = 6;
											break;
									case 0x10:
											ch7_gain = 1;
											break;
									case 0x20:
											ch7_gain = 2;
											break;
									case 0x30:
											ch7_gain = 3;
											break;
									case 0x40:
											ch7_gain = 4;
											break;
									case 0x50:
											ch7_gain = 8;
											break;
									case 0x60:
											ch7_gain = 12;
											break;
									case 0x70:
											ch7_gain = 24;
											break;
									default:
											break;
							}
							break;
					case 0x0c://ch8
							gain=rx[4]&0xF0;
							switch(gain)
							{
									case 0x00:
											ch8_gain = 6;
											break;
									case 0x10:
											ch8_gain = 1;
											break;
									case 0x20:
											ch8_gain = 2;
											break;
									case 0x30:
											ch8_gain = 3;
											break;
									case 0x40:
											ch8_gain = 4;
											break;
									case 0x50:
											ch8_gain = 8;
											break;
									case 0x60:
											ch8_gain = 12;
											break;
									case 0x70:
											ch8_gain = 24;
											break;
									default:
											break;
							}
							break;
						default:
							break;
				
		}
		
		return 1;
	}

	/* 普通命令校验和 = 前 4 字节累加和的低 8 位 */
	{
		uint8_t calc = (uint8_t)(rx[0] + rx[1] + rx[2] + rx[3]);
		if (calc != rx[4])
		{
			return 0;
		}
	}

	/* 处理命令 */
	if (rx[2] == ECG_CMD_SET_DR)
	{
		/* LHE7909_set_dr() 内部已在关中断窗口内完成 SPI 切换并发送确认帧，
		 * 此处不再重复发送，避免 ack 重复。 */
		LHE7909_set_dr(rx[3]);
		/* 指示灯统一由 LHE7909_led_task() 按“是否在上传数据”控制，
		 * 这里不再额外翻转，避免破坏闪烁节奏 */
		return 1;
	}

	/* 暂停/继续采集：
	 * data=0 暂停 → 先 SDATAC 退出连续读模式，再 STOP 停止转换；
	 * data=1 继续 → 只发 START 重新开始转换（本固件用 RDATA 单次读采集，
	 *              不能进 RDATAC 连续读模式）。
	 * 关中断窗口内完成 SPI 操作，避免与 DRDY 中断的 SPI 读取并发冲突。 */
	if (rx[2] == ECG_CMD_PAUSE)
	{
		__disable_irq();
		if (rx[3] == 0x00)
		{
			
				/* ② 双重 SDATAC：停止连续输出模式。重复两次提高成功率，
			*    至少一次在芯片空闲窗口内被识别。 */
				for (i = 0; i < 2; i++)
				{
					LHE7909_SDATAC();
					LHE7909_STOP();
					LHE7909_delay_us(20);
					
				}
		}
		else
		{
			LHE7909_START();
			//LHE7909_RDATAC();
		}
		__enable_irq();

		/* 发送确认帧: [0xAA][0xDB][0x02][data][sum]{0x0D} */
		{
			uint8_t ack[ECG_CMD_LEN];
			ack[0] = ECG_CMD_HEAD1;
			ack[1] = ECG_CMD_HEAD2_MCU2PC;
			ack[2] = ECG_CMD_PAUSE;
			ack[3] = rx[3];
			ack[4] = (uint8_t)(ack[0] + ack[1] + ack[2] + ack[3]);
			ack[5] = 0x0D;
			HAL_UART_Transmit(&huart2, ack, ECG_CMD_LEN, 20);
		}
		return 1;
	}

	/* 读取芯片寄存器配置（上位机仅在暂停时点击"查看配置"） */
	if (rx[2] == ECG_CMD_READ_CFG)
	{
		/* 先发送确认帧，便于上位机确认命令已被固件接收：
		 * [0xAA][0xDB][0x03][0x00][sum][0x0D] */
		{
			uint8_t ack[ECG_CMD_LEN];
			ack[0] = ECG_CMD_HEAD1;
			ack[1] = ECG_CMD_HEAD2_MCU2PC;
			ack[2] = ECG_CMD_READ_CFG;
			ack[3] = 0x00;
			ack[4] = (uint8_t)(ack[0] + ack[1] + ack[2] + ack[3]);
			ack[5] = 0x0D;
			HAL_UART_Transmit(&huart2, ack, ECG_CMD_LEN, 20);
		}
		LHE7909_read_config();
		return 1;
	}

	return 0;

#else
	/* ============ 文本调试模式：BIAS_AMP 闭环 A/B/A′ 对比测试命令 ============
	 * 帧格式沿用现有上位机协议: [0xAA][0xCC][cmd][data][sum]{0x0D}，
	 * sum = 前 4 字节累加和的低 8 位。
	 * 接收实现与二进制模式不同：直接轮询 USART2 的 RXNE/RDR 取字节（RX FIFO
	 * 16 字节已使能），不调用 HAL_UART_Receive —— 它全程持有 HAL 全局锁，
	 * 会把 DRDY 中断里的 printf(HAL_UART_Transmit) 全部挤成 HAL_BUSY 丢字节；
	 * 直读寄存器不经过 HAL 锁，主循环轮询与中断发送即可安全并发
	 * （这也是此前文本模式不调用本函数的原因）。 */
	{
		uint8_t rx[ECG_CMD_LEN];
		uint16_t timeout = 10;
		uint8_t got_head = 0;
		uint8_t i;

		/* 轮询等待帧头：总窗口约 10ms，无命令即返回主循环 */
		while (timeout > 0)
		{
			if (!LHE7909_uart_rx_byte(&rx[0]))
			{
				timeout--;
				continue;
			}
			if (rx[0] != ECG_CMD_HEAD1)
			{
				continue;   /* 等待帧头 */
			}
			got_head = 1;
			break;
		}
		if (!got_head)
		{
			return 0;   /* 无命令 */
		}

		/* 已收到帧头，继续接收剩余 5 字节（每字节最多等 20ms，节奏与原框架一致） */
		for (i = 1; i < ECG_CMD_LEN; i++)
		{
			uint16_t byte_timeout = 20;
			while ((byte_timeout > 0) && (!LHE7909_uart_rx_byte(&rx[i])))
			{
				byte_timeout--;
			}
			if (byte_timeout == 0)
			{
				return 0;
			}
		}

		/* 校验帧头2 */
		if (rx[1] != ECG_CMD_HEAD2_PC2MCU)
		{
			return 0;
		}

		/* 普通命令校验和 = 前 4 字节累加和的低 8 位（与原框架一致） */
		{
			uint8_t calc = (uint8_t)(rx[0] + rx[1] + rx[2] + rx[3]);
			if (calc != rx[4])
			{
				return 0;
			}
		}

		switch (rx[2])
		{
			case ECG_CMD_BIAS_ENTER:          /* 进入测试模式：公共配置+切A组+转储 */
				if (g_bias_test_active)
				{
					printf("BIAS ENTER FAIL: already in test mode (send EXIT first)\r\n");
				}
				else
				{
					LHE7909_bias_enter();
				}
				return 1;

			case ECG_CMD_BIAS_GROUP:          /* 切组: 0x00=A 0x01=B 0x02=A'(复测) */
				if (!g_bias_test_active)
				{
					printf("BIAS GROUP FAIL: not in test mode\r\n");
				}
				else if (rx[3] > 0x02)
				{
					printf("BIAS GROUP FAIL: data=0x%02X, use 0x00(A)/0x01(B)/0x02(A2)\r\n", rx[3]);
				}
				else if (LHE7909_bias_capture_busy())
				{
					printf("BIAS GROUP BUSY: capture running, wait for #END\r\n");
				}
				else
				{
					LHE7909_bias_switch_group(rx[3]);
				}
				return 1;

			case ECG_CMD_BIAS_ACQ:            /* 触发一次定长采集(2000 点) */
				if (!g_bias_test_active)
				{
					printf("BIAS ACQ FAIL: not in test mode\r\n");
				}
				else
				{
					LHE7909_bias_acq_start();
				}
				return 1;

			case ECG_CMD_BIAS_EXIT:           /* 退出测试模式，恢复初始化配置 */
				if (!g_bias_test_active)
				{
					printf("BIAS EXIT: not in test mode\r\n");
				}
				else
				{
					LHE7909_bias_exit();
				}
				return 1;

			default:
				/* 二进制模式命令(SET_DR/PAUSE/READ_CFG/WRITE_REG)的实现在文本模式下
				 * 本就编译为空，未知命令不动作，仅提示 */
				printf("BIAS CMD unknown: 0x%02X\r\n", rx[2]);
				return 0;
		}
	}
#endif
}












