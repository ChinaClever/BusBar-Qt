/*
 *
 *
 *  Created on: 2018年10月1日
 *      Author: Lzy
 */
#include "json_pack.h"


Json_Pack::Json_Pack(QObject *parent) : QThread(parent)
{
    mPro = new sProgress();
    mAlarm = new DpAlarmSlave(this);
    mIp = new IpSettingDlg();
    sDataPacket *shm = get_share_mem();
    for(int i = 0;i <BUS_NUM; i++)
    {
        mBusData[i] = &(shm->data[i]);
        mBoxData[i] = &(shm->data[i].box[0]);
    }
}

Json_Pack *Json_Pack::bulid(QObject *parent)
{
    static Json_Pack* sington = NULL;
    if(sington == NULL) {
        sington = new Json_Pack(parent);
    }
    return sington;
}

//----------------------------始端箱----------------------------
void Json_Pack::Startbox(QJsonObject &obj,int id)
{
    QDateTime t = QDateTime::currentDateTime();
    mPro->datetime = t.toString("yyyy-MM-dd HH:mm:ss");
    obj.insert("addr", 1);
    mPro->dev_ip = mIp->returnIp();
    obj.insert("bar_id", id + 1);
    obj.insert("datetime", mPro->datetime);
    obj.insert("dev_ip", mPro->dev_ip);
    mPro->status = Startbox_Status(id);
    obj.insert("status", mPro->status);

    Startbox_pduInfo(obj,id);
}
void Json_Pack::Startbox_pduInfo(QJsonObject &obj, int id)
{
    Startbox_Data(obj,id);
    Startbox_Alarm(obj,id);
}
void Json_Pack::Startbox_Alarm(QJsonObject &obj ,int id)
{
    QStringList alarmStr = get_alarm_json();
    QString bus_list[4];

    for(int i=0; i<alarmStr.size()-3; i+=3){
        if(alarmStr.at(i).contains("BUS-1"))
            {
                for(int j = i; j< i+3;j++){
                    bus_list[0] += alarmStr.at(j);
                }
                bus_list[0] += tr("\n");
            }else if(alarmStr.at(i).contains("BUS-2")){
                for(int j = i; j< i+3;j++){
                    bus_list[1] += alarmStr.at(j);
                }
                bus_list[1] += tr("\n");
            }else if(alarmStr.at(i).contains("BUS-3")){
                for(int j = i; j< i+3;j++){
                    bus_list[2] += alarmStr.at(j);
                }
                bus_list[2] += tr("\n");
            }else if(alarmStr.at(i).contains("BUS-4")){
                for(int j = i; j< i+3;j++){
                    bus_list[3] += alarmStr.at(j);
                }
                bus_list[3] += tr("\n");
            }
    }

    obj.insert("dev_alarm",bus_list[id]);
}
int Json_Pack::Startbox_Status(int id)
{
    int status = 0;
    uchar boxalarm = mBoxData[id]->boxAlarm;
    if(mBoxData[id]->offLine){
        if(boxalarm) status = 2;//告警
        else if(boxalarm == 0) status = 1;//正常工作
    }else{
        status = 0;                         //离线
    }

    return status;
}

void Json_Pack::Startbox_Data(QJsonObject &obj ,int id)
{
    QJsonArray jsonArray; QJsonObject subObj, envObj, cfgObj, tgObj, dataObj;
//----------------------------配置数据------------------------------------------
    QString version = QString("V%1.%2.%3").arg(mBoxData[id]->version/100).arg(mBoxData[id]->version/10%10).arg(mBoxData[id]->version%10);
    cfgObj.insert("bus_version",version);
    cfgObj.insert("cur_specs", mBoxData[id]->curSpecification);
    cfgObj.insert("baud_rate",mBoxData[id]->baudRate);
    cfgObj.insert("beep",mBoxData[id]->buzzerStatus);
    cfgObj.insert("work_mode",mBoxData[id]->workMode);
    cfgObj.insert("ac_dc",mBoxData[id]->dc);
    cfgObj.insert("item_type",mBoxData[id]->proNum);
    cfgObj.insert("box_num",mBusData[id]->boxNum);
    cfgObj.insert("alarm_count",mBoxData[id]->alarmTime);
    cfgObj.insert("iof",mBoxData[id]->iOF);
    cfgObj.insert("isd",mBoxData[id]->isd);
    cfgObj.insert("shunt_trip",mBoxData[id]->shuntRelease);
    cfgObj.insert("lsp_status",mBoxData[id]->lpsAlarm);
    cfgObj.insert("breaker_status",mBoxData[id]->data.sw[0]);
//----------------------------相数据------------------------------------------
    QJsonArray volValue, volMax, volMin, volArray;
    for(int i = 0;i < START_LINE_NUM; i++)
    {
        volValue.append(mBoxData[id]->data.vol.value[i]/COM_RATE_VOL);
        volMin.append(mBoxData[id]->data.vol.min[i]/COM_RATE_VOL);
        volMax.append(mBoxData[id]->data.vol.max[i]/COM_RATE_VOL);
        volArray.append(mBoxData[id]->data.vol.upalarm[i]);
    }
    subObj.insert("vol_value",volValue);
    subObj.insert("vol_min",volMin);
    subObj.insert("vol_max",volMax);
    subObj.insert("vol_status",volArray);

    QJsonArray curValue, curMin, curMax, curArray;

    for(int i = 0;i < START_LINE_NUM; i++)
    {
        curValue.append(QJsonValue::fromVariant((mBoxData[id]->data.cur.value[i])/COM_RATE_CUR));
        curMin.append(QJsonValue::fromVariant((mBoxData[id]->data.cur.min[i])/COM_RATE_CUR));
        curMax.append(QJsonValue::fromVariant((mBoxData[id]->data.cur.max[i])/COM_RATE_CUR));
        curArray.append(mBoxData[id]->data.cur.upalarm[i]);
    }
    subObj.insert("cur_value",curValue);
    subObj.insert("cur_min",curMin);
    subObj.insert("cur_max",curMax);
    subObj.insert("cur_status",curArray);

    QJsonArray PowerValue, PowerMin, PowerMax, powArray;
    for(int i = 0;i < START_LINE_NUM; i++)
    {
        PowerValue.append(QJsonValue::fromVariant((mBoxData[id]->data.pow.value[i])/COM_RATE_POW));
        PowerMin.append(QJsonValue::fromVariant((mBoxData[id]->data.pow.min[i])/COM_RATE_POW));
        PowerMax.append(QJsonValue::fromVariant((mBoxData[id]->data.pow.max[i])/COM_RATE_POW));
        powArray.append(mBoxData[id]->data.pow.upalarm[i]);
    }
    subObj.insert("pow_value",PowerValue);
    subObj.insert("pow_min",PowerMin);
    subObj.insert("pow_max",PowerMax);
    subObj.insert("pow_status",powArray);

    QJsonArray reactivepowArray, plArray,apArray;
    QJsonArray pfArray, elepowArray, ele_reactive;
    for(int i = 0;i < START_LINE_NUM; i++)
    {
        plArray.append(mBoxData[id]->data.pl[i]);
    }
    subObj.insert("load_rate",plArray);

    for(int i = 0;i < START_LINE_NUM; i++)
    {
        reactivepowArray.append(QJsonValue::fromVariant((mBoxData[id]->data.reactivePower[i])/COM_RATE_POW));
        pfArray.append(mBoxData[id]->data.pf[i]/COM_RATE_PF);
        apArray.append(QJsonValue::fromVariant((mBoxData[id]->data.apPow[i]/COM_RATE_POW)));
        elepowArray.append(QJsonValue::fromVariant((mBoxData[id]->data.ele[i])/COM_RATE_ELE));
        ele_reactive.append("a");
    }
    subObj.insert("pow_apparent",apArray);
    subObj.insert("pow_reactive",reactivepowArray);
    subObj.insert("power_factor",pfArray);
    subObj.insert("ele_active",elepowArray);
    subObj.insert("ele_reactive",ele_reactive);

    QJsonArray linevolValue, linevolMin, linevolMax, linevolArray;
    for(int i = 0;i < START_LINE_NUM; i++)
    {
        linevolValue.append(mBoxData[id]->data.lineVol.value[i]/COM_RATE_VOL);
        linevolMin.append(mBoxData[id]->data.lineVol.min[i]/COM_RATE_VOL);
        linevolMax.append(mBoxData[id]->data.lineVol.max[i]/COM_RATE_VOL);
        linevolArray.append(mBoxData[id]->data.lineVol.upalarm[i]);
    }
    subObj.insert("vol_line_value",linevolValue);
    subObj.insert("vol_line_min",linevolMin);
    subObj.insert("vol_line_max",linevolMax);
    subObj.insert("vol_line_status",linevolArray);
//----------------------------环境数据------------------------------------------
    QJsonArray envValue, envMin, envMax;
    for(int i = 0;i < SENSOR_NUM; i++)
    {
        envValue.append(mBoxData[id]->env.tem.value[i]/COM_RATE_TEM);
        envMin.append(mBoxData[id]->env.tem.min[i]/COM_RATE_TEM);
        envMax.append(mBoxData[id]->env.tem.max[i]/COM_RATE_TEM);
    }
    envObj.insert("tem_value",envValue);
    envObj.insert("tem_min",envMin);
    envObj.insert("tem_max",envMax);
    QJsonArray temArray;
    for(int i = 0;i < 4; i++)
        temArray.append(mBoxData[id]->env.tem.upalarm[i]);
    envObj.insert("tem_status",temArray);
//----------------------------总数据------------------------------------------
    tgObj.insert("pow_value",mBoxData[id]->totalPow.ivalue/COM_RATE_POW);
    tgObj.insert("pow_min",mBoxData[id]->totalPow.imin/COM_RATE_POW);
    tgObj.insert("pow_max",mBoxData[id]->totalPow.imax/COM_RATE_POW);
    tgObj.insert("pow_status",mBoxData[id]->totalPow.iupalarm);
    tgObj.insert("pow_apparent",mBoxData[id]->totalApPow/COM_RATE_POW);
    tgObj.insert("pow_reactive",((mBoxData[id]->totalApPow/COM_RATE_POW)-(mBoxData[id]->totalPow.ivalue/COM_RATE_POW)));
    double eleActive,pfTotal;
    for(int j=0; j<3; ++j) {
        eleActive += ((mBoxData[id]->data.ele[j])/COM_RATE_ELE);
    }
    pfTotal = (mBoxData[id]->totalPow.ivalue/COM_RATE_POW)/(mBoxData[id]->totalApPow/COM_RATE_POW);
    tgObj.insert("power_factor",pfTotal);   //功率因素
    tgObj.insert("ele_active",eleActive);
    tgObj.insert("ele_apparent","a");     //视在电能
    tgObj.insert("ele_reactive","a");     //无功电能
    tgObj.insert("cur_residual_value",mBoxData[id]->reCur.svalue/COM_RATE_CUR);
    tgObj.insert("cur_residual_alarm",mBoxData[id]->reCur.smax/COM_RATE_CUR);
    tgObj.insert("cur_residual_status",mBoxData[id]->reCur.supalarm);
    tgObj.insert("cur_zero_value",QString::number((mBoxData[id]->zeroLineCur.ivalue)/COM_RATE_CUR));
    tgObj.insert("cur_zero_alarm",QString::number((mBoxData[id]->zeroLineCur.imax)/COM_RATE_CUR));
    tgObj.insert("cur_zero_status",mBoxData[id]->zeroLineCur.iupalarm);
    tgObj.insert("vol_unbalance",QJsonValue::fromVariant(mBoxData[id]->volUnbalance));
    tgObj.insert("cur_unbalance",QJsonValue::fromVariant(mBoxData[id]->curUnbalance));
    tgObj.insert("hz_value",mBoxData[id]->rate.svalue/COM_RATE_FREQUENCY);
    tgObj.insert("hz_min",mBoxData[id]->rate.smin/COM_RATE_FREQUENCY);
    tgObj.insert("hz_max",mBoxData[id]->rate.smax/COM_RATE_FREQUENCY);
    tgObj.insert("hz_status", mBoxData[id]->rate.supalarm);

    QJsonArray thdCur, thdVol;
    for(int i = 0;i < START_LINE_NUM; i++)
    {
        for(int j = 0;j < HARMONIC_NUM; j++)
        {
            thdCur.append(mBusData[id]->thdData.curThd[i][j]);
            thdVol.append(mBusData[id]->thdData.volThd[i][j]);
        }
    }
    subObj.insert("cur_thd",thdCur);
    subObj.insert("vol_thd",thdVol);
    dataObj.insert("bus_cfg",cfgObj);
    dataObj.insert("line_item_list",subObj);
    dataObj.insert("env_item_list",envObj);
    dataObj.insert("bus_total_data",tgObj);
    obj.insert("bus_data" ,dataObj);
}

//----------------------------插接箱----------------------------
void Json_Pack::Insertbox(QJsonObject &obj,int bus_id,int insert_id)
{
    QDateTime t = QDateTime::currentDateTime();
    mPro->datetime = t.toString("yyyy-MM-dd HH:mm:ss");
    obj.insert("addr", insert_id + 1);
    obj.insert("bar_id", bus_id + 1);
    mPro->status = Insertbox_Status(bus_id, insert_id);
    obj.insert("status",mPro->status);
    Insertbox_pduInfo(obj,bus_id,insert_id);
}
int Json_Pack::Insertbox_Status(int bus_id,int insert_id)
{
    int status;
    uchar boxalarm = mBusData[bus_id]->box[insert_id].boxAlarm;
    if(mBusData[bus_id]->box[insert_id].offLine){
        if(boxalarm) status = 2;
        else if(boxalarm == 0) status = 1;
    }else{
        status = 0;
    }

    return status;
}
void Json_Pack::Insertbox_pduInfo(QJsonObject &obj, int bus_id,int insert_id)
{
    Insertbox_Data(obj,bus_id,insert_id);
}

void Json_Pack::Insertbox_Data(QJsonObject &obj ,int bus_id, int insert_id)
{
    QJsonArray swArray; QJsonObject subObj, cfgObj, loopObj, lineObj, envObj, totalObj, outputObj;
//----------------------------配置数据------------------------------------------
    QString version = QString("V%1.%2.%3").arg(mBusData[bus_id]->box[insert_id].version/100).arg(mBusData[bus_id]->box[insert_id].version/10%10).arg(mBusData[bus_id]->box[insert_id].version%10);
    cfgObj.insert("box_version",version);
    cfgObj.insert("baud_rate",mBusData[bus_id]->box[insert_id].baudRate);
    cfgObj.insert("beep",mBusData[bus_id]->box[insert_id].buzzerStatus);
    cfgObj.insert("work_mode",mBusData[bus_id]->box[insert_id].workMode);
    cfgObj.insert("item_type",mBusData[bus_id]->box[insert_id].proNum);
    cfgObj.insert("loop_num",mBusData[bus_id]->box[insert_id].loopNum);
    cfgObj.insert("alarm_count",mBusData[bus_id]->box[insert_id].alarmTime);
    cfgObj.insert("iof",mBusData[bus_id]->box[insert_id].iOF);
    cfgObj.insert("box_type",mBusData[bus_id]->box[insert_id].boxType);
    for(int i =0; i < mBusData[bus_id]->box[insert_id].loopNum; i++)
    {
        swArray.append(mBusData[bus_id]->box[insert_id].data.sw[i]);
    }
    cfgObj.insert("breaker_status",swArray);
    subObj.insert("box_cfg",cfgObj);
//-------------------------------回路数据-------------------
    QJsonArray loopvolArray, loopcurArray, looppowArray;
    QJsonArray loopvolMin, loopcurMin, looppowMin;
    QJsonArray loopvolMax, loopcurMax, looppowMax;
    QJsonArray volArray, curArray, powArray;

    for(int i = 0;i < mBusData[bus_id]->box[insert_id].loopNum; i++)
    {
        loopvolArray.append((mBusData[bus_id]->box[insert_id].data.vol.value[i])/COM_RATE_VOL);
        loopvolMin.append((mBusData[bus_id]->box[insert_id].data.vol.min[i])/COM_RATE_VOL);
        loopvolMax.append((mBusData[bus_id]->box[insert_id].data.vol.max[i])/COM_RATE_VOL);
        volArray.append(mBusData[bus_id]->box[insert_id].data.vol.upalarm[i]);

        loopcurArray.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.cur.value[i])/COM_RATE_CUR));
        loopcurMin.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.cur.min[i])/COM_RATE_CUR));
        loopcurMax.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.cur.max[i])/COM_RATE_CUR));
        curArray.append(mBusData[bus_id]->box[insert_id].data.cur.upalarm[i]);

        looppowArray.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.pow.value[i])/COM_RATE_POW));
        looppowMin.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.pow.min[i])/COM_RATE_POW));
        looppowMax.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.pow.max[i])/COM_RATE_POW));
        powArray.append(mBusData[bus_id]->box[insert_id].data.pow.upalarm[i]);
    }
    loopObj.insert("vol_value",loopvolArray);
    loopObj.insert("vol_min",loopvolMin);
    loopObj.insert("vol_max",loopvolMax);
    loopObj.insert("vol_status",volArray);


    loopObj.insert("cur_value",loopcurArray);
    loopObj.insert("cur_min",loopcurMin);
    loopObj.insert("cur_max",loopcurMax);
    loopObj.insert("cur_status",curArray);


    loopObj.insert("pow_value",looppowArray);
    loopObj.insert("pow_min",looppowMin);
    loopObj.insert("pow_max",looppowMax);
    loopObj.insert("pow_status",looppowArray);

    QJsonArray reactivepowValue, appowValue, pfValue, eleValue, reactiveValue;
    for(int i = 0;i < mBusData[bus_id]->box[insert_id].loopNum; i++)
    {
        reactivepowValue.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.reactivePower[i])/COM_RATE_POW));
        appowValue.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.apPow[i])/COM_RATE_POW));
        pfValue.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.pf[i])/COM_RATE_PF));
        eleValue.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].data.ele[i])/COM_RATE_ELE));
        reactiveValue.append("a");
    }
    loopObj.insert("pow_reactive" ,reactivepowValue);
    loopObj.insert("pow_apparent" ,appowValue);
    loopObj.insert("power_factor" ,pfValue);
    loopObj.insert("ele_active" ,eleValue);
    loopObj.insert("ele_reactive" ,reactiveValue);
    subObj.insert("loop_item_list" ,loopObj);
//----------------------------相数据-------------------------------
    QJsonArray lineVol, linecur, linepow, lineele, lineapw,linereactive, linepf, linepl, linereactiveele, curthd;
    for(int i = 0;i < START_LINE_NUM; i++)
    {
        lineVol.append(mBusData[bus_id]->box[insert_id].lineTgBox.vol[i]);
        linecur.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].lineTgBox.cur[i])/COM_RATE_CUR));
        curthd.append(mBusData[bus_id]->box[insert_id].data.curThd[i]);
        linepow.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].lineTgBox.pow[i])/COM_RATE_POW));
        lineele.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].lineTgBox.ele[i])/COM_RATE_ELE));
        lineapw.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].lineTgBox.apPow[i])/COM_RATE_POW));
        linereactive.append(QJsonValue::fromVariant((mBusData[bus_id]->box[insert_id].lineTgBox.reactivePower[i])/COM_RATE_POW));
        linepf.append((mBusData[bus_id]->box[insert_id].lineTgBox.pf[i]/COM_RATE_PF));
        linepl.append(mBusData[bus_id]->box[insert_id].data.pl[i]);
        linereactiveele.append("a");
    }
    lineObj.insert("vol_value",lineVol);
    lineObj.insert("cur_value",linecur);
    lineObj.insert("cur_thd",curthd);
    lineObj.insert("pow_active",linepow);
    lineObj.insert("ele_active",lineele);
    lineObj.insert("pow_apparent",lineapw);
    lineObj.insert("pow_reactive",linereactive);
    lineObj.insert("power_factor",linepf);
    lineObj.insert("load_rate",linepl);
    lineObj.insert("ele_reactive",linereactiveele);     //无功电能

    subObj.insert("line_item_list" ,lineObj);
//----------------------------总数据------------------------------------------
    double pf_Total;
    totalObj.insert("pow_active",(mBusData[bus_id]->box[insert_id].tgBox.pow)/COM_RATE_POW);
    totalObj.insert("pow_apparent",(mBusData[bus_id]->box[insert_id].tgBox.apPow)/COM_RATE_POW);
    totalObj.insert("ele_active",(mBusData[bus_id]->box[insert_id].tgBox.ele)/COM_RATE_ELE);
    totalObj.insert("pow_reactive",((mBusData[bus_id]->box[insert_id].tgBox.apPow)-(mBusData[bus_id]->box[insert_id].tgBox.pow))/COM_RATE_POW);
    pf_Total = ((mBusData[bus_id]->box[insert_id].tgBox.pow)/(mBusData[bus_id]->box[insert_id].tgBox.apPow))/COM_RATE_POW;
    totalObj.insert("power_factor",pf_Total);
    totalObj.insert("ele_apparent","a");
    totalObj.insert("ele_reactive","a");
    subObj.insert("box_total_data" ,totalObj);
//----------------------------输出位数据------------------------------------------
    double pow_active[3], pow_apparent[3],pow_reactive[3], ele_active[3],ele_apparent[3],ele_reactive[3];
    QJsonArray actPow, apPow, reaPow, actEle, apEle, reaEle, pfPow;
    if(mBusData[bus_id]->box[insert_id].phaseFlag){//三相
        for(int i=0; i<3; ++i)
        {
            for(int j=0; j<3; ++j) {
                    pow_apparent[i] += ((mBusData[bus_id]->box[insert_id].data.apPow[i*3+j])/COM_RATE_POW);
                    pow_active[i] += ((mBusData[bus_id]->box[insert_id].data.pow.value[i*3+j])/COM_RATE_POW);
                    pow_reactive[i] += ((mBusData[bus_id]->box[insert_id].data.reactivePower[i*3+j])/COM_RATE_POW);
                    ele_active[i] += ((mBusData[bus_id]->box[insert_id].data.ele[i*3+j])/COM_RATE_ELE);
            }
            if(pow_active[i] > 0) pfPow.append(pow_active[i] * 100.0/ pow_apparent[i]);
            else pfPow.append(0);
            if((pfPow.at(i)).toInt() >99) pfPow.at(i) = 99;

            apPow.append(pow_apparent[i]);
            actPow.append(pow_active[i]);
            reaPow.append(pow_reactive[i]);
            actEle.append(ele_active[i]);
            apEle.append("a");
            reaEle.append("a");
        }

    }else{
        for(int i=0; i<3; ++i)//单相三个输出位
        {
            if(mBusData[bus_id]->box[insert_id].data.pow.value[i] > 0) pfPow.append((mBusData[bus_id]->box[insert_id].data.pow.value[i]) * 100.0 / (mBusData[bus_id]->box[insert_id].data.apPow[i]));
            else pfPow.append(0);
            if((pfPow.at(i)).toInt()>99) pfPow.at(i) = 99;

            actPow.append((mBusData[bus_id]->box[insert_id].data.pow.value[i])/COM_RATE_POW);
            apPow.append((mBusData[bus_id]->box[insert_id].data.apPow[i])/COM_RATE_POW);
            reaPow.append((mBusData[bus_id]->box[insert_id].data.reactivePower[i])/COM_RATE_POW);
            actEle.append((mBusData[bus_id]->box[insert_id].data.ele[i])/COM_RATE_ELE);
            apEle.append("a");
            reaEle.append("a");
        }
    }
    outputObj.insert("pow_active",actPow);
    outputObj.insert("pow_apparent",apPow);
    outputObj.insert("pow_reactive",reaPow);
    outputObj.insert("ele_active",actEle);
    outputObj.insert("ele_apparent",apEle);
    outputObj.insert("ele_reactive",reaEle);
    outputObj.insert("power_factor",pfPow);

    subObj.insert("outlet_item_list" ,outputObj);
//----------------------------环境数据------------------------------------------
    QJsonArray envValue, envMin, envMax, temArray;
    for(int i = 0;i < SENSOR_NUM; i++)
    {
        envValue.append((mBusData[bus_id]->box[insert_id].env.tem.value[i])/COM_RATE_TEM);
        envMin.append((mBusData[bus_id]->box[insert_id].env.tem.min[i])/COM_RATE_TEM);
        envMax.append((mBusData[bus_id]->box[insert_id].env.tem.max[i])/COM_RATE_TEM);
        temArray.append(mBusData[bus_id]->box[insert_id].env.tem.upalarm[i]);
    }
    envObj.insert("tem_value",envValue);
    envObj.insert("tem_min",envMin);
    envObj.insert("tem_max",envMax);
    envObj.insert("tem_status",temArray);

    subObj.insert("env_item_list" ,envObj);
    obj.insert("box_data" ,subObj);
}

void Json_Pack::getStart_Json(QJsonObject &json , QByteArray &ba, int id)
{
    Startbox(json, id);
    QJsonDocument jsonDoc(json);
    ba = jsonDoc.toJson();
}

void Json_Pack::getInsert_Json(QJsonObject &json , QByteArray &ba, int id, int num)
{
    Insertbox(json, id, num);
    QJsonDocument jsonDoc(json);
    ba = jsonDoc.toJson();
}
