#include <QCoreApplication>

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include <QDir>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "helper.h"

QString Helper::APP_NAME = "McLautAccountInfo";

Helper::Helper()
{

}

Helper::~Helper()
{
    delete settings;
}

QMap<QString, QString> Helper::loadSettings()
{
    QString login = properties.value("account/login", "");
    QString pass = properties.value("account/password", "");

    settings = new QSettings(qApp->applicationDirPath() + QDir::separator() + "settings.conf", QSettings::IniFormat);

#ifdef QT_DEBUG
    qDebug() << login;
    qDebug() << pass;
#endif

    properties.clear();
    if (!properties.empty()) return properties;

#ifdef QT_DEBUG
    if (settings->isWritable())
    {
        qDebug() << "writable";
    }
#endif

    login = settings->value("account/login", "").toString();
    pass = settings->value("account/password", "").toString();
    QString runOnbootCheckBox = settings->value("app/runOnbootCheckBox", "0").toString();
    QString startMinimizedCheckBoxBool = settings->value("app/startMinimizedCheckBox", "0").toString();

    properties.insert("account/login", login);
    properties.insert("account/password", pass);
    properties.insert("app/runOnbootCheckBox", runOnbootCheckBox);
    properties.insert("app/startMinimizedCheckBox", startMinimizedCheckBoxBool);

    return properties;
}

void Helper::storeSettings(QMap<QString, QString> properties)
{
    foreach (QString key, properties.uniqueKeys())
    {
        settings->setValue(key, properties.value(key));
    }
    settings->sync();
}

bool Helper::isConnectedToNetwork()
{
    QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    bool result = false;
    for (int i = 0; i < ifaces.count(); i++)
    {
        QNetworkInterface iface = ifaces.at(i);
        if (iface.flags().testFlag(QNetworkInterface::IsUp) && !iface.flags().testFlag(QNetworkInterface::IsLoopBack))
        {
#ifdef QT_DEBUG
            // details of connection
            qDebug() << "name:" << iface.name() << endl
                     << "ip addresses:" << endl
                     << "mac:" << iface.hardwareAddress() << endl;
#endif
            for (int j=0; j<iface.addressEntries().count(); j++)
            {
#ifdef QT_DEBUG
                qDebug() << iface.addressEntries().at(j).ip().toString()
                        << " / " << iface.addressEntries().at(j).netmask().toString() << endl;
#endif
                // got an interface which is up, and has an ip address
                if (result == false)
                    result = true;
            }
        }

    }
    return result;
}

QString Helper::encode(const QString &string)
{
    QByteArray ba;
    ba.append(string);
    return ba.toBase64();
}

QString Helper::decode(const QString &string)
{
    QByteArray ba;
        ba.append(string);
        return QByteArray::fromBase64(ba);
}

QString Helper::prepareUrl(const QString &params)
{
    //    "app=mobile&version=2.0.0.3&version_code=2&action=checkLogin&login=gag47k36&pass=826346GHyD&city=0&system_info=QVBJWzE5XSwgRGV2aWNlW1MxXSwgTW9kZWxbWlA5OThdLFByb2R1Y3RbUzFd"
    //    "API[19], Device[S1], Model[ZP998],Product[S1]"
    QString str1 = "API[" + QString::number(sdkVersion) + "], " + "Device[" + device + "], " + "Model[" + model + "]," + "Product[" + product + "]";
    QString str2 = "app=mobile&version=" + versionCode + "." + versionName + "&version_code=" + versionCode + "&" + params;
    return baseUrl + "?hash=" + encode(str2 + "&system_info=" + encode(str1));
}

QByteArray Helper::executeRequest(const QString &url)
{
    // create custom temporary event loop on stack
    QEventLoop eventLoop;

    // "quit()" the event-loop, when the network request "finished()"
    QNetworkAccessManager mgr;
    QObject::connect(&mgr, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

    QNetworkRequest req( QUrl( QString(url + "") ) );
    QNetworkReply *reply = mgr.get(req);
    eventLoop.exec(); // blocks stack until "finished()" has been called

    if (reply->error() == QNetworkReply::NoError) {
        //success
        QByteArray bytes = reply->readAll();
        QString response = QString::fromUtf8(bytes);

#ifdef QT_DEBUG
        qDebug() << "---------------";
        qDebug() << response;
#endif

        delete reply;
        return bytes;
    }
    else {
        //failure
#ifdef QT_DEBUG
        qDebug() << "Request process failure" << reply->errorString();
#endif
        delete reply;
        return reply->errorString().toUtf8();
    }
}

int Helper::authClient(AccountInfo &accountInfo)
{
    if (!Helper::isConnectedToNetwork()) return 0;

    QString params = "action=checkLogin&login=" + accountInfo.login + "&pass=" + accountInfo.password + "&city=" + accountInfo.city;

    QString url = prepareUrl(params);

#ifdef QT_DEBUG
    qDebug() << url;
#endif

    QByteArray response = executeRequest(url);

    QJsonDocument loadDoc(QJsonDocument::fromJson(response));
    QJsonObject obj = loadDoc.object();

#ifdef QT_DEBUG
    qDebug() << "===============";
    qDebug() << QString::fromUtf8(response);
#endif

    accountInfo.status = obj["result"].toInt();
    accountInfo.certificate = obj["certificate"].toString();

    return accountInfo.status;
}

User Helper::getInfo(AccountInfo &accountInfo)
{
    QString params = "action=getInfo&certificate=" + accountInfo.certificate + "&city=" + accountInfo.city;

#ifdef QT_DEBUG
    qDebug() << params;
#endif

    QString url = prepareUrl(params);

#ifdef QT_DEBUG
    qDebug() << url;
#endif

    QByteArray response = executeRequest(url);

    QJsonDocument loadDoc(QJsonDocument::fromJson(response));
    QJsonObject obj = loadDoc.object();

    accountInfo.status = obj["result"].toInt();

    User user = User();

    if (accountInfo.status == OK)
    {
        accountInfo.id = obj["id"].toString().toInt();
        accountInfo.login = obj["login"].toString();
        accountInfo.account = obj["account"].toString().toInt();
        accountInfo.balance = obj["balance"].toString().toDouble();
        accountInfo.name = QString::fromUtf16(obj["name"].toString().utf16());
        accountInfo.paymentLastDate = QDateTime::fromTime_t(obj["payment_date_last"].toString().toLongLong());
        accountInfo.updateLastDate = QDateTime::currentDateTime();
        accountInfo.inProgress = false;

        QJsonObject userJson = obj["users"].toArray().at(0).toObject();
        user.id = userJson["id"].toString().toInt();
        user.login = userJson["login"].toString();
        user.clientId = userJson["id_client"].toString().toInt();
        user.isActive = bool(userJson["is_active"].toString().toInt());
        user.payAtDay = userJson["pay_at_day"].toString().toDouble();
        user.periodStart = QDateTime::fromTime_t(userJson["payment_date_last"].toString().toLongLong());
        user.periodFinish = QDateTime::fromTime_t(userJson["period_finish"].toString().toLongLong());
        user.tariff = QString::fromUtf16(userJson["tariff"].toString().utf16());
    }
    return user;
}

QList<Payment> Helper::getPayments(AccountInfo &accountInfo)
{
    QList<Payment> payments;

    QString params = "action=getPayments&certificate=" + accountInfo.certificate + "&city=" + accountInfo.city;

#ifdef QT_DEBUG
    qDebug() << params;
#endif

    QString url = prepareUrl(params);

#ifdef QT_DEBUG
    qDebug() << url;
#endif

    QByteArray response = executeRequest(url);

    QJsonDocument loadDoc(QJsonDocument::fromJson(response));
    QJsonObject obj = loadDoc.object();

    accountInfo.status = obj["result"].toInt();

    if (accountInfo.status == OK)
    {
        if (obj["payments"].isArray())
        {
            QJsonArray paymentsJson = obj["payments"].toArray();
            foreach (const QJsonValue &paymentJson, paymentsJson)
            {
                QJsonObject paymentObj = paymentJson.toObject();
                Payment payment = Payment();
                payment.clientId = paymentObj["id_client"].toString().toInt();
                payment.type = paymentObj["type"].toInt();
                payment.date = QDateTime::fromTime_t(paymentObj["date"].toString().toLongLong());
                payment.sum = paymentObj["summ"].toDouble();
                payment.sum_before = paymentObj["summ_before"].toDouble();
                payment.sum = static_cast<double>(static_cast<int>(payment.sum*100+0.5))/100.0;
                payment.sum_before = static_cast<double>(static_cast<int>(payment.sum_before*100+0.5))/100.0;
                payments.append(payment);
            }
        }
    }
    return payments;
}

QList<Payment> Helper::getWithdrawals(AccountInfo &accountInfo)
{
    QList<Payment> payments;

    QString params = "action=getWithdrawals&certificate=" + accountInfo.certificate + "&city=" + accountInfo.city;

#ifdef QT_DEBUG
    qDebug() << params;
#endif

    QString url = prepareUrl(params);

#ifdef QT_DEBUG
    qDebug() << url;
#endif

    QByteArray response = executeRequest(url);

    QJsonDocument loadDoc(QJsonDocument::fromJson(response));
    QJsonObject obj = loadDoc.object();

    accountInfo.status = obj["result"].toInt();

    if (accountInfo.status == OK)
    {
        if (obj["withdrawals"].isArray())
        {
            QJsonArray paymentsJson = obj["withdrawals"].toArray();
            foreach (const QJsonValue &paymentJson, paymentsJson)
            {
                QJsonObject paymentObj = paymentJson.toObject();
                Payment payment = Payment();
                payment.clientId = paymentObj["id_client"].toString().toInt();
                payment.type = paymentObj["type"].toInt();
                payment.date = QDateTime::fromTime_t(paymentObj["date"].toString().toLongLong());
                payment.sum = paymentObj["summ"].toDouble();
                payment.sum_before = paymentObj["summ_before"].toDouble();
                payment.sum = static_cast<double>(static_cast<int>(payment.sum*100+0.5))/100.0;
                payment.sum_before = static_cast<double>(static_cast<int>(payment.sum_before*100+0.5))/100.0;
                payments.append(payment);
            }
        }
    }
    return payments;
}
