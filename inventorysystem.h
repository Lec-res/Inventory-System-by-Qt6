#ifndef INVENTORYSYSTEM_H
#define INVENTORYSYSTEM_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QTextStream>
#include <QDir>
#include <QFileInfo>
#include <QStringConverter>
#include <QStackedWidget>
#include <QPluginLoader>
#include <QProgressBar>

#include "iteminfo.h"
#include "itemformwidget.h"
#include "databasemanager.h"
#include "settingsmanager.h"
#include "settingsdialog.h"
#include "plugin_interface.h"
#include "plugin_interface_ui.h"
#include "updatechecker.h"

class InventorySystem : public QMainWindow
{
    Q_OBJECT

public:
    InventorySystem(QWidget *parent = nullptr);
    ~InventorySystem();

private slots:
    // --- 业务逻辑槽函数 ---
    void onAddItem();
    void onUpdateItem();
    void onDeleteItem();
    void onClearForm();
    void onTableSelectionChanged();
    void onSearch();
    void onResetSearch();

    // --- 菜单动作槽函数 ---
    void onExportToCSV();
    void onShowStatistics();
    void onShowSettings();

    // --- 更新检查相关的槽函数 ---
    void onUpdateAvailable(const QString& version, const QString& releaseNotes, const QUrl& downloadUrl);
    void onNoUpdateAvailable();
    void onUpdateError(const QString& error);
    void onUpdateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void onUpdateDownloadFinished(const QString& savedPath);

private:
    // --- 初始化函数 ---
    void setupUILayout();
    void setupMenuBar();
    void setupStatusBar();
    void setupConnections();

    // --- 核心功能函数 ---
    void refreshTable();
    void updateStatusBar();
    void loadAndRegisterPlugins();
    void applyBackgroundStyle(const QString& resourcePath);

    // --- 辅助函数 ---
    void populateTable(const QList<ItemInfo>& items);
    bool validateForm(const ItemInfo &item);

    // --- 成员变量 ---
    int currentItemId = -1;
    QStringList categories;
    QStringList statusOptions;

    // --- UI 控件指针 ---
    ItemFormWidget *formWidget;
    QTableWidget *itemTable;
    QLineEdit *searchEdit;
    QComboBox *searchCategoryComboBox;
    QStackedWidget *mainContentArea;
    QWidget *defaultView;
    QLabel *itemCountLabel;
    QLabel *totalValueLabel;
    QProgressBar* downloadProgressBar;

    // --- 功能模块指针 ---
    UpdateChecker* updateChecker;

    // --- 数据存储 ---
    QList<InventoryPluginInterface*> loadedActionPlugins;
};
#endif // INVENTORYSYSTEM_H
