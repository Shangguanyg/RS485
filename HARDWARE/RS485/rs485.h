#ifndef __RS485_H
#define __RS485_H
#include "sys.h"
#define RS485_REC_BUFF_SIZE				100
#define RS485_END_FLAG1	'#'			//RS485一桢数据结束标志1 
#define RS485_END_FLAG2	'*'			//RS485一桢数据结束标志2
extern volatile unsigned char RS485_REC_Flag ;
extern volatile unsigned char RS485_buff[RS485_REC_BUFF_SIZE] ;//用于接收数据
extern volatile unsigned int RS485_rec_counter ;//用于RS485接收计数

extern u8 RS485_RX_BUF[64]; 		//接收缓冲,最大64个字节
extern u8 RS485_RX_CNT;   			//接收到的数据长度

void RS485_Init(u32 bound);
void RS485_Send_Data(u8 *buf,u8 len);
void RS485_Receive_Data(u8 *buf,u8 *len);	

#endif
