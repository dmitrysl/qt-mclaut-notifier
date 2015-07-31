#include <QMenu>
#include <QIcon>
#include <QWindowStateChangeEvent>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>
#include <QList>
#include <QMap>
#include <QDir>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include <QJsonDocument>
#include <QJsonObject>

#include <QDesktopServices>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    setWindowFlags(windowFlags() |= Qt::FramelessWindowHint);

    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setFixedSize(width(), height());

    setupTrayIcon();

    ui->mainToolBar->hide();

    //QString applicationLocation = QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation);

#ifdef QT_DEBUG
    //qDebug() << applicationLocation;
#endif

    accountInfo = AccountInfo();
    accountInfo.showInfoNotification = true;
    accountInfo.showErrorNotification = true;
    accountInfo.city = Helper::CHERKASY;

    loadAppState();

    updateAppProperties();

    updateStatistic();

    if (ui->startMinimizedCheckBox->isChecked())
    {
        hide();
    }
    else
    {
        show();
    }
}


void MainWindow::updateAppProperties()
{
//#ifdef Q_WS_WIN
    winType = QSysInfo::windowsVersion();
//    switch(QSysInfo::windowsVersion())
//    {
//      case QSysInfo::WV_2000: return "Windows 2000";
//      case QSysInfo::WV_XP: return "Windows XP";
//      case QSysInfo::WV_VISTA: return "Windows Vista";
//      default: return "Windows";
//    }
    QString registryPath = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run";
//    if (winType == QSysInfo::WV_WINDOWS8)
//    {
//        registryPath = "HKLM\\SOFTWARE\\Wow6432Node\\Microsoft\\Windows\\CurrentVersion\\Run";
//    }

    QSettings bootUpSettings(registryPath, QSettings::NativeFormat);

#ifdef QT_DEBUG
    qDebug() << qApp->applicationFilePath() + qAppName() + ".exe";
    qDebug() << QCoreApplication::applicationFilePath().replace('/','\\');
#endif

    if (ui->runOnbootCheckBox->isChecked())
    {
        bootUpSettings.setValue(qAppName(), QCoreApplication::applicationFilePath().replace('/','\\') + (ui->startMinimizedCheckBox->isChecked() ? " -m" : ""));
    }
    else
        bootUpSettings.remove(qAppName());
//#endif
}


void MainWindow::setupTrayIcon()
{
    QMenu * trayIconMenu = new QMenu();

    QAction *actionOnline = new QAction(tr("&Online"), this);
    actionOnline->setStatusTip(tr("Online"));
    connect(actionOnline, SIGNAL(triggered()), this, SLOT(actionOnline()));
    trayIconMenu->addAction(actionOnline);

    QAction *actionAway = new QAction(tr("&Away"), this);
    actionAway->setStatusTip(tr("Away"));
    connect(actionAway, SIGNAL(triggered()), this, SLOT(actionAway()));
    trayIconMenu->addAction(actionAway);

    QAction *actionShowWindow = new QAction(tr("&Show window"), this);
    actionShowWindow->setStatusTip(tr("Show window"));
    connect(actionShowWindow, SIGNAL(triggered()), this, SLOT(actionShowWindow()));
    trayIconMenu->addAction(actionShowWindow);

    QAction *actionExit = new QAction(tr("&Exit"), this);
    actionExit->setShortcuts(QKeySequence::Close);
    actionExit->setStatusTip(tr("Exit"));
    connect(actionExit, SIGNAL(triggered()), this, SLOT(exit()));
    trayIconMenu->addAction(actionExit);

    tray = new QSystemTrayIcon(QIcon(":/images/wheatgrass-tray-icon-32.png"), this);

    tray->setContextMenu(trayIconMenu);
    tray->show();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateStatistic()));
    timer->start(updateInterval);

    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(onSystemTrayLeftClicked(QSystemTrayIcon::ActivationReason)));
    connect(tray, SIGNAL(messageClicked()), this, SLOT(trayMessageClicked()));
}

void MainWindow::trayMessageClicked()
{
#ifdef QT_DEBUG
    qDebug() << "clicked";
#endif
    accountInfo.showErrorNotification = false;
}

void MainWindow::updateStatistic()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    if (!accountInfo.inProgress && (accountInfo.certificate == "" || accountInfo.updateLastDate < currentDateTime.addSecs(-10)))
    {
        accountInfo.inProgress = true;
        ui->statusBar->showMessage("Connecting", 1000);

#ifdef QT_DEBUG
        qDebug() << "update";
#endif
        if (!isConnectedToNetwork)
        {
            isConnectedToNetwork = Helper::isConnectedToNetwork();
            if (!isConnectedToNetwork)
            {
                accountInfo.inProgress = false;
                showMessage("Cannot connect to the network.", "Network Connection", QSystemTrayIcon::Critical);
                updateInterval = 600000;
                return;
            }
        }
        else
        {
            updateInterval = defaultUpdateInterval;
        }

        if (!authorized)
        {
            authorized = helper.authClient(accountInfo) == Helper::OK;
            if (!authorized)
            {
                accountInfo.inProgress = false;
                return;
            }
        }

        if (trayStatus == 2)
        {
            accountInfo.inProgress = false;
            return;
        }

        ui->statusBar->showMessage("Connected", 2000);

        User user = helper.getInfo(accountInfo);

        helper.getPayments(accountInfo);
        helper.getWithdrawals(accountInfo);

        ui->statusBar->showMessage("Information recieved", 2000);

        checkStatistics();

        accountInfo.inProgress = false;
    }
}

void MainWindow::onSystemTrayLeftClicked( QSystemTrayIcon::ActivationReason reason )
{
//    User user = {0};
    switch (reason) {
        case QSystemTrayIcon::Trigger:
#ifdef QT_DEBUG
            qDebug() << "triggered";
#endif
//            helper.authClient(accountInfo);
//            user = helper.getInfo(accountInfo);
//            helper.getPayments(accountInfo);
//            helper.getWithdrawals(accountInfo);
            on_actionSave_triggered();
            break;
        case QSystemTrayIcon::DoubleClick:
#ifdef QT_DEBUG
            qDebug() << "double click";
#endif
            actionShowWindow();
            break;
     case QSystemTrayIcon::MiddleClick:
#ifdef QT_DEBUG
            qDebug() << "middle click";
#endif
            break;
     default:
            break;
     }
}

void MainWindow::showMessage(const QString &message, const QString &title,
                             QSystemTrayIcon::MessageIcon type, int duration)
 {
    // 2 - warning;
    // 1 - info
    // 3 - error
//    QSystemTrayIcon::MessageIcon type = QSystemTrayIcon::MessageIcon(2);
    tray->showMessage(title, message, type, duration);
 }

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    Q_UNUSED(event)
    showMessage("context menu initiated");
    qDebug("context menu");
    QMenu menu(this);
//    menu.addAction(cutAct);
//    menu.addAction(copyAct);
//    menu.addAction(pasteAct);
//    menu.exec(event->globalPos());
}

void MainWindow::actionShowWindow()
{
    if (isMinimized() || isHidden())
    {
        qDebug() << "minimized";
        qApp->processEvents();
        show();
        setWindowState((windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
    }
}

void MainWindow::actionOnline()
{
    trayStatus = 1;
    updateTrayIcon();
}

void MainWindow::actionAway()
{
    trayStatus = 2;
    updateTrayIcon();
}

void MainWindow::updateTrayIcon()
{
    QString iconName;
    switch(trayStatus)
    {
    case 1:
        iconName = "online"; break;
    case 2:
        iconName = "away"; break;
    case 3:
        iconName = "busy"; break;
    default:
        iconName = "offline";
    }

    tray->setIcon(QIcon(":/images/images/status-tray-" + iconName + "-icon.png"));
}

void MainWindow::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::WindowStateChange)
    {
        QWindowStateChangeEvent *e = (QWindowStateChangeEvent*) event;
        // make sure we only do this for minimize events
        if ((e->oldState() != Qt::WindowMinimized) && isMinimized())
        {
//            QMessageBox::information(this, tr("Systray"),
//                                              tr("The program will keep running in the "
//                                                 "system tray. To terminate the program, "
//                                                 "choose <b>Quit</b> in the context menu "
//                                                 "of the system tray entry."));
            qApp->processEvents();
            hide();
            event->ignore();
        }
    }
}

void MainWindow::hideEvent(QHideEvent* pEvent)
{
    if (isMinimized())
    {
        qDebug() << "minimized";
    }
    QWidget::hideEvent(pEvent);
}

void MainWindow::showEvent(QShowEvent* pEvent)
{
    QWidget::showEvent(pEvent);
}

void MainWindow::exit()
{
    qDebug() << "exit from app from tray menu.";
    QApplication::quit();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openDialog()
{
    aboutDialog = new AboutDialog;
    aboutDialog->setAttribute(Qt::WA_DeleteOnClose, true);
    aboutDialog->show();
}

void MainWindow::on_actionExit_triggered()
{
    qDebug() << "exit from app from main app menu.";
    QApplication::quit();
}

void MainWindow::on_actionAbout_triggered()
{
    QTimer::singleShot(100, this, SLOT(openDialog()));
}

void MainWindow::on_actionOpen_triggered()
{
    qDebug() << "load settings";

    loadAppState();
}

void MainWindow::loadAppState()
{
    QMap<QString, QString> properties = helper.loadSettings();

    QString runOnbootCheckBox = properties.value("app/runOnbootCheckBox", "0");
    QString startMinimizedCheckBox = properties.value("app/startMinimizedCheckBox", "0");

    bool runOnbootCheckBoxBool = bool(runOnbootCheckBox.toInt());
    bool startMinimizedCheckBoxBool = bool(startMinimizedCheckBox.toInt());

    accountInfo.login = Helper::decode(properties.value("account/login", Helper::encode("")));
    accountInfo.password = Helper::decode(properties.value("account/password", Helper::encode("")));

    ui->loginField->setText(accountInfo.login);
    ui->passwordField->setText(accountInfo.password);
    ui->runOnbootCheckBox->setChecked(runOnbootCheckBoxBool);
    ui->startMinimizedCheckBox->setChecked(startMinimizedCheckBoxBool);

    ui->statusBar->showMessage("Settings loaded", 2000);
}

void MainWindow::on_actionStore_Settings_triggered()
{
    qDebug() << "save settings";
    QString login = ui->loginField->text().trimmed();
    QString pass = ui->passwordField->text().trimmed();

    int runOnbootCheckBox = ui->runOnbootCheckBox->isChecked() ? 1 : 0;
    int startMinimizedCheckBox = ui->startMinimizedCheckBox->isChecked() ? 1 : 0;

    QMap<QString, QString> properties = QMap<QString, QString>();

    properties.insert("account/login", Helper::encode(login));
    properties.insert("account/password", Helper::encode(pass));

    properties.insert("app/runOnbootCheckBox", QString::number(runOnbootCheckBox));
    properties.insert("app/startMinimizedCheckBox", QString::number(startMinimizedCheckBox));

    helper.storeSettings(properties);

    ui->statusBar->showMessage("Settings saved", 2000);
}

void MainWindow::on_actionSave_triggered()
{
    qDebug() << "authentication";

    try
    {
        checkNetworkConnection();
    }
    catch(NetworkConnectionFailureError &e)
    {
        showMessage(e.getMessage(), "Network Connection", QSystemTrayIcon::Critical);
        return;
    }

    QString login = ui->loginField->text().trimmed();
    QString password = ui->passwordField->text().trimmed();

    if (login == "" || password == "")
    {
        showMessage(QString("Login or Password field is empty."), QString("Validation Info"), QSystemTrayIcon::Warning, 2);
        return;
    }

    accountInfo.login = login;
    accountInfo.password = password;
    accountInfo.inProgress = true;

    helper.authClient(accountInfo);

    if (accountInfo.status == 1 && accountInfo.certificate != "")
    {
        User user = helper.getInfo(accountInfo);
        checkStatistics();
        QList<Payment> payments = helper.getPayments(accountInfo);
        QList<Payment> withdrawals = helper.getWithdrawals(accountInfo);
    }
}

void MainWindow::checkNetworkConnection()
{
    isConnectedToNetwork = Helper::isConnectedToNetwork();

    if (isConnectedToNetwork) return;

    showMessage("Cannot connect to the network.", "Network Connection", QSystemTrayIcon::Critical);
    updateInterval = 600000;
    throw NetworkConnectionFailureError("Cannot connect to the network.");
}

void MainWindow::checkStatistics()
{
#ifdef QT_DEBUG
    qDebug() << (accountInfo.balance < 10.0);
#endif

    if (accountInfo.balance < 10.0 && accountInfo.showErrorNotification) {
        QString msg = QString("Остаток: ") + QString::number(accountInfo.balance, 'f', 2) + QString("грн. Нужно пополнить счет.");
        showMessage(msg, QString("Пополнение счета!!!"), QSystemTrayIcon::Critical, 10);
    } else if (accountInfo.showInfoNotification) {
        QString message = "Login: " + accountInfo.login + "\n";
        message += "Account: " + QString::number(accountInfo.account, 'f', 0) + "\n";
        message += "User: " + accountInfo.name + "\n";
        message += "Balance: " + QString::number(accountInfo.balance, 'f', 2) + " грн.";
        showMessage(message, "Account statistics");
    }
}

void MainWindow::on_runOnbootCheckBox_clicked()
{
    updateAppProperties();
}

void MainWindow::on_startMinimizedCheckBox_clicked()
{

}
