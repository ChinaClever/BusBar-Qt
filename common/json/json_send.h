#ifndef JSON_SEND_H
#define JSON_SEND_H
#include "json_pack.h"
#include "network.h"
#include <QThread>
#include "tcpsent.h"

#ifdef Q_OS_LINUX
#include <unistd.h>
#endif

#define NAME_SIZE  48
struct sRunTime
{
    uint runSec; // 最近开关运行时间 秒钟为单位
    uint resetCnt; // 重启次数；
    uint daemonSec; // 守护进程使用
    char md5[NAME_SIZE]; // 运行程序的ＭＤ５值
    char start[NAME_SIZE]; // 启动时间
    char compileTime[NAME_SIZE]; // 编译时间
    char reserve[3][NAME_SIZE];
};

class Json_Send : public QThread
{
    Q_OBJECT
    explicit Json_Send(QObject *parent = nullptr);
public:
    static Json_Send *bulid(QObject *parent = nullptr);
    void initFun();
protected:
    void sendData();
    void TcpsendData();
    void Startbox_Alarm(QJsonObject &obj ,int id);


    QString executeCommand(const QString &cmd);
    int freeMemory();
    void freeMemoryCheck();
    void resetProc(sRunTime &proc, const QString &name);
    void mdelay(int msec);
    void proc_log(const QString &arg);
    bool proc_isRun(const QString &p);
    int isRunning(const char *procname);
    void proc_start(sRunTime &proc, const QString &app);
    void proc_time(sRunTime &proc);
    void proc_md5(sRunTime &proc, const QString &fn);
    QString md5(const QString &fn);
private slots:
    void run();
private:
    Json_Pack *mJson;
    QTimer *timer;
    UdpSentSocket *mSocket;
    TcpClient *mTcp;
    sBusData *mBus[4];
    QReadWriteLock  *mLock;
    int *mBoxNum[BUS_NUM];

};

#endif // JSON_SEND_H
