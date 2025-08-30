#ifndef PLUGIN_INTERFACE_UI_H
#define PLUGIN_INTERFACE_UI_H

#include <QString>
#include <QIcon>
#include <QWidget>

// 定义一个专门用于提供可视化界面的接口
class UIVisualPluginInterface
{
public:
    virtual ~UIVisualPluginInterface() {}

    // 插件的名称，用于菜单项或Tab标题
    virtual QString name() const = 0;

    // 插件的图标，可以显示在菜单或Tab上
    virtual QIcon icon() const = 0;

    // 【核心】创建并返回一个QWidget指针，主程序将把它嵌入到UI中
    virtual QWidget* createWidget(QWidget* parent) = 0;
};

// 为这个新接口也定义一个唯一的ID
#define UIVisualPluginInterface_iid "com.mycompany.InventorySystem.UIVisualPluginInterface/1.0"
Q_DECLARE_INTERFACE(UIVisualPluginInterface, UIVisualPluginInterface_iid)

#endif // PLUGIN_INTERFACE_UI_H
