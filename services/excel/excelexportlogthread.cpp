/*
 * excelexportlogthread.cpp
 *
 *  Created on: 2017年10月11日
 *      Author: Lzy
 */
#include "excelexportlogthread.h"
#include "basicExcel/BasicExcel.hpp"
//using namespace YExcel;

ExcelExportLogThread::ExcelExportLogThread(QObject *parent) : QThread(parent)
{

}


ExcelExportLogThread::~ExcelExportLogThread()
{
    wait();
}

/**
 * @brief 百分制来计算
 */
int ExcelExportLogThread::getProgress()
{
    int ret = 100;

    if(mSize <= mAllSize)
        ret = (mSize *100.0) / mAllSize ;

    return ret;
}

void ExcelExportLogThread::progressSlot()
{
    int ret = getProgress();
    if(ret < 100)
         QTimer::singleShot(500,this,SLOT(progressSlot()));

    emit progressSig(ret);
}


void ExcelExportLogThread::set(const QString &path , const QString &file, const QList<QStringList> &list)
{
    mPath = path;
    mFile = file;
    mList = list;
    progressSlot();

    start();
}


/**
 * @brief 数据生成Excel/////需要更改
 */
void ExcelExportLogThread::exportMsg(QList<QStringList> &list)
{
    QStringList strList = mPath.split("/");
    QString str = "sda";
    for(int i = 0 ; i < strList.size() ; i++){
         if(i == strList.size() - 2)str = strList.at(i);
    }
    QString cstr = QString("mount | grep %1").arg(str);
    int ans = system(cstr.toLatin1());
    if(ans < 0) {
         qDebug() << cstr+" err";
    }
    bool ret = true;
    QString fileName = mFile +".csv";
    QFile csvfile(fileName);
    csvfile.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream out(&csvfile);
    out.setCodec("GBK");
    for(int i=0; i<list.size(); ++i)
    {
        for(int j=0; j<list.at(i).size(); ++j) {
            //QByteArray ba = list.at(i).at(j).toLatin1();
            //char* ch = ba.data();
            //sheet->Cell(i, j)->SetString(ch);
            out<<list.at(i).at(j)<<",";
        }
        out<<"\n";
        mSize++;
        msleep(30);
    }

    csvfile.flush();
    csvfile.close();

    
    //if(!ret) {
    //   qDebug() << "csv Export Log Save Err!!!";
    //} else
    //    ret = true;
    ans = system("sync");
    if(ans < 0) {
        qDebug() <<"sync err";
    }

    cstr = QString("umount /run/media/%1").arg(str);
    ans = system(cstr.toLatin1());
    if(ans < 0) {
        qDebug() << cstr+" err";
    }

    emit overSig(ret);
}


void ExcelExportLogThread::run()
{
    mSize = 0;
    mAllSize = mList.size();
    exportMsg(mList);
}
