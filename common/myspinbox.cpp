#include "myspinbox.h"

MySpinBox::MySpinBox(QWidget * p_parent):QSpinBox(p_parent)
{
    ExtendedLineEdit *lineEdit = new ExtendedLineEdit(this);
    setLineEdit(lineEdit);
}

MySpinBox::~MySpinBox()
{

}
