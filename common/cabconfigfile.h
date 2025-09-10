#ifndef CABCONFIGFILE_H
#define CABCONFIGFILE_H
#include <QString>


extern bool cab_configFile_open(void);
extern void cab_configFile_close(void);
extern QString cab_configFile_readStr(QString, QString strGroup="CabCol1");
extern int cab_configFile_readInt(QString, QString strGroup="CabCol1");
extern double cab_configFile_readDouble(QString, QString strGroup="CabCol1");
extern void cab_configFile_write(QString, QString, QString strGroup="CabCol1");
extern bool cab_configFile_contains(QString strParameterName, QString strGroup="CabCol1");

extern void cab_configFile_writeParam(QString, QString, QString strGroup="CabCol1");


#endif // CABCONFIGFILE_H

