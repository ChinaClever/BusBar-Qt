#ifndef LOGOPERATIONEXPORTTHREAD_H
#define LOGOPERATIONEXPORTTHREAD_H

#include <QObject>
#include "excel/excelexportwid.h"
#include "dboperation.h"

class LogOperationExportThread : public QThread
{
    Q_OBJECT
public:
    explicit LogOperationExportThread(QObject *parent = 0);
    ~LogOperationExportThread();

    QList<QStringList> mList;
    void initData(int busId);

signals:
    void readDbSig(QString);
    void exportSig(bool);

protected:
    void run();
    bool readDb();

private:
    DbOperation* mOperation;
    sExcelExportStr *mExcelStr;
};
extern sExcelExportStr *gExcelExportStr;

#endif // LOGOPERATIONEXPORTTHREAD_H
