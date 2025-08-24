#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class SettingsDialog; }
QT_END_NAMESPACE

class SettingsDialog : public QDialog {
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

private slots:
    void onSave();
    void onTestConnection();

private:
    void setupUI();

    QLineEdit* hostEdit;
    QSpinBox* portSpinBox;
    QLineEdit* dbNameEdit;
    QLineEdit* userEdit;
    QLineEdit* passwordEdit;
};

#endif // SETTINGSDIALOG_H
