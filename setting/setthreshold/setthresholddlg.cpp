#include "setthresholddlg.h"
#include "ui_setthresholddlg.h"
#include "setthread.h"

SetThresholdDlg::SetThresholdDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetThresholdDlg)
{
    ui->setupUi(this);
    com_setBackColour(tr("阈值设置"),this);
    ui->checkBox->hide();
    setWindowModality(Qt::WindowModal);
}

SetThresholdDlg::~SetThresholdDlg()
{
    delete ui;
}

void SetThresholdDlg::initSpinBox(sThresholdItem &item)
{

    int range = 63;
    QString str = "A";
    switch (item.type) {
    case 1:
        {
            str = "V"; range = 400;
            ui->mindoubleSpinBox->setMaximum(range);
            ui->maxdoubleSpinBox->setMaximum(range);
            ui->mindoubleSpinBox->setSingleStep(0.1);
            ui->maxdoubleSpinBox->setSingleStep(0.1);

            ui->mindoubleSpinBox->setDecimals(1);
            ui->maxdoubleSpinBox->setDecimals(1);
            ui->mindoubleSpinBox->setValue(item.min/COM_RATE_VOL);
            ui->maxdoubleSpinBox->setValue(item.max/COM_RATE_VOL);
            ui->mindoubleSpinBox->setSuffix(str);
            ui->maxdoubleSpinBox->setSuffix(str);
        }
        break;
    case 2:
            {
            if(!item.box){
                str = "A"; range = 650;
            }
            ui->mindoubleSpinBox->setMaximum(range);
            ui->maxdoubleSpinBox->setMaximum(range);
            ui->mindoubleSpinBox->setSingleStep(0.001);
            ui->maxdoubleSpinBox->setSingleStep(0.001);

            ui->mindoubleSpinBox->setDecimals(3);
            ui->maxdoubleSpinBox->setDecimals(3);
            ui->mindoubleSpinBox->setValue(item.min/COM_RATE_CUR);
            ui->maxdoubleSpinBox->setValue(item.max/COM_RATE_CUR);
            ui->mindoubleSpinBox->setSuffix(str);
            ui->maxdoubleSpinBox->setSuffix(str);
            }
            break;
    case 3: str = "℃"; range = 99;  break;
    case 4:
        {
            str = "kW"; if(item.box)range = 130; else range = 1500;
            ui->mindoubleSpinBox->setMaximum(range);
            ui->maxdoubleSpinBox->setMaximum(range);
            ui->mindoubleSpinBox->setSingleStep(0.01);
            ui->maxdoubleSpinBox->setSingleStep(0.01);
            ui->mindoubleSpinBox->setDecimals(2);
            ui->maxdoubleSpinBox->setDecimals(2);
            ui->mindoubleSpinBox->setValue(item.min/COM_RATE_POW);
            ui->maxdoubleSpinBox->setValue(item.max/COM_RATE_POW);
            ui->mindoubleSpinBox->setSuffix(str);
            ui->maxdoubleSpinBox->setSuffix(str);
        }
        break;
    case 5:
        {
            str = "Hz"; range = 70;
            ui->mindoubleSpinBox->setMaximum(range);
            ui->maxdoubleSpinBox->setMaximum(range);
            ui->mindoubleSpinBox->setSingleStep(0.1);
            ui->maxdoubleSpinBox->setSingleStep(0.1);
            ui->mindoubleSpinBox->setDecimals(1);
            ui->maxdoubleSpinBox->setDecimals(1);
            ui->mindoubleSpinBox->setValue(item.min/COM_RATE_VOL);
            ui->maxdoubleSpinBox->setValue(item.max/COM_RATE_VOL);
            ui->mindoubleSpinBox->setSuffix(str);
            ui->maxdoubleSpinBox->setSuffix(str);
        }
        break;
    }

    ui->minBox->setSuffix(str);
    ui->maxBox->setSuffix(str);

    ui->minBox->setMaximum(range);
    ui->maxBox->setMaximum(range);
    ui->minBox->setValue(item.min);
    ui->maxBox->setValue(item.max);
}

void SetThresholdDlg::setTitle(sThresholdItem &item)
{
    QString str;
    switch (item.type) {
    case 1: str = tr("电压"); break;
    case 2: str = tr("电流"); break;
    case 3: str = tr("温度"); break;
    case 4: str = tr("功率"); break;
    case 5: str = tr("频率"); break;
    }

    sBoxData *dev = &(share_mem_get()->data[item.bus].box[item.box]); //获取共享内存
    QString nameStr = QString("插接箱%1").arg(dev->boxName);
    if(item.box == 0) nameStr = "始端箱";;

    QString busName = share_mem_get()->data[item.bus].busName;
    QString title = tr("母线%1 %2 输入%3 %4设置").arg(busName).arg(nameStr).arg(item.num+1).arg(str);
    if( item.type == 5 ) title = tr("母线%1 %2 %3设置").arg(busName).arg(nameStr).arg(str);
    ui->titleLab->setText(title);
}

void SetThresholdDlg::set(sThresholdItem &item)
{
    int rate = 1;
    sBusData *busData = &(share_mem_get()->data[item.bus]);
    sObjData *obj = &(busData->box[item.box].data);

    sDataUnit  *unit = NULL;
    sDataPowUnit  *unitPower = NULL;
    if( item.type <= 3)
        unit = &(busData->box[item.box].env.tem);
    else if(item.type == 4)
        unitPower = &(obj->pow);
    switch (item.type) {
    case 1: unit = &(obj->vol); /*rate = 10;*/ break;
    case 2: unitPower = &(obj->cur); /*rate = 100;*/  break;//rate = 10; break;
    case 3: unit = &(busData->box[item.box].env.tem);break;
    case 4: unitPower = &(obj->pow);break;
    case 5: rate = 10;break;
    }
    if( item.type == 3){
        ui->mindoubleSpinBox->hide();
        ui->maxdoubleSpinBox->hide();
        item.min = unit->min[item.num];
        item.max = unit->max[item.num];
    }else if( item.type == 1){
        ui->minBox->hide();
        ui->minBox->hide();
        item.min = unit->min[item.num];
        item.max = unit->max[item.num];
    }else if( item.type == 2 ){
        ui->minBox->hide();
        ui->maxBox->hide();
        item.min = unitPower->min[item.num];
        item.max = unitPower->max[item.num];
    }else if( item.type == 4 ){
        ui->minBox->hide();
        ui->maxBox->hide();
        item.min = unitPower->min[item.num];
        item.max = unitPower->max[item.num];
    }else{
        ui->minBox->hide();
        ui->maxBox->hide();
        item.min = busData->box[item.box].rate.smin;
        item.max = busData->box[item.box].rate.smax;
    }

    mItem = item;
    setTitle(item);
    initSpinBox(item);
}


bool SetThresholdDlg::checkData()
{
    bool ret = true;
    uint min = 0;
    uint max = 0;
    if(mItem.type == 3){
        min = ui->minBox->value();
        max = ui->maxBox->value();
    }else if(mItem.type == 1){
        min = ui->mindoubleSpinBox->value()*COM_RATE_VOL;
        max = ui->maxdoubleSpinBox->value()*COM_RATE_VOL;
    }else if(mItem.type == 2){
        min = ui->mindoubleSpinBox->value()*COM_RATE_CUR;
        max = ui->maxdoubleSpinBox->value()*COM_RATE_CUR;
    }else if(mItem.type == 5){
        min = ui->mindoubleSpinBox->value()*COM_RATE_VOL;
        max = ui->maxdoubleSpinBox->value()*COM_RATE_VOL;
    }else{
        min = ui->mindoubleSpinBox->value()*COM_RATE_POW;
        max = ui->maxdoubleSpinBox->value()*COM_RATE_POW;
    }
    if(min < max)  {
        mItem.min = min;
        mItem.max = max;
    } else {
        QMessageBox::warning(this,tr("waring"),tr("最小值大于最大值！"),tr("OK"));
        ret = false;
    }

    return ret;
}


void SetThresholdDlg::on_saveBtn_clicked()
{
    bool ret = checkData();
    if(ret) {
        if(ui->checkBox->isChecked()) {
            if(mItem.box) mItem.box = 0xff;
            else mItem.bus = 0xff;
            SetThread::bulid()->append(mItem);//统一设置发两遍
        }
        SetThread::bulid()->append(mItem);
    }
    this->close();
}

void SetThresholdDlg::on_cancelBtn_clicked()
{
    this->close();
}
