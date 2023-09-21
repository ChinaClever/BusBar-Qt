#ifndef SNMPTHREAD_H
#define SNMPTHREAD_H

#include <QThread>
#include "common.h"
#include "rtuthread.h"
#include "setting/setthreshold/setthread.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
//#include <net-snmp/agent/net-snmp-agent-includes.h>
//#include <net-snmp/library/snmpusm.h>

#define MIB_OID_HEAD "SNMPv2-SMI::enterprises.30966.12"
extern int gVerflag;
extern int gReadWriteflag;//1读取 2写入
class SnmpThread : public QThread
{
    Q_OBJECT
public:
    explicit SnmpThread(QObject *parent = nullptr);
    bool init(int id);
    ~SnmpThread();
    bool mOpen,mClose;
protected:
    void run();
    int initSnmp(netsnmp_session &session , netsnmp_session **ss);
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
    int walkSnmp(netsnmp_session & session,netsnmp_session **ss,netsnmp_pdu *response,netsnmp_pdu *pdu , int index);
    void releaseCon(netsnmp_session &session, netsnmp_session **ss, netsnmp_pdu *response);
    void praseSlaveVal(QString str , int index);
    bool setOid(netsnmp_session **ss, netsnmp_pdu *response, netsnmp_pdu *pdu);

public slots:
    void recvSendSetSlot(sThresholdItem *item);

private:
    void getMasterVolOid(sThresholdItem &item , oid* target_min, oid *target_max);
    void getMasterCurOid(sThresholdItem &item , oid* target_min, oid *target_max);
    void getMasterActivePowerOid(sThresholdItem &item , oid* target_min, oid *target_max);
    void getMasterFrequencyOid(oid* target_min, oid *target_max);
    void getMasterTemperatureOid(sThresholdItem &item , oid* target_min, oid *target_max);

    void getSlaveVolOid(sThresholdItem &item , oid* target_min, oid *target_max);
    void getSlaveCurOid(sThresholdItem &item , oid* target_min, oid *target_max);
    void getSlaveActivePowerOid(sThresholdItem &item , oid* target_min, oid *target_max);
    void getSlaveTemperatureOid(sThresholdItem &item , oid* target_min, oid *target_max);


private:
    sBusData *mBusData;
    int mId;
    QString mOid;
    QString mValue;
    bool isRun;
    QList<sThresholdItem> mItems;
};

#endif // SNMPTHREAD_H
