#include "appsettingsstorage.h"

QMap<AppSettingsStorage::Settings, QString> AppSettingsStorage::settingsMap;
const QSettings *AppSettingsStorage::settings = new QSettings(qApp->applicationDirPath() + QDir::separator() + "settings.conf", QSettings::IniFormat);

AppSettingsStorage::AppSettingsStorage()
{
    initColumnNames();
}

QMap<QString, QString> AppSettingsStorage::loadSettings()
{
    QMap<QString, QString> properties;

    settings = new QSettings(qApp->applicationDirPath() + QDir::separator() + "settings.conf", QSettings::IniFormat);

#ifdef QT_DEBUG
    if (settings->isWritable())
    {
        qDebug() << "writable";
    }
#endif

    QString login = settings->value(settingsMap.take(Settings::ACCOUNT_LOGIN), "").toString();
    QString pass = settings->value(settingsMap.take(Settings::ACCOUNT_PASS), "").toString();
    QString runOnbootCheckBox = settings->value(settingsMap.take(Settings::APP_RUN_ON_BOOT), "0").toString();
    QString startMinimizedCheckBoxBool = settings->value(settingsMap.take(Settings::APP_START_MINIMIZED), "0").toString();
    QString appStatus = settings->value(settingsMap.take(Settings::APP_STATUS), "0").toString();

    properties.insert(settingsMap.take(Settings::ACCOUNT_LOGIN), login);
    properties.insert(settingsMap.take(Settings::ACCOUNT_PASS), pass);
    properties.insert(settingsMap.take(Settings::APP_RUN_ON_BOOT), runOnbootCheckBox);
    properties.insert(settingsMap.take(Settings::APP_START_MINIMIZED), startMinimizedCheckBoxBool);
    properties.insert(settingsMap.take(Settings::APP_STATUS), appStatus);

    delete settings;

    return properties;
}

