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

protected:
    void run();
    void workDown();
    void change(sThresholdItem &item , QString &msg1, QString &msg2, int index = 0);
    QString changeType(int index, QString &sym, double &rate);
    QString calcLoop(int id);

signals:
    void sendSetSnmpSig(sThresholdItem *);

private:
    bool isRun;


    SetShm *mSetShm;
    SetRtuCmd *mRtuCmd;
    SetNetCmd *mNetCmd;
    QList<sThresholdItem> mItems;
};

#endif // SETTHREAD_H
