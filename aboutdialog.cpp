#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

#include "aboutdialog.h"

AboutDialog::AboutDialog(const QRect &geometry, QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    QLabel *label = new QLabel("Developer: ", this);
    layout->addWidget(label);
    layout->addWidget(new QLabel("Dmitry Slepchenko", this));
    this->setGeometry(QRect(geometry.x() + 100, geometry.y() + 100, 250, 80));
    setLayout(layout);
}

AboutDialog::~AboutDialog()
{

}

void AboutDialog::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
#ifdef QT_DEBUG
    qDebug() << "close about dialog";
#endif
}
