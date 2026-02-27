#ifndef DPALARMSLAVE_H
#define DPALARMSLAVE_H

#include <QObject>
#include "common/common.h"

class DpAlarmSlave : public QThread
{
    Q_OBJECT
public:
    explicit DpAlarmSlave(QObject *parent = 0);
    ~DpAlarmSlave();
signals:

protected:
    void run();
    void unitAlarm(QString &typeStr, QString &msg, QString &typeStrEn, QString &msgEn, sDataUnit &unit, double rate, const QString &sym);
    void unitAlarm(QString &typeStr, QString &msg, QString &typeStrEn, QString &msgEn, sDataPowUnit &unit, double rate, const QString &sym);
    void unitAlarmVA(int bus, int addr, sBoxData &box, QString &typeStr, QString &msg, QString &typeStrEn, QString &msgEn, sDataUnit &unit, double rate, const QString &sym);
    void unitAlarmW(int bus, int addr, sBoxData &box, QString &typeStr, QString &msg, QString &typeStrEn, QString &msgEn, sDataPowUnit &unit, double rate, const QString &sym);

    void boxAlarm(int bus, sBoxData &box, int id);
    void busAlarm(int id);
    void checkAlarm();

    void saveMsg(const QString &typeStr, const QString &str , const QString &typeStrEn, const QString &strEn);
//    void getCabColNameAndCabName(int line_no , int line_tapoff_no ,int line_tapoff_line , QString & cabCalName , QString & cabname);

protected slots:
    void timeoutDone();

private:
     bool isRun;
     QTimer *timer;
     sDataPacket *shm;
     int mBusId;
     QStringList mAlarmStr;
};

QString get_email_str();
QStringList get_alarm_str();
QStringList get_alarm_json();
#endif // DPALARMSLAVE_H
