/*
 * rtu_sent.cpp
 * 把接收到的数据还原成结构体
 *
 *  Created on: 2016年10月11日
 *      Author: Lzy
 */
#include "rtu_recv.h"

static int rtu_recv_len_dc(uchar *buf, int len)
{
    int ret = 0;
    int rtn = RTU_SENT_DC_LEN+5;

    if(len < rtn) {
        ret = -1;
        //        qDebug() << "rtu recv Err: len too short!!" << len  << rtn;
    } else if(len > rtn) {
        ret = -2;
        //        qDebug() << "rtu recv Err: len too long!!" << len << rtn ;
    } else {
        //        len = buf[2]*256 + buf[3];
        len = buf[2];
        if(len != RTU_SENT_DC_LEN) {
            ret = -3;
            qDebug() << "rtu recv len Err!!"<< len << rtn  << RTU_SENT_DC_LEN;
        }
    }
    return ret;
}



/**
  * 功　能：长度 校验
  * 入口参数：buf -> 缓冲区  len -> 长度
  * 返回值：0 正确
  */
static int rtu_recv_len(uchar *buf, int len , int rtn)
{
    int ret = 0;
    //int rtn = RTU_SENT_LEN_V25+6;

    if(len < rtn) {  //判断是否为交流数据
        ret = -1;
        //        qDebug() << "rtu recv Err: len too short!!" << len  << rtn;
    } else if(len > rtn) {
        ret = -2;
        //        qDebug() << "rtu recv Err: len too long!!" << len << rtn ;
    } else {
        len = buf[2]*256 + buf[3];
        if(len != rtn - 6 ) {
            ret = -3;
            qDebug() << "rtu recv len Err!!"<< len << rtn  << rtn - 6;
        }
    }

    return ret;
}

/**
  * 功　能：读取数据包头部
  * 入口参数：ptr -> 缓冲区
  * 出口参数：pkt -> 结构体
  * 返回值：0 正确
  */
static int rtu_recv_head(uchar *ptr,  Rtu_recv *pkt)
{
    pkt->addr = *(ptr++);// 从机地址码
    pkt->fn = *(ptr++);  /*功能码*/

    pkt->len = (*ptr)*256 +  *(ptr+1);  ptr += 2;/*数据长度*/
    return 4;
}

/**
  * 功　能：读取电参数 数据
  * 入口参数：ptr -> 缓冲区
  * 出口参数：pkt -> 结构体
  * 返回值：22 正确
  */
static int rtu_recv_data(uchar *ptr, RtuRecvLine *msg)
{
    msg->vol.svalue =  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取电压
    msg->cur.svalue =  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取电流
    msg->pow.ivalue =  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取功率
    msg->ele =  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取电能高8位
    msg->ele <<= 16; // 左移8位
    msg->ele +=  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取电能底8位

    msg->vol.smax =  (*ptr) * 256 + *(ptr+1);  ptr += 2;
    msg->vol.smin =  (*ptr) * 256 + *(ptr+1);  ptr += 2;
    msg->cur.smax =  (*ptr) * 256 + *(ptr+1);  ptr += 2;
    msg->cur.smin =  (*ptr) * 256 + *(ptr+1);  ptr += 2;

    msg->wave =  (*ptr) * 256 + *(ptr+1);  ptr += 2;    // 谐波值
    msg->pf =  *(ptr++);// 功率因数
    msg->sw =  *(ptr++);// 开关状态

    msg->apPow = msg->vol.svalue * msg->cur.svalue / 10.0; // 视在功率

    return 22;   ////============ 加上开关，功率因数之后，是为14
}

/**
  * 功　能：读取电参数 数据
  * 入口参数：ptr -> 缓冲区
  * 出口参数：pkt -> 结构体
  * 返回值：24 正确
  */
static int rtu_recv_new_data(uchar *ptr, RtuRecvLine *msg)
{
    msg->vol.svalue =  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取电压
    msg->vol.smax  =  (*ptr) * 256 + *(ptr+1);  ptr += 2;
    msg->vol.smin =  (*ptr) * 256 + *(ptr+1);  ptr += 2;

    msg->cur.svalue =  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取电流
    msg->cur.smax =  (*ptr) * 256 + *(ptr+1);  ptr += 2;
    msg->cur.smin =  (*ptr) * 256 + *(ptr+1);  ptr += 2;

    msg->pow.ivalue =  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取高16位功率
    msg->pow.ivalue <<= 16; // 左移16位
    msg->pow.ivalue +=  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取低16位功率

    msg->pow.imax = (*ptr) * 256 + *(ptr+1);  ptr += 2;
    msg->pow.imax  <<= 16; // 左移16位
    msg->pow.imax  +=  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取低16位功率

    msg->pow.imin  = (*ptr) * 256 + *(ptr+1);  ptr += 2;
    msg->pow.imin <<= 16; // 左移16位
    msg->pow.imin +=  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取低16位功率

    msg->pf =  *(ptr++);// 功率因数

    msg->reactivePower =  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取无功功率高8位
    msg->reactivePower <<= 16; // 左移16位
    msg->reactivePower +=  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取无功功率底8位

    msg->ele =  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取电能高8位
    msg->ele <<= 16; // 左移8位
    msg->ele +=  (*ptr) * 256 + *(ptr+1);  ptr += 2; // 读取电能底8位

    ptr += 4;//无功电能
    msg->sw =  *(ptr++);// 开关状态

    unsigned long long vol = msg->vol.svalue;
    unsigned long long cur = msg->cur.svalue;
    msg->apPow = vol * cur / 1000.0; // 视在功率

    //return 30;   ////============ 加上开关，功率因数之后，是为14
    ptr += 22;//预留
    return 30+30;
}

/**
  * 功　能：读取环境 数据
  * 入口参数：ptr -> 缓冲区
  * 出口参数：pkt -> 结构体
  * 返回值：12 正确
  */
static int rtu_recv_env(uchar *ptr, RtuEnvUnit *msg)
{
    msg->value = (*ptr) * 256 + *(ptr+1);ptr+=2;
    msg->max = (*ptr) * 256 + *(ptr+1);ptr+=2;
    msg->min = (*ptr) * 256 + *(ptr+1);ptr+=2;

    return 6;
}

/**
  * 功　能：检验码 数据
  * 入口参数：buf -> 缓冲区
  * 出口参数：pkt -> 结构体
  * 返回值：true
  */
static bool rtu_recv_crc(uchar *buf, int len, Rtu_recv *msg)
{
    bool ret = true;
    int rtn = len-2;

    ushort crc = rtu_crc(buf, rtn);
    if(crc != msg->crc) {
        ret = false;
        qDebug() << "rtu recv crc Err!";
    }

    return ret;
}


static int rtu_recv_thd(uchar *ptr, Rtu_recv *msg)
{
    msg->lps = *(ptr++); // 防雷开关
    // 读取负载百分比
    for(int i=0; i<3; ++i) msg->pl[i] = *(ptr++);

    ptr+=6;//1.4版本 ，1.3版本需要注释

    msg->hc = *(ptr++);

    int len = 32;
    if(msg->addr) len = 3;
    for(int i=0; i<len; ++i){
        msg->thd[i] =  (*ptr) * 256 + *(ptr+1);  ptr += 2;
    }

    //return (1+3+1+len*2);//1.3版本
    return (1+3+1+6+len*2); //1.4版本
}

static int rtu_recv_new_thd(uchar *ptr, Rtu_recv *msg)
{
    // 读取负载百分比
    for(int i=0; i<3; ++i) msg->pl[i] = *(ptr++);
    msg->volUnbalance = *(ptr++);
    msg->curUnbalance = *(ptr++);
    msg->zeroLineCur.svalue = *(ptr) * 256 + *(ptr+1); ptr+=2;

    if( msg->addr == 1 ) msg->lps = *(ptr++);// 防雷开关
    else ptr++;

    ptr++;//iOF触点
    ptr+=2;//断路器开关状态
    msg->buzzerStatus = *(ptr++);
    if( msg->addr == 1 ) msg->hc = *(ptr++);
    else ptr++;

    int len = 32;
    if( msg->addr != 1 ) len = 3;
    for(int i=0; i<len; ++i){
        msg->thd[i] =  (*ptr) * 256 + *(ptr+1);  ptr += 2;
    }

    return (3+1+1+2+6+len*2); //2.1版本
}

static int rtu_start_recv_init(uchar *ptr, Rtu_recv *msg)
{
    uint len = 0;
    msg->proNum = (*ptr) * 256 + *(ptr+1); ptr+=2; len+=2;// 项目ID
    msg->version = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;// 软件版本
    msg->dc = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[交直流]
    if( msg->dc ) msg->dc = 0;
    else msg->dc = 1;
    msg->curSpecification = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[电流规格]
    msg->workMode = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[始端箱的工作模式]
    msg->baudRate = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[波特率]
    msg->buzzerStatus = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[蜂鸣器]
    msg->alarmTime = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->lps = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->iOF = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[iOF触点]
    msg->isd = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[isd触点]

    msg->shuntRelease = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->reState = (*ptr) * 256 + *(ptr+1);ptr+=2;len+=2;
    msg->lpsState = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;

    return len; //3.0.0版本
}

static int rtu_start_recv_line_data(uchar *ptr, Rtu_recv *msg , int index)
{
    RtuRecvLine *p = &(msg->data[index]);
    uint len = 0;
    p->lineVol.svalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->lineVol.salarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->vol.svalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->vol.salarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->cur.svalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->cur.salarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->pow.ivalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->pow.ivalue  <<= 16; // 左移16位
    p->pow.ivalue += (*ptr) * 256 + *(ptr+1);  ptr+=2; len+=2;// 读取低16位有功功率
    p->pow.ialarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->reactivePower = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->reactivePower  <<= 16; // 左移16位
    p->reactivePower += (*ptr) * 256 + *(ptr+1);  ptr += 2; len+=2;// 读取低16位无功功率
    p->apPow = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->apPow  <<= 16; // 左移16位
    p->apPow += (*ptr) * 256 + *(ptr+1);  ptr+=2; len+=2;// 读取低16位视在功率
    p->pf = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->ele = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->ele  <<= 16; // 左移16位
    p->ele += (*ptr) * 256 + *(ptr+1);  ptr+=2; len+=2;// 读取低16位电能
    msg->pl[index] = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;

    return len; //3.0.0版本
}

static int rtu_start_recv_other_data(uchar *ptr, Rtu_recv *msg)
{
    uint len = 0;
    msg->totalApPow = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->totalApPow  <<= 16; // 左移16位
    msg->totalApPow += (*ptr) * 256 + *(ptr+1);  ptr+=2; len+=2;
    msg->totalPow.ivalue  = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->totalPow.ivalue  <<= 16; // 左移16位
    msg->totalPow.ivalue  += (*ptr) * 256 + *(ptr+1);  ptr+=2; len+=2;
    msg->totalPow.ialarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->reCur.svalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->reCur.salarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->zeroLineCur.svalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->zeroLineCur.salarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->volUnbalance = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->curUnbalance = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->breaker = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->rate.svalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->rate.salarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    return len; //3.0.0版本
}

static int rtu_start_recv_env_data(uchar *ptr, Rtu_recv *msg , int index)
{
    uint len = 0;
    msg->env[index].tem.value = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->env[index].tem.alarm= (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    return len; //3.0.0版本
}

static int rtu_start_recv_thd_data(uchar *ptr, Rtu_recv *msg , int index)
{
    uint len = 0;
    for(int i = 0 ; i < 32 ; ++i ){
        msg->volThd[index][i] = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    }
    for(int i = 0 ; i < 32 ; ++i ){
        msg->curThd[index][i] = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    }
    return len; //3.0.0版本
}

static int rtu_start_recv_some_alarm_data(uchar *ptr, Rtu_recv *msg)
{
    uint len = 0;
    for(int i = 0 ; i < RTU_TH_NUM ; ++i) // 读取温度阈值数据
    {
        msg->env[i].tem.min = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
        msg->env[i].tem.max = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    }
    msg->reCur.salarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->zeroLineCur.smin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->zeroLineCur.smax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->totalPow.imin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->totalPow.imin  <<= 16; // 左移16位
    msg->totalPow.imin  += (*ptr) * 256 + *(ptr+1);  ptr+=2; len+=2;
    msg->totalPow.imax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->totalPow.imax  <<= 16; // 左移16位
    msg->totalPow.imax  += (*ptr) * 256 + *(ptr+1);  ptr+=2; len+=2;
    msg->rate.smin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->rate.smax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    return len; //3.0.0版本
}

static int rtu_start_recv_last_alarm_data(uchar *ptr, Rtu_recv *msg, int index)
{
    RtuRecvLine *p = &(msg->data[index]);
    uint len = 0;
    p->lineVol.smin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->lineVol.smax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->vol.smin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->vol.smax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->cur.smin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->cur.smax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->pow.imin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->pow.imin <<= 16; // 左移16位
    p->pow.imin += (*ptr) * 256 + *(ptr+1);  ptr += 2; len+=2;// 读取低16位有功功率最小值
    p->pow.imax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->pow.imax <<= 16; // 左移16位
    p->pow.imax += (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;// 读取低16位有功功率最大值
    return len; //3.0.0版本
}

static int rtu_plug_recv_init(uchar *ptr, Rtu_recv *msg)
{
    uint len = 0;
    msg->version = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;// 软件版本
    msg->proNum = (*ptr) * 256 + *(ptr+1); ptr+=2; len+=2;// 项目ID
    msg->lineNum = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//回路数
    //msg->addr = (*ptr) * 256 + *(ptr+1);
    ptr+=2;len+=2;//地址
    msg->baudRate = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[波特率]
    msg->iOF = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[iOF触点]
    msg->buzzerStatus = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;//[蜂鸣器]
    msg->alarmTime = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;

    return len; //3.0.0版本
}

static int rtu_plug_recv_loop_data(uchar *ptr, Rtu_recv *msg , int index)
{
    RtuRecvLine *p = &(msg->data[index]);
    uint len = 0;
    p->vol.svalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->vol.salarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->cur.svalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->cur.salarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;

    p->pow.ivalue = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->pow.ivalue  <<= 16; // 左移16位
    p->pow.ivalue += (*ptr) * 256 + *(ptr+1); ptr+=2; len+=2;// 读取低16位有功功率

    p->pow.ialarm = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;

    p->reactivePower = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->reactivePower  <<= 16; // 左移16位
    p->reactivePower += (*ptr) * 256 + *(ptr+1);  ptr+=2; len+=2;// 读取低16位无功功率

    p->apPow = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->apPow  <<= 16; // 左移16位
    p->apPow += (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;// 读取低16位视在功率

    p->pf = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->sw = (*ptr) * 256 + *(ptr+1) - 1; ptr+=2;len+=2;

    p->ele = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->ele  <<= 16; // 左移16位
    p->ele += (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;// 读取低16位电能
    return len; //3.0.0版本
}

static int rtu_plug_recv_thd_pl_data(uchar *ptr, Rtu_recv *msg)
{
    uint len = 0;
    for(int i = 0 ; i < RTU_LINE_NUM ; ++i){
        msg->thd[i] = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    }
    for(int i = 0 ; i < RTU_LINE_NUM ; ++i){
        msg->pl[i]= (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    }
    return len; //3.0.0版本
}

static int rtu_plug_recv_env_data(uchar *ptr, Rtu_recv *msg , int index)
{
    uint len = 0;
    msg->env[index].tem.value = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->env[index].tem.alarm= (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    return len; //3.0.0版本
}

static int rtu_plug_recv_env_alarm_data(uchar *ptr, Rtu_recv *msg , int index)
{
    uint len = 0;
    msg->env[index].tem.min = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    msg->env[index].tem.max= (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    return len; //3.0.0版本
}

static int rtu_plug_recv_loop_alarm_data(uchar *ptr, Rtu_recv *msg , int index)
{
    RtuRecvLine *p = &(msg->data[index]);
    uint len = 0;
    p->vol.smin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->vol.smax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->cur.smin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->cur.smax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;

    p->pow.imin = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->pow.imin  <<= 16;
    p->pow.imin += (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;

    p->pow.imax = (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    p->pow.imax  <<= 16;
    p->pow.imax += (*ptr) * 256 + *(ptr+1); ptr+=2;len+=2;
    return len; //3.0.0版本
}

static int rtu_recv_rate(uchar *ptr , ushort *cur , ushort *min , ushort *max)
{
    *cur = (*ptr) * 256 + *(ptr+1); ptr+=2;
    *max = (*ptr) * 256 + *(ptr+1); ptr+=2;
    *min = (*ptr) * 256 + *(ptr+1); ptr+=2;
    return 6;
}

/**
  * 功　能：还原数据包
  * 入口参数：buf -> 缓冲区   len -> 数据长度
  * 出口参数：pkt -> 结构体
  * 返回值：true
  */
bool rtu_recv_packet(uchar *buf, int len, Rtu_recv *pkt)
{
    bool ret = false;
    int rtn = rtu_recv_len(buf, len , RTU_SENT_LEN_V25); //判断回收的数据是否完全
    if(rtn == 0) {
        uchar *ptr=buf;
        ptr += rtu_recv_head(ptr, pkt); //指针偏移

        pkt->dc = *(ptr++);  //[交直流]
        pkt->type = *(ptr++);// 箱子类型
        pkt->lineNum = *(ptr++); //[输出位]
        pkt->proNum = *(ptr++);// 项目ID
        ptr+=1;// 电流规格
        pkt->version = *(ptr++); // 软件版本

        for(int i=0; i<RTU_TH_NUM; ++i) // 读取环境 数据
            ptr += rtu_recv_env(ptr, &(pkt->env[i].tem));

        ptr += rtu_recv_rate(ptr , &pkt->rate.svalue , &pkt->rate.smin , &pkt->rate.smax);//  频率上下限

        ptr += 4;//[预留]
        int lineSum = pkt->lineNum; //交流
        if(!pkt->dc) lineSum = 4; //[暂时未加宏]
        for(int i=0; i<lineSum; ++i) // 读取电参数
            ptr += rtu_recv_new_data(ptr, &(pkt->data[i]));

        pkt->totalPow.ivalue = (*ptr) * 256 + *(ptr+1); ptr+=2;
        pkt->totalPow.ivalue <<= 16;
        pkt->totalPow.ivalue += (*ptr) * 256 + *(ptr+1); ptr+=2;
        ptr += rtu_recv_new_thd(ptr, pkt);

        pkt->crc = (buf[RTU_SENT_LEN_V25+6-1]*256) + buf[RTU_SENT_LEN_V25+6-2]; // RTU_SENT_LEN_V23+5
        ret = rtu_recv_crc(buf, len, pkt); //校验码
    }
    return ret;
}


bool rtu_recv_packetV3(uchar *buf, int len, Rtu_recv *pkt)
{
    bool ret = false;
    int rtn = rtu_recv_len(buf, len , RTU_SENT_LEN_V30*2+6);  //判断回收的数据是否完全
    if(rtn == 0) {
        uchar *ptr=buf;
        ptr += rtu_recv_head(ptr, pkt); //指针偏移0
        if( pkt->addr == 0x01 ){//始端箱
            ptr += rtu_start_recv_init(ptr , pkt);
            ptr += 5*2;//保留
            for(int i = 0 ; i < RTU_LINE_NUM ; ++i) // 读取相 数据
                ptr += rtu_start_recv_line_data(ptr , pkt , i);
            ptr += rtu_start_recv_other_data(ptr , pkt);
            for(int i = 0 ; i < RTU_TH_NUM ; ++i) // 读取温度 数据
                ptr += rtu_start_recv_env_data(ptr , pkt , i);
            for(int i = 0 ; i < RTU_LINE_NUM ; ++i) // 读取滤波 数据
                ptr += rtu_start_recv_thd_data(ptr , pkt , i);
            ptr += rtu_start_recv_some_alarm_data(ptr , pkt);
            for(int i = 0 ; i < RTU_LINE_NUM ; ++i) // 读取滤波 数据
                ptr += rtu_start_recv_last_alarm_data(ptr , pkt , i);
            int state = 1;
            if( pkt->breaker == 1 ) state = 0;
            for(int i = 0 ; i < RTU_LINE_NUM ; ++i)
                pkt->data[i].sw = state;// 更新始端箱断路器状态
            pkt->lineNum = 3;
        }
        else{//插接箱
            ptr += rtu_plug_recv_init(ptr , pkt);
            ptr += 8*2;//保留
            for(int i = 0 ; i < RTU_LOOP_NUM ; ++i) // 读取loop 数据
                ptr += rtu_plug_recv_loop_data(ptr , pkt , i);
            ptr += rtu_plug_recv_thd_pl_data(ptr , pkt);
            for(int i = 0 ; i < RTU_TH_NUM ; ++i) // 读取温度 数据
                ptr += rtu_plug_recv_env_data(ptr , pkt , i);
            for(int i = 0 ; i < RTU_TH_NUM ; ++i) // 读取温度 alarm数据
                ptr += rtu_plug_recv_env_alarm_data(ptr , pkt , i);
            for(int i = 0 ; i < RTU_LOOP_NUM ; ++i) // 读取loop alarm数据
                ptr += rtu_plug_recv_loop_alarm_data(ptr , pkt , i);

        }
        pkt->crc = (buf[RTU_SENT_LEN_V30*2+6-1]*256) + buf[RTU_SENT_LEN_V30*2+6-2]; // RTU_SENT_LEN_V23*2+5
        ret = rtu_recv_crc(buf, len, pkt); //校验码
    }
    return ret;
}
