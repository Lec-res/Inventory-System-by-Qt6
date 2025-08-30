#ifndef JSONIMPORTPLUGIN_H
#define JSONIMPORTPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "plugin_interface.h" // 包含主程序的接口头文件

class JsonImportPlugin : public QObject, public InventoryPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(InventoryPluginInterface)
    Q_PLUGIN_METADATA(IID InventoryPluginInterface_iid FILE "plugin.json")

public:
    QString name() const override;
    QString description() const override;
    
    // 【修改】execute 函数的声明以匹配新接口
    void execute(QMainWindow *parentWindow, DatabaseManager* dbManager) override;
};

#endif // JSONIMPORTPLUGIN_H