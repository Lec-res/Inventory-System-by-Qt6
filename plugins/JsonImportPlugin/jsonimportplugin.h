#ifndef JSONIMPORTPLUGIN_H
#define JSONIMPORTPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "plugin_interface.h"

class JsonImportPlugin : public InventoryPluginInterface // <-- 【修改】直接继承新接口
{
    Q_OBJECT
    Q_INTERFACES(InventoryPluginInterface)
    Q_PLUGIN_METADATA(IID InventoryPluginInterface_iid FILE "plugin.json")

public:
    // 构造函数
    JsonImportPlugin(QObject* parent = nullptr) : InventoryPluginInterface(parent) {}
    
    // 实现接口函数
    QString name() const override;
    QString description() const override;
    void execute(QMainWindow *parentWindow) override;
};

#endif // JSONIMPORTPLUGIN_H