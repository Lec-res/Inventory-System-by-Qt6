#ifndef SEARCHTOOLBAR_H
#define SEARCHTOOLBAR_H

#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QStringList>

class SearchToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit SearchToolbar(QWidget *parent = nullptr);

    // 搜索参数
    QString getSearchText() const;
    QString getSelectedCategory() const;
    void clearSearch();

signals:
    void searchRequested(const QString &searchText, const QString &category);
    void resetRequested();

private slots:
    void onSearchClicked();
    void onResetClicked();
    void onSearchTextChanged();

private:
    void setupUI();
    void setupConnections();

    // UI组件
    QGroupBox *m_searchGroupBox;
    QGridLayout *m_searchLayout;
    QHBoxLayout *m_buttonLayout;

    QLineEdit *m_searchEdit;
    QComboBox *m_categoryComboBox;
    QPushButton *m_searchButton;
    QPushButton *m_resetButton;

    QStringList m_categories;
};

#endif // SEARCHTOOLBAR_H
