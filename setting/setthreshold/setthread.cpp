#include "setthread.h"
//#include "snmp/snmpthread.h"

SetThread::SetThread(QObject *parent) : QThread(parent)
{
    isRun = true;
    mSetShm = new SetShm(this);
    mNetCmd = SetNetCmd::bulid(this);
    mRtuCmd = new SetRtuCmd(this);
//    connect(mRtuCmd , SIGNAL(sendTripSig(QString,int)) , this , SIGNAL(sendSetThreadTripSig(QString,int)));
}

SetThread::~SetThread()
{
    isRun = false;
    wait();
}

SetThread *SetThread::bulid(QObject *parent)
{
    static SetThread* sington = NULL;
    if(sington == NULL) {
        sington = new SetThread(parent);
    }
    return sington;
}

void SetThread::workDown()
{
    if(mItems.isEmpty()) {
        //gReadWriteflag = 1;
        msleep(50); // Minor delay when idle to save CPU
        return;
    }

    //gReadWriteflag = 2;
    sThresholdItem item = mItems.first();
    //bool ret = mNetCmd->send(item);
    //if(!ret) mRtuCmd->send(item);//V2.5
    bool ret = false;
    int flag = 0;
    if(!ret){
        if(gVerflag == 2){
            {
                //QMutexLocker locker(&g_rtuMutex);
                if(item.box == 0) flag = mRtuCmd->sendStartV3(item);
                else flag = mRtuCmd->sendPlugV3(item);
            }
            if(item.insertlog == 1){
                QString type = tr("本机阈值设置");
                QString typeen = tr("Local threshold settings");
                QString msg1 = tr("");
                QString msg2 = tr("");
                QString msgen1 = tr("");
                QString msgen2 = tr("");

                if(item.bus == 0xff){
                    for(int i = 0; i < BUS_NUM; i++){
                        change(item , msg1 , msg2 , msgen1 , msgen2 , i);
                        if(item.premin != item.min){
                            db_operation_obj(i)->insertOperation(type , msg1);
                            db_operation_obj_en(i)->insertOperation(typeen , msgen1);
                        }
                        if(item.premax != item.max){
                            db_operation_obj(i)->insertOperation(type , msg2);
                            db_operation_obj_en(i)->insertOperation(typeen , msgen2);
                        }
                    }
                }else{
                    change(item , msg1 , msg2 , msgen1 , msgen2);
                    if(item.premin != item.min){
                        db_operation_obj(item.bus)->insertOperation(type , msg1);
                        db_operation_obj_en(item.bus)->insertOperation(typeen , msgen1);
                    }
                    if(item.premax != item.max){
                        db_operation_obj(item.bus)->insertOperation(type , msg2);
                        db_operation_obj_en(item.bus)->insertOperation(typeen , msgen2);
                    }
                }
            }//if(item.insertlog == 1)
            if(item.insertlog == 2){

                QString name = QString(get_share_mem()->data[item.bus].busName);
                if(item.box != 0) name = QString(get_share_mem()->data[item.bus].box[item.box].boxName);
                QString local = tr("本机");
                QString localen = tr("local");
                QString success = tr("成功");
                QString successen = tr("success");
                QString str = (item.min == 12)?tr("分励脱扣"):tr("RCA");
                QString stren = (item.min == 12)?tr("shunt trip"):tr("RCA");

                QString type = tr("%1控制").arg(str);
                QString typeen = tr("Control %1").arg(stren);
                QString operation = (item.min == 8)?tr("合闸"):tr("分闸");
                QString operationen = (item.min == 8)?tr(" turn on "):tr(" turn off ");
                quint8 bytes[6];
                bytes[0] = (item.crmin >> 8) & 0xFF;
                bytes[1] = item.crmin & 0xFF;
                bytes[2] = (item.crmax >> 8) & 0xFF;
                bytes[3] = item.crmax & 0xFF;
                bytes[4] = (item.max >> 8) & 0xFF;
                bytes[5] = item.max & 0xFF;

                // 拼成 MAC 地址字符串
                QString mac;
                for (int i = 0; i < 6; i++) {
                    mac += QString("%1").arg(bytes[i], 2, 16, QLatin1Char('0')).toUpper();
                    if (i < 5){
                        mac += ":";
                    }
                }
                if(flag != 6){
                    success = tr("失败");
                    successen = tr("failure");
                }
                if(item.txtype == 1){
                    local = tr("远程");
                    localen = tr("remote");
                }
                QString msg1 = tr("%2 %1 mac:%3 %4 %5%6").arg(name).arg(local).arg(mac).arg(str).arg(operation).arg(success);
                QString msgen1 = tr("%2 %1 mac:%3 %4 %5 %6").arg(name).arg(localen).arg(mac).arg(stren).arg(operationen).arg(successen);

                db_operation_obj(item.bus)->insertOperation(type , msg1);
                db_operation_obj_en(item.bus)->insertOperation(typeen , msgen1);

            }//if(item.insertlog == 2){

        }//if(gVerflag == 2)
//            if(gVerflag == 3){
//                gReadWriteflag = 2;
//                emit sendSetSnmpSig(&item);
//            }
    }

    //mSetShm->setItem(item);
    mItems.removeFirst();
    msleep(100);

}

QString SetThread::changeType(int index , QString &sym , double &rate)
{
    QString str = tr("电压");
    sym = "V";
    rate = COM_RATE_VOL;
    if(gLanguage == 1)str = tr("voltage ");
    switch (index) {
    case 1:{
        str = tr("电压");if(gLanguage == 1)str = tr("voltage ");sym = "V";rate = COM_RATE_VOL;break;
    }
    case 2:{
        str = tr("电流");if(gLanguage == 1)str = tr("current ");sym = "A";rate = COM_RATE_CUR;break;
    }
    case 3:{
        str = tr("温度");if(gLanguage == 1)str = tr("temperature ");sym = "°C";rate = COM_RATE_TEM;break;
    }
    case 4:{
        str = tr("功率");if(gLanguage == 1)str = tr("power ");sym = "kW";rate = COM_RATE_POW;break;
    }
    case 5:{
        str = tr("频率");if(gLanguage == 1)str = tr("frequency ");sym = "Hz";rate = COM_RATE_FREQUENCY;break;
    }
    case 8:{
        str = tr("零线电流");if(gLanguage == 1)str = tr("neutral line current ");sym = "A";rate = COM_RATE_CUR;break;
    }
//    case 11: reg = SetStartRelease ; break;
//    case 14: reg = SetStartControlRelease ; break;
    }
    return str;
}

QString SetThread::calcLoop(int id)
{
    int divisor   =  id/3; //除数
    int remainder =  id%3;//余数
    return QString((char)('A' + remainder))+ QString("%1").arg(divisor + 1);
}

void SetThread::change(sThresholdItem &item , QString &msg1 , QString &msg2 , QString &msgen1 , QString &msgen2 ,int index)
{
    sBusData *busdata = &(get_share_mem()->data[item.bus == 0xff ?index:item.bus]);
    QString str;QString sym;
    double rate;
    str = changeType(item.type , sym , rate);
    if(item.box == 0){

        QString phase = QString('A'+item.num);
        QString phaseen = phase;
        if(item.type == 3 && item.num == 3){
            phase = QString(tr("零线"));
            phaseen = QString(tr("neutral line"));
        }
        msg1 = tr("母线: %1 ，将输入 %2 %3最小值 %4%5设置成%6%7 !")
                  .arg(busdata->busName).arg(phase).arg(str).arg(item.premin/rate).arg(sym).arg(item.min/rate).arg(sym);
        msg2 = tr("母线: %1 ，将输入 %2 %3最大值 %4%5设置成%6%7 !")
                   .arg(busdata->busName).arg(phase).arg(str).arg(item.premax/rate).arg(sym).arg(item.max/rate).arg(sym);
        msgen1 = tr("Busbar: %1 ，set the minimum %3 of input %2 from %4%5 to %6%7 !")
                     .arg(busdata->busName).arg(phaseen).arg(str).arg(item.premin/rate).arg(sym).arg(item.min/rate).arg(sym);
        msgen2 = tr("Busbar: %1 ，set the maximum %3 of input %2 from %4%5 to %6%7 !")
                     .arg(busdata->busName).arg(phaseen).arg(str).arg(item.premax/rate).arg(sym).arg(item.max/rate).arg(sym);

    }
    else if(item.box != 0xff-1){
        msg1 = tr("插接箱:%1 ，").arg(busdata->box[item.box].boxName);
        msgen1 = tr("Tap-off box:%1 ，").arg(busdata->box[item.box].boxName);

        int id = item.num;
        QString output = calcLoop(id);
        QString outputen;
        if(item.type == 3){
            output = QString(tr("%1相").arg(QString('A' + item.num)));
            outputen = QString(tr("phase %1").arg(QString('A' + item.num)));
            if( item.num == 3){
                output = QString(tr("零线"));
                outputen = QString(tr("neutral line"));
            }
        }
        msg2 = msg1;
        msg1 += tr("将%1 %2最小值 %3%4设置成%5%6 !").arg(output).arg(str).arg(item.premin/rate).arg(sym).arg(item.min/rate).arg(sym);
        msg2 += tr("将%1 %2最大值 %3%4设置成%5%6 !").arg(output).arg(str).arg(item.premax/rate).arg(sym).arg(item.max/rate).arg(sym);
        msgen2 = msgen1;
        msgen1 += tr("set the minimum %2 of %1 from %3%4 to %5%6 !").arg(outputen).arg(str).arg(item.premin/rate).arg(sym).arg(item.min/rate).arg(sym);
        msgen2 += tr("set the maximum %2 of %1 from %3%4 to %5%6 !").arg(outputen).arg(str).arg(item.premax/rate).arg(sym).arg(item.max/rate).arg(sym);
    }
    else{
        msg1 = tr("插接箱统一设置： ");
        msgen1 = tr("Tap-off box Unified settings: ");
        int id = item.num;
        QString output = calcLoop(id);
        QString outputen;
        if(item.type == 3){
            output = QString(tr("%1相").arg(QString('A' + item.num)));
            outputen = QString(tr("phase %1").arg(QString('A' + item.num)));
            if( item.num == 3){
                output = QString(tr("零线"));
                outputen = QString(tr("neutral line"));
            }
        }
        msg2 = msg1;
        msg1 += tr("将%1 %2最小值 %3%4设置成%5%6 !").arg(output).arg(str).arg(item.premin/rate).arg(sym).arg(item.min/rate).arg(sym);
        msg2 += tr("将%1 %2最大值 %3%4设置成%5%6 !").arg(output).arg(str).arg(item.premax/rate).arg(sym).arg(item.max/rate).arg(sym);
        msgen2 = msgen1;
        msgen1 += tr("set the minimum %2 of %1 from %3%4 to %5%6 !").arg(outputen).arg(str).arg(item.premin/rate).arg(sym).arg(item.min/rate).arg(sym);
        msgen2 += tr("set the maximum %2 of %1 from %3%4 to %5%6 !").arg(outputen).arg(str).arg(item.premax/rate).arg(sym).arg(item.max/rate).arg(sym);
    }
}

void SetThread::run()
{
    while (isRun) {
        workDown();
        msleep(1);
    }
}
