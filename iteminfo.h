#ifndef ITEMINFO_H
#define ITEMINFO_H

#include <QString>
#include <QDate>

struct ItemInfo {
    int id = -1;
    QString name;
    QString category;
    QString description;
    int quantity = 1;
    double price = 0.0;
    QString supplier;
    QDate purchaseDate = QDate::currentDate();
    QString location;
    QString status;
};

#endif // ITEMINFO_H
