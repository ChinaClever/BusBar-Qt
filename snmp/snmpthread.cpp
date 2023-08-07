#include "snmpthread.h"
#include <QDebug>

SnmpThread::SnmpThread(QObject *parent)
    : QThread{parent}
{
    QTimer::singleShot(3*1000,this,SLOT(start()));
}

SnmpThread::~SnmpThread()
{
    isRun = false;
    wait();
}

bool SnmpThread::init(int id)
{
    bool ret = true;
    sDataPacket *shm = get_share_mem(); // 获取共享内存
    mBusData = &(shm->data[id-1]);
    mId = id-1;

    return ret;
}

int SnmpThread::initSnmp(netsnmp_session &session,
                         netsnmp_session **ss)
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
    session.version = SNMP_VERSION_3;
    session.securityName = strdup(name.toStdString().c_str());
    session.securityNameLen = strlen(session.securityName);
    session.securityLevel = SNMP_SEC_LEVEL_AUTHPRIV;
    session.securityAuthProto = usmHMACMD5AuthProtocol;
    session.securityAuthProtoLen = USM_AUTH_PROTO_MD5_LEN;
    session.securityAuthKeyLen = USM_AUTH_KU_LEN;

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
    // 发送SNMP GET请求
    SOCK_STARTUP;
    *ss = snmp_open(&session);

    if (*ss == NULL) {
        snmp_sess_perror("busbar", &session);
        return -1;
    }
    return 0;
}


int SnmpThread::walkSnmp(netsnmp_session **ss,netsnmp_pdu *response,netsnmp_pdu *pdu,int index)
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
        if(  index <= 9 ){
            //if( (t->online1 >> (key - 1)) & 0x03 == 1 ){
                anOID[(int)anOID_len - 1] = key;
                end_oid[(int)end_len - 1] = key + 1;
            //}
        }
        //if( t->online2 != 0 && index > 9 && index <= 18 ){
        if( index > 9 && index <= 18 ){
            //key -= 9;
            //if( (t->online2 >> (key - 1)) & 0x03 == 1 ){
                anOID[(int)anOID_len - 1] = key;
                end_oid[(int)end_len - 1] = key + 1;
            //}
        }
    }


    int count = 0;
    int status = 0;
    int running = 1;
    bool endflag = false;
    snmp_add_null_var(pdu, anOID, anOID_len);

    // 循环进行SNMP Walk
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
                    sBoxData *t= & mBusData->box[index-1];
                    if( t && t->offLine > 0 ) t->offLine--;
                    qDebug()<<"No Such Object t->offLine   "<<index << (t->offLine-'0')<<endl;
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
            sBoxData *t= & mBusData->box[index-1];
            if( t && t->offLine > 0 ) t->offLine--;
            qDebug()<<index << " Error in packet: t->offLine   "<<(t->offLine-'0')<<endl;
            break;
        }

        count++;
    }

    if(status == STAT_TIMEOUT){
        if(response){
            snmp_free_pdu(response);
            response = NULL;
        }
        sBoxData *t= & mBusData->box[index-1];
        if( t && t->offLine > 0 ) t->offLine--;
        qDebug()<<"time out  "<<index << (t->offLine-'0')<<endl;
    }

    else if(status == STAT_ERROR){
        if(response){
            snmp_free_pdu(response);
            response = NULL;
        }
        sBoxData *t= & mBusData->box[index-1];
        if( t && t->offLine > 0 ) t->offLine--;
        qDebug()<<"STAT_ERROR  "<<index << (t->offLine-'0')<<endl;
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
        t->offLine = 3;
        t->dc = 1;
        t->proNum = (val.remove("INTEGER:").simplified().toUInt())&0x0f;
    }break;
    case 3: t->curSpecification = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 4: t->version = val.remove("STRING:").replace("\"","").simplified().remove(".").toUInt(&ok);break;
    case 5: t->workMode = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 6: mBusData->boxNum = val.remove("INTEGER:").simplified().toUInt(&ok);break;
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
    case 1: t->reCur.svalue = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat()*COM_RATE_CUR;break;
    case 2: t->reCur.salarm = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 3: t->zeroLineCur.svalue = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat()*COM_RATE_CUR;break;
    case 4: t->zeroLineCur.salarm = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 5: t->totalApPow = (unsigned long long)val.remove("STRING:").replace("\"","").simplified().toFloat()*COM_RATE_POW;break;
    case 6: t->totalPow.ivalue = (unsigned long long)val.remove("STRING:").replace("\"","").simplified().toFloat()*COM_RATE_POW;break;
    case 7: t->totalPow.ialarm = val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 8: break;
    case 9: t->rate.svalue = (ushort)val.remove("INTEGER:").simplified().toUInt(&ok)/10;break;
    case 10: t->rate.salarm = (ushort)val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 11: t->volUnbalance = (uint)val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 12: t->curUnbalance = (uint)val.remove("INTEGER:").simplified().toUInt(&ok);break;
    case 13: t->reCur.smax = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat()*COM_RATE_CUR;break;
    case 14: t->zeroLineCur.smin = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat()*COM_RATE_CUR;break;
    case 15: t->zeroLineCur.smax = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat()*COM_RATE_CUR;break;
    case 16: t->totalPow.imin = (unsigned long long)val.remove("STRING:").replace("\"","").simplified().toFloat()*COM_RATE_POW;break;
    case 17: t->totalPow.imax = (unsigned long long)val.remove("STRING:").replace("\"","").simplified().toFloat()*COM_RATE_POW;break;
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
        case 1:  t->lineVol.value[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_VOL;break;
        case 2:  t->lineVol.alarm[line] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 3:  t->vol.value[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_VOL;break;
        case 4:  t->vol.alarm[line] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 5:  t->cur.value[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_CUR;break;
        case 6:  t->cur.alarm[line] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 7:  t->pow.value[line] = (uint)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
        case 8:  t->pow.alarm[line] = val.remove("INTEGER:").simplified().toUInt(&ok);break;
        case 9:  t->reactivePower[line] = (uint)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
        case 10:  t->apPow[line] = (uint)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
        case 11:  t->pf[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_PF;break;
        case 12:  t->ele[line] = (uint)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_ELE;break;

        case 13: parseVolHar( val , line );break;
        case 14: parseCurHar( val , line );break;
        case 15:  t->pl[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_PF;break;

        case 16:  t->lineVol.min[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_VOL;break;
        case 17:  t->lineVol.max[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_VOL;break;
        case 18:  t->vol.min[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_VOL;break;
        case 19:  t->vol.max[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_VOL;break;
        case 20:  t->cur.min[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_CUR;break;
        case 21:  t->cur.max[line] = (ushort)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_CUR;break;
        case 22:  t->pow.min[line] = (uint)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
        case 23:  t->pow.max[line] = (uint)val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
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
        case 1: t->data.vol.value[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_VOL;break;
        case 2: t->data.vol.alarm[loop-1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

        case 3: t->data.cur.value[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_CUR;break;
        case 4: t->data.cur.alarm[loop-1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

        case 5: t->data.pow.value[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
        case 6: t->data.pow.alarm[loop-1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

        case 7: t->data.reactivePower[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
        case 8: t->data.apPow[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
        case 9: t->data.pf[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_PF;break;
        case 10: t->data.ele[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_ELE;break;

        case 11: t->data.sw[loop-1] = val.remove("INTEGER:").replace("\"","").simplified().toUInt(&ok)==1?0:1;break;
        case 12: t->data.vol.min[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_VOL;break;
        case 13: t->data.vol.max[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_VOL;break;
        case 14: t->data.cur.min[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_CUR;break;
        case 15: t->data.cur.max[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_CUR;break;
        case 16: t->data.pow.min[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
        case 17: t->data.pow.max[loop-1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_POW;break;
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
    case 1: t->env.tem.value[0] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;
    case 2: t->env.tem.alarm[0] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    case 3: t->env.tem.value[1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;
    case 4: t->env.tem.alarm[1] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    case 5: t->env.tem.value[2] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;
    case 6: t->env.tem.alarm[2] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    case 7: t->env.tem.value[3] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;
    case 8: t->env.tem.alarm[3] = val.remove("INTEGER:").simplified().toUInt(&ok);break;

    case 9: t->env.tem.min[0] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;
    case 10: t->env.tem.max[0] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;

    case 11: t->env.tem.min[1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;
    case 12: t->env.tem.max[1] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;

    case 13: t->env.tem.min[2] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;
    case 14: t->env.tem.max[2] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;

    case 15: t->env.tem.min[3] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;
    case 16: t->env.tem.max[3] = val.remove("STRING:").replace("\"","").simplified().toFloat(&ok)*COM_RATE_TEM;break;

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

void SnmpThread::run()
{
    isRun = true;
    netsnmp_session session, *ss;
    ss = NULL;
    netsnmp_pdu *response = NULL;
    netsnmp_pdu *pdu = NULL;
    initSnmp(session , &ss );
    while(isRun)
    {
        if(gVerflag == 3){

            for(int index = 1 ; index <= mBusData->boxNum+1 ; index++)
                walkSnmp(&ss , response , pdu , index);
        }
    }
    releaseCon(session , &ss , response);
}
