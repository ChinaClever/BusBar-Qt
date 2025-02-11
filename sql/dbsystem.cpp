/*
 * dbsystem.cpp
 *
 *
 *
 *  Created on: 2017年10月1日
 *      Author: Lzy
 */
#include "dbsystem.h"

DbSystem::DbSystem()
{
    createTable();
}

void DbSystem::createTable()
{
    QString cmd =
            "create table if not exists %1("
            "id             INTEGER primary key autoincrement not null,"
            "date           VCHAR,"
            "time           VCHAR,"
            "msg            TEXT);";
    QSqlQuery query(mDb);
    if(!query.exec(cmd.arg(tableName())))
    {
        throwError(query.lastError());
    }
}

bool DbSystem::insertItem(DbSystemItem& item)
{
    bool ret = false;
    item.id = maxId()+1;
    QString cmd = "insert into %1 (id, date, time, msg) "
            "values(:id,:date,:time,:msg)";
    ret = modifyItem(item,cmd.arg(tableName()));
    if(ret)
        emit itemChanged(item.id,Insert);
    return ret;
}

bool DbSystem::insertSystem(const QString &str)
{
    DB_Tran tran;
    DbSystemItem item;
    item.msg = str;
    return insertItem(item);
}


bool DbSystem::modifyItem(const DbSystemItem &item, const QString &cmd)
{
    bool ret = false;
    QSqlQuery query(mDb);
    query.prepare(cmd);
    query.bindValue(":id",item.id);
    query.bindValue(":date",item.date);
    query.bindValue(":time",item.time);
    query.bindValue(":msg",item.msg);
    ret = query.exec();
    if(!ret)
        throwError(query.lastError());
    return ret;
}

void DbSystem::selectItem(QSqlQuery &query,DbSystemItem &item)
{
    item.id = query.value("id").toInt();
    item.date = query.value("date").toString();
    item.time = query.value("time").toString();
    item.msg = query.value("msg").toString();
}


DbSystem* db_system_obj()
{
    static DbSystem* sington1  = new DbSystem();

    DbSystem* sington = sington1;

    return sington;
}

DbSystem* db_system_obj_en()
{
    static DbSystem* sington1  = new DbSystem_en();

    DbSystem* sington = sington1;

    return sington;
}


