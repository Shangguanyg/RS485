#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "rs485.h"
/************************************************
 
************************************************/
	
int main(void)
{ 	
	Cache_Enable();                			//打开L1-Cache
	HAL_Init();				        		//初始化HAL库
	Stm32_Clock_Init(160,5,2,4);  		    //设置时钟,400Mhz 
	delay_init(400);						//延时初始化
	uart_init(115200);						//串口初始化	
  RS485_Init(115200);		        		//初始化RS485	 			
  RS485_Send_Data("RS485收发测试例程",17);	

    while(1)
    { 
    if(RS485_REC_Flag == 1)	   //如果串口接收到一帧数据（以“#*”结尾）
		{
			RS485_REC_Flag = 0;
			delay_ms(50);	
			RS485_Send_Data(RS485_buff,RS485_rec_counter);			
			RS485_rec_counter = 0;
		}			
	  }	     
}

