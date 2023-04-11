#ifndef SETLINETEM_H
#define SETLINETEM_H

#include "setlinewid.h"

namespace Ui {
class SetLineTem;
}

class SetLineTem : public QWidget
{
    Q_OBJECT

public:
    explicit SetLineTem(QWidget *parent = 0 , bool flag = true);
    ~SetLineTem();

    void indexChanged(int index);

protected:
    void updateWid();
    void setTem(int id);
    void setBtnColor(QPushButton *label, int alarm, int crAlarm);

protected slots:
    void timeoutDone();

private slots:
    void on_temBtn_1_clicked();

    void on_temBtn_2_clicked();

    void on_temBtn_3_clicked();

    void on_temBtn_4_clicked();

private:
    Ui::SetLineTem *ui;
    QTimer *timer;
    int mBus;
    sDataUnit *startBoxTem;
    bool mFlag;
    sDataPowUnit *startBoxPow;
};

#endif // SETLINETEM_H
