#ifndef SNMPTHREAD_H
#define SNMPTHREAD_H

#include <QThread>
#include "common.h"
#include "rtuthread.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

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
    int initSnmp(netsnmp_session &session, netsnmp_session **ss);
    void praseMasterVal(QString str);

    int getItemByOid(int id);
    void baseMsInformation(QString val);
    void loopMsInformation(QString val);
    void temMsInformation(QString val);
    void startBoxBaseInfo(QString val);
    void startBoxEleParaInfo(QString val);
    void parseVolHar(QString val, int line);
    void parseCurHar(QString val, int line);


    void baseSlaveInformation(QString val , int addr);
    void loopSlaveInformation(QString val , int addr);
    void temSlaveInformation(QString val , int addr);
    int walkSnmp(netsnmp_session **ss,netsnmp_pdu *response,netsnmp_pdu *pdu , int index);
    void releaseCon(netsnmp_session &session, netsnmp_session **ss, netsnmp_pdu *response);
    void praseSlaveVal(QString str , int index);
private:
    sBusData *mBusData;
    int mId;
    QString mOid;
    QString mValue;
    bool isRun;
};

#endif // SNMPTHREAD_H
