#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "rtuthread.h"
#include "dpthread.h"
#include "currentalarmsdlg.h"
#include "interfacechangesig.h"
#include "common/beep/beepthread.h"
#include "datetime/timesettingdlg.h"
//#include "snmp/snmpthread.h"
#include "mbs/mb_core.h"
#include "modbus/thirdthread.h"
#include "json_send.h"

RtuThread *rtu[4] = {NULL, NULL, NULL, NULL};
ThirdThread *thr = NULL;
//SnmpThread *snmp= NULL;
extern int get_alarm_len();
int gVerflag = 2;
int gReadWriteflag = 1;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mInitShm = new InitShm(this); //线程
    mInitShm->start(); //初始化共享内存 -- 单线程运行一次

    mIndex = 0;
    initWidget();
    QString insertStr,insertStrEn;
    insertStr = tr("系统启动 !");
    insertStrEn = tr("System start !");//插入系统日志
    db_system_obj()->insertSystem(insertStr);
    db_system_obj_en()->insertSystem(insertStrEn);
    mVersion = "V3.0.7.018";//当前软件版本
    initVersion();
    updateTime();
    QTimer::singleShot(1000,this,SLOT(initFunSLot())); //延时初始化
    on_comboBox_currentIndexChanged(0);
    //BeepThread::bulid()->longBeep(); // 线程 -- 'bi~'
//    count = 0;

    for(int i=0; i < 4; i++) rtu[i] = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
    share_mem_del();
}

/**
 * @brief 初始化串口
 */
void MainWindow::initSerial()
{
    rtu[0] = new RtuThread(this);
    rtu[0]->init(SERIAL_COM1, 1); //只操作母线1
    connect(mSettingWid->mSystemDlg->mAutoSetAddress,SIGNAL(autoSetBus1Sig(int)), rtu[0] , SLOT(autoSetBusSlot(int)));
    connect(rtu[0],SIGNAL(sendNumAndIndexSig(int,int)),mSettingWid->mSystemDlg->mAutoSetAddress,SLOT(getNumAndIndexSlot(int,int)));
    connect(rtu[0],SIGNAL(sendDelaySig(int)),mSettingWid->mSystemDlg->mAutoSetAddress,SLOT(getDelaySlot(int)));
#if (SI_RTUWIFI==0)
#if BUS_NUM > 1
    rtu[1] = new RtuThread(this);
    rtu[1]->init(SERIAL_COM2, 2);
    connect(mSettingWid->mSystemDlg->mAutoSetAddress,SIGNAL(autoSetBus2Sig(int)), rtu[1] , SLOT(autoSetBusSlot(int)));
    connect(rtu[1],SIGNAL(sendNumAndIndexSig(int,int)),mSettingWid->mSystemDlg->mAutoSetAddress,SLOT(getNumAndIndexSlot(int,int)));
    connect(rtu[1],SIGNAL(sendDelaySig(int)),mSettingWid->mSystemDlg->mAutoSetAddress,SLOT(getDelaySlot(int)));
#endif
#if BUS_NUM > 2
    rtu[2] = new RtuThread(this);
    rtu[2]->init(SERIAL_COM3, 3);
    connect(mSettingWid->mSystemDlg->mAutoSetAddress,SIGNAL(autoSetBus3Sig(int)), rtu[2] , SLOT(autoSetBusSlot(int)));
    connect(rtu[2],SIGNAL(sendNumAndIndexSig(int,int)),mSettingWid->mSystemDlg->mAutoSetAddress,SLOT(getNumAndIndexSlot(int,int)));
    connect(rtu[2],SIGNAL(sendDelaySig(int)),mSettingWid->mSystemDlg->mAutoSetAddress,SLOT(getDelaySlot(int)));
#endif
#if BUS_NUM > 3
    rtu[3] = new RtuThread(this);
    rtu[3]->init(SERIAL_COM4, 4);
    connect(mSettingWid->mSystemDlg->mAutoSetAddress,SIGNAL(autoSetBus4Sig(int)), rtu[3] , SLOT(autoSetBusSlot(int)));
    connect(rtu[3],SIGNAL(sendNumAndIndexSig(int,int)),mSettingWid->mSystemDlg->mAutoSetAddress,SLOT(getNumAndIndexSlot(int,int)));
    connect(rtu[3],SIGNAL(sendDelaySig(int)),mSettingWid->mSystemDlg->mAutoSetAddress,SLOT(getDelaySlot(int)));
#endif
#endif

    Mb_Core::build(this);//////
    Json_Send::bulid(this);

//    rtu[4] = new RtuThread(this);
//    rtu[4]->init(SERIAL_COM5, 1);
}

void MainWindow::updateTime()
{
    QString time = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    ui->timeBtn->setText(time);
}

void MainWindow::timeoutDone()
{
    updateTime();
    checkAlarm();
    if(get_share_mem()) ui->comboBox->setEnabled(true);
    setBusName(mIndex);
    for(int i = 0; i<BUS_NUM; ++i)
        updateBusName(i);
    //////
    ///截图功能
    ///
    //
//    count++;
//    if(count>50)
//    {
//        if(count % 5 == 0){
//            QScreen *screen = QGuiApplication::primaryScreen();
//            screen->grabWindow(0).save(QString("/home/root/pic/screensshot%1.png").arg(count));
//        }
//    }
}

void MainWindow::updateBusName(int index)
{
    sDataPacket *shm = get_share_mem();
    char *name = shm->data[index].busName;

    QString str = "0" + QString::number(index+1) + " " + name;
    ui->comboBox->setItemText(index, str);
}

void MainWindow::setBusName(int index)
{
    sDataPacket *shm = get_share_mem();
    char *name = shm->data[index].busName;
    QString str = "0" + QString::number(index+1) + " " + name;
    ui->busNameLab->setText(str);
    ui->busNameLab->setText("");
//    ui->nameLab->hide();//legrand need hide

    mIndex = index;

    //ui->ratedLab->setText("V3.0.4_T03/27");
    ui->ratedLab->setText(mVersion);
}

void MainWindow::checkAlarm()
{
    int alarm = get_alarm_len();  /////========= 特意显示出来
    if(alarm)  {
        ui->alarmBtn->setVisible(true);
    } else {
        ui->alarmBtn->setVisible(false);
    }
}

void MainWindow::initNetSLot()
{
    mServer = new Server(this);
    mServer->setMaxPendingConnections(2);
    mServer->listen(QHostAddress::AnyIPv4, 22223);
    Mb_Core::build()->start();
    Json_Send::bulid()->start();
}

void MainWindow::initFunSLot()
{    
    initSerial(); //串口
    new DpThread(this); // 创建数据处理线程
    updateTime();


    timer = new QTimer(this);
    timer->start(1000);
    connect(timer, SIGNAL(timeout()),this, SLOT(timeoutDone()));

//    mClearCachetimer = new QTimer(this);
//    mClearCachetimer->start(2*24*60*60*1000-5*60*1000);
//    connect(mClearCachetimer, SIGNAL(timeout()),this, SLOT(clearCacheDone()));

    mCheckDlg = new CheckPasswordDlg(this);
    connect(mCheckDlg,SIGNAL(dialogClosed(bool)),this,SLOT(dialogClosed(bool)));
    QTimer::singleShot(7750,this,SLOT(initNetSLot())); //延时初始化

    ui->comboBox->setEnabled(false);
    QPixmap pix(1,60);
    pix.fill(Qt::transparent);
    QIcon icon(pix);
    ui->comboBox->setIconSize(QSize(1,60));
    ui->comboBox->setItemIcon(0 , icon);
    ui->comboBox->setItemIcon(1 , icon);
    ui->comboBox->setItemIcon(2 , icon);
    ui->comboBox->setItemIcon(3 , icon);
}

void MainWindow::initLanguage()
{
    bool ret = sys_configFile_open();
    ret = sys_configFile_contains("language");
    if(ret){
        gLanguage = sys_configFile_readInt("language");
    }else{
        sys_configFile_write("language" , QString::number(gLanguage));
    }
    sys_configFile_close();
}

void MainWindow::initSendUse()
{
    bool ret = sys_configFile_open();
    ret = sys_configFile_contains("Senduse");
    if(ret){
        SendIP = sys_configFile_readStr("SendIP");
        Sendport = sys_configFile_readInt("Sendport");
        user = sys_configFile_readInt("Senduse");
    }else{
        sys_configFile_write("Senduse" , QString::number(user));
        sys_configFile_write("SendIP" , SendIP);
        sys_configFile_write("Senduse" , QString::number(Sendport));
    }
    sys_configFile_close();
}

void MainWindow::initVersion()
{
    bool ret = sys_configFile_open();
    ret = sys_configFile_contains("version");
    if(ret){
        QString temp = sys_configFile_readStr("version");
        if(temp != mVersion){
            QString insertStr = tr("系统从%1升级到%2 !").arg(temp).arg(mVersion);
            QString insertStrEn = tr("Upgrading the system from %1 to %2 !").arg(temp).arg(mVersion);//插入系统日志
            db_system_obj()->insertSystem(insertStr);
            db_system_obj_en()->insertSystem(insertStrEn);
            sys_configFile_write("version" , mVersion);
        }
    }else{
        sys_configFile_write("version" , mVersion);
    }
    sys_configFile_close();
}

void MainWindow::initLable()
{
    if(gLanguage == 0){
        ui->homeLabBtn->setText("主界面");
        ui->lineLabBtn->setText("主路\n信息");
        ui->branchLabBtn->setText("支路\n信息");
        ui->logLabBtn->setText("数据\n记录");
        ui->setLabBtn->setText("参数\n设置");
    }else{
        ui->homeLabBtn->setText("Main\ninterface");
        ui->lineLabBtn->setText("Input\ninformation");
        ui->branchLabBtn->setText("Branch\ninformation");
        ui->logLabBtn->setText("Data\nrecord");
        ui->setLabBtn->setText("Parameter\nsetting");
    }
}
void MainWindow::initWidget()
{
    initLanguage();
    //    set_background_color(ui->stackedWid,Qt::white);
    set_background_icon(ui->stackedWid,":/new/prefix1/image/background.png");
    initBackground(); //按钮图标
    initLable();
    initSendUse();
    mHomeWid = new HomeWid(ui->stackedWid); //主界面
    ui->stackedWid->addWidget(mHomeWid);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), mHomeWid, SIGNAL(busChangedSig(int)));

    mLineWid = new LineWid(ui->stackedWid); //主路信息
    ui->stackedWid->addWidget(mLineWid);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), mLineWid, SIGNAL(busChangedSig(int)));

    mBranchWid = new BranchWid(ui->stackedWid); //支路信息
    ui->stackedWid->addWidget(mBranchWid);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), mBranchWid, SIGNAL(busChangedSig(int)));

    mLogsWid = new LogsWid(ui->stackedWid); //日志
    ui->stackedWid->addWidget(mLogsWid);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), mLogsWid, SIGNAL(busChangedSig(int)));

    mSettingWid = new SetMainWid(ui->stackedWid); //配置
    ui->stackedWid->addWidget(mSettingWid);
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), mSettingWid, SLOT(busChangedSlot(int)));
    checkFile();
}

void MainWindow::on_homeBtn_clicked()
{
    ui->stackedWid->setCurrentWidget(mHomeWid);
    setButtonClickedImage(ui->homeBtn,"home_select");

    InterfaceChangeSig::get()->changeType(1);
}

void MainWindow::checkFile()//check file exists ,delete file
{
    QFileInfo fi(QString("/home/root/tmp/busbar"));
    if(fi.exists()){
        int ret = system("rm /home/root/tmp/busbar");
        if(ret < 0) {
            qDebug() <<"rm /home/root/tmp/busbar err";
        }
    }
}

void MainWindow::on_lineBtn_clicked()
{
    ui->stackedWid->setCurrentWidget(mLineWid);
    setButtonClickedImage(ui->lineBtn,"main_select");

    InterfaceChangeSig::get()->changeType(2);
}

void MainWindow::on_branchBtn_clicked()
{
    ui->stackedWid->setCurrentWidget(mBranchWid);
    setButtonClickedImage(ui->branchBtn,"branch_select");

    InterfaceChangeSig::get()->changeType(3);
}

void MainWindow::on_logBtn_clicked()
{
    ui->stackedWid->setCurrentWidget(mLogsWid);
    setButtonClickedImage(ui->logBtn,"data_select");
    InterfaceChangeSig::get()->changeType(4);
}

void MainWindow::on_setBtn_clicked()
{
    if(ui->stackedWid->currentWidget() != mSettingWid) {
        BeepThread::bulid()->beep();
        mCheckDlg->clear();
        mCheckDlg->move(0,0);
        mCheckDlg->exec();
    }
}

void MainWindow::on_alarmBtn_clicked()
{
    BeepThread::bulid()->beep();
    CurrentAlarmsDlg dlg(this);
    dlg.exec();
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    BeepThread::bulid()->beep();
    setBusName(index);
}

void MainWindow::setButtonImage(QToolButton *button, QString name)
{    
    QString str = tr("QToolButton{border-image: url(:/new/prefix1/image/%1.png);}").arg(name);
    button->setStyleSheet(str);
}

void MainWindow::setButtonClickedImage(QToolButton *button, QString name)
{
    initBackground();
    QString str = tr("QToolButton{border-image: url(:/new/prefix1/image/%1.png);}").arg(name);
    button->setStyleSheet(str);
}

void MainWindow::initBackground()
{
    setButtonImage(ui->homeBtn,"home");
    setButtonImage(ui->lineBtn,"main");
    setButtonImage(ui->branchBtn,"branch");
    setButtonImage(ui->logBtn,"data");
    setButtonImage(ui->setBtn,"setting");
}

void MainWindow::dialogClosed(bool ret)
{
    if(ret){
        ui->stackedWid->setCurrentWidget(mSettingWid);
        setButtonClickedImage(ui->setBtn,"setting_select");
        InterfaceChangeSig::get()->changeType(5);
        QString insertStr,insertStrEn;
        insertStr = tr("参数设置页面登录 !");
        insertStrEn = tr("Log in parameter setting !");//插入系统日志
        db_system_obj()->insertSystem(insertStr);
        db_system_obj_en()->insertSystem(insertStrEn);
    }
    else{
        if(gLanguage == 0){
           QuMsgBox box(NULL,"对不起，密码输入不正确，您不具备该权限！");
           box.Exec();
        }
        else{
           QuMsgBox box(NULL,"Sorry,the passward entered is incorrect.You do not have the permission！");
           box.Exec();
        }
    }
}

void MainWindow::on_timeBtn_clicked()
{
#if 1
    //    BeepThread::bulid()->beep();
    TimeSettingDlg dlg(this);
    dlg.exec();
#endif
}
