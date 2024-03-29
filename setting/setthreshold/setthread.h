#ifndef SETTHREAD_H
#define SETTHREAD_H

#include "setnetcmd.h"
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
