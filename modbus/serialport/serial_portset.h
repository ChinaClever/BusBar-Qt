#ifndef SERIAL_PORTSET_H
#define SERIAL_PORTSET_H

/**
  * 设置串口
  */
#include <QtCore>

#define ARM_LINUX 3

#if ARM_LINUX == 1
#define SERIAL_COM1     "ttyS8"
#define SERIAL_COM2     "ttyS2"
#define SERIAL_COM3     "ttyS7"
#define SERIAL_COM4     "ttyS4"
#define SERIAL_COM5     "ttyS1"
#elif ARM_LINUX == 2  //机柜屏
#define SERIAL_COM1     "ttymxc2"
#define SERIAL_COM2     "ttymxc3"
#define SERIAL_COM3     "ttymxc4"
#define SERIAL_COM4     "ttymxc6"
#define SERIAL_COM5     "ttymxc1"
#else
//Android
//#define SERIAL_COM1     "ttyS0"
//#define SERIAL_COM2     "ttyS2"
//#define SERIAL_COM3     "ttyS3"
//#define SERIAL_COM4     "ttyS4"
//#define SERIAL_COM5     "usb/tty1-1.3"
//#define SERIAL_COM6     "usb/tty1-1.2"
//Android
#define SERIAL_COM1     "ttymxc5"
#define SERIAL_COM2     "ttymxc4"
#define SERIAL_COM3     "ttymxc1"
#define SERIAL_COM4     "ttymxc6"
#define SERIAL_COM5     "ttymxc2"
#endif

//#define SERIAL_BAUDRATE    "B9600"
#define SERIAL_BAUDRATE    "B57600"


struct Serial_PortData
{
    QString devName;
    QString baudRate;
    QString dataBit;
    QString parity;
    QString stopBit;
};


class Serial_PortSet
{
public:
    Serial_PortSet();

    void setPortName(const QString &, Serial_PortData &); // 设置端口名
    void setBaudRate(const QString, Serial_PortData &); // 设置波特率

private:
     Serial_PortData m_serialData;
};

#endif // SERIAL_PORTSET_H
