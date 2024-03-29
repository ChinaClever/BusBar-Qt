/*
 * rtu_sent.cpp
 * 把结构体数据打包成RTU数据包
 *
 *  Created on: 2016年10月11日
 *      Author: Lzy
 */
#include "rtu_sent.h"


static ushort calccrc (ushort crc, uchar crcbuf)
{
    uchar x, kkk=0;
    crc = crc^crcbuf;
    for(x=0;x<8;x++)
    {
        kkk = crc&1;
        crc >>= 1;
        crc &= 0x7FFF;
        if(kkk == 1)
            crc = crc^0xa001;
        crc=crc&0xffff;
    }
    return crc;
}

/**
  * 功　能：CRC校验
  * 入口参数：buf -> 缓冲区  len -> 长度
  * 返回值：CRC
  */
ushort rtu_crc(uchar *buf, int len)
{
    ushort crc = 0xffff;
    for(int i=0; i<len; i++)
        crc = calccrc(crc, buf[i]);
    return crc;
}

/**
  * 功　能：发送数据打包
  * 入口参数：pkt -> 发送结构体
  * 出口参数：ptr -> 缓冲区
  * 返回值：打包后的长度
  */
static int rtu_sent_packet(Rtu_Sent *pkt, uchar *ptr)
{
    uchar *buf = ptr;
    *(ptr++) = pkt->addr;  /*地址码*/
    *(ptr++) = pkt->fn; /*功能码*/

    /*填入寄存器首地址*/
    *(ptr++) = ((pkt->reg) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->reg); /*低8位*/

    if( pkt->fn == 0x10 ){
        *(ptr++) = ((pkt->len) >> 24);
        *(ptr++) = ((pkt->len) >> 16);
    }

    /*填入数据长度*/
    *(ptr++) = ((pkt->len) >> 8); /*长度高8位*/
    *(ptr++) = (0xff)&(pkt->len); /*低8位*/

     /*填入CRC*/
    if( pkt->fn == 0x10 ){
        pkt->crc = rtu_crc(buf, 8);
    }else{
        pkt->crc = rtu_crc(buf, 6);
    }
    *(ptr++) = (0xff)&(pkt->crc); /*低8位*/
    *(ptr++) = ((pkt->crc) >> 8); /*高8位*/

    #if (SI_RTUWIFI ==1)
    for(int i = 0 ; i < 8 ; ++i)//SI-BUSBAR无线加头BUSBAR的ascii码
    {
        *(ptr+6-i-1)=*(ptr-i-1);
    }
    *(ptr-8) = 0x42;
    *(ptr-7) = 0x55;
    *(ptr-6) = 0x53;
    *(ptr-5) = 0x42;
    *(ptr-4) = 0x41;
    *(ptr-3) = 0x52;
    return 14;
    #else
    if( pkt->fn == 0x10 ){
        return 10;
    }else{
        return 8;
    }
    #endif
}

/**
  * 功　能：发送数据打包
  * 入口参数：pkt -> 发送结构体
  * 出口参数：ptr -> 缓冲区
  * 返回值：打包后的长度
  */
int rtu_sent_buff(uchar addr, uchar *buf , uint len)
{
    static Rtu_Sent msg;
    static QMutex mutex; // 互拆锁
    QMutexLocker locker(&mutex);

    msg.addr = addr;
    msg.len  = len;
    return rtu_sent_packet(&msg, buf);
}

int rtu_sent_buff(uchar addr, ushort reg, uint len, uchar *buf)
{
    static Rtu_Sent msg;
    static QMutex mutex; // 互拆锁
    QMutexLocker locker(&mutex);

    msg.addr = addr;
    msg.fn   = 0x10;
    msg.reg  = reg;
    msg.len  = len;
    return rtu_sent_packet(&msg, buf);
}

/**
  * 功　能：发送数据打包
  * 入口参数：pkt -> 发送结构体
  * 出口参数：ptr -> 缓冲区
  * 返回值：打包后的长度
  */
static int rtu_sent_packet_uchar_V3(Rtu_Sent_Ushort_V3 *pkt, uchar *ptr)
{
    uchar *buf = ptr;
    *(ptr++) = pkt->addr;  /*地址码*/
    *(ptr++) = pkt->fn; /*功能码*/

    /*填入寄存器首地址*/
    *(ptr++) = ((pkt->reg) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->reg); /*低8位*/

    /*填入寄存器个数*/
    *(ptr++) = ((pkt->num) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->num); /*低8位*/

    *(ptr++) = pkt->num * 2;

    /*填入数据1*/
    *(ptr++) = ((pkt->val1) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->val1); /*低8位*/

    /*填入CRC*/
    pkt->crc = rtu_crc(buf, 9);
    *(ptr++) = (0xff)&(pkt->crc); /*低8位*/
    *(ptr++) = ((pkt->crc) >> 8); /*高8位*/

    return 11;
}

/**
  * 功　能：发送数据打包
  * 入口参数：pkt -> 发送结构体
  * 出口参数：ptr -> 缓冲区
  * 返回值：打包后的长度
  */
static int rtu_sent_packet_ushort_V3(Rtu_Sent_Ushort_V3 *pkt, uchar *ptr)
{
    uchar *buf = ptr;
    *(ptr++) = pkt->addr;  /*地址码*/
    *(ptr++) = pkt->fn; /*功能码*/

    /*填入寄存器首地址*/
    *(ptr++) = ((pkt->reg) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->reg); /*低8位*/

    /*填入寄存器个数*/
    *(ptr++) = ((pkt->num) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->num); /*低8位*/

    *(ptr++) = pkt->num * 2;

    /*填入数据1*/
    *(ptr++) = ((pkt->val1) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->val1); /*低8位*/

    /*填入数据2*/
    *(ptr++) = ((pkt->val2) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->val2); /*低8位*/

    /*填入CRC*/
    pkt->crc = rtu_crc(buf, 11);
    *(ptr++) = (0xff)&(pkt->crc); /*低8位*/
    *(ptr++) = ((pkt->crc) >> 8); /*高8位*/

    return 13;
}

int rtu_sent_ucharV3_buff(uchar addr, ushort reg, uint num,  uint val , uchar *buf)
{
    static Rtu_Sent_Ushort_V3 msg;
    static QMutex mutex; // 互拆锁
    QMutexLocker locker(&mutex);

    msg.addr = addr;
    msg.fn   = 0x10;
    msg.reg  = reg;
    msg.num  = num;
    msg.val1 = val;
    return rtu_sent_packet_uchar_V3(&msg, buf);
}

int rtu_sent_ushortV3_buff(uchar addr, ushort reg, uint num,  uint val1, uint val2 , uchar *buf)
{
    static Rtu_Sent_Ushort_V3 msg;
    static QMutex mutex; // 互拆锁
    QMutexLocker locker(&mutex);

    msg.addr = addr;
    msg.fn   = 0x10;
    msg.reg  = reg;
    msg.num  = num;
    msg.val1 = val1;
    msg.val2 = val2;
    return rtu_sent_packet_ushort_V3(&msg, buf);
}

/**
  * 功　能：发送数据打包
  * 入口参数：pkt -> 发送结构体
  * 出口参数：ptr -> 缓冲区
  * 返回值：打包后的长度
  */
static int rtu_sent_packet_uint_V3(Rtu_Sent_Uint_V3 *pkt, uchar *ptr)
{
    uchar *buf = ptr;
    *(ptr++) = pkt->addr;  /*地址码*/
    *(ptr++) = pkt->fn; /*功能码*/

    /*填入寄存器首地址*/
    *(ptr++) = ((pkt->reg) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->reg); /*低8位*/

    /*填入寄存器个数*/
    *(ptr++) = ((pkt->num) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->num); /*低8位*/

    *(ptr++) = pkt->num * 2;

    /*填入数据1*/
    *(ptr++) = ((pkt->val1) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->val1); /*低8位*/

    /*填入数据2*/
    *(ptr++) = ((pkt->val2) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->val2); /*低8位*/

    /*填入数据3*/
    *(ptr++) = ((pkt->val3) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->val3); /*低8位*/

    /*填入数据4*/
    *(ptr++) = ((pkt->val4) >> 8); /*高8位*/
    *(ptr++) = (0xff)&(pkt->val4); /*低8位*/

    /*填入CRC*/
    pkt->crc = rtu_crc(buf, 15);
    *(ptr++) = (0xff)&(pkt->crc); /*低8位*/
    *(ptr++) = ((pkt->crc) >> 8); /*高8位*/

    return 17;
}

int rtu_sent_uintV3_buff(uchar addr, ushort reg, uint num,  uint val1, uint val2,uchar *buf)
{
    static Rtu_Sent_Uint_V3 msg;
    static QMutex mutex; // 互拆锁
    QMutexLocker locker(&mutex);

    msg.addr = addr;
    msg.fn   = 0x10;
    msg.reg  = reg;
    msg.num  = num;
    msg.val1 = (val1 >> 16) & (0xffff);
    msg.val2 = val1 & (0xffff);
    msg.val3 = (val2 >> 16) & (0xffff);
    msg.val4 = val2 & (0xffff);
    return rtu_sent_packet_uint_V3(&msg, buf);
}


