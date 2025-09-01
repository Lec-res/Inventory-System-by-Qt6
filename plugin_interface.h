#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <QObject>      // <-- 【新增】需要继承 QObject
#include <QString>
#include <QMainWindow>
#include "iteminfo.h"   // <-- 【新增】因为信号会使用 ItemInfo

/// @interface InventoryPluginInterface
/// @brief “动作”插件的接口类（已重构为信号槽模式）。
/// @details 插件不再直接执行数据库操作，而是发出请求信号，由主程序处理。
class InventoryPluginInterface : public QObject // <-- 【修改】继承 QObject
{
    Q_OBJECT // <-- 【新增】添加Q_OBJECT宏以支持信号槽

public:
    // 构造函数需要一个 QObject parent
    InventoryPluginInterface(QObject* parent = nullptr) : QObject(parent) {}
    virtual ~InventoryPluginInterface() {}

    // name() 和 description() 保持不变
    virtual QString name() const = 0;
    virtual QString description() const = 0;

    /// @brief 插件执行的入口函数，签名恢复为简单版本。
    virtual void execute(QMainWindow *parentWindow) = 0;

signals:
    /// @brief 【核心】插件通过此信号请求主程序添加一个物品。
    /// @param item 需要被添加的物品信息。
    void requestAddItem(const ItemInfo& item);

    // 未来可以扩展，例如:
    // void requestUpdateItem(const ItemInfo& item);
    // void requestDeleteItem(int id);
};

#define InventoryPluginInterface_iid "com.mycompany.InventorySystem.InventoryPluginInterface/2.0" // 版本号更新
Q_DECLARE_INTERFACE(InventoryPluginInterface, InventoryPluginInterface_iid)

#endif // PLUGIN_INTERFACE_H
