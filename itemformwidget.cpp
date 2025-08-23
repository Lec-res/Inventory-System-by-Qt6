#include "itemformwidget.h"
#include <QDate>

ItemFormWidget::ItemFormWidget(QWidget *parent) : QWidget(parent) {
    setupUI();
    connect(addButton, &QPushButton::clicked, this, &ItemFormWidget::addTriggered);
    connect(updateButton, &QPushButton::clicked, this, &ItemFormWidget::updateTriggered);
    connect(deleteButton, &QPushButton::clicked, this, &ItemFormWidget::deleteTriggered);
    connect(clearButton, &QPushButton::clicked, this, &ItemFormWidget::clearTriggered);
}

void ItemFormWidget::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    formGroupBox = new QGroupBox("物品信息录入");
    auto formLayout = new QGridLayout(formGroupBox);

    formLayout->addWidget(new QLabel("物品名称 *:"), 0, 0);
    nameEdit = new QLineEdit;
    nameEdit->setPlaceholderText("请输入物品名称");
    formLayout->addWidget(nameEdit, 0, 1);

    formLayout->addWidget(new QLabel("类别:"), 1, 0);
    categoryComboBox = new QComboBox;
    categoryComboBox->setEditable(true);
    formLayout->addWidget(categoryComboBox, 1, 1);

    formLayout->addWidget(new QLabel("描述:"), 2, 0);
    descriptionEdit = new QTextEdit;
    descriptionEdit->setMaximumHeight(80);
    descriptionEdit->setPlaceholderText("请输入物品描述");
    formLayout->addWidget(descriptionEdit, 2, 1);

    formLayout->addWidget(new QLabel("数量:"), 3, 0);
    quantitySpinBox = new QSpinBox;
    quantitySpinBox->setRange(0, 999999);
    quantitySpinBox->setSuffix(" 件");
    formLayout->addWidget(quantitySpinBox, 3, 1);

    formLayout->addWidget(new QLabel("单价:"), 4, 0);
    priceSpinBox = new QDoubleSpinBox;
    priceSpinBox->setRange(0.0, 999999.99);
    priceSpinBox->setDecimals(2);
    priceSpinBox->setPrefix("¥ ");
    formLayout->addWidget(priceSpinBox, 4, 1);

    formLayout->addWidget(new QLabel("供应商:"), 5, 0);
    supplierEdit = new QLineEdit;
    supplierEdit->setPlaceholderText("请输入供应商名称");
    formLayout->addWidget(supplierEdit, 5, 1);

    formLayout->addWidget(new QLabel("购买日期:"), 6, 0);
    purchaseDateEdit = new QDateEdit(QDate::currentDate());
    purchaseDateEdit->setCalendarPopup(true);
    purchaseDateEdit->setDisplayFormat("yyyy-MM-dd");
    formLayout->addWidget(purchaseDateEdit, 6, 1);

    formLayout->addWidget(new QLabel("存放位置:"), 7, 0);
    locationEdit = new QLineEdit;
    locationEdit->setPlaceholderText("请输入存放位置");
    formLayout->addWidget(locationEdit, 7, 1);

    formLayout->addWidget(new QLabel("状态:"), 8, 0);
    statusComboBox = new QComboBox;
    formLayout->addWidget(statusComboBox, 8, 1);

    auto buttonLayout = new QHBoxLayout;
    addButton = new QPushButton("添加");
    updateButton = new QPushButton("更新");
    deleteButton = new QPushButton("删除");
    clearButton = new QPushButton("清空");

    addButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; padding: 8px; }");
    updateButton->setStyleSheet("QPushButton { background-color: #2196F3; color: white; font-weight: bold; padding: 8px; }");
    deleteButton->setStyleSheet("QPushButton { background-color: #f44336; color: white; font-weight: bold; padding: 8px; }");
    clearButton->setStyleSheet("QPushButton { background-color: #FF9800; color: white; font-weight: bold; padding: 8px; }");

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addWidget(clearButton);
    formLayout->addLayout(buttonLayout, 9, 0, 1, 2);

    mainLayout->addWidget(formGroupBox);
    mainLayout->addStretch();
}

ItemInfo ItemFormWidget::getItemFromForm() const {
    ItemInfo item;
    item.name = nameEdit->text().trimmed();
    item.category = categoryComboBox->currentText().trimmed();
    item.description = descriptionEdit->toPlainText().trimmed();
    item.quantity = quantitySpinBox->value();
    item.price = priceSpinBox->value();
    item.supplier = supplierEdit->text().trimmed();
    item.purchaseDate = purchaseDateEdit->date();
    item.location = locationEdit->text().trimmed();
    item.status = statusComboBox->currentText();
    return item;
}

void ItemFormWidget::setForm(const ItemInfo &item) {
    nameEdit->setText(item.name);
    descriptionEdit->setPlainText(item.description);
    quantitySpinBox->setValue(item.quantity);
    priceSpinBox->setValue(item.price);
    supplierEdit->setText(item.supplier);
    purchaseDateEdit->setDate(item.purchaseDate);
    locationEdit->setText(item.location);

    int categoryIndex = categoryComboBox->findText(item.category);
    if (categoryIndex != -1) {
        categoryComboBox->setCurrentIndex(categoryIndex);
    } else {
        categoryComboBox->setCurrentText(item.category);
    }

    int statusIndex = statusComboBox->findText(item.status);
    if (statusIndex != -1) {
        statusComboBox->setCurrentIndex(statusIndex);
    } else {
        statusComboBox->setCurrentIndex(0);
    }
}

void ItemFormWidget::clearForm() {
    nameEdit->clear();
    categoryComboBox->setCurrentIndex(0);
    descriptionEdit->clear();
    quantitySpinBox->setValue(1);
    priceSpinBox->setValue(0.0);
    supplierEdit->clear();
    purchaseDateEdit->setDate(QDate::currentDate());
    locationEdit->clear();
    statusComboBox->setCurrentIndex(0);
    nameEdit->setFocus();
}

void ItemFormWidget::setPredefinedCategories(const QStringList &categories) {
    categoryComboBox->addItems(categories);
}

void ItemFormWidget::setStatusOptions(const QStringList &statuses) {
    statusComboBox->addItems(statuses);
}

void ItemFormWidget::enableEditingControls(bool enable) {
    updateButton->setEnabled(enable);
    deleteButton->setEnabled(enable);
}
