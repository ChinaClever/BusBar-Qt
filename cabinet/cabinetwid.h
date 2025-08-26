#ifndef CABINETWID_H
#define CABINETWID_H

#include <QWidget>
#include "cabinettablewidget.h"

namespace Ui {
class CabinetWid;
}

class CabinetWid : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetWid(QWidget *parent = 0);
    ~CabinetWid();

signals:
    void busChangedSig(int);

private:
    Ui::CabinetWid *ui;
    CabinetTableWidget *mTableWid;
};

#endif // CABINETWID_H
