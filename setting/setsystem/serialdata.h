#ifndef SERIALDATA_H
#define SERIALDATA_H

#include <QWidget>
#include "common/msgbox.h"
namespace Ui {
class Serialdata;
}

class Serialdata : public QWidget
{
    Q_OBJECT

public:
    explicit Serialdata(QWidget *parent = nullptr);
    ~Serialdata();
    void initLanguage();
private slots:
    void on_saveBtn_clicked();

private:
    QString transformerBaud();
    QString transformerParity();
    QString transformerDatabits();
    QString transformerStopbits();
    void initComParameter();
    int baudToIndex(int baud);
    int parityToIndex(int parity);
    int databitsToIndex(int databits);
    int stopbitsToIndex(int stopbits);
private:
    Ui::Serialdata *ui;
};

#endif // SERIALDATA_H
