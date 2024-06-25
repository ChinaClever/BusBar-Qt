#ifndef DBSYSTEM_H
#define DBSYSTEM_H

#include "basicsql.h"

#define DB_TABLE_SYSTEM "system"

struct DbSystemItem : public DbBasicItem{
    QString msg;
};


class DbSystem: public SqlBasic<DbSystemItem>
{    
public:
    DbSystem();
    QString tableName(){return QString("%1").arg(DB_TABLE_SYSTEM);}
    bool insertItem(DbSystemItem &item); // 插入
    bool insertSystem(const QString &str);

    void createTable();
protected:
    bool modifyItem(const DbSystemItem& item,const QString& cmd);
    void selectItem(QSqlQuery &query,DbSystemItem &item);
};
DbSystem* db_system_obj();

#endif // DBSYSTEM_H
