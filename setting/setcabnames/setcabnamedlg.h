#ifndef SETCABNAMEDLG_H
#define SETCABNAMEDLG_H

#include <QDialog>
#include <QComboBox>
#include "shm/setshm.h"
namespace Ui {
class SetCabNameDlg;
}

class SetCabNameDlg : public QDialog
{
    Q_OBJECT

public:
    explicit SetCabNameDlg(QWidget *parent = 0);
    ~SetCabNameDlg();

    void init(int bus, int box, SetShm *setShm);
protected:
    bool save();
    void initBox(QComboBox *box , int type);
//    bool saveToDev();

private slots:
    void on_saveBtn_clicked();
    void on_cancelBtn_clicked();

    void on_lineA_NoBox_currentIndexChanged(int index);

private:
    Ui::SetCabNameDlg *ui;
    int mBusId, mBox;
    SetShm *mSetShm;
};

#endif // SETCABNAMEDLG_H
