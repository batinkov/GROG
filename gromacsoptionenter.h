#ifndef GROMACSOPTIONENTER_H
#define GROMACSOPTIONENTER_H

#include "gromacsoption.h"
#include <QLineEdit>

class GromacsOptionEnter : public GromacsOption
{
    Q_OBJECT
public:
    explicit GromacsOptionEnter(QWidget *parent = 0, QString value = "");

    bool CollectOption(QStringList &listOfOptions)
    {
        listOfOptions.push_back(_lineEdit->text());

        return true;
    }

protected:
    QLineEdit *_lineEdit;
};

#endif // GROMACSOPTIONENTER_H
