#include "rs485.h"
#include "delay.h"

volatile unsigned char RS485_REC_Flag = 0;
volatile unsigned char RS485_buff[RS485_REC_BUFF_SIZE];//用于接收数据
volatile unsigned int RS485_rec_counter = 0;//用于RS485接收计数

UART_HandleTypeDef UART7_RS485Handler;  //UART7句柄(用于RS485)


u8 RS485_RX_BUF[64];  	//接收缓冲,最大64个字节.
//接收到的数据长度
u8 RS485_RX_CNT=0;  

void UART7_IRQHandler(void)
{
    u8 res;	  
    if(__HAL_UART_GET_IT(&UART7_RS485Handler,UART_IT_RXNE)!=RESET)  //接收中断
	{	 	
        HAL_UART_Receive(&UART7_RS485Handler,&res,1,1000);
		RS485_buff[RS485_rec_counter] = res;//
		RS485_rec_counter ++;
/********以RS485_END_FLAG1和RS485_END_FLAG2定义的字符作为一帧数据的结束标识************/
		if(RS485_rec_counter >= 2)	//只有接收到2个数据以上才做判断
		{
			if(RS485_buff[RS485_rec_counter - 2] == RS485_END_FLAG1 && RS485_buff[RS485_rec_counter - 1] == RS485_END_FLAG2) 	//帧起始标志   
			{
				RS485_REC_Flag = 1;
			}
		}
		if(RS485_rec_counter > RS485_REC_BUFF_SIZE)//超过接收缓冲区大小
		{
			RS485_rec_counter = 0;
		}
		
	} 
}    


//初始化IO 串口2
//bound:波特率
void RS485_Init(u32 bound)
{
    //GPIO端口设置
	GPIO_InitTypeDef GPIO_Initure;	
	__HAL_RCC_GPIOE_CLK_ENABLE();			//使能GPIOE时钟
	__HAL_RCC_UART7_CLK_ENABLE();			//使能UART7时钟
	
	GPIO_Initure.Pin=GPIO_PIN_8; //PE8
	GPIO_Initure.Mode=GPIO_MODE_AF_PP;		//复用推挽输出
	GPIO_Initure.Pull=GPIO_PULLUP;			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;//高速
	GPIO_Initure.Alternate=GPIO_AF7_UART7;	//复用为UART7
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);	   	//初始化
	
	GPIO_Initure.Pin=GPIO_PIN_7;			//PE7
	HAL_GPIO_Init(GPIOE,&GPIO_Initure);	   	//初始化PE7

    //USART 初始化设置
	UART7_RS485Handler.Instance=UART7;			        //UART7
	UART7_RS485Handler.Init.BaudRate=bound;		        //波特率
	UART7_RS485Handler.Init.WordLength=UART_WORDLENGTH_8B;	//字长为8位数据格式
	UART7_RS485Handler.Init.StopBits=UART_STOPBITS_1;		//一个停止位
	UART7_RS485Handler.Init.Parity=UART_PARITY_NONE;		//无奇偶校验位
	UART7_RS485Handler.Init.HwFlowCtl=UART_HWCONTROL_NONE;	//无硬件流控
	UART7_RS485Handler.Init.Mode=UART_MODE_TX_RX;		    //收发模式
	HAL_UART_Init(&UART7_RS485Handler);			        //HAL_UART_Init()会使能USART2
    
    __HAL_UART_CLEAR_IT(&UART7_RS485Handler,UART_CLEAR_TCF);

	__HAL_UART_ENABLE_IT(&UART7_RS485Handler,UART_IT_RXNE);//开启接收中断
	HAL_NVIC_EnableIRQ(UART7_IRQn);				        //使能USART1中断
	HAL_NVIC_SetPriority(UART7_IRQn,3,3);			        //抢占优先级3，子优先级3

}

//RS485发送len个字节.
//buf:发送区首地址
//len:发送的字节数(为了和本代码的接收匹配,这里建议不要超过64个字节)
void RS485_Send_Data(u8 *buf,u8 len)
{
    HAL_UART_Transmit(&UART7_RS485Handler,buf,len,1000);//串口2发送数据
	RS485_RX_CNT=0;	  
}
//RS485查询接收到的数据
//buf:接收缓存首地址
//len:读到的数据长度
void RS485_Receive_Data(u8 *buf,u8 *len)
{
	u8 rxlen=RS485_RX_CNT;
	u8 i=0;
	*len=0;				//默认为0
	delay_ms(10);		//等待10ms,连续超过10ms没有接收到一个数据,则认为接收结束
	if(rxlen==RS485_RX_CNT&&rxlen)//接收到了数据,且接收完成了
	{
		for(i=0;i<rxlen;i++)
		{
			buf[i]=RS485_RX_BUF[i];	
		}		
		*len=RS485_RX_CNT;	//记录本次数据长度
		RS485_RX_CNT=0;		//清零
	}
} 

