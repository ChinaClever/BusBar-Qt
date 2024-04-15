#include "linetable.h"
#include <QGridLayout>

LineTable::LineTable(QWidget *parent) : ComTableWid(parent)
{
    QGridLayout *layout = new QGridLayout(parent);
    layout->addWidget(this);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    initWid();
}

void LineTable::initWid()
{
    QString title;
    QStringList header;

    if(gLanguage == 0){
        header << "L" << tr("负载率(%)")<< tr("温度(°C)")<< tr("视在功率(kVA)") << tr("电压谐波含量(%)") << tr("电流谐波含量(%)");
    }else{
        header << "L" << tr("Load rate(%)")<< tr("Temperature(°C)") << tr("Apparent power(kVA)") << tr("Voltage harmonic content(%)") << tr("Current harmonic content(%)");
    }
    initTableWid(header, 3, title);
}


int LineTable::updateObj(sObjData &obj, sEnvData &env, int row, uchar offline)
{
    QStringList list;
    int k = 0;

    list << QString((char)('A' + row));
    k++;

    double value = obj.pl[row];
    list << QString::number(offline==0?0:value);
    k++;

    value = env.tem.value[row] / COM_RATE_TEM;
    list << QString::number(offline==0?0:value);
    if(offline) setItemColor( row , k , env.tem.alarm[row]);
    else setItemColor( row , k , 0);
    k++;

    value = obj.apPow[row] / COM_RATE_POW;
    list << QString::number(offline==0?0:value,'f', 3);
    k++;

    value = obj.volThd[row] / 100.0;
    list << QString::number(offline==0?0:value,'f', 2);
    k++;

    value = obj.curThd[row] / 100.0;
    list << QString::number(offline==0?0:value,'f', 2);
    k++;

    setTableRow(row, list);

    return ++row;
}

void LineTable::updateData(sBoxData &box)
{
    for(int i=0; i<3; ++i) {
        updateObj(box.data, box.env, i , box.offLine);
    }
}
