#ifndef SERIALDATA_H
#define SERIALDATA_H

#include <QWidget>
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
    Ui::Serialdata *ui;
};

#endif // SERIALDATA_H
