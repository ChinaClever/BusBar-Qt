#ifndef DBOPERATION_H
#define DBOPERATION_H

#include "basicsql.h"

#define DB_TABLE_OPERATION "operation"
#define DB_TABLE_OPERATION_EN "operation_en"

struct DbOperationItem : public DbBasicItem{
    QString type;
    QString msg;
};


class DbOperation : public SqlBasic<DbOperationItem>
{    
public:
    DbOperation();
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_OPERATION).arg(1) ;}
    bool insertItem(DbOperationItem &item); // 插入
    bool insertOperation(const QString &type,const QString &str);
    void createTable();

protected:
    bool modifyItem(const DbOperationItem& item,const QString& cmd);
    void selectItem(QSqlQuery &query,DbOperationItem &item);
};


class DbOperation2 : public DbOperation
{
public:
    DbOperation2(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_OPERATION).arg(2) ;}
};


class DbOperation3 : public DbOperation
{
public:
    DbOperation3(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_OPERATION).arg(3) ;}
};

class DbOperation4 : public DbOperation
{
public:
    DbOperation4(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_OPERATION).arg(4) ;}
};

class DbOperationEN1 : public DbOperation
{
public:
    DbOperationEN1(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_OPERATION_EN).arg(1) ;}
};

class DbOperationEN2 : public DbOperation
{
public:
    DbOperationEN2(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_OPERATION_EN).arg(2) ;}
};


class DbOperationEN3 : public DbOperation
{
public:
    DbOperationEN3(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_OPERATION_EN).arg(3) ;}
};

class DbOperationEN4 : public DbOperation
{
public:
    DbOperationEN4(){ createTable(); }
    QString tableName(){return QString("%1_%2").arg(DB_TABLE_OPERATION_EN).arg(4) ;}
};

DbOperation* db_operation_obj(int id);
DbOperation* db_operation_obj_en(int id);
#endif // DBOPERATION_H
