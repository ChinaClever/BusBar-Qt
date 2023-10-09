#ifndef MYDOUBLESPINBOX_H
#define MYDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include <QWidget>
#include "extendedlineedit.h"

class MyDoubleSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit MyDoubleSpinBox(QWidget * p_parent=nullptr);
    ~MyDoubleSpinBox();
};

#endif // MYDOUBLESPINBOX_H
