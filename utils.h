#ifndef UTILS_H
#define UTILS_H

#include <QString>

class Utils
{
public:
    Utils();
    ~Utils();

    static QString buildTooltipMessage(QString details);
};

#endif // UTILS_H
