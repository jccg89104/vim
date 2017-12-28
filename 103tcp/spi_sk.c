/*
* Sample application that makes use of the SPIDEV interface
* to access an SPI slave device. Specifically, this sample
* reads a Device ID of a JEDEC-compliant SPI Flash device.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "w5500.h"
#include "socket.h"
#include "types.h"
#include "tcp_demo.h"
#include "iec103.h"


#define ADDR	0x01




/*??貌?MAC渭??路,猫?1??脿?茅W5500铆???锚锚??掳??煤铆?貌???3?1鈧椕凤俊???锚1贸?2?铆?渭?MAC渭??路*/
uint8 mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11};

/*??貌???猫?IPD??锟?*/
uint8 local_ip[4]={192,168,1,88};												/*??貌?W5500??猫?IP渭??路*/
uint8 subnet[4]={255,255,255,0};												/*??貌?W5500??猫?脳贸铆??煤??*/
uint8 gateway[4]={192,168,1,1};													/*??貌?W5500??猫?铆?1?*/
uint8 dns_server[4]={114,114,114,114};				/*??貌?W5500??猫?DNS*/


uint16 local_port=5000;        
/*??貌?????IPD??锟?*/
uint8  remote_ip[4]={192,168,1,48};				        /*????IP渭??路*/
uint16 remote_port=6000;						/*???????煤o?*/

uint8 buff[2048];				                            /*??貌?貌???2KB渭??o??*/
uint16 SSIZE[MAX_SOCK_NUM]={0,0,0,0,0,0,0,0}; // Max Tx buffer
uint16 RSIZE[MAX_SOCK_NUM]={0,0,0,0,0,0,0,0}; // Max Rx buffer
uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};//tx buffer set	K bits
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};//rx buffet set  K bits

static char ip[20]={0};

/*专用数据*/
unsigned char short_frame[] = {0x10,0x00,ADDR,0x00,0x16};//固定帧长

unsigned char asdu_5[] = {0x68,0x11,0x11,0x68,0x28,ADDR,
						  0x05,0x81,0x05,ADDR,0xFF,0x04,0x03,
						  'A','-','E','N','E','R','G','Y',			  						  
						   0x00,0x16};//标识报文

unsigned char asdu_8[] = {  0x68,0x09,0x09,0x68,0x08,ADDR,			  						  
							0x08,0x81,0x0a,ADDR,0xFF,0x00,0x00,			  						  
							0x00,0x16};//总查询结束（终止）				

unsigned char asdu_15[]= {  0x68,0x1c,0x1c,0x68,0x28,ADDR,			  	                      
							0x0f,0x0a,0x02,ADDR,0x01,0x5c,			  	                      
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	                     
							0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			 	                     
							0x00,0x16};//被测值III格式/11组遥测值

unsigned char asdu_40[]= {  0x68,0x11,0x11,0x68,0x08,ADDR,			  	                      
							0x28,0x81,0x01,ADDR,0x00,0x00,0x00,0x00,			  	                      
							0x00,0x16};//上送变位遥信

unsigned char asdu_41[]= {  0x68,0x0e,0x0e,0x68,0x08,ADDR,			  	                      
							0x29,0x81,0x01,ADDR,0x01,0x5c,			  	                      
							0x00,0x00,0x00,0x00,0x00,0x00,			  	                      
							0x00,0x16};//上送SOE

unsigned char asdu_44[]= {  0x68,0x18,0x18,0x68,0x08,ADDR,			  	                      
							0x2c,0x2e,0x09,ADDR,0x01,0x01,			  	                      
							0x00,0x00,0x00,0x00,0x00,0x00,			  	                      
							0x00,0x16};//上送全遥信/8组遥信量

/*通用数据*/
unsigned char asdu10_title[] = {0x68,0x00,0x00,0x68,0x08,ADDR,				
                                0x0a,0x81,0x2a,ADDR,0xfe,0xf0,0x00,0x0b,			  	
                                0x00,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x01,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x02,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x03,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x04,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x05,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x06,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x07,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x08,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,	 		  	
                                0x09,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x0a,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			  	
                                0x00,0x68};//装置响应的读全部标题

unsigned char asdu10_00desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,				
	                            0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x04,				
	                            0x00,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,				
	                            0x00,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,				
	                            0x00,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,				
	                            0x00,0x03,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,				
	                            0x00,0x68};//装置响应的读00 00组的描述(有源模块电压电流)

unsigned char asdu10_01desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,			     	
	                            0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x06,			     	
	                            0x01,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x01,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x01,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x01,0x03,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x01,0x04,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x01,0x05,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x00,0x68};//装置响应的读01 00组的描述(负载电流与装置电流)

unsigned char asdu10_02desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,			     	
	                            0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x09,			     	
	                            0x02,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x02,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x02,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x02,0x03,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x02,0x04,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x02,0x05,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x02,0x06,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x02,0x07,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x02,0x08,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x00,0x68};//装置响应的读02 00组的描述(三相功率与功率因数)

unsigned char asdu10_03desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,			     	
	                            0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x06,			     	
	                            0x03,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x03,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x03,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x03,0x03,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x03,0x04,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x03,0x05,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
	                            0x00,0x68};//装置响应的读03 00组的描述(电流电压畸变率)
	                            
unsigned char asdu10_04desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,			     	
								0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x19,			     	
								0x04,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x03,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x04,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x05,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x06,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x07,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x08,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x09,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x0a,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x0b,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x0c,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x0d,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x0e,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x0f,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x10,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x11,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x12,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x13,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x14,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x15,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x16,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x17,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x04,0x18,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x00,0x68};//装置响应的读04 00组的描述(系统电压基波与谐波)

unsigned char asdu10_05desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,			     	
								0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x19,			     	
								0x05,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x03,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x04,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x05,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x06,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x07,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x08,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x09,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x0a,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x0b,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x0c,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x0d,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x0e,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x0f,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x10,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x11,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x12,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x13,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x14,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x15,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x16,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x17,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x05,0x18,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x00,0x68};//装置响应的读05 00组的描述(系统电流基波与谐波)

unsigned char asdu10_06desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,			     	
								0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x03,			     	
								0x06,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x06,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x06,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x00,0x68};//装置响应的读06 00组的描述(无源模块输出电流)

unsigned char asdu10_07desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,			     	
								0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x0a,			     	
								0x07,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x07,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x07,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x07,0x03,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x07,0x04,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x07,0x05,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x07,0x06,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x07,0x07,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x07,0x08,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x07,0x09,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x00,0x68};//装置响应的读07 00组的描述(有源运行状态)

unsigned char asdu10_08desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,			     	
								0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x0c,			     	
								0x08,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x03,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x04,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x05,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x06,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x07,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x08,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x09,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x0a,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x08,0x0b,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x00,0x68};//装置响应的读08 00组的描述(无源电容开关状态)

unsigned char asdu10_09desc[]= {0x68,0x00,0x00,0x68,0x08,ADDR,			     	
								0x0a,0x81,0x0a,ADDR,0xfe,0xf1,0x00,0x0c,			     	
								0x09,0x00,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x01,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x02,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x03,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x04,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x05,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x06,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x07,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x08,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x09,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x0a,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x09,0x0b,0x0a,0x01,0x08,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,			     	
								0x00,0x68};//装置响应的读09 00组的描述(无源电容故障状态)






void delay10ms();

int main(int argc, char **argv)
{
/*
* This assumes that a 'mdev -s' has been run to create
* /dev/spidev* devices after the kernel bootstrap.
* First number is the "bus" (SPI contoller id), second number
* is the "chip select" of the specific SPI slave
* ...
* char *name = "/dev/spidev1.1";
*/
char *name;
int fd;
struct spi_ioc_transfer xfer[2];
unsigned char buf[32], *bp;
int len, status;

name = argv[1];
fd = open(name, O_RDWR);
if (fd < 0) {
perror("open");
return 1;
}
//printf("fd=%d",fd);

	memset(xfer, 0, sizeof xfer);
	memset(buf, 0, sizeof buf);
	len = sizeof buf;

/*
* set_mac mac[6]={0x00,0x08,0xdc,0x11,0x11,0x11}
*/


     wiz_write_buf(SHAR0, mac, 6,fd);



     wiz_write_buf(SUBR0, subnet, 4,fd);
	  
/*
* set_gar0 gateway[4]={192,168,1,1}
*/


		wiz_write_buf(GAR0, gateway, 4,fd);
/*
* set_sipr0 local_ip[4]={192,168,1,88}
*/

	    wiz_write_buf(SIPR0, local_ip, 4,fd); 

		socket_buf_init(txsize, rxsize,fd);    /*init socket buf tx & rx*/
		

		while(1)                            /*?-?路??DD渭?o藟锚y*/ 
		{
			do_tcp_client(fd);                  /*TCP_Client 锚y?Y???路2a锚?3矛D貌*/
		}
	}

/**
*@brief		This function set the transmit & receive buffer size as per the channels is used
*@Note: 	TMSR and RMSR bits are as follows\n
					Maximum memory size for Tx, Rx in the W5500 is 16K Bytes,\n
					In the range of 16KBytes, the memory size could be allocated dynamically by each channel.\n
					Be attentive to sum of memory size shouldn't exceed 8Kbytes\n
					and to data transmission and receiption from non-allocated channel may cause some problems.\n
					If the 16KBytes memory is already  assigned to centain channel, \n
					other 3 channels couldn't be used, for there's no available memory.\n
					If two 4KBytes memory are assigned to two each channels, \n
					other 2 channels couldn't be used, for there's no available memory.\n
*@param		tx_size: tx buffer size to set=tx_size[s]*(1024)
*@param		rx_size: rx buffer size to set=rx_size[s]*(1024)
*@return	None
*/
void socket_buf_init( uint8 * tx_size, uint8 * rx_size ,int fd )
{
  int16 i;
  int16 ssum=0,rsum=0;

/*  struct spi_ioc_transfer xfer[2];
  unsigned char buf[32], *bp;
  int len, status;*/
  
 
  for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
  {
			  /*
			* set_socket tx buf size
			*/
/*					buf[0] = 0x00;
					buf[1] = 0x1f;
					buf[2] = 0x0c+i*(0x20);
					buf[3] = 0x02;
					
//					len = 6;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

            /*
			* set_socket rx buf size
			*/
/*					buf[0] = 0x00;
					buf[1] = 0x1e;
					buf[2] = 0x0c+i*(0x20);
					buf[3] = 0x02;
					
					len = 6;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}*/
		IINCHIP_WRITE( (Sn_TXMEM_SIZE(i)), tx_size[i],fd);
                IINCHIP_WRITE( (Sn_RXMEM_SIZE(i)), rx_size[i],fd);


//				printf("read the tx size ");
//printf("tx_size[%d]: %d, Sn_TXMEM_SIZE = %d\r\n",i, tx_size[i], IINCHIP_READ(Sn_TXMEM_SIZE(i),fd));
//printf("rx_size[%d]: %d, Sn_RXMEM_SIZE = %d\r\n",i, rx_size[i], IINCHIP_READ(Sn_RXMEM_SIZE(i)));
//	                    buf[0] = 0x00;       /*read the tx size*/
//			    buf[1] = 0x1f;
//				buf[2] = 0x08+i*(0x20);
//////				buf[3] = 0x00;			
	
//				len = 3;
//				xfer[0].tx_buf = (unsigned long)buf;
//				xfer[0].len = 3;
	
				
//				xfer[1].rx_buf = (unsigned long) buf;
//				xfer[1].len = 3;
	
//				status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
//				if (status < 0) {
//				perror("SPI_IOC_MESSAGE");
//				return;
//				}
				
//	printf("response(%d): ", status);
//	for (bp = buf; len; len--)
//	printf("%02x ", *bp++);
//	//printf("flag=%d",flag++);
//	printf("\n");



          

    SSIZE[i] = (int16)(0);
    RSIZE[i] = (int16)(0);
	
    if (ssum <= 16384)
    {
			SSIZE[i] = (int16)tx_size[i]*(1024);
    }

		if (rsum <= 16384)
		{
			RSIZE[i]=(int16)rx_size[i]*(1024);
    }
			ssum += SSIZE[i];
			rsum += RSIZE[i];

  }
}

/**
*@brief 	This function is to get socket status
*@param		s: socket number
*@return  socket status
*/
uint8 getSn_SR(SOCKET s,int fd)
{
/*	struct spi_ioc_transfer xfer[2];
    unsigned char buf[32], *bp;
    int len, status;
    int flag=0;

//	printf("fd2222=%d",fd);
  
	        buf[0] = 0x00;
			buf[1] = 0x03;
			buf[2] = 0x08+s*(0x20);
//			buf[3] = 0x00;			

			len = 3;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 3;

			
			xfer[1].rx_buf = (unsigned long) buf;
			xfer[1].len = 1;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
			
printf("response(%d): ", status);
for (bp = buf; len; len--)
printf("%02x ", *bp++);
//printf("flag=%d",flag++);
printf("\n");

return buf[0];*/
	return IINCHIP_READ(Sn_SR(s),fd);

}

/**
*@brief		This Socket function initialize the channel in perticular mode, 
*			    and set the port and wait for W5200 done it.
*@param		s: socket number.
*@param		protocol: The socket to chose.
*@param		port:The port to bind.
*@param		flag: Set some bit of MR,such as **< No Delayed Ack(TCP) flag.
*@return  	1 for sucess else 0.
*/
uint8 socket(SOCKET s, uint8 protocol, uint16 port, uint8 flag,int fd)
{
   uint8 ret;
   struct spi_ioc_transfer xfer[2];
    unsigned char buf[32], *bp;
    int len, status;
	
   if (
        ((protocol&0x0F) == Sn_MR_TCP)    ||
        ((protocol&0x0F) == Sn_MR_UDP)    ||
        ((protocol&0x0F) == Sn_MR_IPRAW)  ||
        ((protocol&0x0F) == Sn_MR_MACRAW) ||
        ((protocol&0x0F) == Sn_MR_PPPOE)
      )
   {
      closesk(s,fd);
	  						

 //     printf("i am here ***************************** ");
//      IINCHIP_WRITE(Sn_MR(s) ,protocol | flag);
					buf[0] = 0x00;
					buf[1] = 0x00;
					buf[2] = 0x0c+s*(0x20);
					buf[3] = protocol | flag;
					
//					len = 6;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

		/* set the source port, set local_port number*/
					buf[0] = 0x00;
					buf[1] = 0x04;
					buf[2] = 0x0c+s*(0x20);
					buf[3] = 0x13;
					
//					len = 6;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
			
					buf[0] = 0x00;
					buf[1] = 0x05;
					buf[2] = 0x0c+s*(0x20);
					buf[3] = 0x88;
					
//					len = 6;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
	
     
 //     IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_OPEN); 			/*Run sockinit Sn_CR*/
					buf[0] = 0x00;
					buf[1] = 0x01;
					buf[2] = 0x0c+s*(0x20);
					buf[3] = 0x01;
					
//					len = 6;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
 
			/*Wait to process the command*/

					buf[0] = 0x00;
					buf[1] = 0x01;
					buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
					len = 3;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 3;
					
					xfer[1].rx_buf = (unsigned long) buf;
			                xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/
     
      while( buf[0]);					/*Wait to process the command*/
         
      ret = 1;
   }
   else
   {
      ret = 0;
   }
   return ret;
}

/**
*@brief		This function close the socket and parameter is "s" which represent the socket number
*@param		s: socket number.
*@return	None
*/
void closesk(SOCKET s,int fd)
{
	struct spi_ioc_transfer xfer[2];
    unsigned char buf[32], *bp;
    int len, status;

	memset(xfer, 0, sizeof xfer);
	memset(buf, 0, sizeof buf);
	len = sizeof buf;
	
  // IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_CLOSE);
   					buf[0] = 0x00;
					buf[1] = 0x01;
					buf[2] = 0x0c+s*(0x20);
					buf[3] = 0x10;
					
//					len = 6;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
           for (bp = buf; xfer[0].len; xfer[0].len--)
             printf("%02x ", *bp++);
                printf("\n");*/

  					buf[0] = 0x00;
					buf[1] = 0x01;
					buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
					len = 3;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 3;
					
					xfer[1].rx_buf = (unsigned long) buf;
			        xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/
	
//printf("i am here11111111111***************************** ");	   
   while( buf[0]);						/*Wait to process the command*/
       
//	IINCHIP_WRITE( Sn_IR(s) , 0xFF);					/*All clear*/
					buf[0] = 0x00;
					buf[1] = 0x02;
					buf[2] = 0x0c+s*(0x20);
					buf[3] = 0xff;
					
					len = 6;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
//				 printf("i am here222222***************************** ");
}

/**
*@brief		This function established  the connection for the channel in Active (client) mode.
*			This function waits for the untill the connection is established.
*@param		s: socket number.
*@param		addr: The server IP address to connect.
*@param		port: The server IP port to connect.
*@return  	1 for success else 0.
*/
uint8 connect(SOCKET s, uint8 * addr, uint16 port,int fd)
{
    uint8 ret;

	struct spi_ioc_transfer xfer[2];
		unsigned char buf[32], *bp;
		int len, status;

		memset(xfer, 0, sizeof xfer);
	    memset(buf, 0, sizeof buf);
	    len = sizeof buf;

	
    if
        (
            ((addr[0] == 0xFF) && (addr[1] == 0xFF) && (addr[2] == 0xFF) && (addr[3] == 0xFF)) ||
            ((addr[0] == 0x00) && (addr[1] == 0x00) && (addr[2] == 0x00) && (addr[3] == 0x00)) ||
            (port == 0x00)
        )
    {
      ret = 0;
    }
    else
    {
        ret = 1;
 //       IINCHIP_WRITE( Sn_DIPR0(s), addr[0]);
			buf[0] = 0x00;
			buf[1] = 0x0c;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = 0xc0;
					
//			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
 //       IINCHIP_WRITE( Sn_DIPR1(s), addr[1]);
			buf[0] = 0x00;
			buf[1] = 0x0d;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = 0xa8;
					
//			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
  //      IINCHIP_WRITE( Sn_DIPR2(s), addr[2]);
			buf[0] = 0x00;
			buf[1] = 0x0e;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = 0x01;
					
//			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}	
  //      IINCHIP_WRITE( Sn_DIPR3(s), addr[3]);
			buf[0] = 0x00;
			buf[1] = 0x0f;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = 0x30;   //192.168.1.48 remote ip
					
//			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
//        IINCHIP_WRITE( Sn_DPORT0(s), (uint8)((port & 0xff00) >> 8));
			buf[0] = 0x00;
			buf[1] = 0x10;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = 0x17;
					
//			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
 //       IINCHIP_WRITE( Sn_DPORT1(s), (uint8)(port & 0x00ff));
			buf[0] = 0x00;
			buf[1] = 0x11;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = 0x70;
					
//			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
 //       IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_CONNECT);
			buf[0] = 0x00;
			buf[1] = 0x01;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = 0x04;
					
//			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

				/*Wait for completion*/
  					buf[0] = 0x00;
					buf[1] = 0x01;
					buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
					len = 3;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 3;
					
					xfer[1].rx_buf = (unsigned long) buf;
			        xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/
        
        while ( buf[0] ) ;							/*Wait for completion*/


		/*IINCHIP_READ(Sn_SR(s)) != SOCK_SYNSENT*/
					buf[0] = 0x00;
					buf[1] = 0x03;
					buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
					len = 3;
					xfer[0].tx_buf = (unsigned long)buf;
					xfer[0].len = 3;
					
					xfer[1].rx_buf = (unsigned long) buf;
			                xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/
        while ( buf[0] != SOCK_SYNSENT )
        {

	            buf[0] = 0x00;
		    buf[1] = 0x03;
		    buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len = 3;
		   xfer[0].tx_buf = (unsigned long)buf;
		   xfer[0].len = 3;
					
		   xfer[1].rx_buf = (unsigned long) buf;
		   xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/
            if(buf[0] == SOCK_ESTABLISHED)
            {
                break;
            }

		    buf[0] = 0x00;
		    buf[1] = 0x02;
		    buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len = 3;
		   xfer[0].tx_buf = (unsigned long)buf;
		   xfer[0].len = 3;
					
		   xfer[1].rx_buf = (unsigned long) buf;
		   xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/
            if (buf[0] & Sn_IR_TIMEOUT)
            {
 //               IINCHIP_WRITE(Sn_IR(s), (Sn_IR_TIMEOUT));  			/*Clear TIMEOUT Interrupt*/
		        buf[0] = 0x00;
			buf[1] = 0x02;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = 0x08;
					
//			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
                ret = 0;
                break;
            }
        }
    }

   return ret;
}

/**
*@brief		This fuction is to give size of received data in receive buffer.
*@param		s: socket number
*@return  socket TX free buf size
*/
uint16 getSn_RX_RSR(SOCKET s,int fd)
{
  uint16 val=0,val1=0;
  struct spi_ioc_transfer xfer[2];
		  unsigned char buf[32], *bp;
		  int len1, status;

           memset(xfer, 0, sizeof xfer);
		  memset(buf, 0, sizeof buf);
		  len1= sizeof buf;

  
  do
  {
//    val1 = IINCHIP_READ(Sn_RX_RSR0(s));
		    buf[0] = 0x00;
		    buf[1] = 0x26;
		    buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len1= 3;
		   xfer[0].tx_buf = (unsigned long)buf;
		   xfer[0].len = 3;
					
		   xfer[1].rx_buf = (unsigned long) buf;
		   xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/

    		val1=buf[0];

            buf[0] = 0x00;
		    buf[1] = 0x27;
		    buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len1= 3;
		   xfer[0].tx_buf = (unsigned long)buf;
		   xfer[0].len = 3;
					
		   xfer[1].rx_buf = (unsigned long) buf;
		   xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/

    val1 = (val1 << 8) + buf[0];
    if(val1 != 0)
    {
 //       val = IINCHIP_READ(Sn_RX_RSR0(s));
		    buf[0] = 0x00;
		    buf[1] = 0x26;
		    buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len1= 3;
		   xfer[0].tx_buf = (unsigned long)buf;
		   xfer[0].len = 3;
					
		   xfer[1].rx_buf = (unsigned long) buf;
		   xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/

    		val=buf[0];

		    buf[0] = 0x00;
		    buf[1] = 0x27;
		    buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len1= 3;
		   xfer[0].tx_buf = (unsigned long)buf;
		   xfer[0].len = 3;
					
		   xfer[1].rx_buf = (unsigned long) buf;
		   xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");*/

        val = ((val << 8) + buf[0]);
    }
  } while (val != val1);
   return val;
}

/**
*@brief		This function is an application I/F function which is used to receive the data in TCP mode.
*			It continues to wait for data as much as the application wants to receive.
*@param		s: socket number.
*@param		buf: data buffer to receive.
*@param		len: data length.
*@return  	received data size for success else 0.
*/
uint16 recv(SOCKET s, uint8 * buf, uint16 len,int fd)
{

   uint16 ret=0;
   if ( len > 0 )
   {

      recv_data_processing(s, buf, len,fd);


      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_RECV,fd);

      while( IINCHIP_READ(Sn_CR(s),fd));								 /*Wait to process the command*/

	  
      ret = len;
   }
   printf("ret=%d",ret);
   return ret;
}

/**
*@brief  	This function is being called by recv() also.
					This function read the Rx read pointer register
					and after copy the data from receive buffer update the Rx write pointer register.
					User should read upper byte first and lower byte later to get proper value.
*@param		s: socket number
*@param		data: data buffer to receive
*@param		len: data length
*@return  None
*/
void recv_data_processing(SOCKET s, uint8 *data, uint16 len,int fd)
{
  uint16 ptr = 0;
  uint32 addrbsb = 0;
  
  if(len == 0)
  {
    printf("CH: %d Unexpected2 length 0\r\n", s);
    return;
  }

  ptr = IINCHIP_READ( Sn_RX_RD0(s),fd);
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ( Sn_RX_RD1(s),fd );

  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x18;
  wiz_read_buf(addrbsb, data, len,fd);
  ptr += len;

  IINCHIP_WRITE( Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8),fd);
  IINCHIP_WRITE( Sn_RX_RD1(s), (uint8)(ptr & 0x00ff),fd);
}


/**
*@brief   This function used to send the data in TCP mode
*@param		s: socket number.
*@param		buf: data buffer to send.
*@param		len: data length.
*@return  1 for success else 0.
*/
uint16 send(SOCKET s, const uint8 * buf, uint16 len,int fd)
{
  uint8 status=0;
  uint16 ret=0;
  uint16 freesize=0;



  if (len > SSIZE[s]) ret = SSIZE[s];	 		/*Check size not to exceed MAX size*/
  else ret = len;

 do
  {
    freesize = getSn_TX_FSR(s,fd);
    status = IINCHIP_READ(Sn_SR(s),fd);
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
    {
      ret = 0;
      break;
    }
  } while (freesize < ret);  
  
  send_data_processing(s, (uint8 *)buf, ret,fd);						 /*Copy data*/
  IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_SEND,fd);
  																	 /*Wait to process the command*/
  while( IINCHIP_READ(Sn_CR(s),fd) );

  while ( (IINCHIP_READ(Sn_IR(s) ,fd) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
  {
    status = IINCHIP_READ(Sn_SR(s),fd);
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT) )
    {
      printf("SEND_OK Problem!!\r\n");
      closesk(s,fd);
      return 0;
    }
  }
  IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK,fd);

#ifdef __DEF_IINCHIP_INT__
   putISR(s, getISR(s) & (~Sn_IR_SEND_OK));
#else
   IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK,fd);
#endif

   return ret;

}

/**
*@brief		This fuction is to get socket TX free buf size
					This gives free buffer size of transmit buffer. This is the data size that user can transmit.
					User shuold check this value first and control the size of transmitting data
*@param		s: socket number
*@return  socket TX free buf size
*/
uint16 getSn_TX_FSR(SOCKET s,int fd)
{
  uint16 val=0,val1=0;
  do
  {
    val1 = IINCHIP_READ(Sn_TX_FSR0(s),fd);
    val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR1(s),fd);
      if (val1 != 0)
    {
        val = IINCHIP_READ(Sn_TX_FSR0(s),fd);
        val = (val << 8) + IINCHIP_READ(Sn_TX_FSR1(s),fd);
    }
  } while (val != val1);
   return val;
}

/**
*@brief   This function is being called by send() and sendto() function also.

					This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
					register. User should read upper byte first and lower byte later to get proper value.
*@param		s: socket number
*@param		data: data buffer to send
*@param		len: data length
*@return  socket TX free buf size
*/
void send_data_processing(SOCKET s, uint8 *data, uint16 len,int fd)
{
   uint16 ptr =0;
  uint32 addrbsb =0;
  if(len == 0)
  {
    printf("CH: %d Unexpected1 length 0\r\n", s);
    return;
  }
   
  ptr = IINCHIP_READ( Sn_TX_WR0(s),fd );
  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR1(s),fd);

  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x10;
  wiz_write_buf(addrbsb, data, len,fd);
  
  ptr += len;
  IINCHIP_WRITE( Sn_TX_WR0(s) ,(uint8)((ptr & 0xff00) >> 8),fd);
  IINCHIP_WRITE( Sn_TX_WR1(s),(uint8)(ptr & 0x00ff),fd);
}

/**
*@brief		D?猫?貌???8??锚y?Y渭?W5500
*@param		addrbsb: D?猫?锚y?Y渭?渭??路
*@param   data锟D?猫?渭?8??锚y?Y
*@return	?T
*/
void IINCHIP_WRITE( uint32 addrbsb,  uint8 data,int fd)
{
                          		
/*   IINCHIP_SpiSendData( ((addrbsb & 0x00FF0000)>>16),fd);	
   IINCHIP_SpiSendData(( (addrbsb & 0x0000FF00)>> 8),fd);
   IINCHIP_SpiSendData( ((addrbsb & 0x000000F8) + 4),fd);  
   IINCHIP_SpiSendData(data,fd);*/
    struct spi_ioc_transfer xfer[2];
	unsigned char buf[2048], *bp;
	int len, status;

	memset(xfer, 0, sizeof xfer);
	memset(buf, 0, sizeof buf);
	len = sizeof buf;

   buf[0] = (addrbsb & 0x00FF0000)>>16;
   buf[1] = (addrbsb & 0x0000FF00)>> 8;
   buf[2] = (addrbsb & 0x000000F8) + 4;
   buf[3] =  data;

   xfer[0].tx_buf = (unsigned long)buf;
   xfer[0].len = 4;

   status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
   
if (status < 0) {
perror("SPI_IOC_MESSAGE");
return;
}	
                            
}


/**
*@brief		?貌W5500D?猫?len1脳??煤锚y?Y
*@param		addrbsb: D?猫?锚y?Y渭?渭??路
*@param   buf锟D?猫?脳?路???
*@param   len1锟脳?路???3鈧?猫
*@return	len1锟路渭??脳?路???3鈧?猫
*/
uint16 wiz_write_buf(uint32 addrbsb,uint8* buf1,uint16 len1,int fd)
{
   uint16 idx = 0;
   if(len1== 0) printf("Unexpected2 length 0\r\n");
   struct spi_ioc_transfer xfer[2];
	unsigned char buf[2048], *bp;
	int len, status;

	memset(xfer, 0, sizeof xfer);
	memset(buf, 0, sizeof buf);
	len = sizeof buf; 

   buf[0] = (addrbsb & 0x00FF0000)>>16;
   buf[1] = (addrbsb & 0x0000FF00)>> 8;
   buf[2] = (addrbsb & 0x000000F8) + 4;
  
   for(idx = 0; idx < len1; idx++)
   {
     buf[3+idx] =  buf1[idx];
//     IINCHIP_SpiSendData(buf1[idx]);
   }
   xfer[0].tx_buf = (unsigned long)buf;
   xfer[0].len = 3+len1;

   status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
   if (status < 0) {
   perror("SPI_IOC_MESSAGE");
   return;
   }   

   return len1;  
}

/**
*@brief		?贸W5500?谩3?貌???8??锚y?Y
*@param		addrbsb: D?猫?锚y?Y渭?渭??路
*@param   data锟?贸D?猫?渭?渭??路???谩猫?渭?渭?8??锚y?Y
*@return	?T
*/
uint8 IINCHIP_READ(uint32 addrbsb,int fd)
{
   uint8 data = 0;
   struct spi_ioc_transfer xfer[2];
	unsigned char buf[2048], *bp;
	int len, status;

	memset(xfer, 0, sizeof xfer);
	memset(buf, 0, sizeof buf);
	len = sizeof buf;
                          
//   IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
//   IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
//   IINCHIP_SpiSendData( (addrbsb & 0x000000F8))    ;
      buf[0] = (addrbsb & 0x00FF0000)>>16;
	  buf[1] = (addrbsb & 0x0000FF00)>> 8;
	  buf[2] = (addrbsb & 0x000000F8) ;

//   data = IINCHIP_SpiSendData(0x00);  
   
 	

			len = 3;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 3;

			
			xfer[1].rx_buf = (unsigned long) buf;
			xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
			
/*printf("response(%d): ", status);
for (bp = buf; len; len--)
printf("%02x ", *bp++);
//printf("flag=%d",flag++);
printf("\n");*/

   data=buf[0];                           
   return data;    
}

/**
*@brief		?贸W5500?谩3?len脳??煤锚y?Y
*@param		addrbsb: ?谩猫?锚y?Y渭?渭??路
*@param 	buf锟??路??谩猫?锚y?Y
*@param		len锟脳?路???3鈧?猫
*@return	len锟路渭??脳?路???3鈧?猫
*/
uint16 wiz_read_buf(uint32 addrbsb, uint8* buf1,uint16 len1,int fd)
{
  uint16 idx = 0;
  struct spi_ioc_transfer xfer[2];
	unsigned char buf[2048], *bp;
	int len, status;

	memset(xfer, 0, sizeof xfer);
	memset(buf, 0, sizeof buf);
	len= sizeof buf;
  
  if(len1== 0)
  {
    printf("Unexpected2 length 0\r\n");
  }
//  iinchip_csoff();                                
/*  IINCHIP_SpiSendData( (addrbsb & 0x00FF0000)>>16);
  IINCHIP_SpiSendData( (addrbsb & 0x0000FF00)>> 8);
  IINCHIP_SpiSendData( (addrbsb & 0x000000F8));*/
  
	buf[0] = (addrbsb & 0x00FF0000)>>16;
	buf[1] = (addrbsb & 0x0000FF00)>> 8;
	buf[2] = (addrbsb & 0x000000F8) ;

	len = len1;
	xfer[0].tx_buf = (unsigned long)buf;
	xfer[0].len = 3;

			
	xfer[1].rx_buf = (unsigned long) buf;
	xfer[1].len = len1;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
			
printf("response(%d): ", status);
for (bp = buf; len; len--)
printf("%02x ", *bp++);
//printf("flag=%d",flag++);
printf("\n");


  for(idx = 0; idx < len1; idx++)                   
  {
    buf1[idx] = buf[idx];
  }
//  iinchip_cson();                                  
  return len1;
}

void delay10ms()
{
unsigned char i,j,k;
for(i=5;i>0;i--)
for(j=4;j>0;j--)
for(k=248;k>0;k--);
}


/**
*@brief		TCP Client???路?Y锚?o藟锚y?锟?
*@param		?T
*@return	?T
*/
void do_tcp_client(int fd)
{	
   uint16 len=0;	


	switch(getSn_SR(SOCK_TCPC,fd))  /*??猫?socket渭?脳?矛?*/
	{
		case SOCK_CLOSED:											        		         /*socket??贸煤1?卤?脳?矛?*/


		socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND,fd);
//			printf("i am here11111111111***************************** ");
			
		        break;
		
		case SOCK_INIT:													        	         /*socket??贸煤3?锚??藟脳?矛?*/
			
			
			connect(SOCK_TCPC,remote_ip,remote_port,fd);                /*socket谩??贸路t???梅*/ 

		      break;
		
		case SOCK_ESTABLISHED: 			/*socket??贸煤谩??贸??谩锟犆?矛?*/

			

			if((IINCHIP_READ(Sn_IR(SOCK_TCPC),fd))& Sn_IR_CON)
						{
//							setSn_IR(SOCK_TCPC, Sn_IR_CON); 									 /*??3y?贸锚??D??卤锚????*/
							IINCHIP_WRITE(Sn_IR(SOCK_TCPC),Sn_IR_CON,fd);
						}


			len=getSn_RX_RSR(SOCK_TCPC,fd); 				  /*??貌?len?a貌??贸锚?锚y?Y渭?3鈧?猫*/
//			printf("len=%d",len);
			
	//		printf("i am here len***************************** ");
			if(len>0)
			{
//				recvfrom(SOCK_UDPS,buff, len, remote_ip,&remote_port,fd);               /*W5500接收计算机发送来的数据*/
				recv(SOCK_TCPC,buff,len,fd);          /*W5500接收计算机发送来的数据*/

				/*判断是否为固定帧长*/

				if(buff[0]==0x10 && buff[2]==ADDR)
					{
					    /*遥信变位*/
						if((buff[1]&0x0f)==0x0a)
							{
								//			n = send(sockfd , asdu_40 , asdu_40[1]+6 , 0);//响应1级数据asdu_40
								send(SOCK_TCPC,asdu_40,16,fd);
						    }

						/*遥测*/					
						if((buff[1]&0x0f)==0x0b)					
							{						
//							   sendto(SOCK_UDPS,asdu_15,36, remote_ip, remote_port,fd); //响应2级数据asdu_15	
							   send(SOCK_TCPC,asdu_15,36,fd);
							}

						/*初始化*/
						if(buff[1]==0x40)
							{
								short_frame[1]=0x20;					
								short_frame[2]=ADDR;					
								short_frame[3]=short_frame[1]+short_frame[2];

//								n = send(sockfd , short_frame , 5 , 0);//确认帧，并告知有1级数据
								send(SOCK_TCPC,short_frame,5,fd);

 								recv(SOCK_TCPC,buff,MAX_LINE,fd);          /*W5500接收计算机发送来的数据*/
								if(buff[0]==0x10 && (buff[1]&0x0f)==0x0a && buff[2]==ADDR) 
									{
										send(SOCK_TCPC,asdu_5,23,fd);
								    }
								

						    }
				    }

                /*判断收到的报文是否为可变帧长*/
				if(buff[0]==0x68 && buff[5]==ADDR)
					{
						/*时间同步*/
						if(buff[6]==0x06 && buff[8]==0x08)//ASDU_6
							{
								
						    }

						/*总查询/遥信*/
						if(buff[6]==0x07 && buff[8]==0x09)//判断接收报文是否是ASDU_7
							{
								short_frame[1]=0x20;					
								short_frame[2]=ADDR;					
								short_frame[3]=short_frame[1]+short_frame[2];

								send(SOCK_TCPC,short_frame,5,fd);   //确认帧，并告知有1级数据
								
							//	m = recv(sockfd , buf , MAX_LINE , 0);//请求一级数据

							    recv(SOCK_TCPC,buff,MAX_LINE ,fd); 
							
							if(buff[0]==0x10 && (buff[1]&0x0f)==0x0a && buff[2]==ADDR) 
								{
									
									send(SOCK_TCPC,asdu_44,20,fd);
									recv(SOCK_TCPC,buff,MAX_LINE,fd);
									
									  if(buff[0]==0x10 && (buff[1]&0x0f)==0x0a && buff[2]==ADDR) 
									  	{
											send(SOCK_TCPC,asdu_8,15,fd);

									    }
							    }

							}

				    }

//				buff[len]=0x00;  											                 /*矛铆?贸脳?路????谩锚?路?*/



				printf("%s\r\n",buff);

//				send(SOCK_TCPC,buff,len,fd);			/*?貌Server路锟?铆锚y?Y*/

			}		  
		       break;
			
		case SOCK_CLOSE_WAIT: 					/*socket??贸煤渭猫?y1?卤?脳?矛?*/
			closesk(SOCK_TCPC,fd);
		  break;

	}
}




