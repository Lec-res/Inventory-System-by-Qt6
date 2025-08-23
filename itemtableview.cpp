#include "itemtableview.h"
#include <QStringList>

ItemTableView::ItemTableView(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupConnections();
}

void ItemTableView::setupUI()
{
    m_layout = new QVBoxLayout(this);

    // 表格标题
    m_titleLabel = new QLabel("物品列表");
    m_titleLabel->setStyleSheet("QLabel { font-size: 16px; font-weight: bold; margin: 5px; }");
    m_layout->addWidget(m_titleLabel);

    // 创建表格
    m_tableWidget = new QTableWidget;
    setupTableHeaders();
    setupTableProperties();

    m_layout->addWidget(m_tableWidget);
}

void ItemTableView::setupConnections()
{
    connect(m_tableWidget, &QTableWidget::itemSelectionChanged,
            this, &ItemTableView::onSelectionChanged);
    connect(m_tableWidget, &QTableWidget::cellDoubleClicked,
            this, &ItemTableView::onItemDoubleClicked);
}

void ItemTableView::setupTableHeaders()
{
    m_tableWidget->setColumnCount(COL_COUNT);
    QStringList headers = {"ID", "物品名称", "类别", "描述", "数量", "单价", "供应商", "购买日期", "存放位置", "状态"};
    m_tableWidget->setHorizontalHeaderLabels(headers);
}

void ItemTableView::setupTableProperties()
{
    // 设置表格属性
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->setAlternatingRowColors(true);
    m_tableWidget->setSortingEnabled(true);
    m_tableWidget->horizontalHeader()->setStretchLastSection(true);

    // 设置列宽
    m_tableWidget->setColumnWidth(COL_ID, 50);
    m_tableWidget->setColumnWidth(COL_NAME, 120);
    m_tableWidget->setColumnWidth(COL_CATEGORY, 80);
    m_tableWidget->setColumnWidth(COL_DESCRIPTION, 150);
    m_tableWidget->setColumnWidth(COL_QUANTITY, 60);
    m_tableWidget->setColumnWidth(COL_PRICE, 80);
    m_tableWidget->setColumnWidth(COL_SUPPLIER, 100);
    m_tableWidget->setColumnWidth(COL_PURCHASE_DATE, 100);
    m_tableWidget->setColumnWidth(COL_LOCATION, 100);
    m_tableWidget->setColumnWidth(COL_STATUS, 80);
}

void ItemTableView::setItems(const QList<ItemInfo> &items)
{
    clearTable();

    m_tableWidget->setRowCount(items.size());

    for (int i = 0; i < items.size(); ++i) {
        addItemToTable(items[i], i);
    }
}

void ItemTableView::addItem(const ItemInfo &item)
{
    int newRow = m_tableWidget->rowCount();
    m_tableWidget->insertRow(newRow);
    addItemToTable(item, newRow);
}

void ItemTableView::updateItem(const ItemInfo &item)
{
    int row = findRowById(item.id);
    if (row >= 0) {
        addItemToTable(item, row);
    }
}

void ItemTableView::removeItem(int id)
{
    int row = findRowById(id);
    if (row >= 0) {
        m_tableWidget->removeRow(row);
    }
}

void ItemTableView::clearTable()
{
    m_tableWidget->setRowCount(0);
}

ItemInfo ItemTableView::getSelectedItem() const
{
    int currentRow = m_tableWidget->currentRow();
    if (currentRow < 0 || currentRow >= m_tableWidget->rowCount()) {
        return ItemInfo();
    }

    ItemInfo item;
    item.id = m_tableWidget->item(currentRow, COL_ID)->text().toInt();
    item.name = m_tableWidget->item(currentRow, COL_NAME)->text();
    item.category = m_tableWidget->item(currentRow, COL_CATEGORY)->text();
    item.description = m_tableWidget->item(currentRow, COL_DESCRIPTION)->text();
    item.quantity = m_tableWidget->item(currentRow, COL_QUANTITY)->text().toInt();
    item.price = m_tableWidget->item(currentRow, COL_PRICE)->text().toDouble();
    item.supplier = m_tableWidget->item(currentRow, COL_SUPPLIER)->text();
    item.purchaseDate = QDate::fromString(m_tableWidget->item(currentRow, COL_PURCHASE_DATE)->text(), "yyyy-MM-dd");
    item.location = m_tableWidget->item(currentRow, COL_LOCATION)->text();
    item.status = m_tableWidget->item(currentRow, COL_STATUS)->text();

    return item;
}

int ItemTableView::getSelectedItemId() const
{
    int currentRow = m_tableWidget->currentRow();
    if (currentRow >= 0 && currentRow < m_tableWidget->rowCount()) {
        return m_tableWidget->item(currentRow, COL_ID)->text().toInt();
    }
    return -1;
}

void ItemTableView::clearSelection()
{
    m_tableWidget->clearSelection();
}

int ItemTableView::getItemCount() const
{
    return m_tableWidget->rowCount();
}

void ItemTableView::onSelectionChanged()
{
    int currentRow = m_tableWidget->currentRow();
    if (currentRow >= 0) {
        ItemInfo item = getSelectedItem();
        emit itemSelectionChanged(item);
    } else {
        emit selectionCleared();
    }
}

void ItemTableView::onItemDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    if (row >= 0) {
        ItemInfo item = getSelectedItem();
        emit itemDoubleClicked(item);
    }
}

void ItemTableView::addItemToTable(const ItemInfo &item, int row)
{
    m_tableWidget->setItem(row, COL_ID, new QTableWidgetItem(QString::number(item.id)));
    m_tableWidget->setItem(row, COL_NAME, new QTableWidgetItem(item.name));
    m_tableWidget->setItem(row, COL_CATEGORY, new QTableWidgetItem(item.category));
    m_tableWidget->setItem(row, COL_DESCRIPTION, new QTableWidgetItem(item.description));
    m_tableWidget->setItem(row, COL_QUANTITY, new QTableWidgetItem(QString::number(item.quantity)));
    m_tableWidget->setItem(row, COL_PRICE, new QTableWidgetItem(QString::number(item.price, 'f', 2)));
    m_tableWidget->setItem(row, COL_SUPPLIER, new QTableWidgetItem(item.supplier));
    m_tableWidget->setItem(row, COL_PURCHASE_DATE, new QTableWidgetItem(item.purchaseDate.toString("yyyy-MM-dd")));
    m_tableWidget->setItem(row, COL_LOCATION, new QTableWidgetItem(item.location));
    m_tableWidget->setItem(row, COL_STATUS, new QTableWidgetItem(item.status));

    // 设置行颜色
    setRowColors(row, item.status);
}

void ItemTableView::setRowColors(int row, const QString &status)
{
    QColor rowColor;
    if (status == "已报废" || status == "丢失") {
        rowColor = QColor(255, 200, 200); // 浅红色
    } else if (status == "维修中") {
        rowColor = QColor(255, 255, 200); // 浅黄色
    } else if (status == "借出") {
        rowColor = QColor(200, 200, 255); // 浅蓝色
    }

    if (rowColor.isValid()) {
        for (int col = 0; col < m_tableWidget->columnCount(); ++col) {
            if (m_tableWidget->item(row, col)) {
                m_tableWidget->item(row, col)->setBackground(rowColor);
            }
        }
    }
}

int ItemTableView::findRowById(int id) const
{
    for (int row = 0; row < m_tableWidget->rowCount(); ++row) {
        if (m_tableWidget->item(row, COL_ID)->text().toInt() == id) {
            return row;
        }
    }
    return -1;
}
