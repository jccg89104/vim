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





uint16 local_port=5000;        
/*定义远端IP信息*/
uint8  remote_ip[4]={192,168,1,23};				        /*远端IP地址*/
uint16 remote_port=6000;						/*远端端口号*/

uint8 buff[2048];				                              	         /*定义一个2KB的缓存*/
uint16 SSIZE[MAX_SOCK_NUM]={0,0,0,0,0,0,0,0}; // Max Tx buffer
uint16 RSIZE[MAX_SOCK_NUM]={0,0,0,0,0,0,0,0}; // Max Rx buffer
uint8 txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};//tx buffer set	K bits
uint8 rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};//rx buffet set  K bits


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
	buf[0] = 0x00;
	buf[1] = 0x09;
	buf[2] = 0x04;
	buf[3] = 0x00;
	buf[4] = 0x08;
	buf[5] = 0xdc;
	buf[6] = 0x11;
	buf[7] = 0x11;
	buf[8] = 0x11;
	len = 6;
    xfer[0].tx_buf = (unsigned long)buf;
    xfer[0].len = 9;

status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
if (status < 0) {
perror("SPI_IOC_MESSAGE");
return;
}
//*************				read_mac*******************************test

/*                buf[0] = 0x00;
				buf[1] = 0x09;
				buf[2] = 0x00;
//				buf[3] = 0x00;
					
	
				len = 6;
				xfer[0].tx_buf = (unsigned long)buf;
				xfer[0].len = 4;
	
				
				xfer[1].rx_buf = (unsigned long) buf;
				xfer[1].len = 9;
	
				status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
				if (status < 0) {
				perror("SPI_IOC_MESSAGE");
				return;
				}
				
	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	//printf("flag=%d",flag++);
	printf("\n");*/

/*
* set_subr subnet[4]={255,255,255,0}
*/
	buf[0] = 0x00;
	buf[1] = 0x05;
	buf[2] = 0x04;
	buf[3] = 0xff;
	buf[4] = 0xff;
	buf[5] = 0xff;
	buf[6] = 0x00;

      len = 6;
      xfer[0].tx_buf = (unsigned long)buf;
      xfer[0].len = 7;
      status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
      if (status < 0) {
      perror("SPI_IOC_MESSAGE");
      return;
      }
	  
/*
* set_gar0 gateway[4]={192,168,1,1}
*/
		buf[0] = 0x00;
		buf[1] = 0x01;
		buf[2] = 0x04;
		buf[3] = 0xc0;
		buf[4] = 0xa8;
		buf[5] = 0x01;
		buf[6] = 0x01;

		len = 6;
		xfer[0].tx_buf = (unsigned long)buf;
		xfer[0].len = 7;

		status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
		if (status < 0) {
		perror("SPI_IOC_MESSAGE");
		return;
		}
/*
* set_sipr0 local_ip[4]={192,168,1,88}
*/
	buf[0] = 0x00;
	buf[1] = 0x0f;
	buf[2] = 0x04;
	buf[3] = 0xc0;
	buf[4] = 0xa8;
	buf[5] = 0x01;
	buf[6] = 0x58;

	len = 6;
	xfer[0].tx_buf = (unsigned long)buf;
	xfer[0].len = 7;

	status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
	if (status < 0) {
	perror("SPI_IOC_MESSAGE");
	return;
	}

		socket_buf_init(txsize, rxsize,fd);    /*init socket buf tx & rx*/
		while(1)                            /*循环执行的函数*/ 
		{
			do_tcp_client(fd);                  /*TCP_Client 数据回环测试程序*/
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

  struct spi_ioc_transfer xfer[2];
  unsigned char buf[32], *bp;
  int len, status;
  
 
  for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
  {
			  /*
			* set_socket tx buf size
			*/
					buf[0] = 0x00;
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
					buf[0] = 0x00;
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
			}
//		IINCHIP_WRITE( (Sn_TXMEM_SIZE(i)), tx_size[i]);
//          IINCHIP_WRITE( (Sn_RXMEM_SIZE(i)), rx_size[i]);
				printf("read the tx size ");

	            buf[0] = 0x00;       /*read the tx size*/
			    buf[1] = 0x1f;
				buf[2] = 0x08+i*(0x20);
//				buf[3] = 0x00;			
	
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
				
	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	//printf("flag=%d",flag++);
	printf("\n");



          

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
	struct spi_ioc_transfer xfer[2];
    unsigned char buf[32], *bp;
    int len, status;
    int flag=0;

//	printf("fd2222=%d",fd);
  
	        buf[0] = 0x00;
			buf[1] = 0x03;
			buf[2] = 0x08+s*(0x20);
//			buf[3] = 0x00;			

			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 3;

			
			xfer[1].rx_buf = (unsigned long) buf;
			xfer[1].len = 3;

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

return buf[0];
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
      printf("i am here ***************************** ");
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
     
      while( buf[0])					/*Wait to process the command*/
         ;
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

	printf("response(%d): ", status);
	for (bp = buf; len; len--)
	printf("%02x ", *bp++);
	printf("\n");
	   
   while( buf[0])						/*Wait to process the command*/
       ;
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
			buf[1] = 0x0c;
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
			buf[1] = 0x10;
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
	      struct spi_ioc_transfer xfer[2];
		  unsigned char buf1[32], *bp;
		  int len1, status;
   uint16 ret=0;
   if ( len > 0 )
   {
      recv_data_processing(s, buf, len,fd);
//      IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_RECV);
  	        buf1[0] = 0x00;
			buf1[1] = 0x01;
			buf1[2] = 0x0c+s*(0x20);
			buf1[3] = 0x40;
					
//			len1 = 6;
			xfer[0].tx_buf = (unsigned long)buf1;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}    

		    buf1[0] = 0x00;
		    buf1[1] = 0x01;
		    buf1[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len1 = 3;
		   xfer[0].tx_buf = (unsigned long)buf1;
		   xfer[0].len = 3;
					
		   xfer[1].rx_buf = (unsigned long) buf1;
		   xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len1; len1--)
	printf("%02x ", *bp++);
	printf("\n");*/

      while( buf1[0]);       /*Wait to process the command*/												
      ret = len;
   }
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
  uint16 ptr1 = 0;

  uint32 addrbsb = 0;
  uint16 idx = 0;
  
   struct spi_ioc_transfer xfer[2];
		  unsigned char buf[1024], *bp;
		  int len1, status;
  
  if(len == 0)
  {
    printf("CH: %d Unexpected2 length 0\r\n", s);
    return;
  }

 // ptr = IINCHIP_READ( Sn_RX_RD0(s) );
		    buf[0] = 0x00;
		    buf[1] = 0x28;
		    buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len1 = 3;
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
	for (bp = buf; len1; len1--)
	printf("%02x ", *bp++);
	printf("\n");*/

        ptr =  buf[0];

 // ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ( Sn_RX_RD1(s) );
		    buf[0] = 0x00;
		    buf[1] = 0x29;
		    buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len1 = 3;
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
	for (bp = buf; len1; len1--)
	printf("%02x ", *bp++);
	printf("\n");*/
        ptr1 = buf[0];

  //addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x18;
//  wiz_read_buf(addrbsb, data, len);
		    buf[0] = ptr;
		    buf[1] = ptr1;
		    buf[2] = 0x08+s*(0x20)+0x18;
//		    buf[3] = 0x10;
					
		    len1 = len;
		   xfer[0].tx_buf = (unsigned long)buf;
		   xfer[0].len = len;
					
		   xfer[1].rx_buf = (unsigned long) buf;
		   xfer[1].len = 3;

			status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
     for(idx = 0; idx < len; idx++)                   
  {
    buff[idx] =buf[idx];
  }
/*	printf("response(%d): ", status);
	for (bp = buf; len1; len1--)
	printf("%02x ", *bp++);
	printf("\n");*/

	ptr = ((ptr & 0x00ff) << 8) + ptr1;
        ptr += len;
	
//  IINCHIP_WRITE( Sn_RX_RD0(s), (uint8)((ptr & 0xff00) >> 8));
		    buf[0] = 0x00;
			buf[1] = 0x28;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = (uint8)((ptr & 0xff00) >> 8);
					
//			len1 = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
	
//  IINCHIP_WRITE( Sn_RX_RD1(s), (uint8)(ptr & 0x00ff));
   			buf[0] = 0x00;
			buf[1] = 0x29;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = (uint8)(ptr & 0x00ff);
					
//			len1 = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
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

  struct spi_ioc_transfer xfer[2];
		  unsigned char buf1[32], *bp;
		  int len1, status1;

  if (len > SSIZE[s]) ret = SSIZE[s];	 		/*Check size not to exceed MAX size*/
  else ret = len;

  do
  {
    freesize = getSn_TX_FSR(s,fd);
//    status = IINCHIP_READ(Sn_SR(s));
			buf1[0] = 00;
		    buf1[1] = 03;
		    buf1[2] = 0x08+s*(0x20);

					
		    len1 = 3;
		   xfer[0].tx_buf = (unsigned long)buf1;
		   xfer[0].len = 3;
					
		   xfer[1].rx_buf = (unsigned long) buf1;
		   xfer[1].len = 3;

			status1= ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			if (status1< 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}

/*	printf("response(%d): ", status);
	for (bp = buf; len1; len1--)
	printf("%02x ", *bp++);
	printf("\n");*/

	status = buf1[0];
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT))
    {
      ret = 0;
      break;
    }
  } while (freesize < ret);  
  
  send_data_processing(s, (uint8 *)buf, ret,fd);						 /*Copy data*/
  
//  IINCHIP_WRITE( Sn_CR(s) ,Sn_CR_SEND);
            buf1[0] = 0x00;
			buf1[1] = 0x01;
			buf1[2] = 0x0c+s*(0x20);
			buf1[3] = 0x20;
					
//			len1 = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status1= ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status1< 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}    

		            		buf1[0] = 0x00;
					buf1[1] = 0x01;
					buf1[2] = 0x08+s*(0x20);
		//					buf[3] = 0x10;
							
					len1 = 3;
				   xfer[0].tx_buf = (unsigned long)buf1;
				   xfer[0].len = 3;
							
				   xfer[1].rx_buf = (unsigned long) buf1;
				   xfer[1].len = 3;
		
					status1= ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
					if (status1 < 0) {
					perror("SPI_IOC_MESSAGE");
					return;
					}
		
/*			printf("response(%d): ", status);
			for (bp = buf; len1; len1--)
			printf("%02x ", *bp++);
			printf("\n");*/

  		/*Wait to process the command*/
//  while( IINCHIP_READ(Sn_CR(s) ) );
   while( buf[0]);       /*Wait to process the command*/	
 //  while ( (IINCHIP_READ(Sn_IR(s) ) & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )

                    			buf1[0] = 0x00;
					buf1[1] = 0x02;
					buf1[2] = 0x08+s*(0x20);
		//					buf[3] = 0x10;
							
					len1 = 3;
				   xfer[0].tx_buf = (unsigned long)buf1;
				   xfer[0].len = 3;
							
				   xfer[1].rx_buf = (unsigned long) buf1;
				   xfer[1].len = 3;
		
					status1 = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
					if (status1 < 0) {
					perror("SPI_IOC_MESSAGE");
					return;
					}
		
	/*		printf("response(%d): ", status);
			for (bp = buf; len1; len1--)
			printf("%02x ", *bp++);
			printf("\n");*/
  while ( (buf1[0] & Sn_IR_SEND_OK) != Sn_IR_SEND_OK )
  {
//    status = IINCHIP_READ(Sn_SR(s));
                    buf1[0] = 0x00;
					buf1[1] = 0x03;
					buf1[2] = 0x08+s*(0x20);
		//					buf[3] = 0x10;
							
					len1 = 3;
				   xfer[0].tx_buf = (unsigned long)buf1;
				   xfer[0].len = 3;
							
				   xfer[1].rx_buf = (unsigned long) buf1;
				   xfer[1].len = 3;
		
					status1= ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
					if (status1< 0) {
					perror("SPI_IOC_MESSAGE");
					return;
					}
		
/*			printf("response(%d): ", status);
			for (bp = buf; len1; len1--)
			printf("%02x ", *bp++);
			printf("\n");*/

			status = buf1[0];
    if ((status != SOCK_ESTABLISHED) && (status != SOCK_CLOSE_WAIT) )
    {
      printf("SEND_OK Problem!!\r\n");
      closesk(s,fd);
      return 0;
    }
  }
//  IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);
                        buf1[0] = 0x00;
			buf1[1] = 0x02;
			buf1[2] = 0x0c+s*(0x20);
			buf1[3] = 0x10;
					
//			len1 = 6;
			xfer[0].tx_buf = (unsigned long)buf1;
			xfer[0].len = 4;

			status1= ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status1< 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}    

/*
#ifdef __DEF_IINCHIP_INT__
   putISR(s, getISR(s) & (~Sn_IR_SEND_OK));
#else
   IINCHIP_WRITE( Sn_IR(s) , Sn_IR_SEND_OK);
#endif*/

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
  struct spi_ioc_transfer xfer[2];
		  unsigned char buf[32], *bp;
		  int len1, status;
  do
  {
	//    val1 = IINCHIP_READ(Sn_TX_FSR0(s));
            buf[0] = 00;
		    buf[1] = 20;
		    buf[2] = 0x08+s*(0x20);

					
		    len1 = 3;
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
	for (bp = buf; len1; len1--)
	printf("%02x ", *bp++);
	printf("\n");*/

    val1=buf[0];

//    val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
	              buf[0] = 00;
				  buf[1] = 21;
				  buf[2] = 0x08+s*(0x20);
	  
						  
				  len1 = 3;
				 xfer[0].tx_buf = (unsigned long)buf;
				 xfer[0].len = 3;
						  
				 xfer[1].rx_buf = (unsigned long) buf;
				 xfer[1].len = 3;
	  
				  status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
				  if (status < 0) {
				  perror("SPI_IOC_MESSAGE");
				  return;
				  }
/*	  
		  printf("response(%d): ", status);
		  for (bp = buf; len1; len1--)
		  printf("%02x ", *bp++);
		  printf("\n");*/
		  val1 = (val1 << 8) + buf[0];



      if (val1 != 0)
    {
 //       val = IINCHIP_READ(Sn_TX_FSR0(s));
 	        buf[0] = 00;
		    buf[1] = 20;
		    buf[2] = 0x08+s*(0x20);

					
		    len1 = 3;
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
	for (bp = buf; len1; len1--)
	printf("%02x ", *bp++);
	printf("\n");*/

    val=buf[0];
//        val = (val << 8) + IINCHIP_READ(Sn_TX_FSR1(s));
	              buf[0] = 00;
				  buf[1] = 21;
				  buf[2] = 0x08+s*(0x20);
	  
						  
				  len1 = 3;
				 xfer[0].tx_buf = (unsigned long)buf;
				 xfer[0].len = 3;
						  
				 xfer[1].rx_buf = (unsigned long) buf;
				 xfer[1].len = 3;
	  
				  status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
				  if (status < 0) {
				  perror("SPI_IOC_MESSAGE");
				  return;
				  }
	  
/*		  printf("response(%d): ", status);
		  for (bp = buf; len1; len1--)
		  printf("%02x ", *bp++);
		  printf("\n");*/
		  val = (val << 8) + buf[0];
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
  uint16 ptr = 0;
  uint16 ptr1 = 0;
  uint32 addrbsb =0;
  uint16 idx = 0;

  struct spi_ioc_transfer xfer[2];
		  unsigned char buf[1024], *bp;
		  int len1, status;
		  
  if(len == 0)
  {
    printf("CH: %d Unexpected1 length 0\r\n", s);
    return;
  }
   
//  ptr = IINCHIP_READ( Sn_TX_WR0(s) );
              buf[0] = 0x00;
			  buf[1] = 0x24;
			  buf[2] = 0x08+s*(0x20);
  //				  buf[3] = 0x10;
					  
			  len1 = 3;
			 xfer[0].tx_buf = (unsigned long)buf;
			 xfer[0].len = 3;
					  
			 xfer[1].rx_buf = (unsigned long) buf;
			 xfer[1].len = 3;
  
			  status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
			  if (status < 0) {
			  perror("SPI_IOC_MESSAGE");
			  return;
			  }
  
	  printf("response(%d): ", status);
	  for (bp = buf; len1; len1--)
	  printf("%02x ", *bp++);
	  printf("\n");
	  ptr=buf[0];

//  ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR1(s));
            buf[0] = 0x00;
		    buf[1] = 0x25;
		    buf[2] = 0x08+s*(0x20);
//					buf[3] = 0x10;
					
		    len1 = 3;
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
	for (bp = buf; len1; len1--)
	printf("%02x ", *bp++);
	printf("\n");*/
        ptr1 = buf[0];

//  addrbsb = (uint32)(ptr<<8) + (s<<5) + 0x10;
//  wiz_write_buf(addrbsb, data, len);
            buf[0] = ptr;
		    buf[1] = ptr1;
		    buf[2] = 0x0c+s*(0x20)+0x10;
//		    buf[3] = 0x10;
			for(idx = 0; idx < len; idx++)                   
  {
          buf[idx+3] =buff[idx];
  }

					
		    len1 = 3;
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
	for (bp = buf; len1; len1--)
	printf("%02x ", *bp++);
	printf("\n");*/

	ptr = ((ptr & 0x00ff) << 8) + ptr1;
        ptr += len; 
//  ptr += len;
//  IINCHIP_WRITE( Sn_TX_WR0(s) ,(uint8)((ptr & 0xff00) >> 8));
            buf[0] = 0x00;
			buf[1] = 0x24;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = (uint8)((ptr & 0xff00) >> 8);
					
			len1 = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
 // IINCHIP_WRITE( Sn_TX_WR1(s),(uint8)(ptr & 0x00ff));
 			buf[0] = 0x00;
			buf[1] = 0x25;
			buf[2] = 0x0c+s*(0x20);
			buf[3] = (uint8)(ptr & 0x00ff);
					
			len1 = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
			}
}



/**
*@brief		TCP Client回环演示函数。
*@param		无
*@return	无
*/
void do_tcp_client(int fd)
{	
   uint16 len=0;	
   struct spi_ioc_transfer xfer[2];
  unsigned char buf[32], *bp;
  int len1, status;

	switch(getSn_SR(SOCK_TCPC,fd))  /*获取socket的状态*/
	{
		case SOCK_CLOSED:											        		         /*socket处于关闭状态*/

			 printf("i am here111111 ***************************** ");
			socket(SOCK_TCPC,Sn_MR_TCP,local_port++,Sn_MR_ND,fd);
		  break;
		
		case SOCK_INIT:													        	         /*socket处于初始化状态*/
			
			connect(SOCK_TCPC,remote_ip,remote_port,fd);                /*socket连接服务器*/ 
		  break;
		
		case SOCK_ESTABLISHED: 			/*socket处于连接建立状态*/
			
			 buf[0] = 0x00;
		   	 buf[1] = 0x02;
		     buf[2] = 0x08+(SOCK_TCPC)*(0x20);
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

			if(buf[0] & Sn_IR_CON)
			{
//				setSn_IR(SOCK_TCPC, Sn_IR_CON); 			/*清除接收中断标志位*/
			buf[0] = 0x00;
			buf[1] = 0x02;
			buf[2] = 0x0c+1*(0x20);
			buf[3] = 0x01;
					
//			len = 6;
			xfer[0].tx_buf = (unsigned long)buf;
			xfer[0].len = 4;

			status = ioctl(fd, SPI_IOC_MESSAGE(1), xfer);
			if (status < 0) {
			perror("SPI_IOC_MESSAGE");
			return;
				}				
			}
		
			len=getSn_RX_RSR(SOCK_TCPC,fd); 				  /*定义len为已接收数据的长度*/
			if(len>0)
			{
				recv(SOCK_TCPC,buff,len,fd);          /*接收来自Server的数据*/
				buff[len]=0x00;  											                 /*添加字符串结束符*/
				printf("%s\r\n",buff);
				send(SOCK_TCPC,buff,len,fd);			/*向Server发送数据*/
			}		  
		  break;
			
		case SOCK_CLOSE_WAIT: 					/*socket处于等待关闭状态*/
			closesk(SOCK_TCPC,fd);
		  break;

	}
}


