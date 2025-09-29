#include "setcabnameswid.h"
#include "ui_setcabnameswid.h"
#include "interfacechangesig.h"
extern void set_box_num(int id, int num);

SetCabNamesWid::SetCabNamesWid(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SetCabNamesWid)
{
    ui->setupUi(this);
    mIndex = 0;
    mSetShm = new SetShm;
    mSetCabNameDlg = new SetCabNameDlg(this);
    QTimer::singleShot(365,this,SLOT(initFunSLot()));
//    initScrollArea(); // 开启滑动功能
    connect(InterfaceChangeSig::get(), SIGNAL(typeSig(int)), this,SLOT(interfaceChangedSlot(int)));
    isRun = false;
}

void SetCabNamesWid::interfaceChangedSlot(int id)
{
    if(id == 6) {
        isRun = true;
    } else {
        isRun = false;
    }
}

void SetCabNamesWid::initFunSLot()
{
    indexChanged(mIndex);
//    ui->tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{width:30px;}");
//    ui->tableWidget->horizontalScrollBar()->setStyleSheet("QScrollBar{height:30px;}");
    mTimer = new QTimer(this);
    mTimer->start(3*1000);
    connect(mTimer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
}


SetCabNamesWid::~SetCabNamesWid()
{
    delete ui;
}


void SetCabNamesWid::initTableWidget()
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    if(gLanguage == 0){ui->label->setText("机柜列名称");
        ui->label_3->setText("机柜数量");ui->saveBtn->setText("保存");
    }else{ui->label->setText("Busbar name");
        ui->label_3->setText("Number of cabinet column");ui->saveBtn->setText("Save");
    }

    QStringList horHead;
    if(gLanguage == 0) horHead<< tr("机柜名称")<< tr("电力容量\n(kW)")<< tr("A路母线编号")<< tr("A路插接箱地址")<< tr("A路插接箱输出位")
            << tr("B路母线编号")<< tr("B路插接箱地址")<< tr("B路插接箱输出位");
    else horHead<< tr("cabinet name")<< tr("power capacity\n(kW)")<< tr("busbar No.\nof line A")<< tr("tap-off box No.\nof line A")<< tr("tap-off box\noutput position\nof line A")
                << tr("busbar No.\nof line B")<< tr("tap-off box No.\nof line B")<< tr("tap-off box\noutput position\nof line B");


    ui->tableWidget->setColumnCount(horHead.size());

    ui->tableWidget->setHorizontalHeaderLabels(horHead);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);  //设置不可编辑
    for(int i = 0 ; i < horHead.size() ; i++) ui->tableWidget->setColumnWidth(i,40); //设置宽度

//    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
//    ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);

    //    ui->tableWidget->setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
    //connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(itemDoubleClicked(QTableWidgetItem*)));
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &SetCabNamesWid::onCellDoubleClicked);
}

void SetCabNamesWid::onCellDoubleClicked(int row) {
    QStringList rowData;
    for (int col = 0; col < ui->tableWidget->columnCount(); ++col) {
        QTableWidgetItem *item = ui->tableWidget->item(row, col);
        rowData << (item ? item->text() : "");
    }

//    QString str = QString("第 %1 行数据：\n机柜名称 %2\n 电力容量%3\nA路母线编号%4\nA路插接箱地址%5\nA路插接箱输出位%6\nB路母线编号%7\nB路插接箱地址%8\nB路插接箱输出位%9\n")
//                                 .arg(row + 1)
//                                 .arg(rowData.value(0))
//                                 .arg(rowData.value(1))
//                                 .arg(rowData.value(2))
//                             .arg(rowData.value(3))
//                             .arg(rowData.value(4))
//                             .arg(rowData.value(5))
//                             .arg(rowData.value(6))
//                             .arg(rowData.value(7))
//                             .arg(rowData.value(8));
    mSetCabNameDlg->init(mIndex, row, mSetShm);
    mSetCabNameDlg->setWindowModality(Qt::WindowModal);
    mSetCabNameDlg->show();
    mSetCabNameDlg->move(0,0);
}

//void SetCabNamesWid::clearWidget()
//{
//    int row = ui->tableWidget->rowCount();
//    for(int i = 0 ; i < row ; i++)
//        ui->tableWidget->removeRow(0);
//}

void SetCabNamesWid::resetWidget()
{
    initTableWidget();
    int boxNum = get_share_mem()->cabNum[mIndex];

    for(int i = 0 ;  i < boxNum ; i++)
    {
        ui->tableWidget->insertRow(i);
        int len = LINE_NUM-1; //交流9个

        for(int j=0; j<=len; j++){
            QTableWidgetItem * item = new QTableWidgetItem("---");
            item->setTextAlignment(Qt::AlignCenter);
            ui->tableWidget->setItem(i, j ,item);
        }
    }
}


//void SetCabNamesWid::checkBus()
//{
//    int row = ui->tableWidget->rowCount();
//    int col = ui->tableWidget->columnCount();

//    int dc = mPacket ? mPacket->box[0].dc : 1;
//    int len = dc ? LINE_NUM : 4;
//    if(mPacket->boxNum != row || col != len+1) { //修改判断条件——  2018.3.21——By>MW
//        clearWidget();
//        resetWidget();
//    }
//}

/**
 * @brief 刷新界面
 * @param index 主路源编号
 */
void SetCabNamesWid::indexChanged(int index)
{
    //    if(mIndex == index)  return;
    mIndex = index;
    initWid(index);
}

void SetCabNamesWid::updateWid()
{
//    checkBus();
    resetWidget();
    int row = ui->tableWidget->rowCount();
    for(int i = 0 ; i < row ; i++)
    {
        setName(i,0);
        for(int j=1; j<ui->tableWidget->columnCount(); ++j) {
            setTableItem(i, j);
        }
    }
}

void SetCabNamesWid::timeoutDone()
{
    if(isRun){
        updateWid();
    }
}


void SetCabNamesWid::setName(int row, int column)
{
    QTableWidgetItem *item = ui->tableWidget->item(row,column);
    //QString str = mPacket->box[row+1].boxName;  //第0个为始端箱，所以从第一个开始
    QString str = get_share_mem()->cabData[mIndex][row].cabName;
    item->setText(str);
}


void SetCabNamesWid::setTableItem(int row, int column)
{
    QString str = "---";
    QTableWidgetItem *item = ui->tableWidget->item(row,column);

    //box->rate 直流的情况下，box->rate代表路数
    //if(box->offLine > 0 /* && column <= box->rate */) {
        //if(column <= box->data.lineNum) {
    switch (column) {
    case 1:
        str = QString::number(get_share_mem()->cabData[mIndex][row].capacity/COM_RATE_POW,'f', 3);
        break;
    case 2:
        str = QString::number(get_share_mem()->cabData[mIndex][row].lineA_No);
        break;
    case 3:
        str = QString::number(get_share_mem()->cabData[mIndex][row].lineA_Tapoff_No);
        break;
    case 4:
        str = QString::number(get_share_mem()->cabData[mIndex][row].lineA_Tapoff_Line);
        break;
    case 5:
        str = QString::number(get_share_mem()->cabData[mIndex][row].lineB_No);
        break;
    case 6:
        str = QString::number(get_share_mem()->cabData[mIndex][row].lineB_Tapoff_No);
        break;
    case 7:
        str = QString::number(get_share_mem()->cabData[mIndex][row].lineB_Tapoff_Line);
        break;
    }

        //}
    //}
    if(!str.isEmpty())
        item->setText(str);
}


//void SetCabNamesWid::itemDoubleClicked(QTableWidgetItem *item)
//{
//    if(item->text().compare("---") == 0) return;  //为空不设置
//    disconnect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(itemDoubleClicked(QTableWidgetItem*)));
//    int boxNum = item->row() + 1;
//    int column = item->column();

//    mSetCabNameDlg->init(mIndex, boxNum, column, item->text());
//    mSetCabNameDlg->setWindowModality(Qt::WindowModal);
//    mSetCabNameDlg->show();
//    mSetCabNameDlg->move(0,0);

//    connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(itemDoubleClicked(QTableWidgetItem*)));
//}

void SetCabNamesWid::initWid(int index)
{
    ui->cabnameEdit->setText(QString(get_share_mem()->cabColName[index]));
    ui->cabNumSpin->setValue(get_share_mem()->cabNum[index]);

//    checkBus();//切换通道，更新表格名称  2018-12-20 pmd
    int row = ui->tableWidget->rowCount();
    for(int i = 0 ; i < row ; i++)
    {
        setName(i,0);
        for(int j=1; j<ui->tableWidget->columnCount(); ++j) {
            setTableItem(i, j);
        }
    }
}

bool SetCabNamesWid::saveCabColName()
{
    bool ret = true;
    QString name = ui->cabnameEdit->text();
    if( (!name.isEmpty()) ) {
        if(!(name.size() > NAME_LEN - 2)){
             mSetShm->setCabinetColName(mIndex, name);
//            emit updateBusNameSig(mIndex , name);
        }else{
            if(gLanguage == 0) CriticalMsgBox box(NULL, tr("机柜名称不能超过30个字符保存失败!!"));
            else CriticalMsgBox box(NULL, tr("Cabinet name cannot exceed 30 characters, saving failed!!"));
            ret = false;
        }
    }else {
        if(gLanguage == 0) CriticalMsgBox box(NULL, tr("机柜名称不能为空保存失败!!"));
        else CriticalMsgBox box(NULL, tr("Cabinet name cannot be empty, saving failed!!"));
        ret = false;
    }
    if(ui->cabNumSpin->value() < 0 || ui->cabNumSpin->value() > 18){
        if(gLanguage == 0) CriticalMsgBox box(NULL, tr("个数应在0-18!!"));
        else CriticalMsgBox box(NULL, tr("The number should be between 0-18!!"));
        ret = false;
    }

    return ret;
}


void SetCabNamesWid::on_saveBtn_clicked()
{
    mSetShm->setCabinetNum(mIndex, ui->cabNumSpin->value());
    if(saveCabColName()) {
        //set_box_num(mIndex, ui->cabNumSpin->value());
        updateWid();                               //2018-12-17保存插接箱数量的同时，更新名称设置列表 pmd

        //BeepThread::bulid()->beep();
        if(gLanguage == 0) InfoMsgBox box(NULL, tr("保存成功！"));
        else InfoMsgBox box(NULL, tr("Save successfully！"));
    }
}

