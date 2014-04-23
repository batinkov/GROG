#ifndef GROMACSOPTIONCHOOSE_H
#define GROMACSOPTIONCHOOSE_H

#include "gromacsoption.h"
#include <QComboBox>

class GromacsOptionChoose : public GromacsOption
{
    Q_OBJECT
public:
    explicit GromacsOptionChoose(const QList<QString> &items, const QString &value, QWidget *parent = 0);

    bool CollectOption(QStringList &listOfOptions)
    {
        listOfOptions.push_back(_comboBox->currentText());

        return true;
    }

private:
    QComboBox *_comboBox;
};

#endif // GROMACSOPTIONCHOOSE_H
