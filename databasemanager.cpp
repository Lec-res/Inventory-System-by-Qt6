#include "databasemanager.h"

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager dm_instance;
    return dm_instance;
}

DatabaseManager::DatabaseManager() {}

DatabaseManager::~DatabaseManager() {
    if (database.isOpen()) {
        database.close();
    }
}

bool DatabaseManager::initDatabase(const DbConfig& config) {
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("inventory.db");

    if (!database.open()) {
        qDebug() << "Database connection failed:" << database.lastError().text();
        return false;
    }

    QSqlQuery query;
    const QString createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name VARCHAR(100) NOT NULL,
            category VARCHAR(50),
            description TEXT,
            quantity INTEGER DEFAULT 0,
            price REAL DEFAULT 0.0,
            supplier VARCHAR(100),
            purchase_date DATE,
            location VARCHAR(100),
            status VARCHAR(20) DEFAULT '正常',
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
            updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )";

    if (!query.exec(createTableSQL)) {
        qDebug() << "Failed to create table:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::addItem(const ItemInfo &item) {
    QSqlQuery query;
    query.prepare(R"(
        INSERT INTO items (name, category, description, quantity, price, supplier, purchase_date, location, status)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)
    )");
    query.addBindValue(item.name);
    query.addBindValue(item.category);
    query.addBindValue(item.description);
    query.addBindValue(item.quantity);
    query.addBindValue(item.price);
    query.addBindValue(item.supplier);
    query.addBindValue(item.purchaseDate);
    query.addBindValue(item.location);
    query.addBindValue(item.status);

    if (!query.exec()) {
        qDebug() << "Add item failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateItem(const ItemInfo &item) {
    QSqlQuery query;
    query.prepare(R"(
        UPDATE items SET name=?, category=?, description=?, quantity=?, price=?,
        supplier=?, purchase_date=?, location=?, status=?, updated_at=CURRENT_TIMESTAMP
        WHERE id=?
    )");
    query.addBindValue(item.name);
    query.addBindValue(item.category);
    query.addBindValue(item.description);
    query.addBindValue(item.quantity);
    query.addBindValue(item.price);
    query.addBindValue(item.supplier);
    query.addBindValue(item.purchaseDate);
    query.addBindValue(item.location);
    query.addBindValue(item.status);
    query.addBindValue(item.id);

    if (!query.exec()) {
        qDebug() << "Update item failed:" << query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteItem(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM items WHERE id = ?");
    query.addBindValue(id);
    if (!query.exec()) {
        qDebug() << "Delete item failed:" << query.lastError().text();
        return false;
    }
    return true;
}

QList<ItemInfo> DatabaseManager::getAllItems() const {
    QList<ItemInfo> items;
    QSqlQuery query("SELECT * FROM items ORDER BY updated_at DESC, id DESC");
    while (query.next()) {
        items.append(queryToItem(query));
    }
    return items;
}

QList<ItemInfo> DatabaseManager::searchItems(const QString &keyword, const QString &category) const {
    QList<ItemInfo> items;
    QString sql = "SELECT * FROM items WHERE 1=1";
    if (!keyword.isEmpty()) {
        sql += " AND (name LIKE ? OR supplier LIKE ? OR location LIKE ?)";
    }
    if (!category.isEmpty() && category != "全部类别") {
        sql += " AND category = ?";
    }
    sql += " ORDER BY updated_at DESC, id DESC";

    QSqlQuery query;
    query.prepare(sql);
    if (!keyword.isEmpty()) {
        QString searchPattern = "%" + keyword + "%";
        query.addBindValue(searchPattern);
        query.addBindValue(searchPattern);
        query.addBindValue(searchPattern);
    }
    if (!category.isEmpty() && category != "全部类别") {
        query.addBindValue(category);
    }

    if (query.exec()) {
        while(query.next()) {
            items.append(queryToItem(query));
        }
    } else {
        qDebug() << "Search failed:" << query.lastError().text();
    }
    return items;
}

StatisticsData DatabaseManager::getStatistics() const {
    StatisticsData stats;
    QSqlQuery query;

    query.exec("SELECT COUNT(*), SUM(quantity * price) FROM items");
    if (query.next()) {
        stats.totalItemTypes = query.value(0).toInt();
        stats.totalValue = query.value(0).toDouble();
    }

    query.exec("SELECT category, COUNT(*), SUM(quantity * price) FROM items GROUP BY category");
    while(query.next()){
        QString category = query.value(0).toString();
        stats.countByCategory[category] = query.value(1).toInt();
        stats.valueByCategory[category] = query.value(2).toDouble();
    }

    query.exec("SELECT status, COUNT(*) FROM items GROUP BY status");
    while(query.next()){
        stats.countByStatus[query.value(0).toString()] = query.value(1).toInt();
    }

    return stats;
}

ItemInfo DatabaseManager::queryToItem(const QSqlQuery& query) const {
    ItemInfo item;
    item.id = query.value("id").toInt();
    item.name = query.value("name").toString();
    item.category = query.value("category").toString();
    item.description = query.value("description").toString();
    item.quantity = query.value("quantity").toInt();
    item.price = query.value("price").toDouble();
    item.supplier = query.value("supplier").toString();
    item.purchaseDate = query.value("purchase_date").toDate();
    item.location = query.value("location").toString();
    item.status = query.value("status").toString();
    return item;
}
