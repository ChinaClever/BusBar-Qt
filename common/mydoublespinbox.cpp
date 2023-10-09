#include "mydoublespinbox.h"

MyDoubleSpinBox::MyDoubleSpinBox(QWidget * p_parent):QDoubleSpinBox(p_parent)
{
    ExtendedLineEdit *lineEdit = new ExtendedLineEdit(this);
    setLineEdit(lineEdit);
}

MyDoubleSpinBox::~MyDoubleSpinBox()
{

}
