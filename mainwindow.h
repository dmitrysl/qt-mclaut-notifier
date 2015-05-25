#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QAction>
#include <QTextEdit>
#include <QDate>

#include <QSysInfo>
#include <QSettings>

#include "aboutdialog.h"
#include "helper.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void checkNetworkConnection();
    void setupTrayIcon();
    void contextMenuEvent(QContextMenuEvent *event);
    void changeEvent(QEvent *event);
    void hideEvent(QHideEvent* pEvent);
    void showEvent(QShowEvent* pEvent);
    void updateTrayIcon();
    void showMessage(const QString &message, const QString &title = "Info",
                     QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::Information, int duration = 1000);
    void checkStatistics();
    void loadAppState();
    void updateAppProperties();

private slots:
    void updateStatistic();
    void openDialog();
    void exit();
    void onSystemTrayLeftClicked(QSystemTrayIcon::ActivationReason reason);
    void actionShowWindow();
    void actionOnline();
    void actionAway();
    void trayMessageClicked();
    void on_actionExit_triggered();
    void on_actionAbout_triggered();
    void on_actionOpen_triggered();
    void on_actionSave_triggered();
    void on_actionStore_Settings_triggered();
    void on_runOnbootCheckBox_clicked();
    void on_startMinimizedCheckBox_clicked();

private:
    QSysInfo::WinVersion winType;
    QSettings *settings;
    Helper helper;
    bool isConnectedToNetwork = false;
    int defaultUpdateInterval = 5* 3600 * 1000;
    int updateInterval = 3600 * 1000;
    bool authorized = false;
    AccountInfo accountInfo;
    Ui::MainWindow *ui;
    int trayStatus = 0;
    QSystemTrayIcon *tray;
    AboutDialog *aboutDialog;
    QTextEdit *result;
//    QAction *actionExit;
};

#endif // MAINWINDOW_H
