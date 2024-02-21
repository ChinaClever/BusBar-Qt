#ifndef SERIALDATA_H
#define SERIALDATA_H

#include <QWidget>
#include "common/common.h"
#include "setshm.h"
namespace Ui {
class Serialdata;
}

class Serialdata : public QWidget
{
    Q_OBJECT

public:
    explicit Serialdata(QWidget *parent = nullptr);
    ~Serialdata();
private:
    void initWid();

private slots:
    void initFunSLot();

private:
    Ui::Serialdata *ui;
    sBoxData *mData;
};

#endif // SERIALDATA_H
