#include "gromacsoptionfile.h"
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QFileDialog>
#include <QFile>

GromacsOptionFile::GromacsOptionFile(bool inputFile, QWidget *parent, QString value) :
    GromacsOptionEnter(parent, value),
    _gromacsToolsDefinition(GromacsToolsDefinition::GetInstance()),
    _error(false),
    _inputFile(inputFile)
{
    if(_inputFile)
    {
        QPushButton *pushButton = new QPushButton("...", this);
        pushButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        _layout->addWidget(pushButton);

        connect(pushButton, SIGNAL(clicked()), this, SLOT(HandleSelectFileButton()));
        connect(_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(HandleError()));

        HandleError();
    }
}

bool GromacsOptionFile::CollectOption(QStringList &listOfOptions)
{
    if(_error)
    {
        return false;
    }

    listOfOptions.push_back(_lineEdit->text());

    return true;
}

void GromacsOptionFile::HandleSelectFileButton()
{
    QString fileName = \
        QFileDialog::getOpenFileName(this, tr("Select input file"), _gromacsToolsDefinition.ProjectsDefaultDirectory());

    if("" != fileName)
    {
        _lineEdit->setText(fileName);
    }
}

void GromacsOptionFile::HandleError()
{
    if(_inputFile && isEnabled() && (!QFile::exists(_lineEdit->text())))
    {
        _lineEdit->setStyleSheet("background-color: red");
        _error = true;
    }
    else
    {
        _lineEdit->setStyleSheet("background-color: white");
        _error = false;
    }
}
