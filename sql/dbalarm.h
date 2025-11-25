#ifndef DBALARM_H
#define DBALARM_H

#include "basicsql.h"

#define DB_TABLE_ALARM "alarm"
#define DB_TABLE_ALARM_EN "alarm_en"

struct DbAlarmItem : public DbBasicItem{
    QString type;
    QString msg;
};


class DbAlarm : public SqlBasic<DbAlarmItem>
{    
public:
    DbAlarm();
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_ALARM).arg(1) ;}
    bool insertItem(DbAlarmItem &item); // 插入

    void createTable();
protected:
    bool modifyItem(const DbAlarmItem& item,const QString& cmd);
    void selectItem(QSqlQuery &query,DbAlarmItem &item);
};


class DbAlarm2 : public DbAlarm
{
public:
     DbAlarm2(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_ALARM).arg(2) ;}
};


class DbAlarm3 : public DbAlarm
{
public:
     DbAlarm3(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_ALARM).arg(3) ;}
};

class DbAlarm4 : public DbAlarm
{
public:
     DbAlarm4(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_ALARM).arg(4) ;}
};

class DbAlarmEN1 : public DbAlarm
{
public:
    DbAlarmEN1(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_ALARM_EN).arg(1) ;}
};

class DbAlarmEN2 : public DbAlarm
{
public:
    DbAlarmEN2(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_ALARM_EN).arg(2) ;}
};


class DbAlarmEN3 : public DbAlarm
{
public:
    DbAlarmEN3(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_ALARM_EN).arg(3) ;}
};

class DbAlarmEN4 : public DbAlarm
{
public:
    DbAlarmEN4(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_ALARM_EN).arg(4) ;}
};

DbAlarm* db_alarm_obj(int id);
DbAlarm* db_alarm_obj_en(int id);

#endif // DBALARM_H
