﻿/*
 * tableWidget公共蕨类
 *     提供接口，protect所有方法
 *
 *  Created on: 2018年1月1日
 *      Author: Lzy
 */
#include "comtablewid.h"
#include "ui_comtablewid.h"
#include <QGridLayout>

ComTableWid::ComTableWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ComTableWid)
{
    ui->setupUi(this);
//    groupBox_background_icon(this);

    timer = new QTimer(this);
    timer->start(1*1000+rand()%500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
//    initScrollArea(); // 开启滑动功能
}

ComTableWid::~ComTableWid()
{
    delete ui;
}


//void ComTableWid::initScrollArea()
//{
//    m_scrollBarV = ui->tableWidget->verticalScrollBar();
//    m_scrollBarH = ui->tableWidget->horizontalScrollBar();
//    QObjectList objectList = ui->tableWidget->children();
//    for(int i = 0; i < objectList.count(); i++) {
//        if(objectList.at(i)->objectName() == "qt_scrollarea_viewport") {
//            //objectList.at(i)->installEventFilter(this);
//        }
//    }
//}

//bool ComTableWid::eventFilter(QObject *obj, QEvent *event)
//{
//    static int press_y   = 0;
//    static int move_y    = -1;
//    static int release_y = 0;
//    static QDateTime pressDateTime;
//    static QPropertyAnimation *animation = new QPropertyAnimation();
//    if("qt_scrollarea_viewport" != obj->objectName()) return false;

//    int scrollV_max = m_scrollBarV->maximum();
//    int scrollV_min = m_scrollBarV->minimum();

//    //根据鼠标的动作——按下、放开、拖动，执行相应的操作
//    if(event->type() == QEvent::MouseButtonPress) {  //记录按下的时间、坐标
//        pressDateTime = QDateTime::currentDateTime();
//        move_y  = QCursor::pos().y();
//        press_y = move_y;
//        animation->stop();
//    } else if(event->type() == QEvent::MouseButtonRelease) {
//        //鼠标放开，根据鼠标拖动的垂直距离和持续时间，设置窗口滚动快慢程度和距离
//        if(animation->targetObject() != m_scrollBarV) {
//            animation->setTargetObject(m_scrollBarV);
//            animation->setPropertyName("value");
//        }

//        move_y = -1;
//        release_y = QCursor::pos().y();
//        QObject *parent_obj = obj->parent();
//        if(parent_obj != 0 || parent_obj->inherits("QAbstractItemView")) {
//            QTimer::singleShot(150, (QAbstractItemView *)parent_obj, SLOT(clearSelection()));
//        }

//        int endValue = 0;
//        int pageStep;
//        if(release_y - press_y != 0 && qAbs(release_y - press_y) > 45) {
//            int mseconds = pressDateTime.msecsTo(QDateTime::currentDateTime());
//            int limit = 440;
//            pageStep = 240;//scrollBarV->pageStep();
//            if(mseconds > limit) { //滑动的时间大于某个值的时候，不再滚动(通过增加分母)
//                mseconds = mseconds + (mseconds - limit) * 20;
//            }

//            if(release_y - press_y > 0) {//.0避免避免强制转换为整形
//                endValue = m_scrollBarV->value() - pageStep * (200.0 / mseconds);
//                if(scrollV_min > endValue) endValue = scrollV_min;
//            } else if(release_y - press_y < 0) {
//                endValue = m_scrollBarV->value() + pageStep * (200.0 / mseconds);
//                if(endValue > scrollV_max) endValue = scrollV_max;
//            }
//            if(mseconds > limit) mseconds = 0;//滑动的时间大于某个值的时候，滚动距离变小，减小滑动的时间
//            animation->setDuration(mseconds+550);
//            animation->setEndValue(endValue);

//            if(m_scrollBarH != NULL)
//                m_scrollBarH->setValue(0);
//            animation->setEasingCurve(QEasingCurve::OutQuad);
//            animation->start();
//            return true;
//        }
//        if(m_scrollBarH != NULL)
//            m_scrollBarH->setValue(0);
//    } else if(event->type() == QEvent::MouseMove && move_y >= 0) {   //窗口跟着鼠标移动
//        int move_distance = QCursor::pos().y() - move_y;
//        int endValue = m_scrollBarV->value() - move_distance;
//        if(scrollV_min > endValue) endValue = scrollV_min;
//        if(endValue > scrollV_max)  endValue = scrollV_max;
//        m_scrollBarV->setValue(endValue);
//        move_y = QCursor::pos().y();
//    }

//    return false;
//}

/**
 * @brief 初始化窗口
 */
void ComTableWid::initTableWidget(QStringList &header, const QString &title)
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);        //设置行数/

    ui->tableWidget->setColumnCount(header.size());    //设置列数
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    //ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

//    if(!title.contains("谐波含量")){
//        ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
//        ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);
//        ui->tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{width:35px;}");
//        ui->tableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{height:35px;}");
//    }

    connect(ui->tableWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(itemDoubleClicked(QTableWidgetItem*)));
    connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(itemClicked(QTableWidgetItem*)));
    // ui->tableWidget->setColumnHidden(0, true);
    // ui->tableWidget->setColumnWidth(0,200);
}


/**
 * @brief 增加Item
 * @param row 行
 * @param column 列
 * @param content 信息
 */
void ComTableWid::addItemContent(int row, int column, const QString &content)
{
    QTableWidgetItem *item = new QTableWidgetItem(content);
    item->setTextAlignment(Qt::AlignCenter);

    ui->tableWidget->setItem(row, column, item);
}

/**
 * @brief 增加一行内容
 * @param list
 */
void ComTableWid::addRowContent(QStringList &list)
{
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    // ui->tableWidget->setRowHeight(row, 40);

    for(int i=0; i<list.size(); ++i)
        addItemContent(row,i,list[i]);

    list.clear();
}


/**
 * @brief 初始化一行表格
 * @param str
 */
void ComTableWid::addInitRow()
{
    QStringList list;
    int column = ui->tableWidget->columnCount();
    for(int j=0; j<column; ++j)
        list << "---";
    addRowContent(list);
}

/**
 * @brief 初始化表格
 * @param header 列表头
 * @param line 表格行数
 * @param title 表格名称
 */
void ComTableWid::initTableWid(QStringList &header, int line, const QString &title)
{
    initTableWidget(header,title);
    for(int i=0; i<line; ++i)
        addInitRow();
    ui->groupBox->setTitle(title);
}

/**
 * @brief 修改表格Item
 * @param id  行号
 * @param column 列号
 * @param str 内容
 */
void ComTableWid::setTableItem(int id, int column, const QString &str)
{
    addTableRows(id+1);
    QTableWidgetItem *item = ui->tableWidget->item(id, column);
    if(str.isEmpty()) {
        item->setText("---");
    } else {
        item->setText(str);
    }
}

/**
 * @brief 设置一行数据
 * @param id 行号
 * @param listStr 字符链表
 */
void ComTableWid::setTableRow(int id, QStringList &listStr)
{
    for(int i=0; i<listStr.size(); ++i)
        setTableItem(id, i, listStr.at(i));
}

void ComTableWid::appendTableRow(QStringList &listStr, bool c)
{
    int row = ui->tableWidget->rowCount();
    setTableRow(row, listStr);
    if(c) setBackgroundColor(row);
}


void ComTableWid::addTableRows(int line)
{
    int row = ui->tableWidget->rowCount();
    if(row < line)
    {
        for(int i=0; i<line-row; ++i)
            addInitRow();
    }
}

void ComTableWid::delTableRows(int line)
{
    int row = ui->tableWidget->rowCount();
    if(row > line)
    {
        for(int i=0; i<row-line; ++i)
            ui->tableWidget->removeRow(line);
    }
}

/**
 * @brief 表格行数重新调整
 * @param line 目标行数
 */
void ComTableWid::checkTableRow(int line)
{
    addTableRows(line);
    delTableRows(line);
}

/**
 * @brief 清除一行内容
 * @param row 行号
 */
void ComTableWid::clearRow(int row)
{
    int column = ui->tableWidget->columnCount();
    for(int i=0; i<column; ++i) {
        setTableItem(row, i, "---");
    }
}

/**
 * @brief 清除表格所有内容
 */
void ComTableWid::clearTable()
{
    int row = ui->tableWidget->rowCount();
    for(int i=0; i<row; ++i)
        clearRow(i);
}

void ComTableWid::delTable()
{
    ui->tableWidget->setRowCount(0);        //设置行数/
}

void ComTableWid::setBackgroundColor(int id)
{
    int column = ui->tableWidget->columnCount();
    for(int i=0; i<column; ++i) {
        QTableWidgetItem *item = ui->tableWidget->item(id, i);
        item->setBackgroundColor(QColor(Qt::red));
    }
}

/**
 * @brief 设置Item的颜色
 * @param id 行号
 * @param column 列号
 * @param alarm 颜色号
 */
void ComTableWid::setItemColor(int id, int column, int alarm)
{
    addTableRows(id+1);
    QTableWidgetItem *item = ui->tableWidget->item(id, column);

    switch (alarm) {
    case 0:
        item->setTextColor(QColor(Qt::black));
        break;
    case 1:
//        item->setTextColor(QColor(232,157,18));
//        break;
    case 2:
        item->setTextColor(QColor(Qt::red));
        break;
    default:
        break;
    }
}
