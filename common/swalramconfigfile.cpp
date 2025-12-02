/*
 * sysconfigfile.cpp
 *  配置文件的操作接口
 *  配置参数的保存与读取
 *
 *  Created on: 2016年10月11日
 *      Author: Lzy
 */
#include "swalramconfigfile.h"
#include <QtCore>

extern QString cm_pathOfData(const QString& name);
static QSettings *pSwAlramConfigIni = NULL;

/**
 * 功 能：打开系统配置文件
 * 开发人员：Lzy     2016 - 七夕
 */
bool sw_alram_configFile_open(void)
{
    bool ret = true;

    QString fileName = "swalramconfig.ini";
    QString strFilename = cm_pathOfData(fileName);

   // QString strFilename = QCoreApplication::applicationDirPath();
   // strFilename += "/" + fileName;

    if(!QFileInfo(strFilename).exists())
        ret = false;

    if(pSwAlramConfigIni==NULL)
    {
        pSwAlramConfigIni = new QSettings(strFilename, QSettings::IniFormat);
    }

    return ret;
}

/**
 * 功 能：关闭系统配置文件
 * 开发人员：Lzy     2016 - 七夕
 */
void sw_alram_configFile_close(void)
{
    if(pSwAlramConfigIni){
        delete pSwAlramConfigIni;
        pSwAlramConfigIni = NULL;
    }
   // sync();
}

/**
 * 功 能：读字符串配置文件
 * 开发人员：Lzy     2013 - 七夕
 */
QString sw_alram_configFile_readStr(QString strParameterName, QString strGroup)
{
    QString strParameter = "";
    strParameterName = "/" + strGroup + "/" + strParameterName;
    strParameter = pSwAlramConfigIni->value(strParameterName).toString();
    return strParameter;
}

/**
 * 功 能：读整形串配置文件
 * 开发人员：Lzy     2016 - 七夕
 */
int sw_alram_configFile_readInt(QString strParameterName, QString strGroup)
{
    int nParameter = -1;
    strParameterName = "/" + strGroup + "/" + strParameterName;
    nParameter = pSwAlramConfigIni->value(strParameterName).toInt();

    return nParameter;
}

/**
 * 功 能：判断配置文件键是否存在
 * 开发人员：Lzy     2016 - 七夕
 */
bool sw_alram_configFile_contains(QString strParameterName, QString strGroup)
{
    bool ret = false;
    strParameterName = "/" + strGroup + "/" + strParameterName;
    ret = pSwAlramConfigIni->contains(strParameterName);

    return ret;
}


/**
 * 功 能：读浮点形串配置文件
 * 开发人员：Lzy     2013 - 七夕
 */
double sw_alram_configFile_readDouble(QString strParameterName, QString strGroup)
{
    double fParameter = 0;
    strParameterName = "/" + strGroup + "/" + strParameterName;
    fParameter = pSwAlramConfigIni->value(strParameterName).toDouble();

    return fParameter;
}

/**
 * 功 能：参数写入配置文件
 * 开发人员：Lzy     2016 - 七夕
 */
void sw_alram_configFile_write(QString strParameterName, QString strParameter, QString strGroup)
{
    strParameterName = "/" + strGroup + "/" + strParameterName;
    pSwAlramConfigIni->setValue(strParameterName, strParameter);
}

/**
 * 功 能：写入参数
 * 开发人员：Lzy     2016 - 七夕
 */
void sw_alram_configFile_writeParam(QString name, QString value, QString strGroup)
{
    sw_alram_configFile_open();
    sw_alram_configFile_write(name, value, strGroup);
    sw_alram_configFile_close();
}


