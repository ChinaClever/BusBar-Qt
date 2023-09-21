#ifndef MODEDLG_H
#define MODEDLG_H

#include <QDialog>
#include "common.h"

extern int gVerflag;//1代表一期 2代表二期
namespace Ui {
class ModeDlg;
}

class ModeDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ModeDlg(QWidget *parent = nullptr);
    ~ModeDlg();
    void initMode();

private slots:
    void on_changeModeBtn_clicked();

private:
    Ui::ModeDlg *ui;
};

#endif // MODEDLG_H
