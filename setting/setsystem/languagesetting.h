#ifndef LANGUAGESETTING_H
#define LANGUAGESETTING_H

#include <QDialog>
#include "common.h"
#include "dpalarmslave.h"
namespace Ui {
class Languagesetting;
}

class Languagesetting : public QDialog
{
    Q_OBJECT

public:
    explicit Languagesetting(QWidget *parent = nullptr);
    ~Languagesetting();
    void init();
private slots:
    void on_quitBtn_clicked();
    void on_confirmBtn_clicked();
signals:
    void finshSig();
private:
    Ui::Languagesetting *ui;
    QTimer *timer;
    bool isRun;
};

#endif // LANGUAGESETTING_H
