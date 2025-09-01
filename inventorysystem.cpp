#include "inventorysystem.h"
#include <QMenu>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QApplication>
#include <QTimer> // 用于延迟调用
#include <QFile>   // 用于检查文件是否存在

// 在代码文件的顶部定义常量，方便管理
// 【重要】请将这里的用户名和仓库名替换为您自己的
const QString GITHUB_USER = "Lec-res";
const QString GITHUB_REPO = "Inventory-system-by-Qt6";
// 我们要下载的发布包文件名

// 在代码中定义当前版本
const QString CURRENT_VERSION = "v3.1.0";


InventorySystem::InventorySystem(QWidget *parent)
    : QMainWindow(parent), currentItemId(-1)
{
    // 1. 初始化基础数据
    categories << "电子产品" << "办公用品" << "家具" << "工具" << "消耗品" << "其他";
    statusOptions << "正常" << "维修中" << "已报废" << "借出" << "丢失";

    // 2.【修复】搭建UI布局必须在最前面，这样后续代码才能访问UI控件
    setupUILayout();

    // 3. 从文件加载数据库配置
    DbConfig config = SettingsManager::instance().loadDbConfig();

    // 4. 使用配置初始化数据库
    if (!DatabaseManager::instance().initDatabase(config)) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::critical(this, "数据库错误",
                                      "无法连接到数据库！\n请检查配置文件(config.ini)或在“文件”->“设置”中修改连接信息。",
                                      QMessageBox::Ok | QMessageBox::Open);

        if (reply == QMessageBox::Open) {
            // 使用QTimer::singleShot可以避免在构造函数中直接打开模态对话框可能引发的问题
            QTimer::singleShot(0, this, &InventorySystem::onShowSettings);
        }
        // 【修复】正确地禁用中央控件
        centralWidget()->setEnabled(false);
    }

    // 5. 初始化菜单栏、状态栏和信号槽连接
    setupMenuBar();
    setupStatusBar();
    setupConnections();

    // 6. 初始化更新检查器
    updateChecker = new UpdateChecker(this);
    connect(updateChecker, &UpdateChecker::updateAvailable, this, &InventorySystem::onUpdateAvailable);
    connect(updateChecker, &UpdateChecker::noUpdateAvailable, this, &InventorySystem::onNoUpdateAvailable);
    connect(updateChecker, &UpdateChecker::errorOccurred, this, &InventorySystem::onUpdateError);
    connect(updateChecker, &UpdateChecker::downloadProgress, this, &InventorySystem::onUpdateDownloadProgress);
    connect(updateChecker, &UpdateChecker::downloadFinished, this, &InventorySystem::onUpdateDownloadFinished);

    // 7. 加载插件
    loadAndRegisterPlugins();

    // 8. 加载并应用上次保存的背景
    QString lastBgPath = SettingsManager::instance().loadCurrentBackground();
    if (!lastBgPath.isEmpty() && QFile::exists(lastBgPath)) {
        applyBackgroundStyle(lastBgPath);
    } else {
        applyBackgroundStyle("");
    }

    // 9. 填充初始数据
    formWidget->setPredefinedCategories(categories);
    formWidget->setStatusOptions(statusOptions);
    searchCategoryComboBox->addItems(categories);
    refreshTable();

    // 10. 设置窗口最终属性
    setWindowTitle("模块化物品信息管理系统 v3.0");
    setMinimumSize(1200, 700);
    resize(1400, 800);
}

InventorySystem::~InventorySystem() {}


void InventorySystem::setupUILayout() {
    // 【修复】创建一个QWidget作为中央控件的容器
    QWidget* mainCentralWidget = new QWidget;
    setCentralWidget(mainCentralWidget); // 【修复】调用setCentralWidget()函数来设置

    auto mainSplitter = new QSplitter(Qt::Horizontal, mainCentralWidget);

    formWidget = new ItemFormWidget;
    formWidget->setMaximumWidth(400);
    formWidget->setMinimumWidth(350);

    // --- 实现UI插件的“舞台” ---
    mainContentArea = new QStackedWidget;
    defaultView = new QWidget;
    auto rightLayout = new QVBoxLayout(defaultView);

    // 创建搜索框和表格等默认视图的控件
    auto searchGroupBox = new QGroupBox("搜索筛选");
    auto searchLayout = new QGridLayout(searchGroupBox);
    searchLayout->addWidget(new QLabel("关键词:"), 0, 0);
    searchEdit = new QLineEdit;
    searchLayout->addWidget(searchEdit, 0, 1);
    searchLayout->addWidget(new QLabel("类别:"), 1, 0);
    searchCategoryComboBox = new QComboBox;
    searchCategoryComboBox->addItem("全部类别");
    searchLayout->addWidget(searchCategoryComboBox, 1, 1);
    auto searchButton = new QPushButton("搜索");
    auto resetSearchButton = new QPushButton("重置");
    auto searchButtonLayout = new QHBoxLayout;
    searchButtonLayout->addWidget(searchButton);
    searchButtonLayout->addWidget(resetSearchButton);
    searchLayout->addLayout(searchButtonLayout, 2, 0, 1, 2);
    connect(searchButton, &QPushButton::clicked, this, &InventorySystem::onSearch);
    connect(resetSearchButton, &QPushButton::clicked, this, &InventorySystem::onResetSearch);

    auto tableTitle = new QLabel("物品列表");
    itemTable = new QTableWidget;
    // ... (表格的详细设置不变) ...
    itemTable->setColumnCount(10);
    QStringList headers = {"ID", "物品名称", "类别", "描述", "数量", "单价", "供应商", "购买日期", "存放位置", "状态"};
    itemTable->setHorizontalHeaderLabels(headers);
    itemTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    itemTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    rightLayout->addWidget(searchGroupBox);
    rightLayout->addWidget(tableTitle);
    rightLayout->addWidget(itemTable);
    mainContentArea->addWidget(defaultView);

    mainSplitter->addWidget(formWidget);
    mainSplitter->addWidget(mainContentArea);
    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);

    auto mainLayout = new QHBoxLayout(mainCentralWidget); // 【修复】使用正确的父控件
    mainLayout->addWidget(mainSplitter);
}

void InventorySystem::setupMenuBar()
{
    auto mainMenuBar = menuBar();

    // 文件菜单 (代码不变)
    auto fileMenu = mainMenuBar->addMenu("文件");
    auto settingsAction = new QAction("设置", this);
    connect(settingsAction, &QAction::triggered, this, &InventorySystem::onShowSettings);
    fileMenu->addAction(settingsAction);
    fileMenu->addSeparator();
    auto exportAction = new QAction("导出到CSV", this);
    connect(exportAction, &QAction::triggered, this, &InventorySystem::onExportToCSV);
    fileMenu->addAction(exportAction);
    fileMenu->addSeparator();
    auto exitAction = new QAction("退出", this);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    // 视图菜单
    m_viewMenu = mainMenuBar->addMenu("视图");
    QAction *defaultViewAction = new QAction("主列表视图", this);
    connect(defaultViewAction, &QAction::triggered, this, [this](){
        mainContentArea->setCurrentWidget(defaultView);
    });
    m_viewMenu->addAction(defaultViewAction);
    m_viewMenu->addSeparator();
    QMenu *backgroundMenu = m_viewMenu->addMenu("切换背景");

    // 【修改】为“恢复默认背景”动作实现功能
    QAction *defaultBgAction = new QAction("恢复默认背景", this);
    connect(defaultBgAction, &QAction::triggered, this, [this]() {
        applyBackgroundStyle(""); // 传入空字符串来清除背景
        SettingsManager::instance().saveCurrentBackground(""); // 保存空设置，表示使用默认背景
    });
    backgroundMenu->addAction(defaultBgAction);

    // 【修改】为“从文件选择”动作实现功能
    QAction *chooseFileAction = new QAction("从文件选择...", this);
    connect(chooseFileAction, &QAction::triggered, this, [this]() {
        // 弹出文件选择对话框
        QString filePath = QFileDialog::getOpenFileName(
            this,                                     // 父窗口
            "选择背景图片",                             // 对话框标题
            QDir::homePath(),                         // 默认打开的目录
            "图片文件 (*.png *.jpg *.jpeg *.bmp)"   // 文件类型过滤器
            );

        // 如果用户选择了文件（路径不为空）
        if (!filePath.isEmpty()) {
            applyBackgroundStyle(filePath);
            // 将用户的选择保存到配置文件，以便下次启动时加载
            SettingsManager::instance().saveCurrentBackground(filePath);
        }
    });
    backgroundMenu->addAction(chooseFileAction);

    // 工具菜单 (代码不变)
    auto toolsMenu = mainMenuBar->addMenu("工具");
    auto statisticsAction = new QAction("统计信息", this);
    connect(statisticsAction, &QAction::triggered, this, &InventorySystem::onShowStatistics);
    toolsMenu->addAction(statisticsAction);

    // 插件菜单 (代码不变)
    m_pluginsMenu = mainMenuBar->addMenu("插件");
    QAction *noPluginsAction = new QAction("未找到插件", this);
    noPluginsAction->setEnabled(false);
    m_pluginsMenu->addAction(noPluginsAction);

    // 帮助菜单 (代码不变)
    auto helpMenu = mainMenuBar->addMenu("帮助");
    QAction* checkUpdateAction = new QAction("检查更新...", this);
    connect(checkUpdateAction, &QAction::triggered, this, [this](){
        statusBar()->showMessage("正在检查更新...");
        updateChecker->checkForUpdates();
    });
    helpMenu->addAction(checkUpdateAction);
}

// 【修复】这是包含下载进度条的最终版
void InventorySystem::setupStatusBar()
{
    auto mainStatusBar = statusBar(); // 【修复】调用statusBar()函数
    itemCountLabel = new QLabel;
    totalValueLabel = new QLabel;
    mainStatusBar->addWidget(itemCountLabel);

    downloadProgressBar = new QProgressBar(this);
    downloadProgressBar->setRange(0, 100);
    downloadProgressBar->setValue(0);
    downloadProgressBar->setTextVisible(true);
    downloadProgressBar->setFormat("%p%");
    downloadProgressBar->setMaximumWidth(200);
    downloadProgressBar->hide();
    mainStatusBar->addPermanentWidget(downloadProgressBar);

    mainStatusBar->addPermanentWidget(totalValueLabel);
}


void InventorySystem::setupConnections() {
    connect(formWidget, &ItemFormWidget::addTriggered, this, &InventorySystem::onAddItem);
    connect(formWidget, &ItemFormWidget::updateTriggered, this, &InventorySystem::onUpdateItem);
    connect(formWidget, &ItemFormWidget::deleteTriggered, this, &InventorySystem::onDeleteItem);
    connect(formWidget, &ItemFormWidget::clearTriggered, this, &InventorySystem::onClearForm);
    connect(itemTable, &QTableWidget::itemSelectionChanged, this, &InventorySystem::onTableSelectionChanged);
}

void InventorySystem::onAddItem() {
    ItemInfo item = formWidget->getItemFromForm();
    if (!validateForm(item)) return;

    if (DatabaseManager::instance().addItem(item)) {
        QMessageBox::information(this, "成功", "物品添加成功！");
        onClearForm();
        refreshTable();
        statusBar()->showMessage("成功添加物品：" + item.name, 3000);
    } else {
        QMessageBox::critical(this, "错误", "添加物品失败！");
    }
}

void InventorySystem::onUpdateItem() {
    if (currentItemId == -1) {
        QMessageBox::warning(this, "警告", "请先选择要更新的物品！");
        return;
    }
    ItemInfo item = formWidget->getItemFromForm();
    item.id = currentItemId;
    if (!validateForm(item)) return;

    if (DatabaseManager::instance().updateItem(item)) {
        QMessageBox::information(this, "成功", "物品更新成功！");
        onClearForm();
        refreshTable();
        statusBar()->showMessage("成功更新物品：" + item.name, 3000);
    } else {
        QMessageBox::critical(this, "错误", "更新物品失败！");
    }
}

void InventorySystem::onDeleteItem() {
    if (currentItemId == -1) {
        QMessageBox::warning(this, "警告", "请先选择要删除的物品！");
        return;
    }
    QString itemName = formWidget->getItemFromForm().name;
    int ret = QMessageBox::question(this, "确认删除",
                                    QString("确定要删除物品 \"%1\" 吗？\n此操作不可撤销！").arg(itemName),
                                    QMessageBox::Yes | QMessageBox::No, QMessageBox::No);

    if (ret == QMessageBox::Yes) {
        if (DatabaseManager::instance().deleteItem(currentItemId)) {
            QMessageBox::information(this, "成功", "物品删除成功！");
            onClearForm();
            refreshTable();
            statusBar()->showMessage("成功删除物品：" + itemName, 3000);
        } else {
            QMessageBox::critical(this, "错误", "删除物品失败！");
        }
    }
}

void InventorySystem::onSearch() {
    QString keyword = searchEdit->text().trimmed();
    QString category = searchCategoryComboBox->currentText();
    auto items = DatabaseManager::instance().searchItems(keyword, category);
    populateTable(items);
    statusBar()->showMessage(QString("搜索完成，找到 %1 条记录").arg(itemTable->rowCount()), 3000);
}

void InventorySystem::onResetSearch() {
    searchEdit->clear();
    searchCategoryComboBox->setCurrentIndex(0);
    refreshTable();
}

void InventorySystem::onClearForm() {
    currentItemId = -1;
    itemTable->clearSelection();
    formWidget->clearForm();
    formWidget->enableEditingControls(false);
}

void InventorySystem::onTableSelectionChanged() {
    int currentRow = itemTable->currentRow();
    if (currentRow < 0) return;

    currentItemId = itemTable->item(currentRow, 0)->text().toInt();
    ItemInfo item;
    item.id = currentItemId;
    item.name = itemTable->item(currentRow, 1)->text();
    item.category = itemTable->item(currentRow, 2)->text();
    item.description = itemTable->item(currentRow, 3)->text();
    item.quantity = itemTable->item(currentRow, 4)->text().toInt();
    item.price = itemTable->item(currentRow, 5)->text().toDouble();
    item.supplier = itemTable->item(currentRow, 6)->text();
    item.purchaseDate = QDate::fromString(itemTable->item(currentRow, 7)->text(), "yyyy-MM-dd");
    item.location = itemTable->item(currentRow, 8)->text();
    item.status = itemTable->item(currentRow, 9)->text();

    formWidget->setForm(item);
    formWidget->enableEditingControls(true);
}

void InventorySystem::refreshTable() {
    auto items = DatabaseManager::instance().getAllItems();
    populateTable(items);
    updateStatusBar();
}

void InventorySystem::populateTable(const QList<ItemInfo>& items) {
    itemTable->setRowCount(0);
    itemTable->setSortingEnabled(false); // Disable sorting during population for performance
    for (const auto &item : items) {
        int row = itemTable->rowCount();
        itemTable->insertRow(row);
        itemTable->setItem(row, 0, new QTableWidgetItem(QString::number(item.id)));
        itemTable->setItem(row, 1, new QTableWidgetItem(item.name));
        itemTable->setItem(row, 2, new QTableWidgetItem(item.category));
        itemTable->setItem(row, 3, new QTableWidgetItem(item.description));
        itemTable->setItem(row, 4, new QTableWidgetItem(QString::number(item.quantity)));
        itemTable->setItem(row, 5, new QTableWidgetItem(QString::number(item.price, 'f', 2)));
        itemTable->setItem(row, 6, new QTableWidgetItem(item.supplier));
        itemTable->setItem(row, 7, new QTableWidgetItem(item.purchaseDate.toString("yyyy-MM-dd")));
        itemTable->setItem(row, 8, new QTableWidgetItem(item.location));
        itemTable->setItem(row, 9, new QTableWidgetItem(item.status));

        QColor rowColor;
        if (item.status == "已报废" || item.status == "丢失") rowColor = QColor(255, 200, 200); // Light Red
        else if (item.status == "维修中") rowColor = QColor(255, 255, 200); // Light Yellow
        else if (item.status == "借出") rowColor = QColor(200, 200, 255); // Light Blue

        if (rowColor.isValid()) {
            for (int col = 0; col < itemTable->columnCount(); ++col) {
                itemTable->item(row, col)->setBackground(rowColor);
            }
        }
    }
    itemTable->setSortingEnabled(true);
}

void InventorySystem::onExportToCSV() {
    QString fileName = QFileDialog::getSaveFileName(this, "导出物品清单",
                                                    QDir::homePath() + "/物品清单_" + QDate::currentDate().toString("yyyyMMdd") + ".csv",
                                                    "CSV files (*.csv)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "错误", "无法创建文件：" + fileName);
        return;
    }

    QTextStream stream(&file);
    stream.setEncoding(QStringConverter::Utf8);
    stream << "ID,物品名称,类别,描述,数量,单价,供应商,购买日期,存放位置,状态\n";

    for (int row = 0; row < itemTable->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < itemTable->columnCount(); ++col) {
            QString cellData = itemTable->item(row, col)->text();
            if (cellData.contains(',') || cellData.contains('"')) {
                cellData = '"' + cellData.replace('"', "\"\"") + '"';
            }
            rowData << cellData;
        }
        stream << rowData.join(',') << '\n';
    }
    file.close();
    QMessageBox::information(this, "导出成功", QString("物品清单已导出到：\n%1").arg(fileName));
    statusBar()->showMessage("导出完成：" + QFileInfo(fileName).fileName(), 3000);
}

void InventorySystem::onShowStatistics() {
    StatisticsData stats = DatabaseManager::instance().getStatistics();
    QString categoryStats, statusStats;

    for (auto it = stats.valueByCategory.constBegin(); it != stats.valueByCategory.constEnd(); ++it) {
        categoryStats += QString("%1: %2 种物品, 总价值 ¥%3\n")
                             .arg(it.key()).arg(stats.countByCategory.value(it.key()))
                             .arg(QString::number(it.value(), 'f', 2));
    }
    for (auto it = stats.countByStatus.constBegin(); it != stats.countByStatus.constEnd(); ++it) {
        statusStats += QString("%1: %2 件\n").arg(it.key()).arg(it.value());
    }

    QString message = QString(
                          "=== 物品统计信息 ===\n\n"
                          "总物品种类：%1 种\n"
                          "总估值：¥%2\n\n"
                          "--- 按类别分布 ---\n%3\n"
                          "--- 按状态分布 ---\n%4"
                          ).arg(stats.totalItemTypes)
                          .arg(QString::number(stats.totalValue, 'f', 2))
                          .arg(categoryStats.isEmpty() ? "暂无数据\n" : categoryStats)
                          .arg(statusStats.isEmpty() ? "暂无数据\n" : statusStats);

    QMessageBox::information(this, "统计信息", message);
}

bool InventorySystem::validateForm(const ItemInfo &item) {
    if (item.name.isEmpty()) {
        QMessageBox::warning(this, "验证失败", "物品名称不能为空！");
        return false;
    }
    return true;
}

void InventorySystem::updateStatusBar() {
    StatisticsData stats = DatabaseManager::instance().getStatistics();
    itemCountLabel->setText(QString("总物品种类: %1").arg(stats.totalItemTypes));
    totalValueLabel->setText(QString("总估值: ¥%1").arg(QString::number(stats.totalValue, 'f', 2)));
}

void InventorySystem::onShowSettings() {
    SettingsDialog dialog(this);
    dialog.exec(); // exec() 会以模态方式显示对话框
}

/**
 * @brief 在程序启动时，加载和注册所有插件
 * @details 此函数会：
 * 1. 查找程序目录下的 "plugins" 文件夹。
 * 2. 遍历其中的所有 .dll/.so 文件，并尝试使用 QPluginLoader 加载。
 * 3. 使用 qobject_cast 检查插件是否实现了 UIVisualPluginInterface (UI插件)。
 * - 如果是，则创建其UI，并添加到“视图”菜单。
 * 4. 使用 qobject_cast 检查插件是否实现了 InventoryPluginInterface (动作插件)。
 * - 如果是，则将其信号连接到主窗口的槽，并添加到“插件”菜单。
 */
void InventorySystem::loadAndRegisterPlugins()
{
    // 1. 定位到插件文件夹
    QDir pluginsDir(QCoreApplication::applicationDirPath());
    if (!pluginsDir.cd("plugins")) {
        qDebug() << "Plugins directory not found, skipping plugin load.";
        return;
    }

    // 2. 【已移除】不再需要通过 findChildren 查找菜单
    // auto pluginsMenu = menuBar()->findChildren<QMenu*>("插件").first();
    // auto viewMenu = menuBar()->findChildren<QMenu*>("视图").first();

    // 3. 清理“插件”菜单的默认提示信息
    bool actionPluginFound = false;
    // 【修改】直接使用成员变量，并增加安全检查
    if (m_pluginsMenu) {
        m_pluginsMenu->clear();
    }

    // 4. 遍历插件文件夹中的所有文件
    for (const QString &fileName : pluginsDir.entryList(QDir::Files)) {
        QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
        QObject *plugin = loader.instance();

        if (plugin) {
            // 4a. 检查是否为 UI 插件
            UIVisualPluginInterface *iVisualPlugin = qobject_cast<UIVisualPluginInterface*>(plugin);
            if (iVisualPlugin) {
                QWidget *pluginWidget = iVisualPlugin->createWidget(mainContentArea);
                mainContentArea->addWidget(pluginWidget);

                // 在“视图”菜单中创建对应的切换动作
                QAction *viewAction = new QAction(iVisualPlugin->name(), this);
                viewAction->setIcon(iVisualPlugin->icon());
                if (m_viewMenu) m_viewMenu->addAction(viewAction); // <-- 【修改】使用 m_viewMenu

                // 连接菜单动作，点击时切换到对应的插件UI
                connect(viewAction, &QAction::triggered, this, [this, pluginWidget](){
                    mainContentArea->setCurrentWidget(pluginWidget);
                });
            }

            // 4b. 检查是否为动作插件
            InventoryPluginInterface *iActionPlugin = qobject_cast<InventoryPluginInterface*>(plugin);
            if (iActionPlugin) {
                actionPluginFound = true;
                loadedActionPlugins.append(iActionPlugin);

                // 在“插件”菜单中创建对应的动作
                QAction *pluginAction = new QAction(iActionPlugin->name(), this);
                pluginAction->setToolTip(iActionPlugin->description());
                if (m_pluginsMenu) m_pluginsMenu->addAction(pluginAction); // <-- 【修改】使用 m_pluginsMenu

                // 【核心】将插件发出的“请求”信号，连接到主窗口处理该请求的槽函数
                connect(iActionPlugin, &InventoryPluginInterface::requestAddItem,
                        this, &InventorySystem::onPluginRequestAddItem);

                // 将菜单动作的点击事件，连接到插件的 execute 函数
                connect(pluginAction, &QAction::triggered, this, [this, iActionPlugin]() {
                    iActionPlugin->execute(this);
                });
            }
        } else {
            qDebug() << "Plugin load failed for file" << fileName << ":" << loader.errorString();
        }
    }

    // 5. 如果遍历完所有插件后，一个动作插件都没找到，则恢复提示
    if(!actionPluginFound && m_pluginsMenu){ // <-- 【修改】使用 m_pluginsMenu
        QAction *noPluginsAction = new QAction("未找到插件", this);
        noPluginsAction->setEnabled(false);
        m_pluginsMenu->addAction(noPluginsAction);
    }
}
// =================================================================
//                 新功能槽函数 (更新与主题)
// =================================================================

/**
 * @brief 应用背景样式的核心函数
 * @param path 背景图片的路径 (可以是Qt资源路径或本地文件绝对路径)
 */
void InventorySystem::applyBackgroundStyle(const QString& path)
{
    // 将路径中的反斜杠替换为正斜杠，以兼容QSS的url()语法
    QString forwardPath = path;
    forwardPath.replace('\\', '/');

    if (path.isEmpty()) {
        // 如果路径为空，则清除所有样式，恢复默认外观
        this->setStyleSheet("");
    } else {
        // 使用 QSS 设置主窗口的背景图片
        // 使用 border-image 可以让背景图完美拉伸以适应窗口大小变化
        QString qss = QString("QMainWindow { border-image: url(%1) 0 0 0 0 stretch stretch; }").arg(forwardPath);
        this->setStyleSheet(qss);
    }
}


/**
 * @brief 当检查到有新版本时，由UpdateChecker发出的信号触发此槽
 */
void InventorySystem::onUpdateAvailable(const QString &version, const QString &releaseNotes, const QUrl &downloadUrl)
{
    QString message = QString("发现新版本：%1！\n\n更新日志：\n%2\n\n是否立即下载？").arg(version).arg(releaseNotes);
    QMessageBox::StandardButton reply = QMessageBox::information(this, "检查更新", message, QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        updateChecker->startDownload(downloadUrl);
        downloadProgressBar->setValue(0);
        downloadProgressBar->show();
        statusBar()->showMessage("正在开始下载...");
    }
}

/**
 * @brief 当检查后发现没有新版本时触发
 */
void InventorySystem::onNoUpdateAvailable()
{
    QMessageBox::information(this, "检查更新", "您当前使用的已是最新版本！");
    statusBar()->showMessage("已是最新版本", 3000);
}

/**
 * @brief 在检查更新或下载过程中发生错误时触发
 */
void InventorySystem::onUpdateError(const QString &error)
{
    QMessageBox::critical(this, "更新错误", error);
    statusBar()->showMessage("更新检查失败", 3000);
}

/**
 * @brief 在下载过程中，由UpdateChecker发出的进度信号触发
 */
void InventorySystem::onUpdateDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if (bytesTotal > 0) {
        downloadProgressBar->setValue(100 * bytesReceived / bytesTotal);
    }
    statusBar()->showMessage(QString("正在下载... %1 / %2 MB").arg(bytesReceived / 1024.0 / 1024.0, 0, 'f', 2).arg(bytesTotal / 1024.0 / 1024.0, 0, 'f', 2));
}

/**
 * @brief 当下载完成时触发
 */
void InventorySystem::onUpdateDownloadFinished(const QString &savedPath)
{
    downloadProgressBar->hide();
    statusBar()->showMessage("下载完成！", 5000);
    QMessageBox::information(this, "下载完成", QString("新版本已成功下载至：\n%1\n\n请关闭本程序后，手动解压并运行新版本。").arg(savedPath));
}


/// @brief 处理插件发出的添加物品请求
void InventorySystem::onPluginRequestAddItem(const ItemInfo &item)
{
    if (DatabaseManager::instance().addItem(item)) {
        // 可以在状态栏给出提示
        statusBar()->showMessage("插件添加物品成功：" + item.name, 2000);
    } else {
        statusBar()->showMessage("插件添加物品失败：" + item.name, 2000);
    }
    // 每次处理完请求后刷新表格
    refreshTable();
}
