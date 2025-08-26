#ifndef SETCABNAMEDLG_H
#define SETCABNAMEDLG_H

#include <QDialog>
namespace Ui {
class SetCabNameDlg;
}

class SetCabNameDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SetCabNameDlg(QWidget *parent = 0);
    ~SetCabNameDlg();

    void init(int bus, int box, int loop, const QString &name);
protected:
    bool save();
    bool saveToDev();

private slots:
    void on_saveBtn_clicked();
    void on_cancelBtn_clicked();

private:
    Ui::SetCabNameDlg *ui;
    int mBusId, mBox, mLoop;
};

#endif // SETCABNAMEDLG_H
