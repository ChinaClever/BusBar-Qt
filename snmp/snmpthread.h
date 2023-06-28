#ifndef SNMPTHREAD_H
#define SNMPTHREAD_H

#include <QThread>
#include "common.h"
#include "rtuthread.h"

#define MIB_OID_HEAD "SNMPv2-SMI::enterprises.30966.12"
extern int gVerflag;
class SnmpThread : public QThread
{
    Q_OBJECT
public:
    explicit SnmpThread(QObject *parent = nullptr);
    bool init(int id);
    ~SnmpThread();
protected:
    void run();
    int initSnmp();
    void praseMasterVal(QString str);

    int getItemByOid(int id);
    void baseMsInformation(QString val);
    void loopMsInformation(QString val);
    void temMsInformation(QString val);
    void startBoxBaseInfo(QString val);
    void startBoxEleParaInfo(QString val);
    void parseVolHar(QString val, int line);
    void parseCurHar(QString val, int line);


    void praseSlaveVal(QString str);
    void baseSlaveInformation(QString val , int addr);
    void loopSlaveInformation(QString val , int addr);
    void temSlaveInformation(QString val , int addr);

private:
    sBusData *mBusData;
    int mId;
    QString mOid;
    QString mValue;
    bool isRun;
};

#endif // SNMPTHREAD_H
