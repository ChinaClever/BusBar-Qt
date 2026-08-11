#ifndef BOXOUTPUTWID_H
#define BOXOUTPUTWID_H

#include <QWidget>
#include "common/common.h"

namespace Ui {
class BoxOutputWid;
}

class BoxOutputWid : public QWidget
{
    Q_OBJECT

public:
    explicit BoxOutputWid(QWidget *parent = 0);
    ~BoxOutputWid();

    void initFun(int bus, int box);
    void initlanguage();
protected:
    void updateData();
    void setLabeColor(QLabel *, int , int);
    void initWid();

protected slots:
    void timeoutDone();

private:
    Ui::BoxOutputWid *ui;
    sBoxData *mBox;
    QTimer *timer;
};

#endif // BOXOUTPUTWID_H
