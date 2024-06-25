#ifndef LOGOPERATIONEXPORTDLG_H
#define LOGOPERATIONEXPORTDLG_H

#include <QDialog>
#include "logoperationexportthread.h"
#include "excel/excelexportwid.h"

namespace Ui {
class LogOperationExportDlg;
}

class LogOperationExportDlg : public QDialog
{
    Q_OBJECT

public:
    explicit LogOperationExportDlg(QWidget *parent = 0);
    ~LogOperationExportDlg();

    void set(int id);
    void initLanguage();
protected slots:
     void readDbSlot(QString);
     void exportSlot(bool);
     void overSlot();

private slots:
     void on_pushButton_clicked();

private:
    Ui::LogOperationExportDlg *ui;
    LogOperationExportThread *mDbThread;
    ExcelExportWid *mProgressWid;
};

#endif // LOGOPERATIONEXPORTDLG_H
