#ifndef DASHBOARDPLUGIN_H
#define DASHBOARDPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "plugin_interface_ui.h" // 确保包含的是重命名后的 UI 接口文件

class DashboardPlugin : public QObject, public UIVisualPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(UIVisualPluginInterface)
    Q_PLUGIN_METADATA(IID UIVisualPluginInterface_iid FILE "plugin.json")

public:
    QString name() const override;
    QIcon icon() const override;
    QWidget* createWidget(QWidget* parent) override;
};

#endif // DASHBOARDPLUGIN_H