#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include <QVariant>
#include <QDebug>
#include "iteminfo.h"

// Struct to hold calculated statistics
struct StatisticsData {
    int totalItemTypes = 0;
    double totalValue = 0.0;
    QMap<QString, int> countByCategory;
    QMap<QString, double> valueByCategory;
    QMap<QString, int> countByStatus;
};

class DatabaseManager {
public:
    static DatabaseManager& instance();
    bool initDatabase();

    bool addItem(const ItemInfo &item);
    bool updateItem(const ItemInfo &item);
    bool deleteItem(int id);
    QList<ItemInfo> getAllItems() const;
    QList<ItemInfo> searchItems(const QString &keyword, const QString &category) const;

    StatisticsData getStatistics() const;

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    QSqlDatabase database;
    ItemInfo queryToItem(const QSqlQuery& query) const;
};

#endif // DATABASEMANAGER_H
