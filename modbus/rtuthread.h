#ifndef RTUTHREAD_H
#define RTUTHREAD_H

#include <QThread>
#include "common/common.h"
#include "serialport/serial_trans.h"
#include "rtu485/rtu_recv.h"

#define RTU_BUF_SIZE 1024
extern int gVerflag;//1代表一期 2代表二期

class RtuThread : public QThread
{
    Q_OBJECT
public:
    explicit RtuThread(QObject *parent = 0);
    ~RtuThread();

    bool init(const QString& name, int id);
    int transmit(int addr, ushort reg, uint len); //发送数据并回收
    int sendData(int addr, ushort reg, uint len, bool value = false); //发送数据
    int sendData(uchar *pBuff, int nCount, int msec); //发送数据
    int sendDataUintV3(int addr, ushort reg, uint val1 , uint val2);
    int sendDataUshortV3(int addr, ushort reg, uint val1 , uint val2);
    
signals:    
    
protected:
    void run();
    int transData(int addr);

    void loopObjData(sObjData *loop, int id, RtuRecvLine *data);
    void loopData(sBoxData *box, Rtu_recv *pkt);
    void loopDataV3(sBoxData *box, Rtu_recv *pkt);
    void loopObjDataV3(sObjData *loop, int id, RtuRecvLine *data);

    void envData(sEnvData *env, Rtu_recv *pkt);
    void initData(sBoxData *box, Rtu_recv *pkt);
    void setBoxNum(ushort num);
    void thdData(Rtu_recv *pkt);
    void thdDataV3(Rtu_recv *pkt);
    void BusTransData();

    int transDataV3(int addr);
    void BusTransDataV3();
    void readLocalTemHum();

    #if (SI_RTUWIFI==1)
    void ChangeBusCh(int ch, int index);
    bool SendCmdToWifi(int& steps,int len ,const QString& send,QString& recv);
    void sendSettingCmdData(int index);
    #endif
public:
    Serial_Trans *mSerial;
private:
    uchar *mBuf;
    Rtu_recv *mRtuPkt;
    sBusData *mBusData;
    int mId;
    bool isRun;
};
#if (SI_RTUWIFI == 1)
void set_ch(const QStringList &chs, int recordindex);
void insertBusCmd(int busID, int addr, ushort reg, ushort len);
#endif
#endif // RTUTHREAD_H
