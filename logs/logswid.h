#ifndef LOGSWID_H
#define LOGSWID_H

#include <QWidget>
#include "logmain/logmainelewid.h"
#include "logbranch/logbranchelewid.h"
#include "logalarm/logalarmwid.h"
#include "logoperation/logoperationwid.h"
#include "logsystem/logsystemwid.h"

namespace Ui {
class LogsWid;
}

class LogsWid : public QWidget
{
    Q_OBJECT

public:
    explicit LogsWid(QWidget *parent = 0);
    ~LogsWid();

protected:
    void initWidget();

signals:
    void busChangedSig(int);

private slots:
    void logTypeSlot(int);

private:
    Ui::LogsWid *ui;

    LogMainEleWid *mMainEleWid;
    LogBranchEleWid *mBranchEleWid;
    LogAlarmWid *mAlarmWid;
    LogOperationWid *mOperationWid;
    LogSystemWid *mSystemWid;
};

#endif // LOGSWID_H
