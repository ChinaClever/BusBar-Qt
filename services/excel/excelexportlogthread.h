#ifndef EXCELEXPORTLOGTHREAD_H
#define EXCELEXPORTLOGTHREAD_H

#include <QThread>
#include <QtCore>
#include "common.h"
#include "sql/dbsystem.h"

struct sExcelExportStr
{
    QString path; // 导出路径
    QString file; // 导出文件名
    QString db; // 数据库表
    QString start; // 开始时间
    QString end; // 结束时间
    QStringList title; // 标头
};


class ExcelExportLogThread : public QThread
{
    Q_OBJECT
public:
    explicit ExcelExportLogThread(QObject *parent = 0);
    ~ExcelExportLogThread();

    void set(const QString &path, const QString &file, const QList<QStringList> &list, int type);

signals:
    void overSig(bool);
    void progressSig(int);

protected:
    void run();
    int getProgress();
    virtual void exportMsg(QList<QStringList> &list);
    QString transformer(int type);

protected slots:
    void progressSlot();

protected:
    int mSize, mAllSize;
    QList<QStringList> mList;
    QString mFile; // 导出文件
    QString mPath; // 文件路径
    int mType;
};
extern sExcelExportStr *gExcelExportStr;
#endif // EXCELEXPORTLOGTHREAD_H
