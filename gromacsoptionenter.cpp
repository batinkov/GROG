#include "gromacsoptionenter.h"
#include <QHBoxLayout>

GromacsOptionEnter::GromacsOptionEnter(QWidget *parent, QString value) :
    GromacsOption(parent),
    _lineEdit(new QLineEdit(value, this))
{
    _lineEdit->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    _layout->addWidget(_lineEdit);
}
