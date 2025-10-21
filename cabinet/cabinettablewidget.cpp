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
    cabColChangeSlot(0);///
    timer = new QTimer(this);
    timer->start(1500+ rand() % 500);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));
    connect(InterfaceChangeSig::get(), SIGNAL(typeSig(int)), this,SLOT(interfaceChangedSlot(int)));
    isRun = true;
}


void CabinetTableWidget::cabColChangeSlot(int id)
{
    mBusID = id;
    sDataPacket *shm = get_share_mem();
    mBoxData1 = &(shm->data[shm->cabData[id][0].lineA_No-1]);///
    mBoxData2 = &(shm->data[shm->cabData[id][0].lineB_No-1]);
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
    if(0 == gLanguage)
    header <<tr("机柜名称")<<tr("相") << tr("主路电压\n(V)") << tr("备路电压\n(V)") << tr("主路电流\n(A)") << tr("备路电流\n(A)")<< tr("合计功率\n(kW)")<<
        tr("电能\n(kWh)");
    else
    header <<tr("Cabinet")<<tr("Phase") << tr("Mains voltage\n(V)") << tr("Backup voltage\n(V)") << tr("Mains current\n(A)") << tr("Backup current\n(A)")<< tr("Total power\n(kW)")<<
        tr("Electricity\n(kWh)");
    ui->tableWidget->setColumnCount(header.size());    //设置列数
    ui->tableWidget->setHorizontalHeaderLabels(header);

    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive); // 允许手动设置宽度
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setAlternatingRowColors(true); // 隔行变色
    ui->tableWidget->setPalette(QPalette(Qt::lightGray)); // 设置隔行变色的颜色  gray灰色
//    ui->tableWidget->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
//    ui->tableWidget->verticalHeader()->setDefaultSectionSize(45);
//    ui->tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{width:35px;}");

    //connect(ui->tableWidget,SIGNAL(itemClicked(QTableWidgetItem*)),this,SLOT(getItem(QTableWidgetItem*)));
    // ui->tableWidget->setColumnHidden(0, true); // 隐藏母线名称
    // ui->tableWidget->setColumnWidth(0,200);
//    for(int i = 0 ; i < header.size() ; i++){
//        if(i == 1) ui->tableWidget->setColumnWidth(i,20); //设置宽度
//        else ui->tableWidget->setColumnWidth(i,40); //设置宽度
//    }
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

    int size = getRow(); // 获取插接箱的数量///*3
    for(int i=0; i<size; ++i)
        initTable();
}

int CabinetTableWidget::getRow()
{
    int size = 0;
    int cabnum = get_share_mem()->cabNum[mBusID];
    for(int i = 0; i < cabnum ; i++){
        uchar addr = get_share_mem()->cabData[mBusID][i].lineA_Tapoff_No-1;
        size += mBoxData1->box[addr].phaseFlag == 0 ? 1 : 3;
    }
    return size;
}

/**
 * @brief 表格检查 当输出位与表格行数不相等时，需要重新建立表
 * @return true 表示需要重新建立表格
 */
bool CabinetTableWidget::checkTable()
{
    bool ret = false;

    int size = getRow(); // 获取插接箱的数量///*3
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
 * @brief 设置Item的颜色
 * @param id 行号
 * @param column 列号
 * @param alarm 颜色号
 */
void CabinetTableWidget::setItemColor(int id, int column, int alarm)
{
    QTableWidgetItem *item = ui->tableWidget->item(id-1, column);

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
        item->setTextColor(QColor(Qt::black));
        break;
    }
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

void CabinetTableWidget::setName(int row, int column)
{
    QString name = get_share_mem()->cabData[mBusID][row-1].cabName;
    if(name.isEmpty()) name = tr("Cabinet%1").arg(row);
    setTableItem(row, column, name);
}

void CabinetTableWidget::setPhase(int row, int column , int line1 , int line2 , int flag)
{
    QString name = "";
    if(0 == flag){
        name = QString((char)('A' + line1))+"\\"+QString((char)('A' + line2));
    }else{
        name = QString((char)('A' + line1%3))+QString::number(line1/3+1);
    }
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
    uchar offline1 = mBoxData1->box[id1].offLine;
    uchar offline2 = mBoxData2->box[id2].offLine;
    //if(mBoxData->box[id].offLine)
    {
        sObjData *unit1 = &(mBoxData1->box[id1].data);
        sObjData *unit2 = &(mBoxData2->box[id2].data);
        double value = (offline1?unit1->cur.value[line1]:0+offline2?unit2->cur.value[line2]:0) / COM_RATE_CUR;
        if(value >= 0)
            str = QString::number(value, 'f', 3);
    }
    setTableItem(row, column, str);
}

//id :第几条母线 line ：第几个工业接头
void CabinetTableWidget::setSumPow(int id1, int line1, int id2, int line2, int row,  int column)
{
    QString str = "---";
    uchar offline1 = mBoxData1->box[id1].offLine;
    uchar offline2 = mBoxData2->box[id2].offLine;
    //if(mBoxData->box[id].offLine)
    {
        sObjData *unit1 = &(mBoxData1->box[id1].data);
        sObjData *unit2 = &(mBoxData2->box[id2].data);
        double value = ((offline1?unit1->pow.value[line1]:0)+(offline2?unit2->pow.value[line2]:0)) / COM_RATE_POW;
        if(value >= 0)
            str = QString::number(value, 'f', 3);
    }
    setTableItem(row, column, str);
}

//id :第几条母线 line ：第几个工业接头
void CabinetTableWidget::setSumLoad(int id1, int line1, int id2, int line2, int row,  int column , int load)
{
    QString str = "---";
    uchar offline1 = mBoxData1->box[id1].offLine;
    uchar offline2 = mBoxData2->box[id2].offLine;
    //if(mBoxData->box[id].offLine)
    {
        sObjData *unit1 = &(mBoxData1->box[id1].data);
        sObjData *unit2 = &(mBoxData2->box[id2].data);
        if(load){
            double value = ((offline1?unit1->pow.value[line1]:0+offline2?unit2->pow.value[line2]:0)*100.0) / load;
            if(value >= 0) str = QString::number(value, 'f', 2);
        }
    }
    setTableItem(row, column, str);
}

//id :第几个插接箱 line ：第几个工业接头
void CabinetTableWidget::setCur(sObjData *unit ,int line, int row, int column, uchar offline)
{
    QString str = "---";
    //if(mBoxData->box[id].offLine)
    {
        double value = (offline?unit->cur.value[line]:0 )/ COM_RATE_CUR;
        if(value >= 0)
            str = QString::number(value, 'f', 3);
    }
    setTableItem(row, column, str);
    if(offline) setItemColor(row , column , unit->cur.alarm[line]);
    else setItemColor(row , column , 0);
}

//id :第几条母线 line ：第几个工业接头
void CabinetTableWidget::setVol(sObjData *unit , int line, int row, int column , uchar offline)
{
    QString str = "---";
    //if(mBoxData->box[id].offLine)
    {
        double value = (offline?unit->vol.value[line]:0)/ COM_RATE_VOL;
        if(value >= 0)
            str = QString::number(value, 'f', 1);
    }
    setTableItem(row, column, str);
    if(offline) setItemColor(row , column , unit->vol.alarm[line]);
    else setItemColor(row , column , 0);
}

//id :第几条母线 line ：第几个工业接头
void CabinetTableWidget::setSumEle(int id1, int line1, int id2, int line2, int row, int column)
{
    QString str = "---";
    uchar offline1 = mBoxData1->box[id1].offLine;
    uchar offline2 = mBoxData2->box[id2].offLine;

    //if(mBoxData->box[id].offLine)
    {
        sObjData *unit1 = &(mBoxData1->box[id1].data);
        sObjData *unit2 = &(mBoxData2->box[id2].data);
        double value = ((offline1?unit1->ele[line1]:0) + (offline2?unit2->ele[line2]:0)) / COM_RATE_ELE;
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
    if(ret) initTableWid(); // 重新建立表格
    // id1 , id2 , line1 , line2??
    int row = ui->tableWidget->rowCount();
    sDataPacket *shm = get_share_mem();
    int cabnum = shm->cabNum[mBusID];

    for(int i = 1 , j = 1; i <= cabnum && j <= row; i++ ){
        int id1 = shm->cabData[mBusID][i-1].lineA_Tapoff_No - 1;
        int id2 = shm->cabData[mBusID][i-1].lineB_Tapoff_No - 1;
        int line1 = shm->cabData[mBusID][i-1].lineA_Tapoff_Line - 1;
        int line2 = shm->cabData[mBusID][i-1].lineB_Tapoff_Line - 1;
        if(mBoxData1->box[id1].phaseFlag == 0){
            int k=0;
            setName(j, k++); // 设置名称
            setPhase(j, k++ , line1 , line2 , mBoxData1->box[id1].phaseFlag); // 设置相

            setVol(&(mBoxData1->box[id1].data) , line1, j, k++ , mBoxData1->box[id1].offLine);
            setVol(&(mBoxData2->box[id2].data) , line2, j, k++ , mBoxData2->box[id2].offLine);
            setCur(&(mBoxData1->box[id1].data) , line1, j, k++ , mBoxData1->box[id1].offLine); // 设置A路电流值
            setCur(&(mBoxData2->box[id2].data) , line2, j, k++ , mBoxData2->box[id2].offLine); // 设置B路电流值
            //setSumCur(id1, line1, id2, line2, i, k++);
            setSumPow(id1, line1, id2, line2, j, k++);
            setSumEle(id1, line1, id2, line2, j, k++);
            j++;
        }else{

            int start1 = (line1 == 0)?0:((line1 == 1)?3:6);
            int end1 = (line1 == 0)?3:((line1 == 1)?6:9);
            int start2 = (line2 == 0)?0:((line2 == 1)?3:6);
            int end2 = (line2 == 0)?3:((line2 == 1)?6:9);

            for(int m = start1 , n = start2 ; m < end1 && n < end2 && j <= row ; m++ , n++ , j++){
                int k=0;
                setName(j, k++); // 设置名称
                setPhase(j, k++ , m , n , mBoxData1->box[id1].phaseFlag); // 设置相
                //setAlarmStatus(i, k++); //设置告警状态

                setVol(&(mBoxData1->box[id1].data) , m, j, k++ , mBoxData1->box[id1].offLine);
                setVol(&(mBoxData2->box[id2].data) , n, j, k++ , mBoxData2->box[id2].offLine);
                setCur(&(mBoxData1->box[id1].data) , m, j, k++ , mBoxData1->box[id1].offLine); // 设置A路电流值
                setCur(&(mBoxData2->box[id2].data) , n, j, k++ , mBoxData2->box[id2].offLine); // 设置B路电流值
                //setSumCur(id1, line1, id2, line2, i, k++);
                setSumPow(id1, m, id2, n, j, k++);
                setSumEle(id1, m, id2, n, j, k++);
            }
        }

    }
    for(int i = 1 , j = 1; i <= cabnum && j <= row; i++ ){
        int id1 = shm->cabData[mBusID][i-1].lineA_Tapoff_No - 1;
        if(mBoxData1->box[id1].phaseFlag == 1){
            setTableNameItem(i , j , 0 , mBoxData1->box[id1].phaseFlag);
            j+=3;
        }else{
            setTableNameItem(i , j , 0 , mBoxData1->box[id1].phaseFlag);
            j++;
        }
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


void CabinetTableWidget::setTableNameItem(int id ,int row, int column , int flag)
{
    QString name = get_share_mem()->cabData[mBusID][id-1].cabName;
    if(name.isEmpty()) name = tr("Cabinet%1").arg(id);
    if(flag == 1)ui->tableWidget->setSpan(row-1 , column , 3 , 1);
    QTableWidgetItem *item = ui->tableWidget->item(row-1, column);
    item->setText(name);
}
