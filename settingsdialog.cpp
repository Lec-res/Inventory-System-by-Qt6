#include "settingsdialog.h"
#include "settingsmanager.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QHBoxLayout>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent) {
    setupUI();
    setWindowTitle("数据库设置");
    setMinimumWidth(350);

    // 加载当前配置并显示在界面上
    DbConfig currentConfig = SettingsManager::instance().loadDbConfig();
    hostEdit->setText(currentConfig.host);
    portSpinBox->setValue(currentConfig.port);
    dbNameEdit->setText(currentConfig.dbName);
    userEdit->setText(currentConfig.user);
    passwordEdit->setText(currentConfig.password);
}

SettingsDialog::~SettingsDialog() {}

void SettingsDialog::setupUI() {
    auto mainLayout = new QVBoxLayout(this);
    auto formLayout = new QFormLayout();

    hostEdit = new QLineEdit();
    portSpinBox = new QSpinBox();
    portSpinBox->setRange(1, 65535);
    dbNameEdit = new QLineEdit();
    userEdit = new QLineEdit();
    passwordEdit = new QLineEdit();
    passwordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("主机地址:", hostEdit);
    formLayout->addRow("端口:", portSpinBox);
    formLayout->addRow("数据库名:", dbNameEdit);
    formLayout->addRow("用户名:", userEdit);
    formLayout->addRow("密码:", passwordEdit);

    auto testButton = new QPushButton("测试连接");
    auto saveButton = new QPushButton("保存");
    auto cancelButton = new QPushButton("取消");

    auto buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(testButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    connect(testButton, &QPushButton::clicked, this, &SettingsDialog::onTestConnection);
    connect(saveButton, &QPushButton::clicked, this, &SettingsDialog::onSave);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

void SettingsDialog::onSave() {
    DbConfig newConfig;
    newConfig.host = hostEdit->text().trimmed();
    newConfig.port = portSpinBox->value();
    newConfig.dbName = dbNameEdit->text().trimmed();
    newConfig.user = userEdit->text().trimmed();
    newConfig.password = passwordEdit->text();

    SettingsManager::instance().saveDbConfig(newConfig);

    QMessageBox::information(this, "成功", "设置已保存。\n请重启程序以使新设置生效。");
    accept();
}

void SettingsDialog::onTestConnection() {
    QSqlDatabase testDb = QSqlDatabase::addDatabase("QMYSQL", "test_connection");
    testDb.setHostName(hostEdit->text().trimmed());
    testDb.setPort(portSpinBox->value());
    testDb.setDatabaseName(dbNameEdit->text().trimmed());
    testDb.setUserName(userEdit->text().trimmed());
    testDb.setPassword(passwordEdit->text());

    if (testDb.open()) {
        QMessageBox::information(this, "成功", "数据库连接成功！");
        testDb.close();
    } else {
        QMessageBox::warning(this, "失败", "数据库连接失败：\n" + testDb.lastError().text());
    }
    QSqlDatabase::removeDatabase("test_connection");
}
