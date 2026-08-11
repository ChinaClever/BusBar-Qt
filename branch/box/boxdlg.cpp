#include "boxdlg.h"
#include "ui_boxdlg.h"

BoxDlg::BoxDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BoxDlg)
{
    ui->setupUi(this);
    if(gLanguage == 0){com_setBackColour(tr("插接箱"), this);ui->label->setText("接插箱版本:");}
    else {com_setBackColour(tr("Tap-off box"), this);ui->label->setText("Tap-off box Version:");}
//    set_background_icon(this,":/new/prefix1/image/dialog.png",QSize(815,400));
//    this->setWindowFlags(Qt::WindowSystemMenuHint|Qt::WindowMinimizeButtonHint);// 打开注释时，Android不能全屏
}

BoxDlg::~BoxDlg()
{
    delete ui;
}

void BoxDlg::initBox(int bus, int box)
{
    sDataPacket *shm = get_share_mem();
    mData = &(shm->data[bus].box[box]);

    QString name(mData->boxName);
    ui->titleLab->setText(name);

    QString version = QString("V%1.%2.%3").arg(mData->version/100).arg(mData->version/10%10).arg(mData->version%10);
    if(mData->offLine)
    ui->version->setText(version);
    initWid(bus, box);//插接箱
}

void BoxDlg::initWid(int bus, int box)
{
    mTotalWid = new BoxTotalWid(ui->tabWidget);
    mTotalWid->initFun(bus, box);
    if(gLanguage == 0) ui->tabWidget->addTab(mTotalWid, tr("各相信息"));
    else ui->tabWidget->addTab(mTotalWid, tr("Information of each phase"));

    mLineWid = new BoxLoopTableWid(ui->tabWidget);
    mLineWid->initLine(bus, box);
    if(gLanguage == 0) ui->tabWidget->addTab(mLineWid, tr("各回路信息"));
    else ui->tabWidget->addTab(mLineWid, tr("Information of each circuit"));

    mOutputWid = new BoxOutputWid(ui->tabWidget);
    mOutputWid->initFun(bus, box);
    if(gLanguage == 0) ui->tabWidget->addTab(mOutputWid, tr("各输出信息"));
    else ui->tabWidget->addTab(mOutputWid, tr("Information of each output"));

    ui->tabWidget->setStyleSheet("QTabBar::tab{height:50px; width: 200px;}");
    ui->tabWidget->setTabShape(QTabWidget::Triangular);
}

void BoxDlg::on_pushButton_clicked()
{
    this->close();
    //BeepThread::bulid()->beep();
}
