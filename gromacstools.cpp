#include "gromacstools.h"
#include <QVector>
#include "gromacstool.h"
#include "gromacstoolsdefinition.h"
#include <QMenu>
#include <QDir>

GromacsTools::GromacsTools(QWidget *parent) :
    QDockWidget(parent),
    _toolBox(new QToolBox(this)),
    _tools(GromacsToolsDefinition::GetInstance())
{
    setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    setWindowTitle(tr("Gromacs Tools"));

    setMinimumWidth(400); // TODO: think of something better

    _toolBox->setFrameStyle(QFrame::Box);
    setWidget(_toolBox);

    for(int index=0; index<_tools.NumberOfTools(); ++index)
    {
        GromacsTool *gromacsTool = new GromacsTool(index, this);
        _toolBox->insertItem(index, gromacsTool, _tools.ToolName(index));
    }

    connect(_toolBox, SIGNAL(currentChanged(int)), this, SIGNAL(GromacsToolChanged(int)));
}

bool GromacsTools::CollectOptions(QStringList &listOfOptions)
{
    GromacsTool *currentTool = static_cast<GromacsTool *>(_toolBox->currentWidget());

    if(!currentTool->CollectOptions(listOfOptions))
    {
        listOfOptions.clear();
        return false;
    }

    return true;
}

void GromacsTools::AllToolsOptionsToXML()
{
    for(int index=0; index<_toolBox->count(); ++index)
    {
        GromacsTool *tool = static_cast<GromacsTool *>(_toolBox->widget(index));
        tool->OptionsToXML();
    }
}

void GromacsTools::CurrentToolOptionsToXML()
{
    GromacsTool *currentTool = static_cast<GromacsTool *>(_toolBox->currentWidget());

    currentTool->OptionsToXML();
}
