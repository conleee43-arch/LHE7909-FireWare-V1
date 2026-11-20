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
void LHE7909_init(uint8_t DR)
{
	uint8_t x1=0;
	
    HAL_GPIO_WritePin(GPIOA, LHE7909_DAISY_IN_Pin, GPIO_PIN_RESET);
  
    HAL_GPIO_WritePin(GPIOB, LHE7909_nPWDN_Pin, GPIO_PIN_SET);
  
		HAL_GPIO_WritePin(GPIOB, LHE7909_nRESET_Pin, GPIO_PIN_SET);
	
		HAL_GPIO_WritePin(GPIOB, LHE7909_START_Pin, GPIO_PIN_RESET);

     
	  HAL_Delay(35);//上电等待至少35ms
	
    LHE7909_RESET();//先发RESET命令


    LHE7909_SDATAC();
		
	 DUT_WTREG(0xac,0x01,0x82);//USERKEY UNLOCK;

	x1=LHE7909_read_register(ID_ADDRESS);
	printf("ID_ADDRESS: %x\r\n",x1);
	x1 =0xe0;

   counter=0;

	LHE7909_write_register(CONFIG1_ADDRESS,0xc6);//HR模式;500SPS
	x1=LHE7909_read_register(CONFIG1_ADDRESS);
	printf("CONFIG1_ADDRESS: %x\r\n",x1);

	
	LHE7909_write_register(CONFIG2_ADDRESS,0xC0);//defalut 
	x1=LHE7909_read_register(CONFIG2_ADDRESS);
	printf("CONFIG2_ADDRESS: %x\r\n",x1);
 
 
 
 LHE7909_write_register(CONFIG3_ADDRESS,0x8C);//参考电压为内部2.4V；使能RLD buffer；不使能大电流模式
	x1=LHE7909_read_register(CONFIG3_ADDRESS);
	printf("CONFIG3_ADDRESS: %x\r\n",x1);	
	VrefVolt=2400.0;//参考电压为2400mV
	
  LHE7909_write_register(LOFF_ADDRESS,0x13);//导联脱落比较器阈值VTHH=70%,VTHL=30%;导联脱落检测模式为直流，电阻模式									
	x1=LHE7909_read_register(LOFF_ADDRESS);
	printf("LOFF_ADDRESS: %x\r\n",x1);
	
   //============CHnSET Begin==============================================
   LHE7909_write_register(CH1SET_ADDRESS,0X00);//0x00 G=6 
   x1=LHE7909_read_register(CH1SET_ADDRESS);
   printf("CH1SET_ADDRESS: %x\r\n",x1);
   LHE7909_write_register(CH2SET_ADDRESS,0X00);
   LHE7909_write_register(CH3SET_ADDRESS,0X00);
   LHE7909_write_register(CH4SET_ADDRESS,0X00);
   LHE7909_write_register(CH5SET_ADDRESS,0X00);
   LHE7909_write_register(CH6SET_ADDRESS,0X00);
   LHE7909_write_register(CH7SET_ADDRESS,0X00);
   LHE7909_write_register(CH8SET_ADDRESS,0X00);
	 ch1_gain=6;
	 ch2_gain=6;
	 ch3_gain=6;
	 ch4_gain=6;
	 ch5_gain=6;
	 ch6_gain=6;
	 ch7_gain=6;
	 ch8_gain=6;
	
	LHE7909_write_register(RLD_SENSP_ADDRESS,0x06);//0X06 IN3P IN2P----LL LA
	x1=LHE7909_read_register(RLD_SENSP_ADDRESS);
	printf("RLD_SENSP_ADDRESS: %x\r\n",x1);
	
	LHE7909_write_register(RLD_SENSN_ADDRESS,0x06);//0X06 IN3N IN2N----RA  RA
	x1=LHE7909_read_register(RLD_SENSN_ADDRESS);
	printf("RLD_SENSN_ADDRESS: %x\r\n",x1);


	LHE7909_write_register(LOFF_SENSP_ADDRESS,0xff);//启用IN1P~IN8P导联脱落
	x1=LHE7909_read_register(LOFF_SENSP_ADDRESS);
	printf("LOFF_SENSP_ADDRESS: %x\r\n",x1);


	LHE7909_write_register(LOFF_SENSN_ADDRESS,0xff);//启用IN1N~IN8N导联脱落
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
	
	
	LHE7909_write_register(CONFIG4_ADDRESS,0x03);//连续模式,启用导联脱落比较器，开启SINC4
	x1=LHE7909_read_register(CONFIG4_ADDRESS);
	printf("CONFIG4_ADDRESS: %x\r\n",x1);



	LHE7909_write_register(WCT1_ADDRESS,0x0B);//WCT1使能，RA电极(IN2N)连接到WCTA
	x1=LHE7909_read_register(WCT1_ADDRESS);
	printf("WCT1_ADDRESS: %x\r\n",x1);	
	
	LHE7909_write_register(WCT2_ADDRESS,0xD4);//使能WCTB,WCTC  IN3P---WCTC LL电极;IN2P---WCTB   LA电极
	x1=LHE7909_read_register(WCT2_ADDRESS);
	printf("WCT2_ADDRESS: %x\r\n",x1);	

		
		
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





void LHE7909_delay_us(uint32_t time)
{
	volatile uint32_t i;
  for(i=0;i<10*time;i++)
	{
		__NOP();
	}
}

extern uint8_t LHE7909_adc_data[27];
int32_t ch1_hex,ch2_hex,ch3_hex,ch4_hex,ch5_hex,ch6_hex,ch7_hex,ch8_hex;
float   ch1_voltage,ch2_voltage,ch3_voltage,ch4_voltage,ch5_voltage,ch6_voltage,ch7_voltage,ch8_voltage;
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
		 printf("%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\r\n",ch2_voltage,ch3_voltage,ch4_voltage,ch5_voltage,ch6_voltage,ch7_voltage,ch8_voltage,ch1_voltage);
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
	  //mv = data*2.0*VrefVolt/((1<<24))/gain;
	mv = data*2.0*VrefVolt/((1<<24));
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

/* 外部 UART 句柄（定义于 main.c，本文件 LHE7909_send_frame 之前已声明） */

/* 解析上位机命令帧（主循环轮询调用）：
 * 普通命令: [0xAA][0xCC][cmd][data][sum]{0x0D}      PC->MCU (6B)
 * 写寄存器: [0xAA][0xCC][0x04][addr][val][sum]{0x0D}  PC->MCU (7B)
 * 成功处理返回 1，无命令/校验失败返回 0 */
uint8_t LHE7909_process_command(void)
{
	uint8_t rx[ECG_CMD_LEN],i;
	uint16_t timeout = 10;
	uint8_t gain;

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
}












