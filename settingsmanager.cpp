#include "settingsmanager.h"

SettingsManager& SettingsManager::instance() {
    static SettingsManager sm_instance;
    return sm_instance;
}

SettingsManager::SettingsManager() {
    // 获取可执行文件所在路径
    QString configFilePath = QCoreApplication::applicationDirPath() + "/config.ini";
    // 创建 QSettings 对象，指定使用 INI 文件格式
    settings = new QSettings(configFilePath, QSettings::IniFormat);
}

SettingsManager::~SettingsManager() {
    delete settings;
}

void SettingsManager::saveDbConfig(const DbConfig &config) {
    settings->beginGroup("Database");
    settings->setValue("host", config.host);
    settings->setValue("port", config.port);
    settings->setValue("dbname", config.dbName);
    settings->setValue("user", config.user);
    settings->setValue("password", config.password); // 实际项目中密码应加密存储
    settings->endGroup();
}

DbConfig SettingsManager::loadDbConfig() {
    DbConfig config;

    // 检查配置文件是否存在或者 database 组是否存在
    if (!settings->childGroups().contains("Database", Qt::CaseInsensitive)) {
        // 如果不存在，则写入一个默认的配置，引导用户填写
        config.host = "127.0.0.1";
        config.port = 3306;
        config.dbName = "inventory_db";
        config.user = "root";
        config.password = "your_password_here";
        saveDbConfig(config);
    } else {
        settings->beginGroup("Database");
        config.host = settings->value("host", "127.0.0.1").toString();
        config.port = settings->value("port", 3306).toInt();
        config.dbName = settings->value("dbname", "inventory_db").toString();
        config.user = settings->value("user", "root").toString();
        config.password = settings->value("password").toString();
        settings->endGroup();
    }
    return config;
}

// 新增函数实现
void SettingsManager::saveCurrentBackground(const QString &bgPath)
{
    // 将背景路径保存到 "Appearance" 组下
    settings->setValue("Appearance/background", bgPath);
}

QString SettingsManager::loadCurrentBackground()
{
    // 从 "Appearance" 组读取背景路径，如果不存在则返回空字符串
    return settings->value("Appearance/background", "").toString();
}
