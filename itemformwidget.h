#ifndef ITEMFORMWIDGET_H
#define ITEMFORMWIDGET_H

#include <QWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QPushButton>
#include "iteminfo.h"

class ItemFormWidget : public QWidget {
    Q_OBJECT

public:
    explicit ItemFormWidget(QWidget *parent = nullptr);
    ItemInfo getItemFromForm() const;
    void setForm(const ItemInfo &item);
    void setPredefinedCategories(const QStringList& categories);
    void setStatusOptions(const QStringList& statuses);

signals:
    void addTriggered();
    void updateTriggered();
    void deleteTriggered();
    void clearTriggered();

public slots:
    void clearForm();
    void enableEditingControls(bool enable);

private:
    void setupUI();

    QGroupBox *formGroupBox;
    QLineEdit *nameEdit;
    QComboBox *categoryComboBox;
    QTextEdit *descriptionEdit;
    QSpinBox *quantitySpinBox;
    QDoubleSpinBox *priceSpinBox;
    QLineEdit *supplierEdit;
    QDateEdit *purchaseDateEdit;
    QLineEdit *locationEdit;
    QComboBox *statusComboBox;

    QPushButton *addButton;
    QPushButton *updateButton;
    QPushButton *deleteButton;
    QPushButton *clearButton;
};

#endif // ITEMFORMWIDGET_H
