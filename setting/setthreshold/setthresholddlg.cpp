#include "setthresholddlg.h"
#include "ui_setthresholddlg.h"
#include "setthread.h"

SetThresholdDlg::SetThresholdDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetThresholdDlg)
{
    ui->setupUi(this);
    if(gLanguage == 0) com_setBackColour(tr("阈值设置"),this);
    else com_setBackColour(tr("Threshold setting"),this);
    initLanguage();
    setWindowModality(Qt::WindowModal);
}

SetThresholdDlg::~SetThresholdDlg()
{
    delete ui;
}
void SetThresholdDlg::initLanguage()
{
    if(gLanguage == 0){
        ui->titleLab->setText("阈值设置");
        ui->label_3->setText("最大值：");
        ui->label_2->setText("最小值：");
        ui->checkBox->setText("统一设置");
        ui->saveBtn->setText("保存");
        ui->cancelBtn->setText("取消");
    }else{
        ui->titleLab->setText("Threshold setting");
        ui->label_3->setText("Maximum value:");
        ui->label_2->setText("Minimum value:");
        ui->checkBox->setText("Unified settings");
        ui->saveBtn->setText("Save");
        ui->cancelBtn->setText("Cancel");
    }
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
    case 8:
            {
            if(!item.box){
                str = "A"; range = 1200;
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
    if(gLanguage == 0){
        QString str;
        switch (item.type) {
        case 1: str = tr("电压"); break;
        case 2: str = tr("电流"); break;
        case 3: str = tr("温度"); break;
        case 4: str = tr("功率"); break;
        case 5: str = tr("频率"); break;
        case 8: str = tr("零线电流"); break;
        }

        sBoxData *dev = &(share_mem_get()->data[item.bus].box[item.box]); //获取共享内存
        QString nameStr = QString("插接箱%1").arg(dev->boxName);
        if(item.box == 0) nameStr = "始端箱";;

        QString busName = share_mem_get()->data[item.bus].busName;
        QString title = tr("母线%1 %2 %3相 %4设置").arg(busName).arg(nameStr).arg(QString('A'+item.num)).arg(str);
        if( item.type == 5 || item.type == 8 ) title = tr("母线%1 %2 %3设置").arg(busName).arg(nameStr).arg(str);
        if(item.type == 8) ui->label_3->setText(tr("超限\n告警值："));
        ui->titleLab->setText(title);
    }else{
        QString str;
        switch (item.type) {
        case 1: str = tr("Voltage"); break;
        case 2: str = tr("Current"); break;
        case 3: str = tr("Temperature"); break;
        case 4: str = tr("Power"); break;
        case 5: str = tr("Frequency"); break;
        case 8: str = tr("Zero line current"); break;
        }

        sBoxData *dev = &(share_mem_get()->data[item.bus].box[item.box]); //获取共享内存
        QString nameStr = QString("Plug box%1").arg(dev->boxName);
        if(item.box == 0) nameStr = "Start box";;

        QString busName = share_mem_get()->data[item.bus].busName;
        QString title = tr("Busbar%1 %2 %3-phase %4set").arg(busName).arg(nameStr).arg(QString('A'+item.num)).arg(str);
        if( item.type == 5 || item.type == 8 ) title = tr("Busbar%1 %2 %3set").arg(busName).arg(nameStr).arg(str);
        if(item.type == 8) ui->label_3->setText("Over limit\nalarm value:");
        ui->titleLab->setText(title);
    }

}

void SetThresholdDlg::set(sThresholdItem &item)
{
    int rate = 1;
    sBusData *busData = &(share_mem_get()->data[item.bus]);
    sObjData *obj = &(busData->box[item.box].data);

    sDataUnit  *unit = NULL;
    sDataPowUnit  *unitPower = NULL;
    sRtuULLintUnit *unitZero = NULL;
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
    case 8: unitZero = &(busData->box[item.box].zeroLineCur); /*rate = 100;*/  break;//rate = 10; break;
    }
    ui->label_2->show();
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
    }else if( item.type == 8 ){
        ui->label_2->hide();
        ui->mindoubleSpinBox->hide();
        ui->minBox->hide();
        ui->maxBox->hide();
        item.min = unitZero->imin;
        item.max = unitZero->imax;
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
    }else if(mItem.type == 2 || mItem.type == 8){
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
        if(gLanguage == 0) QMessageBox::warning(this,tr("warning"),tr("最小值大于最大值！"),tr("OK"));
        else QMessageBox::warning(this,tr("warning"),tr("The minimun value is greater than the maximum value！"),tr("OK"));
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
            //else mItem.bus = 0xff;
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
