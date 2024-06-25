#ifndef LOGSYSTEMEXPORTDLG_H
#define LOGSYSTEMEXPORTDLG_H

#include <QDialog>
#include "logsystemexportthread.h"
#include "excel/excelexportwid.h"

namespace Ui {
class LogSystemExportDlg;
}

class LogSystemExportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LogSystemExportDlg(QWidget *parent = 0);
    ~LogSystemExportDlg();

    void set();
    void initLanguage();
protected slots:
     void readDbSlot(QString);
     void exportSlot(bool);
     void overSlot();

private slots:
     void on_pushButton_clicked();

private:
    Ui::LogSystemExportDlg *ui;
    LogSystemExportThread *mDbThread;
    ExcelExportWid *mProgressWid;
};

#endif // LOGSYSTEMEXPORTDLG_H
