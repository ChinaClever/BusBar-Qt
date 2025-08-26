#ifndef CABINETTABLEWIDGET_H
#define CABINETTABLEWIDGET_H

#include <QWidget>
#include "common/common.h"
#include <QTableWidgetItem>

//#include <QScrollBar>
//#include <QPropertyAnimation>
//#include <QDateTime>

namespace Ui {
class CabinetTableWidget;
}

class CabinetTableWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CabinetTableWidget(QWidget *parent = 0);
    ~CabinetTableWidget();

protected:
    void initTableWidget();
    void addItemContent(int row, int column, const QString &content);
    void addRowContent(QStringList &list);

    void initTable();
    void initTableWid();
    void clearTable();
    bool checkTable();

    void setTableItem(int id, int column, const QString &str);
    void setName(int id, int row, int column);
    void setAlarmStatus(int id, int column);
    void setCur(sLoopTgObjData *unit, int line, int row,  int column);
    void setVol(sLoopTgObjData *unit, int line, int row,  int column);
    void setSumEle(int id1, int line1, int id2, int line2,int row, int column);
    void setSumCur(int id1, int line1, int id2, int line2,int row, int column);
    void setSumPow(int id1, int line1, int id2, int line2, int row, int column);
    void setSumLoad(int id1, int line1, int id2, int line2, int row,  int column);
    void updateData();
//    void initScrollArea();
//    bool eventFilter(QObject *obj, QEvent *event);

public slots:
    void busChangeSlot(int id);
    void timeoutDone();
    void initFunSLot();
    void interfaceChangedSlot(int id);

protected slots:
    void getItem(QTableWidgetItem*);

private:
    Ui::CabinetTableWidget *ui;

    int mBusID;
    sBusData *mBoxData1;
    sBusData *mBoxData2;
    QTimer *timer;
    bool isRun;
//    QScrollBar *m_scrollBarV;
//    QScrollBar *m_scrollBarH;
};

#endif // CABINETTABLEWIDGET_H
