#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QCoreApplication>
#include "dbconfig.h" // <-- 1. 将 #include "databasemanager.h" 替换为这

class SettingsManager {
public:
    static SettingsManager& instance();

    void saveDbConfig(const DbConfig& config);
    DbConfig loadDbConfig();

private:
    SettingsManager();
    ~SettingsManager();
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    QSettings* settings;
};

#endif // SETTINGSMANAGER_H
