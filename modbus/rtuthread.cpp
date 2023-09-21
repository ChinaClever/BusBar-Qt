/*
 * rtuthread.cpp
 *
 *
 *
 *  Created on: 2017年10月1日
 *      Author: Lzy
 */
#include "rtuthread.h"
#include <QMultiHash>

static ushort gBoxArray[4] = {0, 0, 0, 0};

void set_box_num(int id, int num)
{
    gBoxArray[id] = num;
}

#if (SI_RTUWIFI==1)
#include "setting/setsystem/channelsettingdlg.h"
static QStringList mChList;
static int mIndex;
struct sBusCmd{
    int addr;
    ushort reg;
    ushort len;

    bool operator==(const sBusCmd& obj)
    { return ((addr==obj.addr)&&(reg==obj.reg)&&(len==obj.len));}
};
QMultiHash<int,sBusCmd> gBusCmd;

void insertBusCmd(int busID, int addr, ushort reg, ushort len)
{
    sBusCmd cmd;
    cmd.addr = addr;
    cmd.reg = reg;
    cmd.len = len;
    if(busID == 0xff || addr == 0xff){
        for(int i = 0 ; i < BUS_NUM ; ++i)
            gBusCmd.insertMulti(i , cmd);
    }
    else
        gBusCmd.insertMulti(busID , cmd);
}

void set_ch(const QStringList &chs,int recordindex)
{
    mChList = chs;
    mIndex = recordindex;
}
#endif

RtuThread::RtuThread(QObject *parent) :
    QThread(parent)
{
    mBuf = (uchar *)malloc(RTU_BUF_SIZE); //申请内存  -- 随便用
    mRtuPkt = new Rtu_recv; //传输数据结构
    mSerial = new Serial_Trans(this); //串口线程
}

RtuThread::~RtuThread()
{
    isRun = false;
    wait();
}

/**
 * @brief RTU通讯初始化
 * @param name 串口名
 * @param id 母线ID
 * @return
 */
bool RtuThread::init(const QString& name, int id)
{
    sDataPacket *shm = get_share_mem(); // 获取共享内存
    mBusData = &(shm->data[id-1]);

    bool ret = mSerial->openSerial(name); // 打开串口
    if(ret) {
        QTimer::singleShot(3*1000,this,SLOT(start()));  // 启动线程
    }

    mId = id-1;

    return ret;
}

int RtuThread::transmit(int addr, ushort reg, uint len)
{
    /*
     *  0 - 发送失败
     *  1 - 发送完成
     * -1 - 不在线
     * -2 - 发送却没收到
     */
    //判断是否在线
    sBoxData *box = &(mBusData->box[addr]); //共享内存
    if(box->offLine == 0){ //不在线
        return -1;
    }
    //打包数据
    uchar *buf = mBuf;
    uchar *get = mBuf + 20;
    int rtn = rtu_sent_buff(addr, reg, len, buf); // 把数据打包成通讯格式的数据
    rtn = mSerial->transmit(buf, rtn, get); //发送并回收
    if(rtn > 0){ //回收到数据
        for(int i = 0; i < rtn; i++){
            if(*buf++ != *get++) return 0;
        }
        return 1;
    }
    return -2;

}

int RtuThread::sendData(int addr, ushort reg, uint len, bool value)
{   
    if(addr == 0xff){
        uchar *buf = mBuf;
        int rtn = rtu_sent_buff(addr, reg, len, buf); // 把数据打包成通讯格式的数据
        return mSerial->sendData(buf, rtn, 250); //发送 -- 并占用串口250ms
    }else{
        sBoxData *box = &(mBusData->box[addr]); //共享内存
        if((box->offLine > 0) || value){ //在线
            //打包数据
            uchar *buf = mBuf;
            int rtn = rtu_sent_buff(addr+1, reg, len, buf); // 把数据打包成通讯格式的数据
            return mSerial->sendData(buf, rtn, 250); //发送 -- 并占用串口250ms 以前800ms
        }
    }
    return -1;
}

int RtuThread::sendDataUintV3(int addr, ushort reg, uint val1 , uint val2)
{
    sBoxData *box = &(mBusData->box[addr]); //共享内存
    if( box->offLine > 0 ){ //在线
        //打包数据
        uchar *buf = mBuf;
        int rtn = rtu_sent_uintV3_buff(addr+1, reg, 4 , val1 , val2, buf); // 把数据打包成通讯格式的数据
        return mSerial->sendData(buf, rtn, 250); //发送 -- 并占用串口250ms 以前800ms
    }
    return -1;
}

int RtuThread::sendDataUshortV3(int addr, ushort reg, uint val1 , uint val2)
{
    sBoxData *box = &(mBusData->box[addr]); //共享内存
    if( box->offLine > 0 ){ //在线
        //打包数据
        uchar *buf = mBuf;
        int rtn = rtu_sent_ushortV3_buff(addr+1, reg, 2 , val1 , val2, buf); // 把数据打包成通讯格式的数据
        return mSerial->sendData(buf, rtn, 250); //发送 -- 并占用串口250ms 以前800ms
    }
    return -1;
}

int RtuThread::sendData(uchar *pBuff, int nCount, int msec)
{
    return mSerial->sendData(pBuff, nCount, msec);
}

void RtuThread::setBoxNum(ushort num)
{
    sendData(0, 0x1040, num, false);
}

void RtuThread::loopObjData(sObjData *loop, int id, RtuRecvLine *data)
{
    loop->vol.value[id] = data->vol.svalue;
    loop->vol.crMin[id] = loop->vol.min[id] = data->vol.smin;
    loop->vol.crMax[id] = loop->vol.max[id] = data->vol.smax;

    loop->cur.value[id] = data->cur.svalue;
    loop->cur.crMin[id] = loop->cur.min[id] = data->cur.smin;
    loop->cur.crMax[id] = loop->cur.max[id] = data->cur.smax;

    loop->pow.value[id] = data->pow.ivalue;
    loop->pow.crMin[id] = loop->pow.min[id] = data->pow.imin;
    loop->pow.crMax[id] = loop->pow.max[id] = data->pow.imax;
    loop->ele[id] = data->ele;
    loop->pf[id] = data->pf;
    loop->sw[id] = data->sw;
    loop->apPow[id] = data->apPow;
    loop->reactivePower[id] = data->reactivePower;
    //    loop->ratedCur[id] = data->curAlarm; ////

    //loop->wave[id] = data->wave;
}

void RtuThread::loopData(sBoxData *box, Rtu_recv *pkt)
{
    sObjData *loop = &(box->data);
    box->loopNum = loop->lineNum = pkt->lineNum;

    for(int i=0; i<loop->lineNum; i++)
    {
        RtuRecvLine *data = &(pkt->data[i]);
        loopObjData(loop, i, data);
    }
}

void RtuThread::envData(sEnvData *env, Rtu_recv *pkt)
{
    for(int i=0; i<SENSOR_NUM; ++i)
    {
        env->tem.value[i] = pkt->env[i].tem.value;
        env->tem.crMin[i] = env->tem.min[i] = pkt->env[i].tem.min;
        env->tem.crMax[i] = env->tem.max[i] = pkt->env[i].tem.max;
    }
}

void RtuThread::thdData(Rtu_recv *pkt)
{
    sBoxData *box = &(mBusData->box[pkt->addr-1]);

    box->lps = pkt->lps;
    for(int i=0; i<3; ++i) {
        box->data.pl[i] = pkt->pl[i];
    }

    if(pkt->addr == 1) {
        int line = pkt->hc % 3;
        ushort *thd = mBusData->thdData.curThd[line];
        if(pkt->hc < 3) thd = mBusData->thdData.volThd[line];
        for(int i=0; i<32; ++i) thd[i] = pkt->thd[i];

        if(pkt->hc < 3) {
            box->data.volThd[line] = thd[0];
        } else {
            box->data.curThd[line] = thd[0];
        }
        thd[0] = 0;

    } else {
        ushort *thd = box->data.curThd;
        for(int i=0; i<3; ++i) {
            thd[i] = pkt->thd[i];
        }
    }
}

void RtuThread::loopDataV3(sBoxData *box, Rtu_recv *pkt)
{
    sObjData *loop = &(box->data);
    box->loopNum = loop->lineNum = pkt->lineNum;

    for(int i=0; i<loop->lineNum; i++)
    {
        RtuRecvLine *data = &(pkt->data[i]);
        loopObjDataV3(loop, i, data);
    }
}

void RtuThread::loopObjDataV3(sObjData *loop, int id, RtuRecvLine *data)
{
    loop->lineVol.value[id] = data->lineVol.svalue;
    loop->lineVol.crMin[id] = loop->lineVol.min[id] = data->lineVol.smin;
    loop->lineVol.crMax[id] = loop->lineVol.max[id] = data->lineVol.smax;

    loop->vol.value[id] = data->vol.svalue;
    loop->vol.crMin[id] = loop->vol.min[id] = data->vol.smin;
    loop->vol.crMax[id] = loop->vol.max[id] = data->vol.smax;

    loop->cur.value[id] = data->cur.svalue;
    loop->cur.crMin[id] = loop->cur.min[id] = data->cur.smin;
    loop->cur.crMax[id] = loop->cur.max[id] = data->cur.smax;

    loop->pow.value[id] = data->pow.ivalue;
    loop->pow.crMin[id] = loop->pow.min[id] = data->pow.imin;
    loop->pow.crMax[id] = loop->pow.max[id] = data->pow.imax;
    loop->ele[id] = data->ele;
    loop->pf[id] = data->pf;
    loop->sw[id] = data->sw;
    loop->apPow[id] = data->apPow;
    loop->reactivePower[id] = data->reactivePower;
    //    loop->ratedCur[id] = data->curAlarm; ////

    //loop->wave[id] = data->wave;
}


void RtuThread::thdDataV3(Rtu_recv *pkt)
{
    sBoxData *box = &(mBusData->box[pkt->addr-1]);

    box->lps = pkt->lps;
    for(int i=0; i<3; ++i) {
        box->data.pl[i] = pkt->pl[i];
    }

    if(pkt->addr == 1) {
        for(int line = 0 ; line < RTU_LINE_NUM ; ++line){
            box->data.volThd[line] = pkt->volThd[line][0];
            box->data.curThd[line] = pkt->curThd[line][0];
            for(int i=0; i<32; ++i){
                mBusData->thdData.curThd[line][i] = pkt->curThd[line][i];
                mBusData->thdData.volThd[line][i] = pkt->volThd[line][i];
            }
        }

    } else {
        ushort *thd = box->data.curThd;
        for(int i=0; i<3; ++i) {
            thd[i] = pkt->thd[i];
        }
    }
}

int RtuThread::transData(int addr)
{
    char offLine = 0;
    uchar *buf = mBuf;
    Rtu_recv *pkt = mRtuPkt; //数据包
    sBoxData *box = &(mBusData->box[addr]); //共享内存

    int rtn = rtu_sent_buff(addr+1 , buf , RTU_SENT_LEN_V25); // 把数据打包成通讯格式的数据
    #if (SI_RTUWIFI==1)
    rtn = mSerial->transmit_p(buf, rtn, buf); // 传输数据，发送同时接收
    static int preaddr = -1;
    if(rtn == 0)
    {
        if(preaddr == addr)
            box->offLine = 0;
        preaddr = addr;
        return 0;//什么都没有读到
    }
    if(rtn == 5)//"00 00 00 00 00"
    {
        box->offLine = 0;
        return -1;
    }
    #elif(SI_RTUWIFI==0)
//    QByteArray sendarray;
//    QString sendstrArray;
//    sendarray.append((char *)buf, rtn);
//    sendstrArray = sendarray.toHex(); // 十六进制
//    for(int i=0; i<sendarray.size(); ++i)
//        sendstrArray.insert(2+3*i, " "); // 插入空格
//    qDebug()<<"  send:" << sendstrArray;
//    qDebug()<< "rtn  "<<rtn;
    rtn = mSerial->transmit(buf, rtn, buf); // 传输数据，发送同时接收
    #endif

//    QByteArray array;
//    QString strArray;
//    array.append((char *)buf, rtn);
//    strArray = array.toHex(); // 十六进制
//    for(int i=0; i<array.size(); ++i)
//        strArray.insert(2+3*i, " "); // 插入空格
//    qDebug()<< "rtn  "<<rtn<<"  recv:" << strArray;

    if(rtn > 0) {
        bool ret = rtu_recv_packet(buf, rtn, pkt); // 解析数据 data - len - it
        if(ret) {
            if(addr+1 == pkt->addr) { //回收地址和发送地址同
                offLine = 4;
                loopData(box, pkt); //更新数据
                envData(&(box->env), pkt);
                box->rate.svalue = pkt->rate.svalue;
                box->rate.smin = pkt->rate.smin;
                box->rate.smax = pkt->rate.smax;
                box->dc = pkt->dc;
                box->version = pkt->version;
                box->data.totalPow.value[0] = pkt->totalPow.ivalue;
                box->data.totalPow.alarm[0] = pkt->totalPow.ialarm;
                box->data.totalPow.max[0] = pkt->totalPow.imax;
                box->data.totalPow.min[0] = pkt->totalPow.imin;
                box->data.totalPow.crMax[0] = pkt->totalPow.icrMax;
                box->data.totalPow.crMin[0] = pkt->totalPow.icrMin;
                thdData(pkt);
            }

            box->rtuLen = rtn;
            for(int i = 0; i < box->rtuLen; i++){
                box->rtuArray[i] = buf[i];
            }

        }else{
            box->rtuLen = 0;  //数据出错清零
        }
    }
    if(offLine) {
        box->offLine = offLine; //在线
    } else {
        if(box->offLine > 0)
            box->offLine--;
    }

    return offLine;
}

#if (SI_RTUWIFI==1)
bool RtuThread::SendCmdToWifi(int& steps,int len ,const QString& send,QString& recv)
{
    bool flag = false;
    int i = 0;//发送次数计数
    int times = 2;//重复发送次数
    steps ++;
    QString recvstr = tr("");
    uchar *buf = mBuf;
    while(i < times)//重复发两次
    {
        mSerial->transmit((uchar*)send.toLatin1().data(),len,buf);
        recvstr = QString((char*)buf);
        if( !(recvstr.contains(recv)) ) //接收"a"
        {if(i == 1)return flag;}
        else
            flag = true;
        if(flag) break;
        i++;
    }
    recv = recvstr;
    return flag;
}

void RtuThread::ChangeBusCh(int ch,int index)
{
    QString  sendstr("+++");
    QString  recvstr("a");

    int steps = 0;//计算步骤数目

    //发送+++ 接收a  进入命令模式步骤1
    if(SendCmdToWifi(steps,sendstr.length() ,sendstr,recvstr)){
        sendstr = tr("a");
        recvstr = tr("OK");
        //发送a 接收OK 进入命令模式步骤2
        if(SendCmdToWifi(steps,sendstr.length() ,sendstr,recvstr)){
            sendstr = QString("AT+CH=%1\r\n").arg( ch- 398);
            recvstr = tr("OK");
            //发送AT+CH=%1\r\n 接收OK 更改频道命令
            if(SendCmdToWifi(steps,sendstr.length() ,sendstr,recvstr)){
                sendstr = QString("AT+CH\r\n");
                recvstr = tr("OK");
                //发送AT+CH\r\n 接收OK 查询频道命令
                if(SendCmdToWifi(steps,sendstr.length() ,sendstr,recvstr)){

                    QStringList channel = recvstr.trimmed().simplified().split(":");
                    int chInt = channel.at(1).split(" ").at(0).toInt()+398;
                    //qDebug()<<channel.at(1)<<chInt;
                    if(index == mIndex)
                        getHZ(chInt);
                }
                sendstr = QString("AT+Z\r\n");
                recvstr = tr("OK");
                //发送AT+Z\r\n 接收OK 退出命令模式
                if(SendCmdToWifi(steps,sendstr.length() ,sendstr,recvstr)){
                }
            }
        }
    }
}

void RtuThread::sendSettingCmdData(int index)
{
    QList<sBusCmd> cmds = gBusCmd.values(index);
    if(!cmds.isEmpty()){

        for(int i = 0 ; i < cmds.size(); ++i){
            sendData(cmds.at(i).addr , cmds.at(i).reg , cmds.at(i).len , false);
            gBusCmd.remove(index , cmds.at(i));
        }
        //gBusCmd.remove(index);
    }
}
#endif

void RtuThread::BusTransData()
{
    for(int i=0; i<=mBusData->boxNum; ++i)
    {
        if(transData(i) == 0 ) {
            msleep(900);//900
            transData(i);
        }
#if( SI_RTUWIFI == 0)
        msleep(750);//750
#endif
#if( SI_RTUWIFI == 1)
        msleep(900);
#endif
    }
}

void RtuThread::BusTransDataV3()
{
    for(int i=0; i<=mBusData->boxNum; ++i)
    {
        int ret = transDataV3(i);
        if( ret == 0 ) {
            msleep(900);//900
            transDataV3(i);
        }
        msleep(750);//750
    }
}

void RtuThread::initData(sBoxData *box, Rtu_recv *pkt)
{
    box->dc = pkt->dc;
    box->version = pkt->version;
    box->lps = pkt->lps;
    box->lpsAlarm = pkt->lpsState;
    box->proNum = pkt->proNum;
    box->curSpecification = pkt->curSpecification;
    box->workMode = pkt->workMode; // 通讯协议版本
    box->baudRate = pkt->baudRate;
    box->buzzerStatus = pkt->buzzerStatus;
    box->alarmTime = pkt->alarmTime;
    box->iOF = pkt->iOF;
    box->isd = pkt->isd;
    box->reState = pkt->reState;
}

void RtuThread::readLocalTemHum()
{
    if(0 == mId){
        sBoxData *box = &(mBusData->box[0]); //共享内存
        int data[4];
        int fd;
        char *path = "/dev/sht30";
        fd = open(path, O_RDONLY);
        if (fd < 0)
        {
            //perror("open");
            return ;
        }
        bzero(data, sizeof(data));
        if (read(fd, data, sizeof(data)) < 0){
            perror("read");
        }
        else{
            //for (int i = 0; i < sizeof(data) / sizeof(data[0]) / 2; i++){
            for (int i = 0; i < sizeof(data) / sizeof(data[0]) / 2; i++){
                if (data[i * 2] == -1 || data[i * 2 + 1] == -1)
                    printf("th%d不存在\n", i);
                else{
                    if(i == 0){
                        //printf("th%d:温度(%d),湿度(%d)\n", i, data[i * 2], data[i * 2 + 1]);
                        box->env.tem.value[7] = data[i * 2];
                        box->env.tem.value[8] = data[i * 2 + 1];
                    }
                }
            }
        }
        close(fd);
    }
}

int RtuThread::transDataV3(int addr)
{
    char offLine = 0;
    uchar *buf = mBuf;
    Rtu_recv *pkt = mRtuPkt; //数据包
    sBoxData *box = &(mBusData->box[addr]); //共享内存
    readLocalTemHum();

    int rtn = rtu_sent_buff(addr+1,buf); // 把数据打包成通讯格式的数据
//        QByteArray sendarray;
//        QString sendstrArray;
//        sendarray.append((char *)buf, rtn);
//        sendstrArray = sendarray.toHex(); // 十六进制
//        for(int i=0; i<sendarray.size(); ++i)
//            sendstrArray.insert(2+3*i, " "); // 插入空格
//        qDebug()<<"  send:" << sendstrArray;
//        qDebug()<< "rtn  "<<rtn;
    rtn = mSerial->transmitV3(buf, rtn, buf); // 传输数据，发送同时接收
//        QByteArray array;
//        QString strArray;
//        array.append((char *)buf, rtn);
//        strArray = array.toHex(); // 十六进制
//        for(int i=0; i<array.size(); ++i)
//            strArray.insert(2+3*i, " "); // 插入空格
//        qDebug()<< "rtn  "<<rtn<<"  recv:" << strArray;

    if(rtn > 0) {
        bool ret = rtu_recv_packetV3(buf, rtn, pkt); // 解析数据 data - len - it
        if(ret) {
            if(addr+1 == pkt->addr) { //回收地址和发送地址同
                box->boxOffLineAlarm = 1;
                offLine = 4;
                loopData(box, pkt); //更新数据
                envData(&(box->env), pkt);
                initData(box, pkt);
                box->rate.svalue = pkt->rate.svalue;
                box->rate.smin = pkt->rate.smin;
                box->rate.smax = pkt->rate.smax;
                box->reCur.svalue = pkt->reCur.svalue;
                box->reCur.smin = pkt->reCur.smin;
                box->reCur.smax = pkt->reCur.smax;
                box->zeroLineCur.svalue = pkt->zeroLineCur.svalue;
                box->zeroLineCur.smin = pkt->zeroLineCur.smin;
                box->zeroLineCur.smax = pkt->zeroLineCur.smax;
                box->volUnbalance = pkt->volUnbalance;
                box->curUnbalance = pkt->curUnbalance;

                box->data.totalPow.value[0] = pkt->totalPow.ivalue;
                box->data.totalPow.min[0] = pkt->totalPow.imin;
                box->data.totalPow.max[0] = pkt->totalPow.imax;
                thdDataV3(pkt);
            }

            box->rtuLen = rtn;
            for(int i = 0; i < box->rtuLen; i++){
                box->rtuArray[i] = buf[i];
            }

        }else{
            box->rtuLen = 0;  //数据出错清零
        }
    }
    if(offLine) {
        box->offLine = offLine; //在线
    } else {
        if(box->offLine > 0)
            box->offLine--;
    }

    return offLine;
}

void RtuThread::run()
{
    isRun = true;
    while(isRun)
    {
#if( SI_RTUWIFI == 0)
        if(gVerflag == 1){
            ushort num = gBoxArray[mId];
            if(num) {
                setBoxNum(num);
                gBoxArray[mId] = 0;
            }
            BusTransData();
        }else if(gVerflag == 2){
            BusTransDataV3();
        }
        else
            sleep(1000);
#endif
#if( SI_RTUWIFI == 1)
        for(int k = 1 ; k <= 4 ; ++k)
        {
            sDataPacket *shm = get_share_mem(); // 获取共享内存
            mBusData = &(shm->data[k-1]);
            if(mBusData->boxNum && !mChList.at(k-1).isEmpty())
            {
                if(mRecoder != k)
                ChangeBusCh(mChList.at(k-1).toInt(),k-1);//切换频道
                mRecoder = k;

                ushort num = gBoxArray[k-1];
                if(num) {
                    setBoxNum(num);                 //发送数量给始端箱
                    gBoxArray[k-1] = 0;
                }
                sendSettingCmdData(k-1);           //发送设置命令
                BusTransData();                    //轮询插接箱
            }
        }
#endif

    }
}
