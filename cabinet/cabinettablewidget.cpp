#include "cabinettablewidget.h"
#include "ui_cabinettablewidget.h"
#include "interfacechangesig.h"


CabinetTableWidget::CabinetTableWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CabinetTableWidget)
{
    ui->setupUi(this);
    QTimer::singleShot(1000,this,SLOT(initFunSLot())); //延时初始化
}

CabinetTableWidget::~CabinetTableWidget()
{
    delete ui;
}

void CabinetTableWidget::initFunSLot()
{
    busChangeSlot(0);///
    timer = new QTimer(this);
    timer->start(1500+ rand() % 500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
    connect(InterfaceChangeSig::get(), SIGNAL(typeSig(int)), this,SLOT(interfaceChangedSlot(int)));
    isRun = false;
}


void CabinetTableWidget::busChangeSlot(int id)
{
    mBusID = id;
    sDataPacket *shm = get_share_mem();
    if(id <= 1) id = 0;///
    else id = 2;///
    mBoxData1 = &(shm->data[id]);///
    mBoxData2 = &(shm->data[id+1]);
    updateData();
}


/**
 * @brief 初始化窗口
 */
void CabinetTableWidget::initTableWidget()
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);        //设置行数/

    QStringList header;
    header <<tr("机柜名称") << tr("A路电压\n(V)") << tr("B路电压\n(V)") << tr("A路电流\n(A)") << tr("B路电流\n(A)") << tr("合计电流\n(A)") << tr("合计功率\n(kW)")<<
        tr("电能\n(kWh)") << tr("负载率\n(%)");
    ui->tableWidget->setColumnCount(header.size());    //设置列数
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true); // 隔行变色
    ui->tableWidget->setPalette(QPalette(Qt::lightGray)); // 设置隔行变色的颜色  gray灰色
//    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
//    ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);
//    ui->tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{width:35px;}");

    //connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(getItem(QTableWidgetItem*)));
    // ui->tableWidget->setColumnHidden(0, true); // 隐藏母线名称
    // ui->tableWidget->setColumnWidth(0,200);
}


/**
 * @brief 增加Item
 * @param row 行
 * @param column 列
 * @param content 信息
 */
void CabinetTableWidget::addItemContent(int row, int column, const QString &content)
{
    QTableWidgetItem *item = new QTableWidgetItem(content);
    //if(column > 0)
        item->setTextAlignment(Qt::AlignCenter);
    //else
    //    item->setTextAlignment(Qt::AlignVCenter);

    ui->tableWidget->setItem(row, column, item);
}

void CabinetTableWidget::addRowContent(QStringList &list)
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
void CabinetTableWidget::initTable()
{
    QStringList list;
    int column = ui->tableWidget->columnCount();
    for(int j=0; j<column; ++j)
        list << "---";
    addRowContent(list);
}

/**
 * @brief 初始化表格窗口
 */
void CabinetTableWidget::initTableWid()
{
    initTableWidget();

    int size = mBoxData1->boxNum; // 获取插接箱的数量///*3
    for(int i=0; i<size; ++i)
        initTable();
}

/**
 * @brief 表格检查 当输出位与表格行数不相等时，需要重新建立表
 * @return true 表示需要重新建立表格
 */
bool CabinetTableWidget::checkTable()
{
    bool ret = false;

    int size = mBoxData1->boxNum;///*3
    int row = ui->tableWidget->rowCount();
    if(size != row)
        ret = true;

    return ret;
}


void CabinetTableWidget::setTableItem(int id, int column, const QString &str)
{
    QTableWidgetItem *item = ui->tableWidget->item(id-1, column);
    item->setText(str);
}

/**
 * @brief 清空表格
 */
void CabinetTableWidget::clearTable()
{
    int row = ui->tableWidget->rowCount();
    int column = ui->tableWidget->columnCount();

    for(int i=1; i<=row; ++i)
    {
        for(int j=0; j<column; ++j)
            setTableItem(i, j, "---");
    }
}

void CabinetTableWidget::setName(int id, int row, int column)
{
    QString name = mBoxData1->box[id].boxName;
    if(name.isEmpty())
        name = tr("cabibet-%1").arg(id);
    setTableItem(row, column, name);
}



void CabinetTableWidget::setAlarmStatus(int id, int column)
{
    QString str;
    QTableWidgetItem *item = ui->tableWidget->item(id-1, column);
    if(mBoxData1->box[id].offLine){
        int curAlarm = mBoxData1->box[id].boxCurAlarm;
        int volAlarm = mBoxData1->box[id].boxVolAlarm;
        if((curAlarm == 2)  || (volAlarm == 2)) { // 告警
            str = tr("告警");
            item->setTextColor(QColor(Qt::red));
        } else  if((curAlarm == 1)  || (volAlarm == 1))  { // 预警
            str = tr("预警");
            item->setTextColor(QColor("#CD7E80"));
        } else {
            str = tr("正常");
            item->setTextColor(QColor(Qt::black));
        }
    } else {
        str = tr("离线");
        item->setTextColor(QColor(Qt::red));
    }
    item->setText(str);
}

//id :第几条母线 line ：第几个工业接头
void CabinetTableWidget::setSumCur(int id1, int line1, int id2, int line2, int row, int column)
{
    QString str = "---";
    //if(mBoxData->box[id].offLine)
    {
        sLoopTgObjData *unit1 = &(mBoxData1->box[id1].loopTgBox);
        sLoopTgObjData *unit2 = &(mBoxData2->box[id2].loopTgBox);
        double value = (unit1->cur[line1]+unit2->cur[line2]) / COM_RATE_CUR;
        if(value >= 0)
            str = QString::number(value, 'f', 3);
    }
    setTableItem(row, column, str);
}

//id :第几条母线 line ：第几个工业接头
void CabinetTableWidget::setSumPow(int id1, int line1, int id2, int line2, int row,  int column)
{
    QString str = "---";
    //if(mBoxData->box[id].offLine)
    {
        sLoopTgObjData *unit1 = &(mBoxData1->box[id1].loopTgBox);
        sLoopTgObjData *unit2 = &(mBoxData2->box[id2].loopTgBox);
        double value = (unit1->pow[line1]+unit2->pow[line2]) / COM_RATE_POW;
        if(value >= 0)
            str = QString::number(value, 'f', 3);
    }
    setTableItem(row, column, str);
}

//id :第几条母线 line ：第几个工业接头
void CabinetTableWidget::setSumLoad(int id1, int line1, int id2, int line2, int row,  int column)
{
    QString str = "---";
    //if(mBoxData->box[id].offLine)
    {
        sLoopTgObjData *unit1 = &(mBoxData1->box[id1].loopTgBox);
        sLoopTgObjData *unit2 = &(mBoxData2->box[id2].loopTgBox);
        double value = ((unit1->pow[line1]+unit2->pow[line2])*100) / (5*COM_RATE_POW);
        if(value >= 0)
            str = QString::number(value, 'f', 2);
    }
    setTableItem(row, column, str);
}

//id :第几个插接箱 line ：第几个工业接头
void CabinetTableWidget::setCur(sLoopTgObjData *unit ,int line, int row, int column)
{
    QString str = "---";
    //if(mBoxData->box[id].offLine)
    {
        double value = unit->cur[line] / COM_RATE_CUR;
        if(value >= 0)
            str = QString::number(value, 'f', 3);
    }
    setTableItem(row, column, str);
}

//id :第几条母线 line ：第几个工业接头
void CabinetTableWidget::setVol(sLoopTgObjData *unit ,int line, int row, int column)
{
    QString str = "---";
    //if(mBoxData->box[id].offLine)
    {
        double value = unit->vol[line] / COM_RATE_VOL;
        if(value >= 0)
            str = QString::number(value, 'f', 2);
    }
    setTableItem(row, column, str);
}

//id :第几条母线 line ：第几个工业接头
void CabinetTableWidget::setSumEle(int id1, int line1, int id2, int line2, int row, int column)
{
    QString str = "---";

    //if(mBoxData->box[id].offLine)
    {
        sLoopTgObjData *unit1 = &(mBoxData1->box[id1].loopTgBox);
        sLoopTgObjData *unit2 = &(mBoxData2->box[id2].loopTgBox);
        double value = (unit1->ele[line1] + unit2->ele[line2]) / COM_RATE_ELE;
        if(value >= 0)
            str = QString::number(value, 'f', 1);
    }
    setTableItem(row, column, str);
}


/**
 * @brief 数据更新入口函数
 */
void CabinetTableWidget::updateData()
{
    bool ret = checkTable();
    if(ret)
        initTableWid(); // 重新建立表格
    // id1 , id2 , line1 , line2??
    int row = ui->tableWidget->rowCount();
    for(int i=1; i<=row; ++i) //更新数据
    {
        int k=0;
        setName(i ,i, k++); // 设置输出位名称
        //setAlarmStatus(i, k++); //设置告警状态

        setVol(&(mBoxData1->box[i].loopTgBox) , 0, i, k++);
        setVol(&(mBoxData2->box[i].loopTgBox) , 0, i, k++);
        setCur(&(mBoxData1->box[i].loopTgBox) , 0, i, k++); // 设置A路电流值
        setCur(&(mBoxData2->box[i].loopTgBox) , 0, i, k++); // 设置B路电流值
        setSumCur(i, 0, i, 0, i, k++);
        setSumPow(i, 0, i, 0, i, k++);
        setSumEle(i, 0, i, 0, i, k++);
        setSumLoad(i, 0, i, 0, i, k++);
    }
}


void CabinetTableWidget::interfaceChangedSlot(int id)
{
    if(id == 1) {
        isRun = true;
    } else {
        isRun = false;
    }
}

void CabinetTableWidget::timeoutDone()
{
    if(isRun) {
        updateData();
    }
}

void CabinetTableWidget::getItem(QTableWidgetItem*)
{

}
