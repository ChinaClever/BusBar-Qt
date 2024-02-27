#ifndef AUTOSETADDRESS_H
#define AUTOSETADDRESS_H

#include <QWidget>
#include "common.h"
#include <QProgressBar>

namespace Ui {
class AutoSetAddress;
}

class AutoSetAddress : public QWidget
{
    Q_OBJECT

public:
    explicit AutoSetAddress(QWidget *parent = nullptr);
    ~AutoSetAddress();

public slots:
    void getNumAndIndexSlot(int index , int curnum);
    void getDelaySlot(int index);

private slots:
    void on_autoSet1Btn_clicked();
    void on_autoSet2Btn_clicked();
    void on_autoSet3Btn_clicked();
    void on_autoSet4Btn_clicked();

signals:
    void autoSetBus1Sig(int index);
    void autoSetBus2Sig(int index);
    void autoSetBus3Sig(int index);
    void autoSetBus4Sig(int index);

private:
    Ui::AutoSetAddress *ui;
    void setProgressbarPowValue(QProgressBar *bar,int value, int index);
    void setProcessBarColor(QProgressBar *bar, QString color);
    int mCurBoxNum[4];
};

#endif // AUTOSETADDRESS_H
