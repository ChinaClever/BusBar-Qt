#include "json_send.h"


Json_Send::Json_Send(QObject *parent) : QThread(parent)
{
    mJson = Json_Pack::bulid();
    mSocket = new UdpSentSocket();
    mTcp = new TcpClient(this);

    sDataPacket *shm = get_share_mem();
    for(int i = 0;i <BUS_NUM; i++)
    {
        mBus[i] = &(shm->data[i]);
        mBoxNum[i] = &(shm->data[i].boxNum);
    }

//    timer = new QTimer(this);
//    timer->start(8*1000+rand()%500);
//    connect(timer, SIGNAL(timeout()),this, SLOT(run()));
}

Json_Send *Json_Send::bulid(QObject *parent)
{
    static Json_Send* sington = NULL;
    if(sington == NULL) {
        sington = new Json_Send(parent);
    }
    return sington;
}

void Json_Send::initFun()
{
    bool ret = sys_configFile_open();
    ret = sys_configFile_contains("Senduse");
    if(ret){
        gSendIP = sys_configFile_readStr("SendIP");
        gSendport = sys_configFile_readInt("Sendport");
        gUser = sys_configFile_readInt("Senduse");
        //qDebug()<<"SendIP"<<SendIP;
    }else {
        sys_configFile_write("Senduse" , QString::number(gUser));
        sys_configFile_write("SendIP" , gSendIP);
        sys_configFile_write("Sendport" , QString::number(gSendport));
    }
    sys_configFile_close();

}

void Json_Send::proc_time(sRunTime &proc)
{
    QDateTime dateTime = QDateTime::currentDateTime();
    QString str = dateTime.toString("yyyy-MM-dd hh:mm:ss");
    qstrcpy(proc.start, str.toUtf8().data());
}

QString Json_Send::md5(const QString &fn)
{
    QFile sourceFile(fn);
    qint64 fileSize = sourceFile.size();
    const qint64 bufferSize = 8*1024;

    if (sourceFile.open(QIODevice::ReadOnly)) {
        char buffer[bufferSize]; int bytesRead;
        int readSize = qMin(fileSize, bufferSize);
        QCryptographicHash hash(QCryptographicHash::Md5);
        while (readSize > 0 && (bytesRead = sourceFile.read(buffer, readSize)) > 0) {
            fileSize -= bytesRead;
            hash.addData(buffer, bytesRead);
            readSize = qMin(fileSize, bufferSize);
        } sourceFile.close();
        return QString(hash.result().toHex());
    }

    return QString();
}

void Json_Send::proc_md5(sRunTime &proc, const QString &fn)
{
    proc.resetCnt += 1; return;
    if(fn.contains("busbar")) return;
    QString str = md5(fn);
    qstrcpy(proc.md5, str.toUtf8().data());
}

bool Json_Send::proc_isRun(const QString &p)
{
    return isRunning(p.toStdString().c_str());
}

void Json_Send::proc_start(sRunTime &proc, const QString &app)
{
    QString path = "/home/root/"; //"/usr/data/pdu/app/";
    QString fn = path + app;
    if(proc_isRun(app)) return;

    proc_time(proc);
    proc_md5(proc, fn);
    QString cmd = fn + " -platform linxufb &";
    system(cmd.toLatin1().data());
    qDebug() << "process start " +cmd;
    proc_log(app +" startup"); mdelay(150);
}

int Json_Send::isRunning(const char *procname)
{
    int ret = 0;
    char sCurrPid[16] = {0};
    sprintf(sCurrPid, "%d\n", getpid());

    FILE *fstream=NULL;
    char buff[1024] = {0};

    snprintf(buff,1024,"ps | grep %s | grep -v grep | awk '{print $1}'",procname);
    if(NULL==(fstream=popen(buff, "r")))
    {
        fprintf(stderr,"execute command failed: %s", strerror(errno));
        return -1;
    }
    memset(buff, 0, sizeof(buff));
    while(NULL!=fgets(buff, sizeof(buff), fstream)) {
        if (strlen(buff) > 0) {
            if (strcmp(buff, sCurrPid) !=0) {
                //printf("******%s, %s****\n", buff, sCurrPid);
                ret = 1;
                break;
            }
        }
    }
    pclose(fstream);
    return ret;
}

void Json_Send::proc_log(const QString &arg)
{
    QString fn = "/tmp/process_log";
    QString t = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz\t");
    QString str = t + arg; QString fmd = "echo '%1' >> %2 ";
    QString cmd = fmd.arg(str, fn);
    system(cmd.toLatin1().data());
}

void Json_Send::mdelay(int msec)
{
    if (msec <= 0) {
        return;
    }

#if 1
    //非阻塞方式延时,现在很多人推荐的方法
    QCoreApplication::processEvents(QEventLoop::AllEvents, 10);
    QEventLoop loop; QTimer::singleShot(msec, &loop, SLOT(quit())); loop.exec();
#else
#if (QT_VERSION >= QT_VERSION_CHECK(6,0,0))
    //阻塞方式延时,如果在主线程会卡住主界面
    QThread::msleep(msec);
#else
    //非阻塞方式延时,不会卡住主界面,据说可能有问题
    QTime endTime = QTime::currentTime().addMSecs(msec);
    while (QTime::currentTime() < endTime) {
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    }
#endif
#endif
}

void Json_Send::resetProc(sRunTime &proc, const QString &name)
{
    QString cmd = "killall " + name;
    system("chmod 755 -R /home/root/");
    system(cmd.toLatin1().data()); mdelay(100);
    proc_log(name +"_exit"); mdelay(100);
    proc_start(proc, name); mdelay(5000);
}

void Json_Send::freeMemoryCheck()
{
    static uint rst=0,cnt = 0;
    if((++cnt) % 600) return ;
    int ret = freeMemory();
    if((ret >0 && ret<100) && rst==0) {

        rst = 1;cnt = 0;
        //if(ret>0 && ret<100) resetProc(mProcs->core, "/home/root/busbar");
    } else if((ret>0 && ret<8) && rst) system("reboot");
    else if(ret > 100){
        rst = 0;
        cnt = 0;
    }
}

// 方法1：通过调用 `free -m` 命令获取空闲内存
int Json_Send::freeMemory()
{
    QString output = executeCommand("free -m");
    QStringList lines = output.split("\n"); // 按行分割输出

    if (lines.size() > 1) {
        QStringList memoryInfo = lines[1].split(" ", QString::SkipEmptyParts); // 分割第二行（Mem行）
        if (memoryInfo.size() > 3) {
            int freeMemory = memoryInfo[3].toInt(); // 获取空闲内存（MB）
//            static uint cnt = 0;
//            if((++cnt) % 600 && freeMemory >0 && freeMemory<100){
//                cnt = 0;
//                system("echo 3 > /proc/sys/vm/drop_caches");
//            }
            return freeMemory;
        }
    }

    return -1; // 如果获取失败，返回 -1
}


QString Json_Send::executeCommand(const QString &cmd)
{
    FILE* pipe = popen(cmd.toStdString().c_str(), "r");
    if (!pipe)  return QString();

    char buffer[4096];
    QString result;
    //size_t resultSize = 0;
    //size_t bufferSize = sizeof(buffer);

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        //size_t len = strlen(buffer);
        //char* temp = (char*)realloc(result, resultSize + len + 1);
        //if (!temp) { free(result); pclose(pipe); return NULL; }
//        result = temp; strcpy(result + resultSize, buffer);
//        resultSize += len;
        result += QString::fromLocal8Bit(buffer);
    }

    //QString res = result;
    pclose(pipe);// free(result);
    return result;
}

void Json_Send::run()
{
//    sendData();//udp
//    TcpsendData();//tcp
//    msleep(1000);
    bool ret = true;
    initFun();
    while(ret){
        if(gUser) {
            sendData();
        }else{
            break;
        }
        //freeMemoryCheck();
        msleep(1000);
    }
}

void Json_Send::TcpsendData()
{
    QString mHost = "192.168.1.41";
    int port = 55320; bool ret = true;

    QJsonObject bar_json, box_json ; QByteArray ba;
    for(int i = 0;i < BUS_NUM;i++)
    {
        if(mBus[i]->box[0].offLine)
        {
            bar_json.empty(); ba.clear();
            mJson->getStart_Json(bar_json, ba, i);
            ret = get_tcp_connect();
            if(ret == false)
            {
                ret = mTcp->newConnect(mHost,port);
            }
            if(ret)
            {
                mTcp->sentMessage(ba);
            }
            if(!ret) break;
        }
        for(int j = 1;j < *(mBoxNum[i]) + 1;j++)
        {
            if(mBus[i]->box[j].offLine)
            {
                ba.clear();
                mJson->getInsert_Json(box_json, ba, i, j);
                qDebug()<<"   tcp    "<<ba.size();
                ret = get_tcp_connect();
                if(ret == false)
                {
                    ret = mTcp->newConnect(mHost,port);
                }
                if(ret)
                {
                    mJson->delay(2);
                    mTcp->sentMessage(ba);
                }

                if(!ret) break;
                box_json.empty();
            }
        }
    }
}

void Json_Send::sendData()
{
    QHostAddress address; bool ret;
    address.setAddress(gSendIP);
    QJsonObject bar_json[BUS_NUM], box_json[BUS_NUM][BOX_NUM] ; QByteArray ba;

    for(int i = 0;i < BUS_NUM;i++)
    {
        if(mBus[i]->box[0].offLine) {
            mJson->getStart_Json(bar_json[i], ba, i);
            ret = mSocket->sentData(address, ba ,gSendport);
            if(!ret) break;
            //bar_json.empty();
            ba.clear();

            for(int j = 1;j < *(mBoxNum[i]) + 1;j++)
            {
                if(mBus[i]->box[j].offLine) {
                    mJson->getInsert_Json(box_json[i][j], ba, i, j);
//                    qDebug()<<"   udp    "<<ba;
                    ret = mSocket->sentData(address, ba ,gSendport);
//                    qDebug()<<"   udp    "<<ba<<ret<<gSendIP<<gSendport;
                    if(!ret) break;
                    //box_json.empty();
                    ba.clear();
                }
            }
        }
    }
}
