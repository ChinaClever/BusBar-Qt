#ifndef EXTENDEDLINEEDIT_H
#define EXTENDEDLINEEDIT_H

#include <QLineEdit>
#include <QMouseEvent>

class ExtendedLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit ExtendedLineEdit(QWidget * p_parent=nullptr);

protected:
    void mousePressEvent(QMouseEvent *event);
};

#endif // EXTENDEDLINEEDIT_H
