#ifndef SWALRAMCONFIGFILE_H
#define SWALRAMCONFIGFILE_H
#include <QString>


extern bool sw_alram_configFile_open(void);
extern void sw_alram_configFile_close(void);
extern QString sw_alram_configFile_readStr(QString, QString strGroup="Bus1");
extern int sw_alram_configFile_readInt(QString, QString strGroup="Bus1");
extern double sw_alram_configFile_readDouble(QString, QString strGroup="Bus1");
extern void sw_alram_configFile_write(QString, QString, QString strGroup="Bus1");
extern bool sw_alram_configFile_contains(QString strParameterName, QString strGroup="Bus1");

extern void sw_alram_configFile_writeParam(QString, QString, QString strGroup="Bus1");


#endif // SWALRAMCONFIGFILE_H

