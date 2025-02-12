/**
  * 时间设置窗口
  *         Lzy       2016-6-2
  */
#include "timesettingdlg.h"
#include "ui_timesettingdlg.h"
#include <QMessageBox>
#include <QTextCharFormat>
#include "msgbox.h"
#include "datapacket.h"
//#include <QAndroidJniObject>
//#include <QAndroidJniEnvironment>
//#include <QtAndroid>

extern void com_setBackColour(const QString &str,QWidget *target);

QList<QPair<QString , QString>> g_ZoneTimeList={
    {"(UTC-12:00) International Date Line West", "posix/Etc/GMT+12"},
    {"(UTC-11:00) Coordinated Universal Time-11", "Etc/GMT+11"},
    {"(UTC-11:00) Midway, Samoa", "Pacific/Midway"},
    {"(UTC-10:00) Hawaii", "Pacific/Honolulu"},
    {"(UTC-10:00) Aleutian Islands", "America/Adak"},
    {"(UTC-09:30) Marquesas Islands", "Pacific/Marquesas"},
    {"(UTC-09:00) Alaska", "America/Anchorage"},
    {"(UTC-09:00) Coordinated Universal Time-09", "Etc/GMT+9"},
    {"(UTC-08:00) Baja California", "America/Santa_Isabel"},
    {"(UTC-08:00) Pacific Time (US & Canada)", "America/Los_Angeles"},
    {"(UTC-08:00) Coordinated Universal Time-08", "Etc/GMT+8"},
    {"(UTC-07:00) La Paz, Mazatlan", "America/Mazatlan"},
    {"(UTC-07:00) Arizona", "America/Phoenix"},
    {"(UTC-07:00) Mountain Time (US & Canada)", "America/Denver"},
    {"(UTC-06:00) Central America", "America/Guatemala"},
    {"(UTC-06:00) Central Time (US & Canada)", "America/Chicago"},
    {"(UTC-06:00) Saskatchewan", "America/Regina"},
    {"(UTC-06:00) Guadalajara, Mexico City, Monterey", "America/Mexico_City"},
    {"(UTC-06:00) Easter Island", "Pacific/Easter"},
    {"(UTC-05:00) Bogota, Lima, Quito", "America/Bogota"},
    {"(UTC-05:00) Indiana (East)", "America/Indiana/Indianapolis"},
    {"(UTC-05:00) Eastern Time (US & Canada)", "America/New_York"},
    {"(UTC-05:00) Havana", "America/Havana"},
    {"(UTC-05:00) Haiti", "America/Port-au-Prince"},
    {"(UTC-05:00) Chetumal", "America/Cancun"},
    {"(UTC-05:00) Turks and Caicos", "America/Grand_Turk"},
    {"(UTC-04:00) Caracas", "America/Caracas"},
    {"(UTC-04:00) Atlantic Time (Canada)", "America/Halifax"},
    {"(UTC-04:00) Asuncion", "America/Asuncion"},
    {"(UTC-04:00) Georgetown, La Paz, Manaus, San Juan", "America/La_Paz"},
    {"(UTC-04:00) Cuiaba", "America/Cuiaba"},
    {"(UTC-04:00) Santiago", "America/Santiago"},
    {"(UTC-03:30) Newfoundland", "America/St_Johns"},
    {"(UTC-03:00) Araguaina", "America/Araguaina"},
    {"(UTC-03:00) Brasilia", "America/Sao_Paulo"},
    {"(UTC-03:00) Greenland", "America/Godthab"},
    {"(UTC-03:00) Cayenne, Fortaleza", "America/Cayenne"},
    {"(UTC-03:00) City of Buenos Aires", "America/Argentina/Buenos_Aires"},
    {"(UTC-03:00) Buenos Aires", "America/Argentina/Buenos_Aires"},
    {"(UTC-03:00) Montevideo", "America/Montevideo"},
    {"(UTC-03:00) Punta Arenas", "America/Punta_Arenas"},
    {"(UTC-03:00) Saint Pierre and Miquelon", "America/Miquelon"},
    {"(UTC-03:00) Salvador", "America/Bahia"},
    {"(UTC-02:00) Coordinated Universal Time-2", "Etc/GMT+2"},
    {"(UTC-01:00) Cape Verde", "Atlantic/Cape_Verde"},
    {"(UTC-01:00) Azores", "Atlantic/Azores"},
    {"(UTC+00:00) Monrovia, Reykjavik", "Atlantic/Reykjavik"},
    {"(UTC+00:00) Dublin, Edinburgh, Lisbon, London", "Europe/London"},
    {"(UTC+00:00) Coordinated Universal Time", "Etc/GMT"},
    {"(UTC+01:00) Casablanca", "Africa/Casablanca"},
    {"(UTC+01:00) Amsterdam, Berlin, Bern, Rome, Stockholm, Vienna", "Europe/Berlin"},
    {"(UTC+01:00) Brussels, Copenhagen, Madrid, Paris", "Europe/Paris"},
    {"(UTC+01:00) West Central Africa", "Africa/Lagos"},
    {"(UTC+01:00) Belgrade, Bratislava, Budapest, Ljubljana, Prague", "Europe/Budapest"},
    {"(UTC+01:00) Sarajevo, Skopje, Warsaw, Zagreb", "Europe/Warsaw"},
    {"(UTC+02:00) Windhoek", "Africa/Windhoek"},
    {"(UTC+02:00) Athens, Bucharest", "Europe/Athens"},
    {"(UTC+02:00) Helsinki, Kyiv, Riga, Sofia, Tallinn, Vilnius", "Europe/Kiev"},
    {"(UTC+02:00) Cairo", "Africa/Cairo"},
    {"(UTC+02:00) Damascus", "Asia/Damascus"},
    {"(UTC+02:00) Amman", "Asia/Amman"},
    {"(UTC+02:00) Harare, Pretoria", "Africa/Johannesburg"},
    {"(UTC+02:00) Jerusalem", "Asia/Jerusalem"},
    {"(UTC+02:00) Beirut", "Asia/Beirut"},
    {"(UTC+02:00) Chisinau", "Europe/Chisinau"},
    {"(UTC+02:00) Gaza, Hebron", "Asia/Gaza"},
    {"(UTC+02:00) Kaliningrad", "Europe/Kaliningrad"},
    {"(UTC+02:00) Tripoli", "Africa/Tripoli"},
    {"(UTC+02:00) Khartoum", "Africa/Khartoum"},
    {"(UTC+03:00) Juba", "Africa/Juba"},
    {"(UTC+03:00) Istanbul", "Europe/Istanbul"},
    {"(UTC+03:00) Baghdad", "Asia/Baghdad"},
    {"(UTC+03:00) Minsk", "Europe/Minsk"},
    {"(UTC+03:00) Kuwait, Riyadh", "Asia/Riyadh"},
    {"(UTC+03:00) Nairobi", "Africa/Nairobi"},
    {"(UTC+03:00) Moscow, St. Petersburg, Volgograd", "Europe/Moscow"},
    {"(UTC+03:30) Tehran", "Asia/Tehran"},
    {"(UTC+04:00) Tbilisi", "Asia/Tbilisi"},
    {"(UTC+04:00) Yerevan", "Asia/Yerevan"},
    {"(UTC+04:00) Abu Dhabi, Muscat", "Asia/Dubai"},
    {"(UTC+04:00) Baku", "Asia/Baku"},
    {"(UTC+04:00) Port Louis", "Indian/Mauritius"},
    {"(UTC+04:00) Astrakhan, Ulyanovsk", "Europe/Astrakhan"},
    {"(UTC+04:00) Izhevsk, Samara", "Europe/Samara"},
    {"(UTC+04:00) Saratov", "Europe/Saratov"},
    {"(UTC+04:30) Kabul", "Asia/Kabul"},
    {"(UTC+05:00) Ashgabat, Tashkent", "Asia/Tashkent"},
    {"(UTC+05:00) Islamabad, Karachi", "Asia/Karachi"},
    {"(UTC+05:00) Yekaterinburg", "Asia/Yekaterinburg"},
    {"(UTC+05:00) Qyzylorda", "Asia/Qyzylorda"},
    {"(UTC+05:30) Sri Jayewardenepura Kotte", "Asia/Colombo"},
    {"(UTC+05:30) Chennai, Kolkata, Mumbai, New Delhi", "Asia/Kolkata"},
    {"(UTC+05:45) Kathmandu", "Asia/Kathmandu"},
    {"(UTC+06:00) Astana", "Asia/Almaty"},
    {"(UTC+06:00) Dhaka", "Asia/Dhaka"},
    {"(UTC+06:00) Omsk", "Asia/Omsk"},
    {"(UTC+06:00) Bishkek", "Asia/Bishkek"},
    {"(UTC+06:30) Yangon", "Asia/Yangon"},
    {"(UTC+07:00) Bangkok, Hanoi, Jakarta", "Asia/Bangkok"},
    {"(UTC+07:00) Novosibirsk", "Asia/Novosibirsk"},
    {"(UTC+07:00) Krasnoyarsk", "Asia/Krasnoyarsk"},
    {"(UTC+07:00) Barnaul, Gorno-Altaysk", "Asia/Barnaul"},
    {"(UTC+07:00) Khovd", "Asia/Hovd"},
    {"(UTC+07:00) Tomsk", "Asia/Tomsk"},
    {"(UTC+08:00) Ulaanbaatar", "Asia/Ulaanbaatar"},
    {"(UTC+08:00) Beijing, Chongqing, Hong Kong, Urumqi", "Asia/Shanghai"},
    {"(UTC+08:00) Perth", "Australia/Perth"},
    {"(UTC+08:00) Kuala Lumpur, Singapore", "Asia/Singapore"},
    {"(UTC+08:00) Taipei", "Asia/Taipei"},
    {"(UTC+08:00) Irkutsk", "Asia/Irkutsk"},
    {"(UTC+08:45) Eucla", "Australia/Eucla"},
    {"(UTC+09:00) Seoul", "Asia/Seoul"},
    {"(UTC+09:00) Osaka, Sapporo, Tokyo", "Asia/Tokyo"},
    {"(UTC+09:00) Yakutsk", "Asia/Yakutsk"},
    {"(UTC+09:00) Chita", "Asia/Chita"},
    {"(UTC+09:00) Pyongyang", "Asia/Pyongyang"},
    {"(UTC+09:30) Darwin", "Australia/Darwin"},
    {"(UTC+10:00) Hobart", "Australia/Hobart"},
    {"(UTC+10:00) Brisbane", "Australia/Brisbane"},
    {"(UTC+10:00) Guam, Port Moresby", "Pacific/Port_Moresby"},
    {"(UTC+10:00) Canberra, Melbourne, Sydney", "Australia/Sydney"},
    {"(UTC+10:00) Vladivostok", "Asia/Vladivostok"},
    {"(UTC+10:30) Adelaide", "Australia/Adelaide"},
    {"(UTC+11:00) Lord Howe Island", "Australia/Lord_Howe"},
    {"(UTC+11:00) Bougainville Island", "Pacific/Bougainville"},
    {"(UTC+11:00) Solomon Islands, New Caledonia", "Pacific/Guadalcanal"},
    {"(UTC+11:00) Magadan", "Asia/Magadan"},
    {"(UTC+12:00) Coordinated Universal Time+12", "Etc/GMT-12"},
    {"(UTC+12:00) Fiji, Marshall Islands", "Pacific/Fiji"},
    {"(UTC+12:00) Auckland, Wellington", "Pacific/Auckland"},
    {"(UTC+12:00) Anadyr, Petropavlovsk-Kamchatskiy", "Asia/Anadyr"},
    {"(UTC+12:45) Chatham", "Pacific/Chatham"},
    {"(UTC+13:00) Nuku'alofa", "Pacific/Tongatapu"},
    {"(UTC+13:00) Coordinated Universal Time+13", "Etc/GMT-13"},
    {"(UTC+14:00) Kiritimati Island", "Pacific/Kiritimati"}
};

TimeSettingDlg::TimeSettingDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TimeSettingDlg)
{
    ui->setupUi(this);
//    com_setBackColour(tr("时间设置"),this);
    initLanguage();

    QDate date = QDate::currentDate();
    ui->yearSpin->setValue(date.year());
    ui->monSpin->setValue(date.month());
    ui->daySpin->setValue(date.day());

    m_editBtn =0;
    ui->timeSet_but->setEnabled(false);

    ui->hourSpin->setValue(QDateTime::currentDateTime().time().hour());
    ui->minSpin->setValue(QDateTime::currentDateTime().time().minute());
    ui->secSpin->setValue(QDateTime::currentDateTime().time().second());

    preYear = ui->yearSpin->value();
    preMonth = ui->monSpin->value();

    connect(ui->monSpin, SIGNAL(valueChanged(int)), this, SLOT(dateSetLimit(int)));
    connect(ui->yearSpin, SIGNAL(valueChanged(int)), this, SLOT(dateSetLimit(int)));
//    connect(ui->timeSet_but, SIGNAL(clicked()), this, SLOT(on_timeSet_but_clicked()));

    initEditBtn();
    initKey();
}

TimeSettingDlg::~TimeSettingDlg()
{
    delete ui;
}

void TimeSettingDlg::initTimeZone()
{
    QPixmap pix(1,60);
    pix.fill(Qt::transparent);
    QIcon icon(pix);
    ui->TimeZoneBox->setIconSize(QSize(1,60));
    bool ret = sys_configFile_open();
    ret = sys_configFile_contains("timezone");
    QString str = "Asia/Shanghai";
    if(ret){
        str = sys_configFile_readStr("timezone");
    }else{
        sys_configFile_write("timezone" , "Asia/Shanghai");
    }
    sys_configFile_close();
    for(int i = 0 ; i < g_ZoneTimeList.size() ; i++){
        ui->TimeZoneBox->setItemIcon(i , icon);
        if(str == g_ZoneTimeList.at(i).second){
            ui->TimeZoneBox->setCurrentText(g_ZoneTimeList.at(i).first);
        }
    }
}

void TimeSettingDlg::initLanguage()
{
    initTimeZone();
    if(gLanguage == 0){
        ui->label_11->setText("时间设置");
        ui->yearBtn->setText("编辑");
        ui->label_32->setText("年");
        ui->monBtn->setText("编辑");
        ui->label_33->setText("月");
        ui->dayBtn->setText("编辑");
        ui->label_34->setText("日");
        ui->hourBtn->setText("编辑");
        ui->label_35->setText("时");
        ui->minBtn->setText("编辑");
        ui->label_36->setText("分");
        ui->secBtn->setText("编辑");
        ui->label_37->setText("秒");
        ui->label_7->setText("修改值");
        ui->btn_enter->setText("输入");
        ui->timeSet_but->setText("确定");
        ui->quitBtn->setText("退出");
        ui->label_12->setText("时区设置");
    }else{
        ui->label_11->setText("Time setting");
        ui->yearBtn->setText("Edit");
        ui->label_32->setText("year");
        ui->monBtn->setText("Edit");
        ui->label_33->setText("month");
        ui->dayBtn->setText("Edit");
        ui->label_34->setText("day");
        ui->hourBtn->setText("Edit");
        ui->label_35->setText("hour");
        ui->minBtn->setText("Edit");
        ui->label_36->setText("minute");
        ui->secBtn->setText("Edit");
        ui->label_37->setText("second");
        ui->label_7->setText("Modify value");
        ui->btn_enter->setText("Iuput");
        ui->timeSet_but->setText("Confirm");
        ui->quitBtn->setText("Quit");
        ui->label_12->setText("Time zone setting");
    }
}


/**
 * 功 能：验证日期合法性
 */
void TimeSettingDlg::dateSetLimit(int)
{
    switch(ui->monSpin->value())
    {
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
        if (ui->daySpin->value() > 31)
            goto SHOWMESSAGEBOX;
        ui->daySpin->setMaximum(31);
        break;
    case 2:
        if (isLeap(ui->yearSpin->value())){
            if (ui->daySpin->value() > 29)
                goto SHOWMESSAGEBOX;
            ui->daySpin->setMaximum(29);
        }
        else{
            if (ui->daySpin->value() > 28)
                goto SHOWMESSAGEBOX;
            ui->daySpin->setMaximum(28);
        }
        break;
    case 4:
    case 6:
    case 9:
    case 11:
        if (ui->daySpin->value() > 30)
            goto SHOWMESSAGEBOX;
        ui->daySpin->setMaximum(30);
        break;
    }
    preYear = ui->yearSpin->value();
    preMonth = ui->monSpin->value();
    return;

SHOWMESSAGEBOX:
    ui->yearSpin->setValue(preYear);
    ui->monSpin->setValue(preMonth);

    //    QMessageBox::critical(this, tr("信息提示"),tr("日期设置不合法，请重新输入"));
    if(gLanguage == 0) CriticalMsgBox dlg(this, tr("日期设置不合法，请重新输入"));
    else CriticalMsgBox dlg(this, tr("The date setting is invalid, please re-enter it"));
}

/**
 * 功 能：闰年
 */
bool TimeSettingDlg::isLeap(int yN)
{
    if ((yN % 400 == 0 && yN % 3200 != 0)
            || (yN % 4 == 0 && yN % 100 != 0)
            || (yN % 3200 == 0 && yN % 172800 == 0))
        return true;
    else
        return false;

}


/**
 * 功 能：时间设置
 */
void TimeSettingDlg::timeSetup(void)
{
//    struct tm setDataTime;
//    time_t timep;
//    struct timeval tv;

//    setDataTime.tm_year = ui->yearSpin->value() - 1900;
//    setDataTime.tm_mon = ui->monSpin->value() - 1;
//    setDataTime.tm_mday = ui->daySpin->value();

//    setDataTime.tm_hour = ui->hourSpin->value();
//    setDataTime.tm_min = ui->minSpin->value();
//    setDataTime.tm_sec = ui->secSpin->value();

//    timep = mktime(&setDataTime);

//    tv.tv_sec = timep;
//    tv.tv_usec = 0;

    //if (settimeofday(&tv, (struct timezone *)0) < 0)
    //    return;
    QString command = QString("date -s \'%1-%2-%3 %4:%5:%6\'")
                          .arg(ui->yearSpin->value())
                      .arg(ui->monSpin->value())
                      .arg(ui->daySpin->value())
                      .arg(ui->hourSpin->value())
                      .arg(ui->minSpin->value())
                          .arg(ui->secSpin->value());
    int ret = system(command.toLatin1().data());
    qDebug()<<command;
    ret = system("hwclock -f /dev/rtc0 -w");
 
    //int ret = system(command.toLatin1().data());

    //ret = system("busybox hwclock -w\n");
}



/**
   * 功　能：初始化编辑按键
   * 入口参数：
   * 返回值：无
   */
void TimeSettingDlg::initEditBtn(void)
{
    sm = new QSignalMapper(this);
    connect(ui->hourBtn,SIGNAL(clicked()),sm,SLOT(map()));
    connect(ui->minBtn,SIGNAL(clicked()),sm,SLOT(map()));
    connect(ui->secBtn,SIGNAL(clicked()),sm,SLOT(map()));

    connect(ui->yearBtn,SIGNAL(clicked()),sm,SLOT(map()));
    connect(ui->monBtn,SIGNAL(clicked()),sm,SLOT(map()));
    connect(ui->dayBtn,SIGNAL(clicked()),sm,SLOT(map()));

    sm->setMapping(ui->hourBtn,1);
    sm->setMapping(ui->minBtn,2);
    sm->setMapping(ui->secBtn,3);

    sm->setMapping(ui->yearBtn,4);
    sm->setMapping(ui->monBtn,5);
    sm->setMapping(ui->dayBtn,6);

    connect(sm,SIGNAL(mapped(int)),this,SLOT(editBtnPress(int)));
}

/**
   * 功　能：初始化背景色
   * 入口参数：
   * 返回值：无
  */
void TimeSettingDlg::initBackground(void)
{
    QString str =  "background-color:white;";

    ui->hourSpin->setStyleSheet(str);
    ui->minSpin->setStyleSheet(str);
    ui->secSpin->setStyleSheet(str);

    ui->yearSpin->setStyleSheet(str);
    ui->monSpin->setStyleSheet(str);
    ui->daySpin->setStyleSheet(str);
}

void TimeSettingDlg::editBtnPress(int value)
{
    QSpinBox *spinBox=NULL;
    QString colour = "background-color:rgb(199,237,204)";

    m_editBtn = value;
    switch(value)
    {
    case 1:
        spinBox = ui->hourSpin;
        break;
    case 2:
        spinBox = ui->minSpin;
        break;
    case 3:
        spinBox = ui->secSpin;
        break;

    case 4:
        spinBox = ui->yearSpin;
        break;
    case 5:
        spinBox = ui->monSpin;
        break;
    case 6:
        spinBox = ui->daySpin;
        break;
    }
    initBackground();
    spinBox->setStyleSheet(colour);

    int temp = spinBox->value();
    QString str = QString::number(temp);
    ui->lineEdit->setText(str);
    ui->lineEdit->selectAll();
}

/**
   * 功　能：初始化键盘按键
   * 入口参数：
   * 返回值：无
 */
void TimeSettingDlg::initKey(void)
{
    m = new QSignalMapper(this);
    connect(ui->btn_0,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_1,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_2,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_3,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_4,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_5,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_6,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_7,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_8,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_9,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_del,SIGNAL(clicked()),m,SLOT(map()));
    connect(ui->btn_point,SIGNAL(clicked()),m,SLOT(map()));

    m->setMapping(ui->btn_0,0);
    m->setMapping(ui->btn_1,1);
    m->setMapping(ui->btn_2,2);
    m->setMapping(ui->btn_3,3);
    m->setMapping(ui->btn_4,4);
    m->setMapping(ui->btn_5,5);
    m->setMapping(ui->btn_6,6);
    m->setMapping(ui->btn_7,7);
    m->setMapping(ui->btn_8,8);
    m->setMapping(ui->btn_9,9);
    m->setMapping(ui->btn_del,-1);
    m->setMapping(ui->btn_point,10);

    connect(m,SIGNAL(mapped(int)),this,SLOT(keyPress(int)));
}

void TimeSettingDlg::keyPress(int value)
{
    if(ui->lineEdit->hasSelectedText())
        ui->lineEdit->clear();

    QString str = ui->lineEdit->text();
    if(value == -1) // 删除一个字符
    {
        if(str.size() > 0)
            str = str.left(str.length() - 1);
    }
    else if(value == 10)
        str.append(".");
    else
        str.append(QString::number(value));

    ui->lineEdit->setText(str);
}



void TimeSettingDlg::on_btn_enter_clicked()
{
    QSpinBox *spinBox=NULL;

    switch(m_editBtn)
    {
    case 1:
        spinBox = ui->hourSpin;
        break;
    case 2:
        spinBox = ui->minSpin;
        break;
    case 3:
        spinBox = ui->secSpin;
        break;

    case 4:
        spinBox = ui->yearSpin;
        break;
    case 5:
        spinBox = ui->monSpin;
        break;
    case 6:
        spinBox = ui->daySpin;
        break;
    }

    if(spinBox)
    {
        int temp = ui->lineEdit->text().toInt();
        spinBox->setValue(temp);
        ui->timeSet_but->setEnabled(true);
    }
}

void TimeSettingDlg::on_quitBtn_clicked()
{
    bool ret = ui->timeSet_but->isEnabled();
    if(ret)
    {
        QMessageBox msgBox(this);
        if(gLanguage == 0){
            com_setBackColour(tr("信息提示"),&msgBox);
            msgBox.setText(tr("\n您已修改时间，是否要放弃修改？\n"));
            msgBox.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            msgBox.setButtonText (QMessageBox::Ok,QString("确 定"));
            msgBox.setButtonText (QMessageBox::Cancel,QString("取 消"));
            msgBox.setDefaultButton(QMessageBox::Ok);
            int res = msgBox.exec();
            if(res == QMessageBox::Ok)
                this->close();
        }else{
            com_setBackColour(tr("Message notification"),&msgBox);
            msgBox.setText(tr("\nYou have modified the time. Do you want to abandon the modification？\n"));
            msgBox.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            msgBox.setButtonText (QMessageBox::Ok,QString("Confirm"));
            msgBox.setButtonText (QMessageBox::Cancel,QString("Cancel"));
            msgBox.setDefaultButton(QMessageBox::Ok);
            int res = msgBox.exec();
            if(res == QMessageBox::Ok)
                this->close();
        }
    }
    else
        this->close();
}

void TimeSettingDlg::on_timeSet_but_clicked()
{
    bool ret = ui->timeSet_but->isEnabled();
    if(ret)
    {
        QDateTime currentTime_old = QDateTime::currentDateTime();
        int year_old = currentTime_old.date().year();
        int month_old = currentTime_old.date().month();
        int day_old = currentTime_old.date().day();
        int hour_old = currentTime_old.time().hour();
        int minute_old = currentTime_old.time().minute();
        QMessageBox msgBox(this);
        if(gLanguage == 0){
            com_setBackColour(tr("信息提示"),&msgBox);
            msgBox.setText(tr("\n您已修改时间，是否要修改？\n"));
            msgBox.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            msgBox.setButtonText (QMessageBox::Ok,QString("确 定"));
            msgBox.setButtonText (QMessageBox::Cancel,QString("取 消"));
            msgBox.setDefaultButton(QMessageBox::Ok);
        }else{
            com_setBackColour(tr("Message notification"),&msgBox);
            msgBox.setText(tr("\nYou have modified the time. Do you want to modify it？\n"));
            msgBox.setStandardButtons (QMessageBox::Ok|QMessageBox::Cancel);
            msgBox.setButtonText (QMessageBox::Ok,QString("Confirm"));
            msgBox.setButtonText (QMessageBox::Cancel,QString("Cancel"));
            msgBox.setDefaultButton(QMessageBox::Ok);
        }
        int res = msgBox.exec();
        if(res == QMessageBox::Ok)
        {
            timeSetup();
            QString str;
            QDateTime currentTime_new = QDateTime::currentDateTime();
            int year_new = currentTime_new.date().year();
            int month_new = currentTime_new.date().month();
            int day_new = currentTime_new.date().day();
            int hour_new = currentTime_new.time().hour();
            int minute_new = currentTime_new.time().minute();

            if (year_old != year_new || month_old != month_new
                    || day_old != day_new || hour_old != hour_new
                    || minute_old != minute_new)  {
                if(gLanguage == 0) str = tr("时间设置成功");
                else str = tr("Time set successfully");
                QString insertStr,insertStrEn;
                QString dateTimeString = currentTime_new.toString("yyyy-MM-dd hh:mm:ss");
                insertStr = tr("系统时间修改为 %1 !").arg(dateTimeString);
                insertStrEn = tr("The system time is changed to %1 !").arg(dateTimeString);//插入系统日志
                db_system_obj()->insertSystem(insertStr);
                db_system_obj_en()->insertSystem(insertStrEn);
            }  else {
                if(gLanguage == 0) str = tr("时间未修改");
                else str = tr("Time not modified");
            }
            //        QMessageBox::information(this,tr("信息提示"),str);
            InfoMsgBox dlg(this,str);
            this->close();
        }
    }
    else
        this->close();
}



void TimeSettingDlg::on_TimeZoneBox_currentTextChanged(const QString &arg1)
{
    system("rm -rf /etc/localtime");
    QString cmd = "ln -sf /usr/share/zoneinfo/";
    for(int i = 0 ; i < g_ZoneTimeList.size() ; i++){
        if(arg1 == g_ZoneTimeList.at(i).first){
            cmd += g_ZoneTimeList.at(i).second;
            QString str = g_ZoneTimeList.at(i).second;
            sys_configFile_writeParam("timezone" , str);
        }
    }
    cmd += "  /etc/localtime";
    system(cmd.toLocal8Bit().data()); //qDebug() << cmd;
}

