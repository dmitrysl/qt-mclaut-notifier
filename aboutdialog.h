#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QWidget>

class AboutDialog : public QWidget
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

protected:
    void closeEvent(QCloseEvent *e);

signals:

public slots:
};

#endif // ABOUTDIALOG_H
