#include "searchtoolbar.h"
#include <QVBoxLayout>

SearchToolbar::SearchToolbar(QWidget *parent)
    : QWidget(parent)
{
    m_categories << "电子产品" << "办公用品" << "家具" << "工具" << "消耗品" << "其他";

    setupUI();
    setupConnections();
}

void SearchToolbar::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // 搜索区域
    m_searchGroupBox = new QGroupBox("搜索筛选");
    m_searchLayout = new QGridLayout(m_searchGroupBox);

    m_searchLayout->addWidget(new QLabel("关键词:"), 0, 0);
    m_searchEdit = new QLineEdit;
    m_searchEdit->setPlaceholderText("输入物品名称、供应商或位置...");
    m_searchLayout->addWidget(m_searchEdit, 0, 1);

    m_searchLayout->addWidget(new QLabel("类别:"), 1, 0);
    m_categoryComboBox = new QComboBox;
    m_categoryComboBox->addItem("全部类别");
    m_categoryComboBox->addItems(m_categories);
    m_searchLayout->addWidget(m_categoryComboBox, 1, 1);

    m_buttonLayout = new QHBoxLayout;
    m_searchButton = new QPushButton("搜索");
    m_resetButton = new QPushButton("重置");
    m_searchButton->setStyleSheet("QPushButton { background-color: #607D8B; color: white; padding: 6px; }");
    m_resetButton->setStyleSheet("QPushButton { background-color: #9E9E9E; color: white; padding: 6px; }");

    m_buttonLayout->addWidget(m_searchButton);
    m_buttonLayout->addWidget(m_resetButton);
    m_searchLayout->addLayout(m_buttonLayout, 2, 0, 1, 2);

    mainLayout->addWidget(m_searchGroupBox);
}

void SearchToolbar::setupConnections()
{
    connect(m_searchButton, &QPushButton::clicked, this, &SearchToolbar::onSearchClicked);
    connect(m_resetButton, &QPushButton::clicked, this, &SearchToolbar::onResetClicked);
    connect(m_searchEdit, &QLineEdit::returnPressed, this, &SearchToolbar::onSearchClicked);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &SearchToolbar::onSearchTextChanged);
}

QString SearchToolbar::getSearchText() const
{
    return m_searchEdit->text().trimmed();
}

QString SearchToolbar::getSelectedCategory() const
{
    return m_categoryComboBox->currentText();
}

void SearchToolbar::clearSearch()
{
    m_searchEdit->clear();
    m_categoryComboBox->setCurrentIndex(0);
}

void SearchToolbar::onSearchClicked()
{
    emit searchRequested(getSearchText(), getSelectedCategory());
}

void SearchToolbar::onResetClicked()
{
    clearSearch();
    emit resetRequested();
}

void SearchToolbar::onSearchTextChanged()
{
    // 实时搜索（可选）
    // emit searchRequested(getSearchText(), getSelectedCategory());
}
