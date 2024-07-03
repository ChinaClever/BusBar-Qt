#ifndef LOGALARMEXPORTDLG_H
#define LOGALARMEXPORTDLG_H

#include <QDialog>
#include "logalarmexportthread.h"
#include "excel/excelexportwid.h"

namespace Ui {
class LogAlarmExportDlg;
}

class LogAlarmExportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LogAlarmExportDlg(QWidget *parent = 0);
    ~LogAlarmExportDlg();

    void set(int id);
    void initLanguage();
protected slots:
     void readDbSlot(QString);
     void exportSlot(bool);
     void overSlot();

private slots:
     void on_pushButton_clicked();

private:
    Ui::LogAlarmExportDlg *ui;
    LogAlarmExportThread *mDbThread;
    ExcelExportWid *mProgressWid;
};

#endif // LOGALARMEXPORTDLG_H
