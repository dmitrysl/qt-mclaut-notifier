#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

#include "aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    QLabel *label = new QLabel("Developer: ", this);
    layout->addWidget(label);
    layout->addWidget(new QLabel("Dmitry Slepchenko", this));
    setLayout(layout);
}

AboutDialog::~AboutDialog()
{

}

void AboutDialog::closeEvent(QCloseEvent *e)
{
    qDebug() << "close about dialog";
}
