#include <QCoreApplication>
#include <QtNetwork/QNetworkInterface>

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
    QString str1 = "API[" + sdkVersion + "], " + "Device[" + device + "], " + "Model[" + model + "]," + "Product[" + product + "]";
    QString str2 = "app=mobile&version=" + versionCode + "." + versionName + "&version_code=" + versionCode + "&" + params;
    return baseUrl + "?hash=" + encode(str2 + "&system_info=" + encode(str1));
}

QString Helper::executeRequest()
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

        delete reply;
        return response;
    }
    else {
        //failure
        qDebug() << "Request process failure" << reply->errorString();
        showMessage(reply->errorString(), "Failure", QSystemTrayIcon::Critical);
        accountInfo.inProgress = false;
        delete reply;
        return NULL;
    }
}

int Helper::authClient(const QString &login, const QString &password, int cityId)
{
     if (!Helper::isConnectedToNetwork()) return 0;

     QString params = "action=checkLogin&login=" + login + "&pass=" + password + "&city=" + cityId;

     qDebug() << params;

     QString url = prepareUrl(params);

     qDebug() << url;

     return 1;
}

AccountInfo Helper::getInfo(const QString certificate, int cityId)
{
    AccountInfo accountInfo = {0};

    QString params = "action=getInfo&certificate=" + certificate + "&city=" + cityId;

    qDebug() << params;

    QString url = prepareUrl(params);

    qDebug() << url;
}

QList<Payment> Helper::getPayments(const QString certificate, int cityId)
{

    QString params = "action=getPayments&certificate=" + certificate + "&city=" + cityId;

    qDebug() << params;

    QString url = prepareUrl(params);

    qDebug() << url;

    int i = localJSONObject1.getInt("result");
            if (i == 1)
            {
getJSONArray("payments");

localPaymentsEntity.setClient(localJSONObject2.getInt("id_client"));
            localPaymentsEntity.setType(localJSONObject2.getInt("type"));
            localPaymentsEntity.setDate(localJSONObject2.getInt("date"));
            localPaymentsEntity.setSum(localJSONObject2.getDouble("sum"));
            localPaymentsEntity.setSumBefore(localJSONObject2.getDouble("sum_before"));
            }
}

QList<Payment> Helper::getWithdrawal(const QString certificate, int cityId)
{
    QString params = "action=getWithdrawals&certificate=" + certificate + "&city=" + cityId;

    qDebug() << params;

    QString url = prepareUrl(params);

    qDebug() << url;

    int i = localJSONObject1.getInt("result");
            if (i == 1)
            {

getJSONArray("withdrawals");

localPaymentsEntity.setClient(localJSONObject2.getInt("id_client"));
            localPaymentsEntity.setType(localJSONObject2.getInt("type"));
            localPaymentsEntity.setDate(localJSONObject2.getInt("date"));
            localPaymentsEntity.setSum(localJSONObject2.getDouble("sum"));
            localPaymentsEntity.setSumBefore(localJSONObject2.getDouble("sum_before"));
            }
}
