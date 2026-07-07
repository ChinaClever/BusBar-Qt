#ifndef SERIAL_TRANS_H
#define SERIAL_TRANS_H

/**
  * 串口基本操作 打开、关闭、读写
  */
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <QObject>
#include <qsocketnotifier.h>
//#include <sys/termios.h>
#include <termios.h>
#include <QDebug>
#include <QMutexLocker>
#include "serial_portset.h"
//// 在头文件或全局定义中
extern QMutex g_rtuMutex;

class Serial_Trans : public QThread
{
    Q_OBJECT
public:
    int fd; //串口句柄
    explicit Serial_Trans(QObject *parent = 0);
    ~Serial_Trans();
    
    bool openSerial(const QString);
    int getSerialFd(void);
    void closeSerial();

    int sendData(uchar *pBuff, int nCount, int msec);
    int sendData(uchar *pBuff, int nCount);
    int recvData(uchar *pBuf, int msecs);
    int transmit(uchar *sent, int len, uchar *recv);
    int transmit_p(uchar *sent, int len, uchar *recv);

    int recvDataV3(uchar *pBuf, int msecs);
    int recvDataRecvResultV3(uchar *pBuf, int msecs);
    int transmitV3(uchar *sent, int len, uchar *recv);
    int transmitRecvV3(uchar *sent, int len, uchar *recv, int msec=100);

protected:
    int readData(uchar *pBuf, int nCount);

signals:
    void serialReadSig();
    void serialClosed();
    void closeSerialSig();

protected  slots:
    void readDataSlot();
    void closeSerialSlot();

private:

    QMutex mutex; // 互拆锁
    QSocketNotifier * monitor;
};

#endif // SERIAL_TRANS_H
