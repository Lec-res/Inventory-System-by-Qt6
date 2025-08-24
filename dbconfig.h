#ifndef DBCONFIG_H
#define DBCONFIG_H

#include <QString>

// 这个结构体专门用于存储和传递数据库配置信息
struct DbConfig {
    QString host = "127.0.0.1";
    int port = 3306;
    QString dbName = "inventory_db";
    QString user = "root";
    QString password;
};

#endif // DBCONFIG_H
