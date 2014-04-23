#include "gromacsoption.h"
#include <QHBoxLayout>

#include <QPushButton>

GromacsOption::GromacsOption(QWidget *parent) :
    QWidget(parent),
    _layout(new QHBoxLayout(this))
{
    setLayout(_layout);
}
