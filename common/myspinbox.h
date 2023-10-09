#ifndef MYSPINBOX_H
#define MYSPINBOX_H

#include <QSpinBox>
#include <QWidget>
#include "extendedlineedit.h"

class MySpinBox : public QSpinBox
{
    Q_OBJECT
public:
    explicit MySpinBox(QWidget * p_parent=nullptr);
    ~MySpinBox();
};

#endif // MYSPINBOX_H
