#include "snmpthread.h"
#include <QDebug>

int gReadWriteflag = 1;
SnmpThread::SnmpThread(QObject *parent)
    : QThread{parent}
{

}

SnmpThread::~SnmpThread()
{
    isRun = false;
    wait();
}

bool SnmpThread::init(int id)
{
    bool ret = true;
    mClose = false;
    mOpen = true;
    sDataPacket *shm = get_share_mem(); // 获取共享内存
    mBusData = &(shm->data[id-1]);
    mId = id-1;

    QTimer::singleShot(3*1000,this,SLOT(start()));
    return ret;
}

int SnmpThread::initSnmp(netsnmp_session &session , netsnmp_session **ss)
{
    // 初始化NET-SNMP库
    init_snmp("busbar");

    // 创建SNMP会话
    //netsnmp_session session, *ss;
    //ss = NULL;
    //netsnmp_pdu *response = NULL;
    //netsnmp_pdu *pdu = NULL;
    QString ip = "192.168.1.163";
    QString name = "IDC-BUSBAR";
    QString v3auth = "authkey123";
    QString v3priv = "privatekey1";

    snmp_sess_init(&session);
    session.peername = strdup(ip.toStdString().c_str());
    //    session.version = SNMP_VERSION_2c;
    //    session.community = (u_char*)"public";
    //    session.community_len = strlen("public");
    session.version = SNMP_VERSION_3;
    session.securityName = strdup(name.toStdString().c_str());
    session.securityNameLen = strlen(session.securityName);
    session.securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
    session.securityAuthProto = usmHMACMD5AuthProtocol;
    session.securityAuthProtoLen = USM_AUTH_PROTO_MD5_LEN;
    session.securityAuthKeyLen = USM_AUTH_KU_LEN;
    //    session.contextEngineID = (u_char*)strdup("8000000003020101");
    //    session.contextEngineIDLen = strlen((char*)session.contextEngineID);
    //    session.engineBoots = 0;
    //    session.engineTime = 0;
    //    session.contextName = strdup("contextName");
    //    session.contextNameLen = strlen(session.contextName);
    //    session.securityEngineID = (u_char*)strdup("8000000003020102");
    //    session.securityEngineIDLen = strlen((char*)session.securityEngineID);
    session.retries = 2;
    session.timeout = 500000;

    if(generate_Ku(session.securityAuthProto ,
                    session.securityAuthProtoLen,
                    (u_char *) v3auth.toStdString().c_str(),
                    v3auth.length(),
                    session.securityAuthKey ,
                    &session.securityAuthKeyLen) != SNMPERR_SUCCESS){
        snmp_perror("Error generating Ku from authentication pass phrase");
        return -1;
    }

    session.securityPrivProto = usmDESPrivProtocol;
    session.securityPrivProtoLen = USM_PRIV_PROTO_DES_LEN;
    session.securityPrivKeyLen = USM_PRIV_KU_LEN;

    if(generate_Ku(session.securityAuthProto ,
                    session.securityAuthProtoLen,
                    (u_char *) v3priv.toStdString().c_str(),
                    v3priv.length(),
                    session.securityPrivKey ,
                    &session.securityPrivKeyLen) != SNMPERR_SUCCESS){
        snmp_perror("Error generating Ku from encryption pass phrase");
        return -1;
    }

    SOCK_STARTUP;
    *ss = snmp_open(&session);

    if (*ss == NULL) {
        snmp_sess_perror("busbar", &session);
        return -1;
    }
    return 0;
}



int SnmpThread::walkSnmp(netsnmp_session & session,netsnmp_session **ss,netsnmp_pdu *response,netsnmp_pdu *pdu,int index)
{

    oid anOID[]={1,3,6,1,4,1,30966,12,1}; // OID
    // 创建SNMP请求PDU
    pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
    size_t anOID_len = OID_LENGTH(anOID);

    // 发送SNMP GET请求
    oid end_oid[]={1,3,6,1,4,1,30966,12,2}; // OID
    size_t end_len = OID_LENGTH(end_oid);

    //判断是否插接箱在线
    sBoxData *t= & mBusData->box[0];
    int key = index;
    if(index != 1){
        //if( t->online1 != 0 && index <= 9 ){
        if(  index <= 9 )
        {
            //if( (t->online1 >> (key - 1)) & 0x03 == 1 ){
            anOID[(int)anOID_len - 1] = index;
            end_oid[(int)end_len - 1] = index + 1;
            //}
        }
        //if( t->online2 != 0 && index > 9 && index <= 18 ){
        if( index > 9 && index <= 18 )
        {
            //key -= 9;
            //if( (t->online2 >> (key - 1)) & 0x03 == 1 ){
            anOID[(int)anOID_len - 1] = index;
            end_oid[(int)end_len - 1] = index + 1;
            //}
        }
    }


    int count = 0;
    int status = 0;
    int running = 1;
    bool endflag = false;
    snmp_add_null_var(pdu, anOID, anOID_len);

    // 循环进行SNMP Walk//snmp_synch_response
    while (running && (status = snmp_synch_response(*ss, pdu, &response)) == STAT_SUCCESS) {
        if (response->errstat == SNMP_ERR_NOERROR) {
            // 遍历每个返回的变量绑定
            for (netsnmp_variable_list* vars = response->variables; vars; vars = vars->next_variable) {
                // 打印OID和值
                if (snmp_oid_compare(end_oid, end_len,
                                     vars->name, vars->name_length) <= 0) {
                    running = 0;
                    continue;
                }
                char buf[1024];
                if( vars->name_length >= 1024 ) continue;//length long
                snprint_variable(buf, sizeof(buf), vars->name, vars->name_length, vars);
                //                printf("%s\n", buf);
                QString oidValue = QString(buf);
                if(oidValue.contains("No Such Object available on this agent at this OID")){
                    if(response){
                        snmp_free_pdu(response);
                        response = NULL;
                    }
                    endflag = true;
                    sBoxData *it= & mBusData->box[index-1];
                    //it->offLine = 5;///
                    if( it && it->offLine > 0 ) it->offLine--;
                    qDebug()<<"No Such Object t->offLine   "<<index << (it->offLine-'0')<<endl;
                    break;
                }
                //                msleep(100);
                if(index == 1){
                    praseMasterVal(oidValue);
                }else{
                    praseSlaveVal(oidValue , index);
                }
            }
            // 准备下一个SNMP请求
            //snmp_free_pdu( response);
            if( endflag == true ) break;

            pdu = snmp_pdu_create(SNMP_MSG_GETNEXT);
            snmp_add_null_var(pdu, response->variables->name, response->variables->name_length);
            if(response){
                snmp_free_pdu(response);
                response = NULL;
            }
        } else {
            // 出错处理
            fprintf(stderr, "Error in packet: %s\n", snmp_errstring(response->errstat));
            if(response){
                snmp_free_pdu(response);
                response = NULL;
            }
            sBoxData *it= & mBusData->box[index-1];
            if( it && it->offLine > 0 ) it->offLine--;
            qDebug()<<index << " Error in packet: t->offLine   "<<(it->offLine-'0')<<endl;
            break;
        }

        count++;
    }

    if(status == STAT_TIMEOUT){
        qDebug()<< "snmp request failed:"<<snmp_api_errstring((*ss)->s_snmp_errno)<<endl;
        if(response){
            qDebug()<< "error in snmp response:"<<snmp_api_errstring(response->errstat)<<endl;

            snmp_free_pdu(response);
            response = NULL;
        }
        sBoxData *it= & mBusData->box[index-1];
        if( it && it->offLine > 0 ) t->offLine--;
        qDebug()<<"time out  "<<index << (t->offLine-'0')<<endl;

        if(index == 1) {
            for(int off = 2 ; off <= mBusData->boxNum+1 ; off++){
                it = & mBusData->box[off-1];
                if( it && it->offLine > 0 ) it->offLine--;
                if( it && it->offLine == 0 ){
//                    if(*ss){
//                        snmp_close(*ss);
//                        *ss = NULL;
//                    }
//                    initSnmp(session , ss);
                }

            }
        }
    }

    else if(status == STAT_ERROR){
        if(response){
            snmp_free_pdu(response);
            response = NULL;
        }
        sBoxData *it= & mBusData->box[index-1];
        if( it && it->offLine > 0 ) it->offLine--;
        qDebug()<<"STAT_ERROR  "<<index << (it->offLine-'0')<<endl;
        if(index == 1) {
            for(int off = 2 ; off <= mBusData->boxNum+1 ; off++){
                it = & mBusData->box[off-1];
                if( it && it->offLine > 0 ) it->offLine--;
            }
        }
    }
    return 0;
}

void SnmpThread::startBoxBaseInfo(QString val)
{
    int item = getItemByOid(4);
    sBoxData *t= & mBusData->box[0];
    bool ok;
    switch(item){
    case 1: break;
    case 2: {
        t->offLine = 5;
        t->dc = 1;
        t->proNum = (val.remove("INTEGER:").simplified().toUInt())&0x0f;
    }break;
    case 3: t->curSpecification = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 4: t->version = val.remove("STRING:").replace("\"","").simplified().remove(".").toUInt(&ok);break;
    case 5: t->workMode = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 6: mBusData->boxNum = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        // mBusData->boxNum = 10;break;
    case 7: t->online1 = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 8: t->online2 = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 9: t->buzzerStatus = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 10: t->alarmTime = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 11: break;
    case 12: t->iOF = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 13: t->isd = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 14: t->shuntRelease = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 15: t->lps = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    default: break;
    }
}

void SnmpThread::startBoxEleParaInfo(QString val)
{
    int item = getItemByOid(4);
    sBoxData *t= & mBusData->box[0];
    bool ok;
    switch(item){
    case 1: t->reCur.svalue = (val.remove("STRING:").replace("\"","").simplified().toFloat()*(short)COM_RATE_CUR);break;
    case 2: t->reCur.salarm = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 3: t->zeroLineCur.svalue = (val.remove("STRING:").replace("\"","").simplified().toFloat()*(short)COM_RATE_CUR);break;
    case 4: t->zeroLineCur.salarm = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 5: t->totalApPow = (unsigned long long)(val.remove("STRING:").replace("\"","").simplified().toFloat()*(short)COM_RATE_POW);break;
    case 6: t->totalPow.ivalue = (unsigned long long)(val.remove("STRING:").replace("\"","").simplified().toFloat()*(short)COM_RATE_POW);break;
    case 7: t->totalPow.ialarm = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 8: break;
    case 9: t->rate.svalue = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 10: t->rate.salarm = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 11: t->volUnbalance = (uint)val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 12: t->curUnbalance = (uint)val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 13: t->reCur.smax = (val.remove("STRING:").replace("\"","").simplified().toFloat()*(short)COM_RATE_CUR);break;
    case 14: t->zeroLineCur.smin = (val.remove("STRING:").replace("\"","").simplified().toFloat()*(short)COM_RATE_CUR);break;
    case 15: t->zeroLineCur.smax = (val.remove("STRING:").replace("\"","").simplified().toFloat()*(short)COM_RATE_CUR);break;
    case 16: t->totalPow.imin = (unsigned long long)(val.remove("STRING:").replace("\"","").simplified().toFloat()*(short)COM_RATE_POW);break;
    case 17: t->totalPow.imax = (unsigned long long)(val.remove("STRING:").replace("\"","").simplified().toFloat()*(short)COM_RATE_POW);break;
    case 18: t->rate.smin = (ushort)val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 19: t->rate.smax = (ushort)val.remove("INTEGER:").simplified().toUInt(&ok);break;
    default: break;
    }
}

void SnmpThread::baseMsInformation(QString val)
{
    int item = getItemByOid(3);
    switch(item){
    case 1:break;
    case 2:startBoxBaseInfo(val);break;
    case 3:startBoxEleParaInfo(val);break;
    default:break;
    }
}

void SnmpThread::parseVolHar(QString val, int line)
{
    QString str = val.remove("STRING:").replace("\"","").simplified();
    QStringList strList = str.split("/");
    int len = strList.length();
    sThdData *t= & mBusData->thdData;
    bool ok;
    for(int i = 0 ; i < len ; i++){
        t->volThd[line][i] = strList.at(i).toInt(&ok);
    }
}

void SnmpThread::parseCurHar(QString val, int line)
{
    QString str = val.remove("STRING:").replace("\"","").simplified();
    QStringList strList = str.split("/");
    int len = strList.length();
    sThdData *t= & mBusData->thdData;
    bool ok;
    for(int i = 0 ; i < len ; i++){
        t->curThd[line][i] = strList.at(i).toInt(&ok);
    }
}

void SnmpThread::loopMsInformation(QString val)
{
    int line = 1;
    if( getItemByOid(3) >= 1 && getItemByOid(3) <= 3 )
        line = getItemByOid(3) - 1;
    else return;
    int item = getItemByOid(4);
    sObjData *t= & mBusData->box[0].data;
    bool ok;
    if(t){
        switch(item){
        case 1:  t->lineVol.value[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_VOL);break;
        case 2:  t->lineVol.alarm[line] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 3:  t->vol.value[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_VOL);break;
        case 4:  t->vol.alarm[line] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 5:  t->cur.value[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_CUR);break;
        case 6:  t->cur.alarm[line] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 7:  t->pow.value[line] = (uint)(val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW);break;
        case 8:  t->pow.alarm[line] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 9:  t->reactivePower[line] = (uint)(val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW);break;
        case 10:  t->apPow[line] = (uint)(val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW);break;
        case 11:  t->pf[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_PF);break;
        case 12:  t->ele[line] = (uint)(val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_ELE);break;

        case 13: parseVolHar( val , line );break;
        case 14: parseCurHar( val , line );break;
        case 15:  t->pl[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_PF);break;

        case 16:  t->lineVol.min[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_VOL);break;
        case 17:  t->lineVol.max[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_VOL);break;
        case 18:  t->vol.min[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_VOL);break;
        case 19:  t->vol.max[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_VOL);break;
        case 20:  t->cur.min[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_CUR);break;
        case 21:  t->cur.max[line] = (val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_CUR);break;
        case 22:  t->pow.min[line] = (uint)(val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW);break;
        case 23:  t->pow.max[line] = (uint)(val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW);break;
        default: break;
        }
    }
}


void SnmpThread::temMsInformation(QString val)
{
    int item = getItemByOid(3);
    sDataUnit *t= & mBusData->box[0].env.tem;
    bool ok;
    if(t){
        switch(item){
        case 1:  t->value[0] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 2:  t->alarm[0] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 3:  t->value[1] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 4:  t->alarm[1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 5:  t->value[2] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 6:  t->alarm[2] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 7:  t->value[3] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 8:  t->alarm[3] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 9:  t->min[0] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 10:  t->max[0] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 11:  t->min[1] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 12:  t->max[1] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 13:  t->min[2] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 14:  t->max[2] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 15:  t->min[3] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        case 16:  t->max[3] = val.remove("STRING:").replace("\"","").simplified().toUInt(&ok);break;
        default: break;
        }
    }
}

void SnmpThread::praseMasterVal(QString str)
{
    if( str.contains("=") ){
        QStringList oidStrValue = str.remove( MIB_OID_HEAD ).simplified().split( "=" );
        mOid = oidStrValue.at(0);
        QString value = oidStrValue.at(1);
        int item = getItemByOid(1);
        if( item != 1 ) return;
        item = getItemByOid(2);
        switch(item){
        case 1: baseMsInformation(value); break;
        case 2: loopMsInformation(value); break;
        case 3: temMsInformation(value); break;
        default: break;
        }
    }
}

void SnmpThread::baseSlaveInformation(QString val , int addr)
{
    int item = getItemByOid(3);
    sBoxData *t= & mBusData->box[addr-1];
    bool ok;
    switch(item){
    case 1: {
        t->version = val.remove("STRING:").replace("\"","").simplified().remove(".").toUInt(&ok);
        t->offLine = 10;
        t->dc = 1;
    }
    break;
    case 2: t->buzzerStatus = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    case 3: t->data.curThd[0] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 4: t->data.curThd[1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 5: t->data.curThd[2] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 6: t->data.pl[0] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 7: t->data.pl[1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 8: t->data.pl[2] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 9: t->data.lineNum = t->loopNum = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 10: t->alarmTime = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    default: break;
    }
}

void SnmpThread::loopSlaveInformation(QString val , int addr)
{
    int item = getItemByOid(4);
    sBoxData *t= & mBusData->box[addr-1];
    int loop = getItemByOid(3);
    if( loop <= mBusData->box[addr-1].loopNum ){
        bool ok;
        switch(item){
        case 1: t->data.vol.value[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_VOL;break;
        case 2: t->data.vol.alarm[loop-1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

        case 3: t->data.cur.value[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_CUR;break;
        case 4: t->data.cur.alarm[loop-1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

        case 5: t->data.pow.value[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW;break;
        case 6: t->data.pow.alarm[loop-1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

        case 7: t->data.reactivePower[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW;break;
        case 8: t->data.apPow[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW;break;
        case 9: t->data.pf[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_PF;break;
        case 10: t->data.ele[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_ELE;break;

        case 11: t->data.sw[loop-1] = val.remove("INTEGER:").replace("\"","").simplified().toUInt(&ok)==1?1:0;break;
        case 12: t->data.vol.min[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_VOL;break;
        case 13: t->data.vol.max[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_VOL;break;
        case 14: t->data.cur.min[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_CUR;break;
        case 15: t->data.cur.max[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_CUR;break;
        case 16: t->data.pow.min[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW;break;
        case 17: t->data.pow.max[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_POW;break;
        default: break;
        }
    }
}

void SnmpThread::temSlaveInformation(QString val , int addr)
{
    int item = getItemByOid(3);
    sBoxData *t= & mBusData->box[addr-1];

    bool ok;
    switch(item){
    case 1: t->env.tem.value[0] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;
    case 2: t->env.tem.alarm[0] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    case 3: t->env.tem.value[1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;
    case 4: t->env.tem.alarm[1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    case 5: t->env.tem.value[2] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;
    case 6: t->env.tem.alarm[2] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    case 7: t->env.tem.value[3] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;
    case 8: t->env.tem.alarm[3] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    case 9: t->env.tem.min[0] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;
    case 10: t->env.tem.max[0] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;

    case 11: t->env.tem.min[1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;
    case 12: t->env.tem.max[1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;

    case 13: t->env.tem.min[2] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;
    case 14: t->env.tem.max[2] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;

    case 15: t->env.tem.min[3] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;
    case 16: t->env.tem.max[3] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*(short)COM_RATE_TEM;break;

    default: break;
    }
}

void SnmpThread::praseSlaveVal(QString str , int index)
{
    if( str.contains("=") ){
        QStringList oidStrValue = str.remove( MIB_OID_HEAD ).simplified().split( "=" );
        mOid = oidStrValue.at(0);
        QString value = oidStrValue.at(1);
        int item = getItemByOid(1);
        if( item == 1 ) return;
        int addr = item;
        if( addr == index ){
            item = getItemByOid(2);
            switch(item){
            case 1: baseSlaveInformation( value , addr ); break;
            case 2: loopSlaveInformation( value , addr ); break;
            case 3: temSlaveInformation( value , addr ); break;
            default: break;
            }
        }
    }
}

void SnmpThread::releaseCon(netsnmp_session &session,
                            netsnmp_session **ss,
                            netsnmp_pdu *response)
{
    // 处理SNMP Walk结束的情况
    //    if (status != STAT_TARGET_STATS_END) {
    //        snmp_sess_perror("busbar", ss);
    //    }

    // 释放SNMP会话和响应
    if (response) {
        snmp_free_pdu(response);
        response = NULL;
    }
    if(*ss){
        snmp_close(*ss);
        *ss = NULL;
    }
    //SNMP_FREE(session.community);
    SNMP_FREE(session.peername);
    SOCK_CLEANUP;
}

int SnmpThread::getItemByOid(int id)
{
    QStringList list = mOid.split(".");
    return list.at(id).toInt();
}

bool SnmpThread::setOid(netsnmp_session **ss,netsnmp_pdu *response,netsnmp_pdu *pdu)
{
    // 创建SNMP SET请求
    bool ret = false;
    if(mItems.size()){
        sThresholdItem item = mItems.first();
        //        qDebug() << " item bus "<<item.bus
        //                 << " item box "<<item.box
        //                 << " item num "<<item.num
        //                 << " item type "<<item.type
        //                 << " item min "<<item.min
        //                 << " item max "<<item.max;

        oid target_min[13],target_max[13];
        char type = 's';
        char min_value[20];  // 要设置的最小值
        char max_value[20];  // 要设置的最大值
        if( item.box == 0 ){//master
            switch(item.type){
            case 1: {
                getMasterVolOid( item , target_min , target_max );
                //for(int i = 0 ; i < OID_LENGTH(target_o) ; i++) qDebug()<<target_min[i];
                //                QString str1 = QString::number(item.min/10.0, 'f' , 1);
                //                QString str2 = QString::number(item.max/10.0, 'f' , 1);
                QString str1 = QString::number(item.min);
                QString str2 = QString::number(item.max);
                qstrncpy(min_value , str1.toLatin1().data() , str1.size()+1);
                qstrncpy(max_value , str2.toLatin1().data() , str2.size()+1);
                break;
            }
            case 2:{
                getMasterCurOid( item , target_min , target_max);
                //for(int i = 0 ; i < OID_LENGTH(target_o) ; i++) qDebug()<<target_min[i];
                //                QString str1 = QString::number(item.min/100.0, 'f' , 2);
                //                QString str2 = QString::number(item.max/100.0, 'f' , 2);
                QString str1 = QString::number(item.min);
                QString str2 = QString::number(item.max);
                qstrncpy(min_value , str1.toLatin1().data() , str1.size()+1);
                qstrncpy(max_value , str2.toLatin1().data() , str2.size()+1);
                break;
            }
            case 3:{
                getMasterTemperatureOid( item , target_min , target_max );
                QString str1 = QString::number(item.min);
                QString str2 = QString::number(item.max);
                qstrncpy(min_value , str1.toLatin1().data() , str1.size()+1);
                qstrncpy(max_value , str2.toLatin1().data() , str2.size()+1);
                //for(int i = 0 ; i < OID_LENGTH(target_o) ; i++) qDebug()<<target_min[i];
                break;
            }
            case 4:{
                getMasterActivePowerOid( item , target_min , target_max );
                //                QString str1 = QString::number(item.min/1000.0, 'f' , 3);
                //                QString str2 = QString::number(item.max/1000.0, 'f' , 3);
                QString str1 = QString::number(item.min);
                QString str2 = QString::number(item.max);
                qstrncpy(min_value , str1.toLatin1().data() , str1.size()+1);
                qstrncpy(max_value , str2.toLatin1().data() , str2.size()+1);
                //for(int i = 0 ; i < OID_LENGTH(target_o) ; i++) qDebug()<<target_min[i];
                break;
            }
            case 5:{
                getMasterFrequencyOid( target_min , target_max );type = 'i';
                //for(int i = 0 ; i < OID_LENGTH(target_o) ; i++) qDebug()<<target_min[i];
                QString str1 = QString::number(item.min);
                QString str2 = QString::number(item.max);
                qstrncpy(min_value , str1.toLatin1().data() , str1.size()+1);
                qstrncpy(max_value , str2.toLatin1().data() , str2.size()+1);
                break;
            }
            }
        }else{
            switch(item.type){
            case 1: {
                getSlaveVolOid( item , target_min , target_max );
                //                QString str1 = QString::number(item.min/10.0, 'f' , 1);
                //                QString str2 = QString::number(item.max/10.0, 'f' , 1);
                QString str1 = QString::number(item.min);
                QString str2 = QString::number(item.max);
                qstrncpy(min_value , str1.toLatin1().data() , str1.size()+1);
                qstrncpy(max_value , str2.toLatin1().data() , str2.size()+1);

                //for(int i = 0 ; i < OID_LENGTH(target_o) ; i++) qDebug()<<target_min[i];
                break;
            }
            case 2: {
                getSlaveCurOid( item , target_min , target_max);
                //                QString str1 = QString::number(item.min/100.0, 'f' , 1);
                //                QString str2 = QString::number(item.max/100.0, 'f' , 1);
                QString str1 = QString::number(item.min);
                QString str2 = QString::number(item.max);
                qstrncpy(min_value , str1.toLatin1().data() , str1.size()+1);
                qstrncpy(max_value , str2.toLatin1().data() , str2.size()+1);
                //for(int i = 0 ; i < OID_LENGTH(target_o) ; i++) qDebug()<<target_min[i];
                break;
            }
            case 3:{ getSlaveTemperatureOid( item , target_min , target_max );
                QString str1 = QString::number(item.min);
                QString str2 = QString::number(item.max);
                qstrncpy(min_value , str1.toLatin1().data() , str1.size()+1);
                qstrncpy(max_value , str2.toLatin1().data() , str2.size()+1);
                //for(int i = 0 ; i < OID_LENGTH(target_o) ; i++) qDebug()<<target_min[i];
                break;
            }
            case 4:{ getSlaveActivePowerOid( item , target_min , target_max );
                //                QString str1 = QString::number(item.min/1000.0, 'f' , 3);
                //                QString str2 = QString::number(item.max/1000.0, 'f' , 3);
                QString str1 = QString::number(item.min);
                QString str2 = QString::number(item.max);
                qstrncpy(min_value , str1.toLatin1().data() , str1.size()+1);
                qstrncpy(max_value , str2.toLatin1().data() , str2.size()+1);
                //for(int i = 0 ; i < OID_LENGTH(target_o) ; i++) qDebug()<<target_min[i];
                break;
            }
            }
        }
        pdu = snmp_pdu_create(SNMP_MSG_SET);
        //oid target_oid1[] = {1,3,6,1,4,1,30966,12,1,1,3,18,0};  // 要设置的OID
        size_t target_min_len = OID_LENGTH(target_min);

        snmp_add_var(pdu,target_min,target_min_len,type,min_value);

        //oid target_oid2[] = {1,3,6,1,4,1,30966,12,1,3,16,0};  // 要设置的OID
        size_t target_max_len = OID_LENGTH(target_max);

        snmp_add_var(pdu,target_max,target_max_len,type,max_value);


        // 发送SNMP SET请求
        int status = snmp_synch_response(*ss, pdu, &response);

        if( status == STAT_SUCCESS ) printf("SNMP SET STAT_SUCCESS.\n");
        else printf("SNMP SET failed.\n");
        // 处理SNMP响应
        if (response) {
            if (response->errstat == SNMP_ERR_NOERROR) {
                printf("SNMP SET successful.\n");
                ret = true;
                for(netsnmp_variable_list *var = response->variables ; var ;var = var->next_variable){
                    print_variable(var->name , var->name_length , var);
                }
            } else {
                fprintf(stderr, "Error in SNMP response: %s\n", snmp_errstring(response->errstat));
            }
            snmp_free_pdu(response);
        } else {
            fprintf(stderr, "No SNMP response received.\n");
        }
        mItems.removeFirst();
    }
    return ret;
}


void SnmpThread::run()
{
    isRun = true;
    netsnmp_session session, *ss;
    ss = NULL;
    netsnmp_pdu *response = NULL;
    netsnmp_pdu *pdu = NULL;

    if(gVerflag == 3){
        initSnmp(session , &ss);
        while(isRun)
        {
            for(int index = 1 ; index <= mBusData->boxNum+1 ; ){
                if(gReadWriteflag == 1){
                    walkSnmp(session, &ss , response , pdu , index);
                    index++;
                    //if(mClose) break;
                }else{
                    setOid(&ss , response , pdu);
                    gReadWriteflag = 1;
                }
            }
            //if(mClose) isRun = false;

        }
        releaseCon(session , &ss , response);
    }

}

void SnmpThread::recvSendSetSlot(sThresholdItem *item)
{
    mItems.append(*item);
}

void SnmpThread::getMasterVolOid(sThresholdItem &item, oid *target_min , oid *target_max )
{
    QString oidstr = "1.3.6.1.4.1.30966.12.1.2.1.18.0";
    QStringList list = oidstr.split(".");
    for(int i = 0 ; i < list.size() ; i++){
        if(i == list.size() - 3){
            target_min[i] = item.num + 1;
            target_max[i] = item.num + 1;
        }
        else if(i == list.size() - 2){
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt() + 1;
        }
        else{
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt();
        }
    }
}

void SnmpThread::getMasterCurOid(sThresholdItem &item , oid *target_min, oid *target_max)
{
    QString oidstr = "1.3.6.1.4.1.30966.12.1.2.1.20.0";
    QStringList list = oidstr.split(".");
    for(int i = 0 ; i < list.size() ; i++){
        if(i == list.size() - 3){
            target_min[i] = item.num + 1;
            target_max[i] = item.num + 1;
        }
        else if(i == list.size() - 2){
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt() + 1;
        }
        else{
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt();
        }
    }
}

void SnmpThread::getMasterActivePowerOid(sThresholdItem &item , oid* target_min, oid *target_max)
{
    QString oidstr = "1.3.6.1.4.1.30966.12.1.2.1.22.0";
    QStringList list = oidstr.split(".");
    for(int i = 0 ; i < list.size() ; i++){
        if(i == list.size() - 3){
            target_min[i] = item.num + 1;
            target_max[i] = item.num + 1;
        }
        else if(i == list.size() - 2){
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt() + 1;
        }
        else{
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt();
        }
    }
}

void SnmpThread::getMasterFrequencyOid(oid *target_min, oid *target_max)
{
    QString oidstr = "1.3.6.1.4.1.30966.12.1.1.3.18.0";
    QStringList list = oidstr.split(".");
    for(int i = 0 ; i < list.size() ; i++){
        if(i == list.size() - 2){
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt() + 1;
        }
        else{
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt();
        }
    }
}

void SnmpThread::getMasterTemperatureOid(sThresholdItem &item , oid* target_min, oid *target_max)
{
    QString oidstr = "1.3.6.1.4.1.30966.12.1.3.9.0";
    QStringList list = oidstr.split(".");
    for(int i = 0 ; i < list.size() ; i++){
        if(i == list.size() - 2){
            target_min[i] = item.num * 2 + 9;
            target_max[i] = item.num * 2 + 10;
        }
        else {
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt();
        }
    }
}

void SnmpThread::getSlaveVolOid(sThresholdItem &item , oid* target_min, oid *target_max)
{
    QString oidstr = "1.3.6.1.4.1.30966.12.2.2.1.12.0";
    QStringList list = oidstr.split(".");
    int box = item.box + 1;
    int loop = item.num + 1;
    for(int i = 0 ; i < list.size() ; i++){
        if(i == list.size() - 5) {
            target_min[i] = box;
            target_max[i] = box;
        }else if(i == list.size() - 3){
            target_min[i] = loop;
            target_max[i] = loop;
        }else if(i == list.size() - 2){
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt() + 1;
        }
        else{
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt();
        }
    }
}

void SnmpThread::getSlaveCurOid(sThresholdItem &item , oid* target_min, oid *target_max)
{
    QString oidstr = "1.3.6.1.4.1.30966.12.2.2.1.14.0";
    QStringList list = oidstr.split(".");
    int box = item.box + 1;
    int loop = item.num + 1;
    for(int i = 0 ; i < list.size() ; i++){
        if(i == list.size() - 5) {
            target_min[i] = box;
            target_max[i] = box;
        }else if(i == list.size() - 3){
            target_min[i] = loop;
            target_max[i] = loop;
        }else if(i == list.size() - 2){
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt() + 1;
        }
        else{
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt();
        }
    }
}

void SnmpThread::getSlaveActivePowerOid(sThresholdItem &item , oid* target_min, oid *target_max)
{
    QString oidstr = "1.3.6.1.4.1.30966.12.2.2.1.16.0";
    QStringList list = oidstr.split(".");
    int box = item.box + 1;
    int loop = item.num + 1;
    for(int i = 0 ; i < list.size() ; i++){
        if(i == list.size() - 5) {
            target_min[i] = box;
            target_max[i] = box;
        }else if(i == list.size() - 3){
            target_min[i] = loop;
            target_max[i] = loop;
        }else if(i == list.size() - 2){
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt() + 1;
        }
        else{
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt();
        }
    }
}

void SnmpThread::getSlaveTemperatureOid(sThresholdItem &item , oid* target_min, oid *target_max)
{
    QString oidstr = "1.3.6.1.4.1.30966.12.2.3.9.0";
    QStringList list = oidstr.split(".");
    int box = item.box + 1;
    for(int i = 0 ; i < list.size() ; i++){
        if(i == list.size() - 4) {
            target_min[i] = box;
            target_max[i] = box;
        }else if(i == list.size() - 2){
            target_min[i] = item.num * 2 + 9;
            target_max[i] = item.num * 2 + 10;
        }
        else{
            target_min[i] = list.at(i).toInt();
            target_max[i] = list.at(i).toInt();
        }
    }
}
