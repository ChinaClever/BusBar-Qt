/*
 *
 *
 *  Created on: 2018年10月1日
 *      Author: Lzy
 */
#include "json_pack.h"

//extern QString get_alarm_json();

Json_Pack::Json_Pack(QObject *parent) : QThread(parent)
{
    mPro = new sProgress();
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
    mPro->status = Startbox_Status(obj ,id);
    obj.insert("status", mPro->status);
    obj.insert("bus_name", mBusData[id]->busName);
    obj.insert("box_name", "");

    Startbox_pduInfo(obj,id);
}
void Json_Pack::Startbox_pduInfo(QJsonObject &obj, int id)
{
    Startbox_Data(obj,id);
    //Startbox_Alarm(obj,id);
}

bool Json_Pack::Startbox_Alarm(QJsonObject &obj ,int id)
{
    QStringList alarmStr = get_alarm_json();
    QString bus_list; bus_list.clear();

    for(int i=0; i<alarmStr.size()-3; i+=3){
        if(alarmStr.at(i).contains(mBusData[id]->busName))
        {
            for(int j = i; j< i+3;j++){
                bus_list += alarmStr.at(j);
            }
            bus_list += tr("\n");
        }
//        else if(alarmStr.at(i).contains("BUS-2")){
//                for(int j = i; j< i+3;j++){
//                    bus_list[1] += alarmStr.at(j);
//                }
//                bus_list[1] += tr("\n");
//            }else if(alarmStr.at(i).contains("BUS-3")){
//                for(int j = i; j< i+3;j++){
//                    bus_list[2] += alarmStr.at(j);
//                }
//                bus_list[2] += tr("\n");
//            }else if(alarmStr.at(i).contains("BUS-4")){
//                for(int j = i; j< i+3;j++){
//                    bus_list[3] += alarmStr.at(j);
//                }
//                bus_list[3] += tr("\n");
//            }
    }
    if(!bus_list.isEmpty())m_buslist[id] = bus_list;
    obj.insert("dev_alarm",m_buslist[id]);
    return m_buslist[id].isEmpty();
}
int Json_Pack::Startbox_Status(QJsonObject &obj,int id)
{
    int status = 0;
    uchar boxalarm = mBoxData[id]->boxAlarm;
    if(mBoxData[id]->offLine){
        if(boxalarm && !Startbox_Alarm(obj,id)) status = 2;//告警
        else{
            status = 1;//正常工作
            if(!m_buslist[id].isEmpty())
            m_buslist[id].clear();
        }
    }else{
        status = 0;//离线
        if(!m_buslist[id].isEmpty())
        m_buslist[id].clear();
    }

    return status;
}

void Json_Pack::Startbox_Data(QJsonObject &obj ,int id)
{
    QJsonArray jsonArray; QJsonObject subObj, envObj, cfgObj, tgObj, dataObj;
    char dc;
//----------------------------配置数据------------------------------------------
    QString version = QString("V%1.%2.%3").arg(mBoxData[id]->version/100).arg(mBoxData[id]->version/10%10).arg(mBoxData[id]->version%10);
    cfgObj.insert("bus_version",version);
    cfgObj.insert("cur_specs", mBoxData[id]->curSpecification);
    cfgObj.insert("baud_rate",mBoxData[id]->baudRate);
    cfgObj.insert("beep",mBoxData[id]->buzzerStatus);
    cfgObj.insert("work_mode",mBoxData[id]->workMode);
    if(mBoxData[id]->dc) dc = 0;
    else dc = 1;
    cfgObj.insert("ac_dc",dc);
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
        pfArray.append(QJsonValue::fromVariant(mBoxData[id]->data.pf[i]/COM_RATE_PF));
        apArray.append(QJsonValue::fromVariant((mBoxData[id]->data.apPow[i]/COM_RATE_POW)));
        elepowArray.append(QJsonValue::fromVariant((mBoxData[id]->data.ele[i])/COM_RATE_ELE));
        ele_reactive.append("");
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
    tgObj.insert("pow_reactive",((mBoxData[id]->totalApPow-mBoxData[id]->totalPow.ivalue)/COM_RATE_POW));
    double eleActive = 0,pfTotal = 0;
    for(int j=0; j<3; ++j) {
        eleActive += ((mBoxData[id]->data.ele[j])/COM_RATE_ELE);
    }
    if(mBoxData[id]->totalApPow)
        pfTotal = (((mBoxData[id]->totalPow.ivalue)/(mBoxData[id]->totalApPow))/COM_RATE_POW);
    else pfTotal = 0;

    tgObj.insert("power_factor",QJsonValue::fromVariant(mBoxData[id]->tgBox.pf/COM_RATE_PF));
//    tgObj.insert("power_factor",QString::number(pfTotal,'f',2).toDouble());   //功率因素
    tgObj.insert("ele_active",eleActive);
    tgObj.insert("ele_apparent","");     //视在电能
    tgObj.insert("ele_reactive","");     //无功电能
    tgObj.insert("cur_residual_value",mBoxData[id]->reCur.svalue/COM_RATE_CUR);
    tgObj.insert("cur_residual_alarm",mBoxData[id]->reCur.smax/COM_RATE_CUR);
    tgObj.insert("cur_residual_status",mBoxData[id]->reCur.supalarm);
    tgObj.insert("cur_zero_value",(mBoxData[id]->zeroLineCur.ivalue)/COM_RATE_CUR);
    tgObj.insert("cur_zero_alarm",(mBoxData[id]->zeroLineCur.imax)/COM_RATE_CUR);
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
    dataObj.insert("bus_total_data",tgObj);

    obj.insert("bus_data" ,dataObj);
    obj.insert("env_item_list" ,envObj);
}

//----------------------------插接箱----------------------------
void Json_Pack::Insertbox(QJsonObject &obj,int bus_id,int insert_id)
{
    QDateTime t = QDateTime::currentDateTime();
    mPro->datetime = t.toString("yyyy-MM-dd HH:mm:ss");
    obj.insert("datetime", mPro->datetime);
    obj.insert("addr", insert_id + 1);
    obj.insert("bar_id", bus_id + 1);
    obj.insert("dev_ip", mPro->dev_ip);
    mPro->status = Insertbox_Status(bus_id, insert_id);
    obj.insert("status",mPro->status);
    obj.insert("bus_name", mBusData[bus_id]->busName);
    obj.insert("box_name", mBusData[bus_id]->box[insert_id].boxName);

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
    sBoxData *BoxData = &(mBusData[bus_id]->box[insert_id]);
    sObjData *LoopData = &(mBusData[bus_id]->box[insert_id].data);
    QJsonArray swArray; QJsonObject subObj, cfgObj, loopObj, lineObj, envObj, totalObj, outputObj;
//----------------------------配置数据------------------------------------------
    QString version = QString("V%1.%2.%3").arg(BoxData->version/100).arg(BoxData->version/10%10).arg(BoxData->version%10);
    cfgObj.insert("box_version",version);
    cfgObj.insert("baud_rate",BoxData->baudRate);
    cfgObj.insert("beep",BoxData->buzzerStatus);
    cfgObj.insert("work_mode",insert_id +1);
    cfgObj.insert("item_type",BoxData->proNum);
    cfgObj.insert("loop_num",BoxData->loopNum);
    cfgObj.insert("alarm_count",BoxData->alarmTime);
    cfgObj.insert("iof",BoxData->iOF);
    cfgObj.insert("box_type",BoxData->boxType);
    for(int i =0; i < BoxData->loopNum; i++)
    {
        swArray.append(BoxData->data.sw[i]);
    }
    cfgObj.insert("breaker_status",swArray);
    subObj.insert("box_cfg",cfgObj);
//-------------------------------回路数据-------------------
    QJsonArray loopvolArray, loopcurArray, looppowArray;
    QJsonArray loopvolMin, loopcurMin, looppowMin;
    QJsonArray loopvolMax, loopcurMax, looppowMax;
    QJsonArray volArray, curArray, powArray;

    for(int i = 0;i < BoxData->loopNum; i++)
    {
        loopvolArray.append((LoopData->vol.value[i])/COM_RATE_VOL);
        loopvolMin.append((LoopData->vol.min[i])/COM_RATE_VOL);
        loopvolMax.append((LoopData->vol.max[i])/COM_RATE_VOL);
        volArray.append(LoopData->vol.upalarm[i]);

        loopcurArray.append(QJsonValue::fromVariant((LoopData->cur.value[i])/COM_RATE_CUR));
        loopcurMin.append(QJsonValue::fromVariant((LoopData->cur.min[i])/COM_RATE_CUR));
        loopcurMax.append(QJsonValue::fromVariant((LoopData->cur.max[i])/COM_RATE_CUR));
        curArray.append(BoxData->data.cur.upalarm[i]);

        looppowArray.append(QJsonValue::fromVariant((LoopData->pow.value[i])/COM_RATE_POW));
        looppowMin.append(QJsonValue::fromVariant((LoopData->pow.min[i])/COM_RATE_POW));
        looppowMax.append(QJsonValue::fromVariant((LoopData->pow.max[i])/COM_RATE_POW));
        powArray.append(LoopData->pow.upalarm[i]);
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
    loopObj.insert("pow_status",powArray);

    QJsonArray reactivepowValue, appowValue, pfValue, eleValue, reactiveValue;
    for(int i = 0;i < BoxData->loopNum; i++)
    {
        reactivepowValue.append(QJsonValue::fromVariant((LoopData->reactivePower[i])/COM_RATE_POW));
        appowValue.append(QJsonValue::fromVariant((LoopData->apPow[i])/COM_RATE_POW));
        pfValue.append(QJsonValue::fromVariant((LoopData->pf[i])/COM_RATE_PF));
        eleValue.append(QJsonValue::fromVariant((LoopData->ele[i])/COM_RATE_ELE));
        reactiveValue.append("");
    }
    loopObj.insert("pow_reactive" ,reactivepowValue);
    loopObj.insert("pow_apparent" ,appowValue);
    loopObj.insert("power_factor" ,pfValue);
    loopObj.insert("ele_active" ,eleValue);
    loopObj.insert("ele_reactive" ,reactiveValue);

    subObj.insert("loop_item_list" ,loopObj);
//----------------------------相数据-------------------------------
    QJsonArray lineVol, linecur, linepow, lineele, lineapw,linereactive;
    QJsonArray linepf, linepl, linereactiveele, curthd;
    for(int i = 0;i < START_LINE_NUM; i++)
    {
        lineVol.append(BoxData->lineTgBox.vol[i]/COM_RATE_VOL);
        linecur.append(QJsonValue::fromVariant((BoxData->lineTgBox.cur[i])/COM_RATE_CUR));
        curthd.append(LoopData->curThd[i]);
        linepow.append(QJsonValue::fromVariant((BoxData->lineTgBox.pow[i])/COM_RATE_POW));
        lineele.append(QJsonValue::fromVariant((BoxData->lineTgBox.ele[i])/COM_RATE_ELE));
        lineapw.append(QJsonValue::fromVariant((BoxData->lineTgBox.apPow[i])/COM_RATE_POW));
        linereactive.append(QJsonValue::fromVariant((BoxData->lineTgBox.reactivePower[i])/COM_RATE_POW));

        if(BoxData->lineTgBox.apPow[i] == 0 )
            linepf.append(QJsonValue::fromVariant(0));
        else
        {
            double pf = (BoxData->lineTgBox.pow[i]*1000.0)/(BoxData->lineTgBox.apPow[i]);
            if( pf > 0.99 ) linepf.append(QJsonValue::fromVariant(pf));
            else linepf.append(QJsonValue::fromVariant(pf));;
        }

        linepl.append(LoopData->pl[i]);
        linereactiveele.append("");
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
    double pf_Total = 0;
    totalObj.insert("pow_active",(BoxData->tgBox.pow)/COM_RATE_POW);
    totalObj.insert("pow_apparent",(BoxData->tgBox.apPow)/COM_RATE_POW);
    totalObj.insert("ele_active",(BoxData->tgBox.ele)/COM_RATE_ELE);
    totalObj.insert("pow_reactive",((BoxData->tgBox.apPow)-(BoxData->tgBox.pow))/COM_RATE_POW);
    totalObj.insert("power_factor",(BoxData->tgBox.pf/COM_RATE_PF));

    totalObj.insert("ele_apparent","");
    totalObj.insert("ele_reactive","");
    subObj.insert("box_total_data" ,totalObj);
//----------------------------输出位数据------------------------------------------
    double pow_active[3] = {0}, pow_apparent[3] = {0},pow_reactive[3] = {0}, ele_active[3] = {0};
    QJsonArray actPow, apPow, reaPow, actEle, apEle, reaEle, pfPow;
    int lineNum = BoxData->loopNum /3; double pf_factor=0;

    if(BoxData->phaseFlag){//三相
        for(int i=0; i< lineNum; ++i)
        {
            for(int j=0; j<3; ++j)
            {
                pow_apparent[i] += ((LoopData->apPow[i*3+j])/COM_RATE_POW);
                pow_active[i] += ((LoopData->pow.value[i*3+j])/COM_RATE_POW);
                pow_reactive[i] += ((LoopData->reactivePower[i*3+j])/COM_RATE_POW);
                ele_active[i] += ((LoopData->ele[i*3+j])/COM_RATE_ELE);
            }

            if(pow_active[i] > 0) {
                pf_factor = (pow_active[i] * 100.0/ pow_apparent[i]) /COM_RATE_PF;
                pfPow.append(pf_factor);
            } else pfPow.append(0);

            if((pfPow.at(i).toDouble()) >0.99) pfPow.at(i) = 0.99;

            apPow.append(pow_apparent[i]);
            actPow.append(pow_active[i]);
            reaPow.append(pow_reactive[i]);
            actEle.append(ele_active[i]);
            apEle.append("");
            reaEle.append("");
        }

    } else {
        for(int i=0; i<3; ++i)//单相三个输出位
        {
            if(LoopData->pow.value[i] > 0) {
                pf_factor = (LoopData->pow.value[i] * 100.0 / LoopData->apPow[i]) /COM_RATE_PF;
                pfPow.append(pf_factor);
            } else pfPow.append(0);

            if((pfPow.at(i).toDouble())>0.99) pfPow.at(i) = 0.99;

            actPow.append((LoopData->pow.value[i])/COM_RATE_POW);
            apPow.append((LoopData->apPow[i])/COM_RATE_POW);
            reaPow.append((LoopData->reactivePower[i])/COM_RATE_POW);
            actEle.append((LoopData->ele[i])/COM_RATE_ELE);
            apEle.append("");
            reaEle.append("");
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
        envValue.append((BoxData->env.tem.value[i])/COM_RATE_TEM);
        envMin.append((BoxData->env.tem.min[i])/COM_RATE_TEM);
        envMax.append((BoxData->env.tem.max[i])/COM_RATE_TEM);
        temArray.append(BoxData->env.tem.upalarm[i]);
    }
    envObj.insert("tem_value",envValue);
    envObj.insert("tem_min",envMin);
    envObj.insert("tem_max",envMax);
    envObj.insert("tem_status",temArray);

    obj.insert("box_data" ,subObj);
    obj.insert("env_item_list" ,envObj);
}

void Json_Pack::getStart_Json(QJsonObject &json , QByteArray &ba, int id)
{
    Startbox(json, id);
    QJsonDocument jsonDoc(json);
    ba = jsonDoc.toJson(QJsonDocument::Compact);
}

void Json_Pack::getInsert_Json(QJsonObject &json , QByteArray &ba, int id, int num)
{
    Insertbox(json, id, num);
    QJsonDocument jsonDoc(json);
    ba = jsonDoc.toJson(QJsonDocument::Compact);
}

bool Json_Pack::delay(int s)
{
    bool ret = true;
    for(int i=0; i<s; ++i) {
        QThread::msleep(1000);
    }

    return ret;
}
