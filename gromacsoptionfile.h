#ifndef GROMACSOPTIONFILE_H
#define GROMACSOPTIONFILE_H

#include "gromacsoptionenter.h"
#include "gromacstoolsdefinition.h"

class QLineEdit;

class GromacsOptionFile : public GromacsOptionEnter
{
    Q_OBJECT
public:
    explicit GromacsOptionFile(bool inputFile, QWidget *parent = 0, QString value = "");

    bool CollectOption(QStringList &listOfOptions);

public slots:
    void HandleError();

private:
    GromacsToolsDefinition &_gromacsToolsDefinition;
    bool _error;
    bool _inputFile;

private slots:
    void HandleSelectFileButton();
};

#endif // GROMACSOPTIONFILE_H
