#ifndef GROMACSOPTION_H
#define GROMACSOPTION_H

#include <QWidget>

class QHBoxLayout;

class GromacsOption : public QWidget
{
    Q_OBJECT
public:
    explicit GromacsOption(QWidget *parent = 0);

    virtual bool CollectOption(QStringList &listOfOptions) = 0;

protected:
    QHBoxLayout *_layout;
};

#endif // GROMACSOPTION_H
