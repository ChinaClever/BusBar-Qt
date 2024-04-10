/*
 *
 *
 *  Created on: 2018年10月1日
 *      Author: Lzy
 */
#include "json_pack.h"


Json_Pack::Json_Pack(QObject *parent)
{
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
    startPro->datetime = t.toString("yyyy-MM-dd HH:mm:ss");
    obj.insert("addr", startPro->addr);
    obj.insert("datetime", startPro->datetime);
    obj.insert("bus_start", id);
    obj.insert("name", mBoxData[id]->boxName);
    Startbox_pduInfo(obj,id);
}

void Json_Pack::Startbox_pduInfo(QJsonObject &obj, int id)
{
    Startbox_ConfigData(obj,id);
    Startbox_StateData(obj,id);
    Startbox_Data(obj,id);
}

void Json_Pack::Startbox_ConfigData(QJsonObject &obj ,int id)
{
    QJsonArray jsonArray;

    QJsonObject subObj;
    subObj.insert("version",mBoxData[id]->version);
    subObj.insert("current_type", mBoxData[id]->curSpecification);
    subObj.insert("baud_rate",mBoxData[id]->baudRate);
    subObj.insert("beep",mBoxData[id]->buzzerStatus);
    subObj.insert("workmode",mBoxData[id]->workMode);
    subObj.insert("ac_dc",mBoxData[id]->dc);
    subObj.insert("pro_id",mBoxData[id]->proNum);
    subObj.insert("insert_num",mBusData[id]->boxNum);
    subObj.insert("alarm_time",mBoxData[id]->alarmTime);
    subObj.insert("iof",mBoxData[id]->iOF);
    subObj.insert("isd",mBoxData[id]->isd);
    subObj.insert("shunt_release",mBoxData[id]->shuntRelease);
    subObj.insert("sw_state",mBoxData[id]->data.sw[0]);

    jsonArray.append(subObj);
    obj.insert("startbox_config" ,QJsonValue(jsonArray));
}
void Json_Pack::Startbox_StateData(QJsonObject &obj ,int id)
{
    QJsonArray jsonArray;

    QJsonObject subObj;
    subObj.insert("lsp_state",mBoxData[id]->lpsAlarm);
    subObj.insert("hz_state", mBoxData[id]->rate.supalarm);
    subObj.insert("zeroline_state",mBoxData[id]->zeroLineCur.iupalarm);
    subObj.insert("recur_state",mBoxData[id]->reCur.supalarm);
    QJsonArray temArray;
    for(int i = 0;i < 4; i++)
        temArray.append(mBoxData[id]->env.tem.upalarm[i]);

    QJsonArray volArray, curArray, linevolArray;
    QJsonArray powArray,totalpowArray;
    for(int i = 0;i < 3; i++)
    {
        volArray.append(mBoxData[id]->data.vol.upalarm[i]);
        curArray.append(mBoxData[id]->data.cur.upalarm[i]);
        linevolArray.append(mBoxData[id]->data.lineVol.upalarm[i]);
        powArray.append(mBoxData[id]->data.pow.upalarm[i]);
        totalpowArray.append(mBoxData[id]->data.totalPow.upalarm[i]);
    }
    subObj.insert("tem_state",temArray);
    subObj.insert("vol_state",volArray);
    subObj.insert("cur_state",curArray);
    subObj.insert("linevol_state",linevolArray);
    subObj.insert("pow_state",powArray);
    subObj.insert("totalpow_state",totalpowArray);

    jsonArray.append(subObj);
    obj.insert("startbox_state" ,QJsonValue(jsonArray));
}

void Json_Pack::Startbox_Data(QJsonObject &obj ,int id)
{
    QJsonArray jsonArray; QJsonObject subObj;

    QJsonArray reactivepowArray, plArray;
    QJsonArray pfArray, elepowArray;
    for(int i = 0;i < 3; i++)
    {
        plArray.append(mBoxData[id]->data.pl[i]);
    }
    subObj.insert("pl_data",plArray);

    for(int i = 0;i < LINE_NUM_MAX; i++)
    {
        reactivepowArray.append(QString(mBoxData[id]->data.reactivePower[i]));
        pfArray.append(mBoxData[id]->data.pf[i]);
        elepowArray.append(QString(mBoxData[id]->data.ele[i]));
    }
    subObj.insert("reactivepow_data",reactivepowArray);

    subObj.insert("totalpow_data",mBoxData[id]->zeroLineCur.iupalarm);
    subObj.insert("totalappow",mBoxData[id]->reCur.supalarm);

    subObj.insert("pf_data",pfArray);
    subObj.insert("ele_data",elepowArray);

    subObj.insert("recur",mBoxData[id]->reCur.svalue);
    subObj.insert("zeroline",QString::number(mBoxData[id]->zeroLineCur.ivalue));
    subObj.insert("hz",mBoxData[id]->rate.svalue);
    subObj.insert("volunbalance",QString(mBoxData[id]->volUnbalance));
    subObj.insert("curunbalance",QString(mBoxData[id]->curUnbalance));

    QJsonArray Power, PowerMValue, PowerMin, PowerMax; QJsonObject powObj;
    for(int i = 0;i < LINE_NUM_MAX; i++)
    {
        PowerMValue.append(QString(mBoxData[id]->data.pow.value[i]));
        PowerMin.append(QString(mBoxData[id]->data.pow.min[i]));
        PowerMax.append(QString(mBoxData[id]->data.pow.max[i]));
    }
    powObj.insert("pow_value",Power);
    powObj.insert("pow_min",PowerMin);
    powObj.insert("pow_max",PowerMax);

    Power.append(powObj);
    subObj.insert("pow_data" ,QJsonValue(Power));

    QJsonArray Vol, volValue, volMax, volMin;
    QJsonObject volObj;
    for(int i = 0;i < LINE_NUM_MAX; i++)
    {
        volValue.append(mBoxData[id]->data.vol.value[i]);
        volMin.append(mBoxData[id]->data.vol.min[i]);
        volMax.append(mBoxData[id]->data.vol.max[i]);
    }
    volObj.insert("vol_value",volValue);
    volObj.insert("vol_min",volMin);
    volObj.insert("vol_max",volMax);

    Vol.append(volObj);
    subObj.insert("vol_data" ,QJsonValue(Vol));

    QJsonArray Cur, curValue, curMin, curMax;
    QJsonObject curObj;
    for(int i = 0;i < LINE_NUM_MAX; i++)
    {
        curValue.append(QString(mBoxData[id]->data.cur.value[i]));
        curMin.append(QString(mBoxData[id]->data.cur.min[i]));
        curMax.append(QString(mBoxData[id]->data.cur.max[i]));
    }
    curObj.insert("cur_value",curValue);
    curObj.insert("cur_min",curMin);
    curObj.insert("cur_max",curMax);

    Cur.append(curObj);
    subObj.insert("cur_data" ,QJsonValue(Cur));

    QJsonArray Linevol, linevolValue, linevolMin, linevolMax;
    QJsonObject linevolObj;
    for(int i = 0;i < LINE_NUM_MAX; i++)
    {
        linevolValue.append(mBoxData[id]->data.lineVol.value[i]);
        linevolMin.append(mBoxData[id]->data.lineVol.min[i]);
        linevolMax.append(mBoxData[id]->data.lineVol.max[i]);
    }
    linevolObj.insert("linevol_value",linevolValue);
    linevolObj.insert("linevol_min",linevolMin);
    linevolObj.insert("linevol_max",linevolMax);

    Linevol.append(linevolObj);
    subObj.insert("linevol_data" ,QJsonValue(Linevol));

    QJsonArray Thd, thdCur, thdVol; QJsonObject thdObj;
    for(int i = 0;i < START_LINE_NUM; i++)
    {
        for(int j = 0;j < HARMONIC_NUM; j++)
        {
            thdCur.append(mBusData[id]->thdData.curThd[i][j]);
            thdVol.append(mBusData[id]->thdData.volThd[i][j]);
        }
    }
    thdObj.insert("cur_thd",thdCur);
    thdObj.insert("vol_thd",thdVol);

    Thd.append(thdObj);
    subObj.insert("thd_data" ,QJsonValue(Thd));

    QJsonArray Env, envValue, envMin, envMax; QJsonObject envObj;
    for(int i = 0;i < SENSOR_NUM; i++)
    {
        envValue.append(mBoxData[id]->env.tem.value[i]);
        envMin.append(mBoxData[id]->env.tem.min[i]);
        envMax.append(mBoxData[id]->env.tem.max[i]);
    }
    envObj.insert("tem_value",envValue);
    envObj.insert("tem_min",envMin);
    envObj.insert("tem_max",envMax);

    Env.append(envObj);
    subObj.insert("env_data" ,QJsonValue(Env));

    jsonArray.append(subObj);
    obj.insert("start_data" ,QJsonValue(jsonArray));
}

//----------------------------插接箱----------------------------
void Json_Pack::Insertbox(QJsonObject &obj,int bus_id,int insert_id)
{
    QDateTime t = QDateTime::currentDateTime();
    insertPro->datetime = t.toString("yyyy-MM-dd HH:mm:ss");
    obj.insert("addr", insertPro->addr);
    obj.insert("datetime", insertPro->datetime);

    QJsonArray numArray;
    numArray.append(bus_id);
    numArray.append(insert_id);
    obj.insert("bus_insert", numArray);

    obj.insert("name", mBusData[bus_id]->box[insert_id].boxName);
    Insertbox_pduInfo(obj,bus_id,insert_id);
}

void Json_Pack::Insertbox_pduInfo(QJsonObject &obj, int bus_id,int insert_id)
{
    Insertbox_ConfigData(obj,bus_id,insert_id);
    Insertbox_StateData(obj,bus_id,insert_id);
    Insertbox_Data(obj,bus_id,insert_id);
}

void Json_Pack::Insertbox_ConfigData(QJsonObject &obj ,int bus_id,int insert_id)
{
    QJsonArray jsonArray;

    QJsonObject subObj;
    subObj.insert("version",mBusData[bus_id]->box[insert_id].version);
    subObj.insert("baud_rate",mBusData[bus_id]->box[insert_id].baudRate);
    subObj.insert("beep",mBusData[bus_id]->box[insert_id].buzzerStatus);
    subObj.insert("workmode",mBusData[bus_id]->box[insert_id].workMode);
    subObj.insert("pro_id",mBusData[bus_id]->box[insert_id].proNum);
    subObj.insert("loop_num",mBusData[bus_id]->box[insert_id].loopNum);
    subObj.insert("alarm_time",mBusData[bus_id]->box[insert_id].alarmTime);
    subObj.insert("iof",mBusData[bus_id]->box[insert_id].iOF);
    subObj.insert("boxtype",mBusData[bus_id]->box[insert_id].boxType);



    jsonArray.append(subObj);
    obj.insert("insert_config" ,QJsonValue(jsonArray));
}

void Json_Pack::Insertbox_StateData(QJsonObject &obj,int bus_id,int insert_id)
{
    QJsonArray jsonArray; QJsonObject subObj;
    QJsonArray swArray, volArray, curArray, powArray;

    for(int i =0; i < LINE_NUM_MAX;i++)
    {
        swArray.append(mBusData[bus_id]->box[insert_id].data.sw[i]);
        volArray.append(mBusData[bus_id]->box[insert_id].data.vol.upalarm[i]);
        curArray.append(mBusData[bus_id]->box[insert_id].data.cur.upalarm[i]);
        powArray.append(mBusData[bus_id]->box[insert_id].data.pow.upalarm[i]);
    }
    subObj.insert("sw_state",swArray);
    subObj.insert("vol_state",volArray);
    subObj.insert("cul_state",curArray);
    subObj.insert("pow_state",powArray);

    QJsonArray temArray;
    for(int i = 0;i < SENSOR_NUM; i++)
        temArray.append(mBusData[bus_id]->box[insert_id].env.tem.upalarm[i]);
    subObj.insert("tem_state",temArray);

    jsonArray.append(subObj);
    obj.insert("startbox_state" ,QJsonValue(jsonArray));
}

void Json_Pack::Insertbox_Data(QJsonObject &obj ,int bus_id, int insert_id)
{
    QJsonArray jsonArray; QJsonObject subObj;
//-------------------------------回路数据-------------------
    QJsonArray loopvolArray, loopcurArray, looppowArray;
    QJsonArray loopvolMin, loopcurMin, looppowMin;
    QJsonArray loopvolMax, loopcurMax, looppowMax;
    QJsonArray loopvol, loopcur, looppow;
    QJsonObject loopvolObj, loopcurObj, looppowObj;
    for(int i = 0;i < LINE_NUM_MAX; i++)
    {
        loopvolArray.append(mBusData[bus_id]->box[insert_id].data.vol.value[i]);
        loopvolMin.append(mBusData[bus_id]->box[insert_id].data.vol.min[i]);
        loopvolMax.append(mBusData[bus_id]->box[insert_id].data.vol.max[i]);

        loopcurArray.append(QString(mBusData[bus_id]->box[insert_id].data.cur.value[i]));
        loopcurMin.append(QString(mBusData[bus_id]->box[insert_id].data.cur.min[i]));
        loopcurMax.append(QString(mBusData[bus_id]->box[insert_id].data.cur.max[i]));

        looppowArray.append(QString(mBusData[bus_id]->box[insert_id].data.pow.value[i]));
        looppowMin.append(QString(mBusData[bus_id]->box[insert_id].data.pow.min[i]));
        looppowMax.append(QString(mBusData[bus_id]->box[insert_id].data.pow.max[i]));
    }
    loopvolObj.insert("vol_value",loopvolArray);
    loopvolObj.insert("vol_min",loopvolMin);
    loopvolObj.insert("vol_max",loopvolMax);
    loopvol.append(loopvolObj);
    subObj.insert("vol_loop" ,QJsonValue(loopvol));

    loopcurObj.insert("cur_value",loopcurArray);
    loopcurObj.insert("cur_min",loopcurMin);
    loopcurObj.insert("cur_max",loopcurMax);
    loopcur.append(loopcurObj);
    subObj.insert("cur_loop" ,QJsonValue(loopcur));

    looppowObj.insert("pow_value",looppowArray);
    looppowObj.insert("pow_min",looppowMin);
    looppowObj.insert("pow_max",looppowMax);
    looppow.append(looppowObj);
    subObj.insert("pow_loop" ,QJsonValue(looppow));

    QJsonArray reactivepowValue, appowValue, pfValue, eleValue; QJsonObject loopObj;
    for(int i = 0;i < LINE_NUM_MAX; i++)
    {
        reactivepowValue.append(QString(mBusData[bus_id]->box[insert_id].data.reactivePower[i]));
        appowValue.append(QString(mBusData[bus_id]->box[insert_id].data.apPow[i]));
        pfValue.append(QString(mBusData[bus_id]->box[insert_id].data.pf[i]));
        eleValue.append(QString(mBusData[bus_id]->box[insert_id].data.ele[i]));
    }
    subObj.insert("reactivepow_loop" ,reactivepowValue);
    subObj.insert("appow_loop" ,appowValue);
    subObj.insert("pf_loop" ,pfValue);
    subObj.insert("ele_loop" ,eleValue);
//----------------------------相数据-------------------------------
    QJsonArray lineVol, linecur, linepow, lineele, lineapw,linereactive, linepf;
    QJsonObject lineObj; QJsonArray lineArray;
    for(int i = 0;i < START_LINE_NUM; i++)
    {
        lineVol.append(mBusData[bus_id]->box[insert_id].lineTgBox.vol[i]);
        linecur.append(QString(mBusData[bus_id]->box[insert_id].lineTgBox.cur[i]));
        linepow.append(QString(mBusData[bus_id]->box[insert_id].lineTgBox.pow[i]));
        lineele.append(QString(mBusData[bus_id]->box[insert_id].lineTgBox.ele[i]));
        lineapw.append(QString(mBusData[bus_id]->box[insert_id].lineTgBox.apPow[i]));
        linereactive.append(QString(mBusData[bus_id]->box[insert_id].lineTgBox.reactivePower[i]));
        linepf.append(mBusData[bus_id]->box[insert_id].lineTgBox.pf[i]);
    }
    lineObj.insert("line_vol",lineVol);
    lineObj.insert("line_cur",linecur);
    lineObj.insert("line_pow",linepow);
    lineObj.insert("line_ele",lineele);
    lineObj.insert("line_appow",lineapw);
    lineObj.insert("line_reactivepower",linereactive);
    lineObj.insert("line_pf",linepf);

    lineArray.append(lineObj);
    subObj.insert("line_data" ,QJsonValue(lineArray));

    QJsonArray Env, envValue, envMin, envMax; QJsonObject envObj;
    for(int i = 0;i < SENSOR_NUM; i++)
    {
        envValue.append(mBusData[bus_id]->box[insert_id].env.tem.value[i]);
        envMin.append(mBusData[bus_id]->box[insert_id].env.tem.min[i]);
        envMax.append(mBusData[bus_id]->box[insert_id].env.tem.max[i]);
    }
    envObj.insert("tem_value",envValue);
    envObj.insert("tem_min",envMin);
    envObj.insert("tem_max",envMax);

    Env.append(envObj);
    subObj.insert("env_data" ,QJsonValue(Env));

    jsonArray.append(subObj);
    obj.insert("insert_data" ,QJsonValue(jsonArray));
}

void Json_Pack::getJson(QJsonObject &json , QByteArray &ba, int id, int num)
{

    Startbox(json, id);
    Insertbox(json, id, num);
    QJsonDocument jsonDoc(json);
    ba = jsonDoc.toJson();
}
