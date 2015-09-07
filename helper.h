#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <QException>
#include <QSettings>
#include <QMap>
#include <QUrlQuery>

#include <exception>

#include "utils.h"

class GeneralError { /* Your runtime error base class */ };

class NetworkConnectionFailureError: public std::exception  {
public:
    NetworkConnectionFailureError(QString message) throw()
    {
        this->message = message;
    }
    virtual ~NetworkConnectionFailureError() throw()
    {

    }
    virtual const char* what() const throw()
    {
        return message.toStdString().c_str();;
    }
    virtual QString getMessage()
    {
        return message;
    }

private:
    QString message;
};


typedef struct {
    int id;
    int clientId;
    bool isActive;
    QString login;
    double payAtDay;
    QDateTime periodFinish;
    QDateTime periodStart;
    QString tariff;
} User;

typedef struct {
    int clientId;
    QDateTime date;
    int kind;
    double sum;
    double sum_before;
    int type;
} Payment;

struct AccountInfo {
    int id;
    int city;
    int status;
    QString errorMessage;
    QString certificate;
    QString login;
    QString password;
    int account;
    double balance;
    QString name;
    QDateTime paymentLastDate;
    QDateTime updateLastDate;
    bool inProgress;
    bool showInfoNotification;
    bool showErrorNotification;
    AccountInfo& operator=(AccountInfo const &info)
    {
        id = info.id;
        city = info.city;
        status = info.status;
        errorMessage = info.errorMessage;
        certificate = info.certificate;
        login = info.login;
        account = info.account;
        balance = info.balance;
        name = info.name;
        paymentLastDate = info.paymentLastDate;
        updateLastDate = info.updateLastDate;
        inProgress = info.inProgress;
        showInfoNotification = info.showInfoNotification;
        showErrorNotification = info.showErrorNotification;
        return *this;
    }
};
typedef struct AccountInfo AccountInfo;



class Helper
{
public:
    enum City { CHERKASY = 0 };
    enum STATUS {NONE = 0, OK};
    enum HttpMethod {OPTIONS = 0, GET, POST, PUT, DELETE};

    Helper();
    ~Helper();

    static bool isConnectedToNetwork();
    static QString encode(const QString &string);
    static QString decode(const QString &string);

    int authClient(AccountInfo &accountInfo);
    User getInfo(AccountInfo &accountInfo);
    QList<Payment> getPayments(AccountInfo &accountInfo);
    QList<Payment> getWithdrawals(AccountInfo &accountInfo);
    QMap<QString, QString> loadSettings();
    void storeSettings(QMap<QString, QString> properties);


private:
    QByteArray executeRequest(const QUrlQuery &qUrlQuery, const HttpMethod &method = HttpMethod::GET);
    QByteArray executeRequest(const QString &params, const HttpMethod &method = HttpMethod::GET);
    QString prepareUrl(QUrlQuery &query);
    QString prepareUrl(const QString &params);
    enum ActionType { NO_ACTION = 0, CHECK_LOGIN, GET_INFO, GET_PAYMENTS, GET_WITHDRAWALS };

public:
    static QString APP_NAME;
private:
    QMap<QString, QString> properties;
    QSettings *settings;
    QString baseUrl = "http://app.mclaut.com/api.php";
    int sdkVersion = 19;
    QString device = "S1";
    QString model = "ZP998";
    QString product = "S1";

    QString versionCode = "2";
    QString versionName = "0.0.3";
};

#endif // HELPER_H
