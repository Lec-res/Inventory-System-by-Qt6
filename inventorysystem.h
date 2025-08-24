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

#include "iteminfo.h"
#include "itemformwidget.h"
#include "databasemanager.h"
#include "settingsmanager.h"
#include "settingsdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class InventorySystem; }
QT_END_NAMESPACE

class InventorySystem : public QMainWindow {
    Q_OBJECT

public:
    InventorySystem(QWidget *parent = nullptr);
    ~InventorySystem();

private slots:
    // Form Actions
    void onAddItem();
    void onUpdateItem();
    void onDeleteItem();
    void onClearForm();

    // Table and Search Actions
    void onTableSelectionChanged();
    void onSearch();
    void onResetSearch();

    // Menu Actions
    void onExportToCSV();
    void onShowStatistics();

    void onShowSettings();
private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupConnections();

    void refreshTable();
    void populateTable(const QList<ItemInfo>& items);
    void updateStatusBar();
    bool validateForm(const ItemInfo &item);

    // Member variables
    int currentItemId = -1;
    QStringList categories;
    QStringList statusOptions;

    // UI Widgets
    QWidget *centralWidget;
    ItemFormWidget *formWidget;
    QTableWidget *itemTable;
    QLineEdit *searchEdit;
    QComboBox *searchCategoryComboBox;

    // Status Bar
    QLabel *itemCountLabel;
    QLabel *totalValueLabel;
};
#endif // INVENTORYSYSTEM_H
