#ifndef LOGSYSTEMWID_H
#define LOGSYSTEMWID_H

#include <QWidget>
#include "logcom/logbtnbar.h"
#include "sqltablemodel.h"

#include <QScrollBar>
//#include <QPropertyAnimation>
//#include <QDateTime>

namespace Ui {
class LogSystemWid;
}

class LogSystemWid : public QWidget
{
    Q_OBJECT

public:
    explicit LogSystemWid(QWidget *parent = 0);
    ~LogSystemWid();


protected:
    void initBtnBar();
    QString getTableName();
    bool refreshTable(const QString &table);
//    void initScrollArea();
//    bool eventFilter(QObject *obj, QEvent *event);


public slots:
    void initTableSlot();

protected slots:
    void initFunSLot();
    void doubleSlot(QModelIndex);
    void clearTableSlot();
    void refreshSlot();
    void exportSlot(int);

private:
    Ui::LogSystemWid *ui;
    LogBtnBar *mBtnBar;
    SqlTableModel *model;
    QString m_table;
    QStringList mHeadList;
    int mCount;
//    QScrollBar *m_scrollBarV;
//    QScrollBar *m_scrollBarH;
};

#endif // LOGSYSTEMWID_H
