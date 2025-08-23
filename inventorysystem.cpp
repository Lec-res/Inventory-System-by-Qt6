#include "inventorysystem.h"

InventorySystem::InventorySystem(QWidget *parent)
    : QMainWindow(parent), currentItemId(-1)
{
    categories << "电子产品" << "办公用品" << "家具" << "工具" << "消耗品" << "其他";
    statusOptions << "正常" << "维修中" << "已报废" << "借出" << "丢失";

    if (!DatabaseManager::instance().initDatabase()) {
        QMessageBox::critical(this, "数据库错误", "无法初始化数据库！");
        return;
    }

    setupUI();
    setupMenuBar();
    setupStatusBar();
    setupConnections();

    formWidget->setPredefinedCategories(categories);
    formWidget->setStatusOptions(statusOptions);
    searchCategoryComboBox->addItems(categories);

    refreshTable();

    setWindowTitle("物品信息管理系统 v1.0");
    setMinimumSize(1200, 700);
    resize(1400, 800);
}

InventorySystem::~InventorySystem() {}

void InventorySystem::setupUI() {
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    auto mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);

    formWidget = new ItemFormWidget;
    formWidget->setMaximumWidth(400);
    formWidget->setMinimumWidth(350);

    auto rightPanel = new QWidget;
    auto rightLayout = new QVBoxLayout(rightPanel);

    // Search Area
    auto searchGroupBox = new QGroupBox("搜索筛选");
    auto searchLayout = new QGridLayout(searchGroupBox);
    searchLayout->addWidget(new QLabel("关键词:"), 0, 0);
    searchEdit = new QLineEdit;
    searchEdit->setPlaceholderText("输入物品名称、供应商或位置...");
    searchLayout->addWidget(searchEdit, 0, 1);
    searchLayout->addWidget(new QLabel("类别:"), 1, 0);
    searchCategoryComboBox = new QComboBox;
    searchCategoryComboBox->addItem("全部类别");
    searchLayout->addWidget(searchCategoryComboBox, 1, 1);
    auto searchButton = new QPushButton("搜索");
    auto resetSearchButton = new QPushButton("重置");
    searchButton->setStyleSheet("QPushButton { background-color: #607D8B; color: white; padding: 6px; }");
    resetSearchButton->setStyleSheet("QPushButton { background-color: #9E9E9E; color: white; padding: 6px; }");
    auto searchButtonLayout = new QHBoxLayout;
    searchButtonLayout->addWidget(searchButton);
    searchButtonLayout->addWidget(resetSearchButton);
    searchLayout->addLayout(searchButtonLayout, 2, 0, 1, 2);

    connect(searchButton, &QPushButton::clicked, this, &InventorySystem::onSearch);
    connect(resetSearchButton, &QPushButton::clicked, this, &InventorySystem::onResetSearch);
    connect(searchEdit, &QLineEdit::returnPressed, this, &InventorySystem::onSearch);

    // Table Area
    auto tableTitle = new QLabel("物品列表");
    tableTitle->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 5px; }");
    itemTable = new QTableWidget;
    itemTable->setColumnCount(10);
    QStringList headers = {"ID", "物品名称", "类别", "描述", "数量", "单价", "供应商", "购买日期", "存放位置", "状态"};
    itemTable->setHorizontalHeaderLabels(headers);
    itemTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    itemTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    itemTable->setAlternatingRowColors(true);
    itemTable->setSortingEnabled(true);
    itemTable->horizontalHeader()->setStretchLastSection(true);
    itemTable->setColumnWidth(0, 50); itemTable->setColumnWidth(1, 120); itemTable->setColumnWidth(2, 80);
    itemTable->setColumnWidth(3, 150); itemTable->setColumnWidth(4, 60); itemTable->setColumnWidth(5, 80);
    itemTable->setColumnWidth(6, 100); itemTable->setColumnWidth(7, 100); itemTable->setColumnWidth(8, 100);
    itemTable->setColumnWidth(9, 80);

    rightLayout->addWidget(searchGroupBox);
    rightLayout->addWidget(tableTitle);
    rightLayout->addWidget(itemTable);

    mainSplitter->addWidget(formWidget);
    mainSplitter->addWidget(rightPanel);
    mainSplitter->setStretchFactor(0, 0);
    mainSplitter->setStretchFactor(1, 1);

    auto mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->addWidget(mainSplitter);
}

void InventorySystem::setupMenuBar() {
    auto mainMenuBar = menuBar();
    auto fileMenu = mainMenuBar->addMenu("文件");
    auto exportAction = new QAction("导出到CSV", this);
    exportAction->setShortcut(QKeySequence::SaveAs);
    connect(exportAction, &QAction::triggered, this, &InventorySystem::onExportToCSV);
    fileMenu->addAction(exportAction);
    fileMenu->addSeparator();
    auto exitAction = new QAction("退出", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, this, &QWidget::close);
    fileMenu->addAction(exitAction);

    auto toolsMenu = mainMenuBar->addMenu("工具");
    auto statisticsAction = new QAction("统计信息", this);
    statisticsAction->setShortcut(QKeySequence("Ctrl+I"));
    connect(statisticsAction, &QAction::triggered, this, &InventorySystem::onShowStatistics);
    toolsMenu->addAction(statisticsAction);
}

void InventorySystem::setupStatusBar() {
    auto mainStatusBar = statusBar();
    itemCountLabel = new QLabel;
    totalValueLabel = new QLabel;
    mainStatusBar->addWidget(itemCountLabel);
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
