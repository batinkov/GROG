#include "groginfobar.h"

#include <QPushButton>
#include <QSpacerItem>
#include <gromacstoolsdefinition.h>


GrogInfoBar::GrogInfoBar(QWidget *parent) :
    QToolBar(tr("Grog Info Bar"), parent),
    _toolsDefinition(GromacsToolsDefinition::GetInstance()),
    _startText(tr("&Start")),
    _stopText(tr("&Stop"))
{
    setMinimumHeight(35); // TODO: this 35 does not look good - try to think of something better

    setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
    setOrientation(Qt::Horizontal);

    addWidget(new QLabel(tr("<B>ExecutableName: </B>"), this));
    _executableName = new QLabel("Unnamed", this);
    addWidget(_executableName);
    addSeparator();

    addWidget(new QLabel(tr("<B>Working dir: </B>"), this));
    _workingDir = new QLabel("NotSpecified", this);
    addWidget(_workingDir);
    addSeparator();

    _startStop = new QPushButton(_startText, this);
    addWidget(_startStop);
    connect(_startStop, SIGNAL(clicked()), this, SLOT(HandleStartStopButton()));

    _help = new QPushButton(tr("&Help"), this);
    addWidget(_help);
    connect(_help, SIGNAL(clicked()), this, SLOT(HandleHelpButton()));
    addSeparator();
}

void GrogInfoBar::EnableInterface(bool enabled)
{
    _workingDir->setEnabled(enabled);

    enabled ? _startStop->setText(_startText) : _startStop->setText(_stopText);

    _help->setEnabled(enabled);
}

void GrogInfoBar::ToolChanged(int index)
{
    if(-1 != index)
    {
        _executableName->setText(_toolsDefinition.ExecutableName(index));

        _workingDir->setText(_toolsDefinition.ProjectsDefaultDirectory() + "/" + _toolsDefinition.WorkingDir(index));

        WorkingDirExists() ? _workingDir->setStyleSheet("color: black;") : _workingDir->setStyleSheet("color: red;");
    }
}

void GrogInfoBar::HandleStartStopButton()
{
    if(_startText == _startStop->text())
    {
        emit StartClicked(_executableName->text(), _workingDir->text());
    }
    else
    {
        emit StopClicked();
    }
}

void GrogInfoBar::HandleHelpButton()
{
    emit HelpClicked(_executableName->text(), _workingDir->text());
}
