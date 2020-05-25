#ifndef __I2C_H
#define __I2C_H
//============================================================
// 完成于2019年11月6日  BY:杨金生
// 这是I2C的通讯协议,可以驱动所有通用I2C器件;
// 如果驱动24CXX系列EEPROM则多次写入每字节间隔5ms~10ms否则写入会失败
// 24CXX内带缓存如果执行页写入（不跨页不跳地址）则可连续写入，每页16字节
// 24C（01/02）地址设置高四位固定为1010，低四位为（A2）（A1）（A0）（R/W），最低位为读写位（低写高读）
// 
//============================================================
//#include<reg52.h>
//#include<intrins.h>
#include"define.h"
//#define TRUE 1
//#define FALSE 0

//sbit SDA=P2^6;
//sbit SCL=P2^7;

void delay(unsigned char xus)     //时钟延时程序
{
   while(--xus!=0);
}

void delayms(unsigned int xms)     //读写延时程序
{
   unsigned char x;
   for(x=0;x<10;x++)
   {
      for(;xms>0;xms--);
   }
}

void i2c_star()                   //开始信号
{
   SDA=TRUE;
   delay(1);
   SCL=TRUE;
   delay(1);
   SDA=FALSE;
   delay(1);
   SCL=FALSE;
   delay(1);
}

void i2c_ack()                   //应答
{
   SDA=TRUE;
   delay(1);
   while(SDA);
   SCL=TRUE;
   delay(1);
   SCL=FALSE;
   delay(1);
}

void i2c_master_ack()              //主机应答
{
   SDA=FALSE;
   delay(1);
   SCL=TRUE;
   delay(1);
   SCL=FALSE;
   delay(1);
}

void i2c_no_ack()                  //主机不应答
{
   delay(1);
   SCL=TRUE;
   delay(1);
   SCL=FALSE;
   delay(1);     
}

void i2c_stop()                    //停止
{
   SDA=FALSE;
   delay(1);
   SCL=TRUE;
   delay(1);
   SDA=TRUE;
}

void i2c_write_operation(unsigned char write_data)              //写操作
{
   unsigned char num;
   for(num=0;num<8;num++)
   {
	  write_data=write_data<<1;
	  SDA=CY;
      delay(1);
      SCL=TRUE;
      delay(1);
      SCL=FALSE;
	  delay(1);         
   }
}

unsigned char i2c_read_operation()                              //读操作,返回无符号字节变量
{
   unsigned char num,read_data;
   for(num=0;num<8;num++)
   {
      read_data=read_data<<1;
      SCL=TRUE;
	  read_data|=SDA;
	  delay(1);
	  SCL=FALSE;
      delay(1);
   }
   return read_data;
}

void i2c_write_byte(unsigned char address,unsigned char internal_add,unsigned char DATA)      //写一个字节,形参(器件地址,内部地址,要写入的数据(无符号字节型))
{
   i2c_star();
   i2c_write_operation(address);
   i2c_ack();
   i2c_write_operation(internal_add);
   i2c_ack();
   i2c_write_operation(DATA);
   i2c_ack();
   i2c_stop();         
}

unsigned char i2c_read_byte(unsigned char address,unsigned char internal_add)              //读一个字节,返回无符号字节变量,形参(器件地址,内部地址)
{
   unsigned char read_data;
   i2c_star();
   i2c_write_operation(address);
   i2c_ack();
   i2c_write_operation(internal_add);
   i2c_ack();
   i2c_star();
   i2c_write_operation(address+1);
   i2c_ack();
   read_data=i2c_read_operation();
   i2c_no_ack();
   i2c_stop();
   return read_data;   
}

void i2c_write_multiple(unsigned char address,unsigned char internal_add,unsigned char *p_data,unsigned char number)      //连续写多个字节,形参(器件地址,内部地址,数组地址,写入的个数)
{
   i2c_star();
   i2c_write_operation(address);
   i2c_ack();
   i2c_write_operation(internal_add);
   i2c_ack();
   for(;number>0;number--)
   {
      i2c_write_operation(*p_data);
	  p_data+=1;
	  i2c_ack();
   }
   i2c_stop();
}

void i2c_read_multiple(unsigned char address,unsigned char internal_add,unsigned char * p_data,unsigned char number)     //连续读多个字节,形参(器件地址,内部地址,数组地址,读出的个数)
{
   unsigned char loop;
   for(loop=0;loop<number;loop++)
   {
      *(p_data+loop)=i2c_read_byte(address,internal_add+loop);
	  delayms(1);
   }   
}

#endif