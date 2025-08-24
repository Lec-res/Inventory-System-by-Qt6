#include <QApplication>
#include <QStyleFactory>
#include <QDir>
#include <QStandardPaths>
#include <QLoggingCategory>
#include "inventorysystem.h"
#include <QIcon> // <-- 1. 包含 QIcon 头文件

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // 设置应用程序信息
    app.setApplicationName("物品信息管理系统");
    app.setApplicationVersion("2.0");
    app.setOrganizationName("InventoryManagement");
    app.setOrganizationDomain("inventorymanagement.local");

    // 设置应用程序样式
    app.setStyle(QStyleFactory::create("Fusion"));

    // 设置应用程序图标（如果有的话）
    // app.setWindowIcon(QIcon(":/icons/app_icon.png"));

    // 确保应用程序数据目录存在
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataPath);

    // 设置日志过滤
    QLoggingCategory::setFilterRules("qt.sql.*.debug=false");

    // 创建并显示主窗口
    InventorySystem window;
    window.show();

    return app.exec();
}
