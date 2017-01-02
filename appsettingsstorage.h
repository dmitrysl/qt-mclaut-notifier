#ifndef APPSETTINGSSTORAGE_H
#define APPSETTINGSSTORAGE_H

#include <QDebug>
#include <QCoreApplication>
#include <QString>
#include <QMap>
#include <QSettings>
#include <QDir>

class AppSettingsStorage
{
public:
    enum Settings {
        ACCOUNT_LOGIN = 0,
        ACCOUNT_PASS,
        APP_RUN_ON_BOOT,
        APP_START_MINIMIZED,
        APP_STATUS
    };

    AppSettingsStorage();

    static void initColumnNames() {
        settingsMap.insert(ACCOUNT_LOGIN, "account/login");
        settingsMap.insert(ACCOUNT_PASS, "account/password");
        settingsMap.insert(APP_RUN_ON_BOOT, "app/runOnbootCheckBox");
        settingsMap.insert(APP_START_MINIMIZED, "app/startMinimizedCheckBox");
        settingsMap.insert(APP_STATUS, "app/status");
    }

    QMap<QString, QString> loadSettings();
    void storeSettings(QMap<QString, QString> properties);

signals:

public slots:

public:
    const static QSettings *settings;
    static QMap<Settings, QString> settingsMap;
};

#endif // APPSETTINGSSTORAGE_H
