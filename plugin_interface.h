#ifndef PLUGIN_INTERFACE_H
#define PLUGIN_INTERFACE_H

#include <QString>
#include <QMainWindow>

// 向前声明 DatabaseManager 类。
// 这可以避免为了一个指针类型而包含完整的 "databasemanager.h" 头文件，
// 有效地防止了头文件循环依赖的问题。
class DatabaseManager;

/// @interface InventoryPluginInterface
/// @brief “动作”插件的接口类 (Interface)。
/// @details 所有希望为系统添加一个“动作”(如导入、导出)的插件，都必须继承此类并实现其纯虚函数。
///          这定义了主程序与动作插件之间的“合同”。
class InventoryPluginInterface
{
public:
    virtual ~InventoryPluginInterface() {}

    /// @brief 插件的名称，将显示在主窗口的“插件”菜单中。
    /// @return 插件名称字符串。
    virtual QString name() const = 0;

    /// @brief 插件的简单描述，可用作菜单项的提示文本。
    /// @return 插件描述字符串。
    virtual QString description() const = 0;

    /// @brief 插件执行的入口函数。
    /// @param parentWindow 主程序的主窗口指针，插件可以用它作为父窗口来创建对话框等。
    /// @param dbManager 【已修改】主程序通过此参数将数据库管理器的实例“注入”到插件中。
    virtual void execute(QMainWindow *parentWindow, DatabaseManager* dbManager) = 0;
};

/// @brief 定义接口的唯一标识符 (Interface Identifier)。
/// @details 这个字符串是主程序用来识别和验证插件类型的关键。它必须是独一无二的。
#define InventoryPluginInterface_iid "com.mycompany.InventorySystem.InventoryPluginInterface/1.0"

// 使用Qt宏将C++接口注册到Qt的元对象系统中
Q_DECLARE_INTERFACE(InventoryPluginInterface, InventoryPluginInterface_iid)

#endif // PLUGIN_INTERFACE_H
