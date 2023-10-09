#include "extendedlineedit.h"

ExtendedLineEdit::ExtendedLineEdit(QWidget * p_parent):QLineEdit(p_parent)
{

}

void ExtendedLineEdit::mousePressEvent(QMouseEvent *event)
{
    if( event->button() == Qt::LeftButton ){
        selectAll();
        event->accept();
        return;
    }
    QLineEdit::mousePressEvent(event);
}
