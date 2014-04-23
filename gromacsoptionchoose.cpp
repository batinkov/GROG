#include "gromacsoptionchoose.h"
#include <QHBoxLayout>

GromacsOptionChoose::GromacsOptionChoose(const QList<QString> &items, const QString &value, QWidget *parent) :
    GromacsOption(parent),
    _comboBox(new QComboBox(this))
{
    _comboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    _layout->addWidget(_comboBox);

    for(int i=0; i<items.size(); ++i)
    {
        _comboBox->addItem(items[i]);
    }

    if(!value.isEmpty())
    {
        int i = _comboBox->findText(value);
        _comboBox->setCurrentIndex(i);
    }
}
