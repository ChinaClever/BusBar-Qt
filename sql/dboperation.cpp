/*
 * dboperation.cpp
 *
 *
 *
 *  Created on: 2017年10月1日
 *      Author: Lzy
 */
#include "dboperation.h"

DbOperation::DbOperation()
{
    createTable();
}

void DbOperation::createTable()
{
    QString cmd =
            "create table if not exists %1("
            "id             INTEGER primary key autoincrement not null,"
            "date           VCHAR,"
            "time           VCHAR,"
            "type           VCHAR,"
            "msg            TEXT);";
    QSqlQuery query(mDb);
    if(!query.exec(cmd.arg(tableName())))
    {
        throwError(query.lastError());
    }
}

bool DbOperation::insertItem(DbOperationItem& item)
{
    bool ret = false;
    item.id = maxId()+1;
    QString cmd = "insert into %1 (id, date, time,type, msg) "
            "values(:id,:date,:time,:type,:msg)";
    ret = modifyItem(item,cmd.arg(tableName()));
    if(ret)
        emit itemChanged(item.id,Insert);
    return ret;
}

bool DbOperation::insertOperation(const QString &type,const QString &str)
{
    DB_Tran tran;
    DbOperationItem item;
    item.type = type;
    item.msg = str;
    return insertItem(item);
}


bool DbOperation::modifyItem(const DbOperationItem &item, const QString &cmd)
{
    bool ret = false;
    QSqlQuery query(mDb);
    query.prepare(cmd);
    query.bindValue(":id",item.id);
    query.bindValue(":date",item.date);
    query.bindValue(":time",item.time);
    query.bindValue(":type",item.type);
    query.bindValue(":msg",item.msg);
    ret = query.exec();
    if(!ret)
        throwError(query.lastError());
    return ret;
}

void DbOperation::selectItem(QSqlQuery &query,DbOperationItem &item)
{
    item.id = query.value("id").toInt();
    item.date = query.value("date").toString();
    item.time = query.value("time").toString();
    item.type = query.value("type").toString();
    item.msg = query.value("msg").toString();
}


DbOperation* db_operation_obj(int id)
{
    static DbOperation* sington1  = new DbOperation();
    static DbOperation2* sington2  = new DbOperation2();
    static DbOperation3* sington3  = new DbOperation3();
    static DbOperation4* sington4  = new DbOperation4();

    DbOperation* sington = NULL;
    switch (id)
    {
    case 0:
        sington = sington1;
        break;

    case 1:
        sington = sington2;
        break;

    case 2:
        sington = sington3;
        break;

    case 3:
        sington = sington4;
        break;
    }
    return sington;
}

