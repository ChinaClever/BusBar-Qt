#ifndef SETCABNAMESWID_H
#define SETCABNAMESWID_H

#include <QWidget>
#include "common/common.h"
#include "setshm.h"
#include <QTableWidgetItem>
#include "setcabnamedlg.h"

#include <QScrollBar>
//#include <QPropertyAnimation>
//#include <QDateTime>


namespace Ui {
class SetCabNamesWid;
}

class SetCabNamesWid : public QWidget
{
    Q_OBJECT

public:
    explicit SetCabNamesWid(QWidget *parent = 0);
    ~SetCabNamesWid();

    void indexChanged(int index);
    void updateWid();

protected:
    void initWid(int index);
    bool saveBusName();
    void initTableWidget();
    void clearWidget();
    void resetWidget();
    void checkBus();
    void setName(int row, int column);
    void setTableItem(int row, int column);
//    void initScrollArea();
//    bool eventFilter(QObject *obj, QEvent *event);

private slots:
    void on_saveBtn_clicked();
    void itemDoubleClicked(QTableWidgetItem *item);
    void initFunSLot();
    void timeoutDone();
    void interfaceChangedSlot(int id);

signals:
    void updateBusNameSig(int index ,QString &name);

private:
    Ui::SetCabNamesWid *ui;
    SetCabNameDlg *mSetCabNameDlg;

    int mIndex;
    SetShm *mSetShm;
    sBusData *mPacket ;
    QTimer* mTimer;
//    QScrollBar *m_scrollBarV;
//    QScrollBar *m_scrollBarH;
    bool isRun;
};

#endif // SETCABNAMESWID_H
