#ifndef RTU_SENT_H
#define RTU_SENT_H

#include <QtCore>

#define RTU_LOOP_NUM 9 // 9回路
#define RTU_LINE_NUM 3 // 3相
#define RTU_TH_NUM 4 // 4个传感器
#define RTU_THD_NUM 4 // 4个谐波含量
//1.4版本
//#define RTU_SENT_LEN (22*RTU_LINE_NUM+1+3*3+1+1+3+11+6)
//1.3版本
//#define RTU_SENT_LEN (22*RTU_LINE_NUM+1+3*3+1+1+3+11+6)  // 长度需要改变 ////============ 加上开关，功率因素之后，是为14 [追加交直流区分]
#define RTU_SENT_LEN 0xFF
#define RTU_SENT_LEN_V23 0x135//V2.3 and V2.4
#define RTU_SENT_LEN_V25 0x280//V2.5
#define RTU_SENT_LEN_V30 0x15E//V3.0.0
#define RTU_SENT_LEN_V303 0x1DB//V3.0.3
#define RTU_SENT_DC_LEN (22*4+1+3*3+1+1+3+5)  // 长度需要改变 直流长度 [追加交直流区分

struct Rtu_Sent {
    Rtu_Sent():fn(3),reg(0),len(RTU_SENT_LEN_V303){} // 下位机有问题
    uchar addr; // 表示从机地址码
    uchar fn;  // 表示功能码
    ushort reg; // 表示寄存器首地址
    uint len; // 表示数据长度
    ushort crc; // 表示CRC校验
};

struct Rtu_Sent_Ushort_V3 {
    Rtu_Sent_Ushort_V3():fn(3),reg(0),num(2){} // 下位机有问题
    uchar addr; // 表示从机地址码
    uchar fn;  // 表示功能码
    ushort reg; // 表示寄存器首地址
    ushort num; // 表示寄存器个数
    ushort val1; // 表示最小值
    ushort val2; // 表示最大值
    ushort crc; // 表示CRC校验
};

struct Rtu_Sent_Uint_V3 {
    Rtu_Sent_Uint_V3():fn(3),reg(0),num(4){} // 下位机有问题
    uchar addr; // 表示从机地址码
    uchar fn;  // 表示功能码
    ushort reg; // 表示寄存器首地址
    ushort num; // 表示寄存器个数
    ushort val1; // 表示最小值低位
    ushort val2; // 表示最小值高位
    ushort val3; // 表示最大值低位
    ushort val4; // 表示最大值高位
    ushort crc; // 表示CRC校验
};

ushort rtu_crc(uchar *buf, int len);
int rtu_sent_buff(uchar addr, uchar *buf , uint len = RTU_SENT_LEN_V303);
int rtu_sent_buff(uchar addr, ushort reg, uint len, uchar *buf);
int rtu_sent_ucharV3_buff(uchar addr, ushort reg, uint num,  uint val , uchar *buf);
int rtu_sent_ushortV3_buff(uchar addr, ushort reg, uint num,  uint val1, uint val2 , uchar *buf);
int rtu_sent_uintV3_buff(uchar addr, ushort reg, uint num,  uint val1, uint val2,uchar *buf);
#endif // RTU_SENT_H
