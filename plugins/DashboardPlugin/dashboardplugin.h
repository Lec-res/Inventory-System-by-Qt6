#ifndef DASHBOARDPLUGIN_H
#define DASHBOARDPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "../inventorysystem-formal/plugin_interface_ui.h" // 包含新的UI接口

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
