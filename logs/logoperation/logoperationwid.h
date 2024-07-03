#ifndef LOGOPERATIONWID_H
#define LOGOPERATIONWID_H

#include <QWidget>
#include "logcom/logbtnbar.h"
#include "sqltablemodel.h"

#include <QScrollBar>
//#include <QPropertyAnimation>
//#include <QDateTime>

namespace Ui {
class LogOperationWid;
}

class LogOperationWid : public QWidget
{
    Q_OBJECT

public:
    explicit LogOperationWid(QWidget *parent = 0);
    ~LogOperationWid();


protected:
    void initBtnBar();
    QString getTableName(int id);
    bool refreshTable(const QString &table);
//    void initScrollArea();
//    bool eventFilter(QObject *obj, QEvent *event);


public slots:
    void initTableSlot(int id);

protected slots:
    void initFunSLot();
    void doubleSlot(QModelIndex);
    void clearTableSlot();
    void refreshSlot();
    void exportSlot(int);

private:
    Ui::LogOperationWid *ui;
    int mid;
    LogBtnBar *mBtnBar;
    SqlTableModel *model;
    QString m_table;
    QStringList mHeadList;
    int mCount;
//    QScrollBar *m_scrollBarV;
//    QScrollBar *m_scrollBarH;
};

#endif // LOGOPERATIONWID_H
