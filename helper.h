#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QList>
#include <QDateTime>
#include <QException>
#include <exception>

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
    int isActive;
    QString login;
    double payAtDay;
    int periodFinish;
    int periodStart;
    QString tariff;
} User;

typedef struct {
    int client;
    int date;
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

    Helper();
    ~Helper();
    static bool isConnectedToNetwork();
    int authClient(const QString &login, const QString &password, Helper::City city = Helper::CHERKASY);
    int authClient(AccountInfo &accountInfo);
    void getInfo(AccountInfo &accountInfo);
    QList<Payment> getPayments(const QString certificate, int cityId);
    QList<Payment> getWithdrawal(const QString certificate, int cityId);
    QString decode(const QString &string);


private:
    QByteArray executeRequest(const QString &params);
    QString prepareUrl(const QString &params);
    QString encode(const QString &string);
    enum ActionType { NO_ACTION = 0, CHECK_LOGIN, GET_INFO, GET_PAYMENTS, GET_WITHDRAWALS };


private:
    QString baseUrl = "http://app.mclaut.com/api.php";
    int sdkVersion = 19;
    QString device = "S1";
    QString model = "ZP998";
    QString product = "S1";

    QString versionCode = "2";
    QString versionName = "0.0.3";
};

#endif // HELPER_H
