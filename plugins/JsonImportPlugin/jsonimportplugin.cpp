#include "jsonimportplugin.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>
#include "databasemanager.h" 
#include "iteminfo.h"

QString JsonImportPlugin::name() const
{
    return "从 JSON 批量导入";
}

QString JsonImportPlugin::description() const
{
    return "从一个 JSON 文件中读取物品数据并批量添加到数据库。";
}

// 【修改】execute 函数的定义和实现
void JsonImportPlugin::execute(QMainWindow *parentWindow, DatabaseManager* dbManager)
{
    if (!dbManager) {
        QMessageBox::critical(parentWindow, "错误", "数据库管理器无效！");
        return;
    }

    QString filePath = QFileDialog::getOpenFileName(parentWindow, "选择 JSON 文件", "", "JSON Files (*.json)");
    if (filePath.isEmpty()) {
        return;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(parentWindow, "错误", "无法打开文件！");
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) {
        QMessageBox::critical(parentWindow, "格式错误", "JSON 文件顶层必须是一个数组！");
        return;
    }

    int successCount = 0;
    QJsonArray itemsArray = doc.array();
    for (const QJsonValue &value : itemsArray) {
        QJsonObject obj = value.toObject();
        ItemInfo item;
        item.name = obj["name"].toString();
        item.category = obj["category"].toString();
        item.quantity = obj["quantity"].toInt();
        item.price = obj["price"].toDouble();
        
        // 【核心修改】使用主程序传递进来的 dbManager 指针
        if (!item.name.isEmpty() && dbManager->addItem(item)) {
            successCount++;
        }
    }

    QMessageBox::information(parentWindow, "导入完成", QString("成功导入 %1 条物品信息！").arg(successCount));
}