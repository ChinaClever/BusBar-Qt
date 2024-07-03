#ifndef LOGSYSTEMEXPORTTHREAD_H
#define LOGSYSTEMEXPORTTHREAD_H

#include <QObject>
#include "excel/excelexportwid.h"
#include "dbsystem.h"

class LogSystemExportThread : public QThread
{
    Q_OBJECT
public:
    explicit LogSystemExportThread(QObject *parent = 0);
    ~LogSystemExportThread();

    QList<QStringList> mList;
    void initData();

signals:
    void readDbSig(QString);
    void exportSig(bool);

protected:
    void run();
    bool readDb();

private:
    DbSystem* mSystem;
    sExcelExportStr *mExcelStr;
};
extern sExcelExportStr *gExcelExportStr;

#endif // LOGSYSTEMEXPORTTHREAD_H
