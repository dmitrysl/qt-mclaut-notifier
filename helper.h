#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QList>


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

typedef struct {
    int id;
    QString certificate;
    QString login;
    int account;
    double balance;
    QString name;
    QDateTime paymentLastDate;
    QDateTime updateLastDate;
    bool inProgress;
    bool showInfoNotification;
    bool showErrorNotification;
} AccountInfo;



class Helper
{
public:
    Helper();
    ~Helper();
    static bool isConnectedToNetwork();
    int authClient(const QString &login, const QString &password, int cityId);
    AccountInfo getInfo(const QString certificate, int cityId);
    QList<Payment> getPayments(const QString certificate, int cityId);
    QList<Payment> getWithdrawal(const QString certificate, int cityId);
    QString decode(const QString &string);

private:
    QString prepareUrl(const QString &params);
    QString encode(const QString &string);
    enum ActionType { NO_ACTION = 0, CHECK_LOGIN, GET_INFO, GET_PAYMENTS, GET_WITHDRAWALS };
    enum City { CHERKASY = 0 };

private:
    QString baseUrl = "http://app.mclaut.com/api.php?hash=";
    int sdkVersion = 19;
    QString device = "S1";
    QString model = "ZP998";
    QString product = "S1";

    QString versionCode = "2";
    QString versionName = "0.0.3";
};

#endif // HELPER_H
