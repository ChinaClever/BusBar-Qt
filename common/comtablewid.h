#ifndef COMTABLEWID_H
#define COMTABLEWID_H

#include <QWidget>
#include <QTableWidget>
#include "common.h"

//#include <QScrollBar>
//#include <QPropertyAnimation>
//#include <QDateTime>

namespace Ui {
class ComTableWid;
}

class ComTableWid : public QWidget
{
    Q_OBJECT

public:
    explicit ComTableWid(QWidget *parent = 0);
    ~ComTableWid();

protected:
    void initTableWid(QStringList &header, int line, const QString &title);
    void setTableItem(int id, int column, const QString &str);
    void setTableRow(int id, QStringList &listStr);
    void appendTableRow(QStringList &listStr, bool c=false);
    void checkTableRow(int line);

    void clearRow(int row);
    void clearTable();
    void delTable();
    void setBackgroundColor(int id);
    void setItemColor(int id, int column, int alarm);
//    void initScrollArea();
//    bool eventFilter(QObject *obj, QEvent *event);

protected slots:
    virtual void timeoutDone(){}
    virtual void itemDoubleClicked(QTableWidgetItem* ){}
    virtual void itemClicked(QTableWidgetItem* ){}

protected:
    QTimer *timer;

private:
    void initTableWidget(QStringList &header, const QString &title);
    void addItemContent(int row, int column, const QString &content);
    void addRowContent(QStringList &list);
    void addInitRow();
    void addTableRows(int line);
    void delTableRows(int line);

private:
    Ui::ComTableWid *ui;
//    QScrollBar *m_scrollBarV;
//    QScrollBar *m_scrollBarH;
};

#endif // COMTABLEWID_H
