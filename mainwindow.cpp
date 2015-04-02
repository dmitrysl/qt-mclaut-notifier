#include <QMenu>
#include <QIcon>
#include <QWindowStateChangeEvent>
#include <QTimer>
#include <QDebug>
#include <QMessageBox>

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include <QJsonDocument>
#include <QJsonObject>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "helper.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    QIcon icon(":/images/status-tray-away-icon.png");
    Q_ASSERT(!icon.isNull());
    qDebug() << "icon null: " + !icon.isNull();

    setupTrayIcon();

//    actionExit = new QAction(tr("&Exit"), this);
//    actionExit->setShortcuts(QKeySequence::Close);
//    actionExit->setStatusTip(tr("Exit"));
//    connect(actionExit, SIGNAL(clicked()), this, SLOT(exit()));


    accountInfo = {0};
    accountInfo.showInfoNotification = true;
    accountInfo.showErrorNotification = true;
    qDebug() << accountInfo.id;
    qDebug() << accountInfo.login;
    qDebug() << accountInfo.account;
    qDebug() << accountInfo.balance;
    qDebug() << accountInfo.name;
    qDebug() << accountInfo.paymentLastDate;
    qDebug() << accountInfo.updateLastDate;
    ui->setupUi(this);
    isConnectedToNetwork = Helper::isConnectedToNetwork();

    qDebug() << isConnectedToNetwork;

    qDebug() << "---------------\n";

    Helper helper;
    QString str2 = "app=mobile&version=2.0.0.3&version_code=2&action=checkLogin&login=LOGIN&pass=PASS&city=0&system_info=QVBJWzE5XSwgRGV2aWNlW1MxXSwgTW9kZWxbWlA5OThdLFByb2R1Y3RbUzFd";
    QString str3 = "QVBJWzE5XSwgRGV2aWNlW1MxXSwgTW9kZWxbWlA5OThdLFByb2R1Y3RbUzFd";
    qDebug() << helper.decode(str3);
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
    qDebug() << "clicked";
    accountInfo.showErrorNotification = false;
}

void MainWindow::updateStatistic()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();

    if (!accountInfo.inProgress && (accountInfo.certificate == "" || accountInfo.updateLastDate < currentDateTime.addSecs(-10)))
    {
        accountInfo.inProgress = true;
        qDebug() << "update";
        if (!isConnectedToNetwork) {
            isConnectedToNetwork = Helper::isConnectedToNetwork();
            if (!isConnectedToNetwork)
            {
                showMessage("Cannot connect to the network.", "Network Connection", QSystemTrayIcon::Critical);
                updateInterval = 600000;
                return;
            }
        }
        else
        {
            updateInterval = defaultUpdateInterval;
        }
        if (!authorized) {
            authorize();
            return;
        }
        getStatistics();
        getPayments();
        getWithdrawals();
        return;
    }
}

void MainWindow::onSystemTrayLeftClicked( QSystemTrayIcon::ActivationReason reason )
{
    switch (reason) {
        case QSystemTrayIcon::Trigger:
            qDebug() << "triggered";
            authorize();
            break;
        case QSystemTrayIcon::DoubleClick:
            qDebug() << "double click";
            actionShowWindow();
            break;
     case QSystemTrayIcon::MiddleClick:
            qDebug() << "middle click";
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
    if (isMinimized())
    {
        qDebug() << "minimized";
        qApp->processEvents();
        show();
        QApplication::setActiveWindow(this);
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
            QMessageBox::information(this, tr("Systray"),
                                              tr("The program will keep running in the "
                                                 "system tray. To terminate the program, "
                                                 "choose <b>Quit</b> in the context menu "
                                                 "of the system tray entry."));
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
    qDebug() << "open";
}

void MainWindow::on_actionSave_triggered()
{
    qDebug() << "save";
    authorize();
}

void MainWindow::checkStatistics()
{
    qDebug() << (accountInfo.balance < 26.0);
    if (accountInfo.balance < 26.0 && accountInfo.showErrorNotification) {
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

bool MainWindow::authorize()
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest req( QUrl( QString("http://app.mclaut.com/api.php?hash=") ) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        actionOnline();
        //success
        QByteArray bytes = reply->readAll();
        QString response = QString::fromUtf8(bytes);
        QJsonDocument loadDoc(QJsonDocument::fromJson(bytes));
        QJsonObject obj = loadDoc.object();

        qDebug() << "Success: " << response;
        ui->result->setText(response);

        int status = obj["result"].toInt();
        if (status != 1) return false;
        accountInfo.certificate = obj["certificate"].toString();

        authorized = true;
        actionOnline();
        getStatistics();

        delete reply;
        return true;
    }
    else {
        //failure
        actionAway();
        qDebug() << "Authoriztion Failure" << reply->errorString();
        showMessage(reply->errorString(), "Failure", QSystemTrayIcon::Critical);
        accountInfo.inProgress = false;
        delete reply;
        return false;
    }
}

bool MainWindow::getStatistics()
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest req( QUrl( QString("http://app.mclaut.com/api.php?hash=") ) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        actionOnline();
        //success
        QByteArray bytes = reply->readAll();

        QString response = QString::fromUtf8(bytes);
        QJsonDocument loadDoc(QJsonDocument::fromJson(bytes));
        QJsonObject obj = loadDoc.object();

        qDebug() << "Success: " << response;
        ui->result->setText(response);

        accountInfo.id = obj["id"].toString().toInt();
        accountInfo.login = obj["login"].toString();
        accountInfo.account = obj["account"].toString().toInt();
        accountInfo.balance = obj["balance"].toString().toDouble();
        accountInfo.name = QString::fromUtf16(obj["name"].toString().utf16());
        long long paymentLastDate = obj["payment_date_last"].toString().toLongLong();
        if (paymentLastDate > 1000000) {
            accountInfo.paymentLastDate = QDateTime::fromTime_t(paymentLastDate);
        }
        accountInfo.updateLastDate = QDateTime::currentDateTime();
        accountInfo.inProgress = false;

        checkStatistics();

        delete reply;
        return true;
    }
    else {
        //failure
        actionAway();
        qDebug() << "Statistic retrieving Failure" << reply->errorString();
        showMessage(reply->errorString(), "Failure", QSystemTrayIcon::Critical);
        accountInfo.inProgress = false;
        delete reply;
        return false;
    }
}

bool MainWindow::getPayments()
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest req( QUrl( QString("http://app.mclaut.com/api.php?hash=") ) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        actionOnline();
        //success
        QByteArray bytes = reply->readAll();
        QString response = QString::fromUtf8(bytes);
        //QJsonDocument loadDoc(QJsonDocument::fromJson(bytes));
        //QJsonObject obj = loadDoc.object();
        //QString ip = obj["ip"].toString();

        qDebug() << "Success: " << response;
        ui->result->setText(response);
        delete reply;
        return true;
    }
    else {
        //failure
        actionAway();
        qDebug() << "Failure" << reply->errorString();
        showMessage(reply->errorString(), "Failure", QSystemTrayIcon::Critical);
        delete reply;
        return false;
    }
}

bool MainWindow::getWithdrawals()
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest req( QUrl( QString("http://app.mclaut.com/api.php?hash=") ) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        actionOnline();
        //success
        QByteArray bytes = reply->readAll();
        QString response = QString::fromUtf8(bytes);
        //QJsonDocument loadDoc(QJsonDocument::fromJson(bytes));
        //QJsonObject obj = loadDoc.object();
        //QString ip = obj["ip"].toString();

        qDebug() << "Success: " << response;
        ui->result->setText(response);
        delete reply;
        return true;
    }
    else {
        //failure
        actionAway();
        qDebug() << "Failure" << reply->errorString();
        showMessage(reply->errorString(), "Failure", QSystemTrayIcon::Critical);
        delete reply;
        return false;
    }
}






