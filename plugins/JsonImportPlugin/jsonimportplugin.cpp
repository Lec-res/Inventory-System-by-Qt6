#include "jsonimportplugin.h"
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QDebug>
#include "iteminfo.h" // <-- 【修改】不再需要 databasemanager.h

QString JsonImportPlugin::name() const
{
    return "从 JSON 批量导入";
}

QString JsonImportPlugin::description() const
{
    return "从一个 JSON 文件中读取物品数据并批量添加到数据库。";
}

void JsonImportPlugin::execute(QMainWindow *parentWindow)
{
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
        
        if (!item.name.isEmpty()) {
            // 【核心修改】发出请求信号，而不是直接调用数据库函数
            emit requestAddItem(item);
            successCount++;
        }
    }

    QMessageBox::information(parentWindow, "请求完成", QString("已发送 %1 条物品信息的添加请求！\n请在主界面查看结果。").arg(successCount));
}