#include <QCoreApplication>

#include <QtNetwork/QNetworkInterface>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QUrl>
#include <QUrlQuery>

#include <QJsonDocument>
#include <QJsonObject>

#include "helper.h"

Helper::Helper()
{

}

Helper::~Helper()
{

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
            #ifdef DEBUG
            // details of connection
            qDebug() << "name:" << iface.name() << endl
                     << "ip addresses:" << endl
                     << "mac:" << iface.hardwareAddress() << endl;
            #endif
            for (int j=0; j<iface.addressEntries().count(); j++)
            {
                #ifdef DEBUG
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

        qDebug() << "---------------";
        qDebug() << response;

        delete reply;
        return bytes;
    }
    else {
        //failure
        qDebug() << "Request process failure" << reply->errorString();
        delete reply;
        return reply->errorString().toUtf8();
    }
}

int Helper::authClient(AccountInfo &accountInfo)
{
    if (!Helper::isConnectedToNetwork()) return 0;

    QString params = "action=checkLogin&login=" + accountInfo.login + "&pass=" + accountInfo.password + "&city=" + accountInfo.city;

    QString url = prepareUrl(params);

    qDebug() << url;

    QByteArray response = executeRequest(url);

    QJsonDocument loadDoc(QJsonDocument::fromJson(response));
    QJsonObject obj = loadDoc.object();

    qDebug() << "===============";
    qDebug() << QString::fromUtf8(response);

//    AccountInfo accountInfo = {0};

//    accountInfo.city = (int) city;
    accountInfo.status = obj["result"].toInt();
    accountInfo.certificate = obj["certificate"].toString();
}

int Helper::authClient(const QString &login, const QString &password, Helper::City city)
{
     if (!Helper::isConnectedToNetwork()) return 0;

     QString params = "action=checkLogin&login=" + login + "&pass=" + password + "&city=" + (int) city;

     qDebug() << params;

     QString url = prepareUrl(params);

     qDebug() << url;

     QByteArray response = executeRequest(url);

     QJsonDocument loadDoc(QJsonDocument::fromJson(response));
     QJsonObject obj = loadDoc.object();

     AccountInfo accountInfo = {0};

     accountInfo.city = (int) city;
     accountInfo.status = obj["result"].toInt();
     accountInfo.certificate = obj["certificate"].toString();

     return accountInfo.status;
}

void Helper::getInfo(AccountInfo &accountInfo)
{
    QString params = "action=getInfo&certificate=" + accountInfo.certificate + "&city=" + accountInfo.city;

    qDebug() << params;

    QString url = prepareUrl(params);

    qDebug() << url;

    QByteArray response = executeRequest(url);

    QJsonDocument loadDoc(QJsonDocument::fromJson(response));
    QJsonObject obj = loadDoc.object();

    accountInfo.status = obj["result"].toInt();

    if (accountInfo.status == 1)
    {
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
    }
}

QList<Payment> Helper::getPayments(const QString certificate, int cityId)
{

    QString params = "action=getPayments&certificate=" + certificate + "&city=" + cityId;

    qDebug() << params;

    QString url = prepareUrl(params);

    qDebug() << url;

//    int i = localJSONObject1.getInt("result");
//            if (i == 1)
//            {
//getJSONArray("payments");

//localPaymentsEntity.setClient(localJSONObject2.getInt("id_client"));
//            localPaymentsEntity.setType(localJSONObject2.getInt("type"));
//            localPaymentsEntity.setDate(localJSONObject2.getInt("date"));
//            localPaymentsEntity.setSum(localJSONObject2.getDouble("sum"));
//            localPaymentsEntity.setSumBefore(localJSONObject2.getDouble("sum_before"));
//            }
}

QList<Payment> Helper::getWithdrawal(const QString certificate, int cityId)
{
    QString params = "action=getWithdrawals&certificate=" + certificate + "&city=" + cityId;

    qDebug() << params;

    QString url = prepareUrl(params);

    qDebug() << url;

//    int i = localJSONObject1.getInt("result");
//            if (i == 1)
//            {

//getJSONArray("withdrawals");

//localPaymentsEntity.setClient(localJSONObject2.getInt("id_client"));
//            localPaymentsEntity.setType(localJSONObject2.getInt("type"));
//            localPaymentsEntity.setDate(localJSONObject2.getInt("date"));
//            localPaymentsEntity.setSum(localJSONObject2.getDouble("sum"));
//            localPaymentsEntity.setSumBefore(localJSONObject2.getDouble("sum_before"));
//            }
}
