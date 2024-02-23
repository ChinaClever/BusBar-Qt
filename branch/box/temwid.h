#ifndef TEMWID_H
#define TEMWID_H

#include <QDialog>
#include "temmain.h"

namespace Ui {
class TemWid;
}

class TemWid : public QDialog
{
    Q_OBJECT

public:
    explicit TemWid(QWidget *parent = 0);
    ~TemWid();
    void updateWid(int bus, int box);
private slots:
    void on_pushButton_clicked();
private:
    Ui::TemWid *ui;
    TemMain *mTemMain;
    sBoxData *mData;
};
#endif // TEMWID_H
