#ifndef SETTHREAD_H
#define SETTHREAD_H

#include "setnetcmd.h"
#include "sql/dboperation.h"

extern int gVerflag;//1代表一期 2代表二期
extern int gReadWriteflag;
class SetThread : public QThread
{
    Q_OBJECT
    explicit SetThread(QObject *parent = nullptr);
public:
    ~SetThread();
    static SetThread *bulid(QObject *parent = nullptr);
    void append(sThresholdItem &item) {mItems.append(item);}
    void workDown();
    QList<sThresholdItem> mItems;
    SetRtuCmd *mRtuCmd;
protected:
    void run();
    void change(sThresholdItem &item , QString &msg1, QString &msg2, QString &msgen1, QString &msgen2, int index = 0);
    QString changeType(int index, QString &sym, double &rate);
    QString calcLoop(int id);

signals:
    void sendSetSnmpSig(sThresholdItem *);
//    void sendSetThreadTripSig(QString mac, int val);

private:
    bool isRun;


    SetShm *mSetShm;
    SetNetCmd *mNetCmd;

};

#endif // SETTHREAD_H
