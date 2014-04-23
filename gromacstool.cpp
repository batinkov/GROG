#include "gromacstool.h"

#include <QVBoxLayout>
#include <gromacsoptionsgroup.h>
#include <gromacsoption.h>
#include <gromacstoolsdefinition.h>
#include <QMouseEvent>
#include <QFile>
#include <QDomDocument>
#include <QMenu>
#include <QAction>
#include <QTextStream>

GromacsTool::GromacsTool(int toolId, QWidget *parent) :
    QWidget(parent),
    _myId(toolId),
    _toolsDefinition(GromacsToolsDefinition::GetInstance()),
    _layout(new QVBoxLayout(this)),
    _optionsGroups()
{
    _layout->setContentsMargins(0, 0, 0, 0);
    setLayout(_layout);

    GenerateInterface();
    // TestInterface();
}

void GromacsTool::mousePressEvent(QMouseEvent *event)
{
    if(Qt::RightButton == event->button())
    {
        ShowContextMenu();
    }
    else
    {
        event->ignore();
    }
}

void GromacsTool::ExpandAllGroups()
{
    for(unsigned i=0; i<_optionsGroups.size(); ++i)
    {
        _optionsGroups[i]->ShowOptions();
    }
}

void GromacsTool::CollapseAllGroups()
{
    for(unsigned i=0; i<_optionsGroups.size(); ++i)
    {
        _optionsGroups[i]->HideOptions();
    }
}

bool GromacsTool::CollectOptions(QStringList & listOfOptions)
{
    for(unsigned int i=0; i<_optionsGroups.size(); ++i)
    {
        if(false == _optionsGroups[i]->CollectOptions(listOfOptions))
        {
            return false;
        }
    }

    return true;
}

bool GromacsTool::OptionsToXML()
{
    // the following code is very ugly and needs to be beautified

    QStringList options;
    if(!CollectOptions(options))
    {
        return false;
    }

    QFile configFile(_toolsDefinition.ConfigFile(_myId, true));

    if(!configFile.exists())
    {
        qDebug("The file \"%s\" does not exist!", configFile.fileName().toStdString().c_str());

        return false;
    }

    QString errorMessage;
    int row, col;

    QDomDocument document;
    if(!document.setContent(&configFile, &errorMessage, &row, &col))
    {
        qDebug("Error at row \"%d\", col \"%d\" - \"%s\"", row, col, errorMessage.toStdString().c_str());

        configFile.close();

        return false;
    }

    configFile.close();

    int counter = 0;
    QDomElement root = document.documentElement();
    for(QDomNode optionGroupNode = root.firstChild(); !optionGroupNode.isNull(); optionGroupNode = optionGroupNode.nextSibling())
    {
        for(QDomNode optionNode = optionGroupNode.firstChild(); !optionNode.isNull(); optionNode = optionNode.nextSibling())
        {
            QDomElement optionElenemt = optionNode.toElement();

            if((counter<options.size()) && (optionElenemt.attribute("name") == options[counter]))
            {
                optionElenemt.setAttribute("value", options[counter+1]);
                optionElenemt.removeAttribute("enabled");

                counter += 2;
            }
            else if((counter<options.size()) && (optionElenemt.attribute("name").contains(options[counter])))
            {
                optionElenemt.setAttribute("value", options[counter]);
                optionElenemt.removeAttribute("enabled");

                ++counter;
            }
            else
            {
                optionElenemt.setAttribute("enabled", "no");
            }
        }
    }

    if(!configFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        qDebug("Error! Cannot open \"%s\" properly!", configFile.fileName().toStdString().c_str());

        return false;
    }

    QTextStream textStream(&configFile);
    textStream << document.toString(4);

    configFile.close();

    return true;
}

void GromacsTool::GenerateInterface()
{
    // TODO: the code is very ugly - rewrite it in a better way
    // TODO: remove all qDebug with some kind of logger

    QFile configFile(_toolsDefinition.ConfigFile(_myId, true));

    if(!configFile.exists())
    {
        qDebug("The file \"%s\" does not exist!", configFile.fileName().toStdString().c_str());

        return;
    }

    QString errorMessage;
    int row, col;

    QDomDocument domParser;
    if(!domParser.setContent(&configFile, &errorMessage, &row, &col))
    {
        qDebug("Error at row \"%d\", col \"%d\" - \"%s\"", row, col, errorMessage.toStdString().c_str());

        return;
    }

    QDomElement root = domParser.documentElement();
    if("GrogConfigFile" != root.tagName()) // check for root element "GrogConfigFile"
    {
        qDebug("The root element is \"%s\", but \"%s\" is expected!",
               root.tagName().toStdString().c_str(),
               _toolsDefinition.ToolName(_myId).toStdString().c_str() );

        return;
    }

    if(_toolsDefinition.ToolName(_myId) != root.attribute("tool")) // check for the attribute "tool"
    {
        qDebug("The root element is expected to have attribute \"tool=%s\", but the value found is \"%s\"!",
               _toolsDefinition.ToolName(_myId).toStdString().c_str(),
               root.attribute("tool").toStdString().c_str() );

        return;
    }

    for(QDomNode optionGroupNode = root.firstChild(); !optionGroupNode.isNull(); optionGroupNode = optionGroupNode.nextSibling())
    {
        // the node is expected to be OptionGroup
        if("OptionsGroup" == optionGroupNode.nodeName())
        {
            // create OptionGroup
            GromacsOptionsGroup *optionsGroup = GenerateOptionGroup(optionGroupNode);

            // iterate over the children and create the options for each group
            for(QDomNode optionNode = optionGroupNode.firstChild(); !optionNode.isNull(); optionNode = optionNode.nextSibling())
            {
                GenerateOption(optionNode, optionsGroup);
            }
        }
        else
        {
            qDebug("%s is expected but %s is found!",
                   "OptionsGroup",
                   optionGroupNode.nodeName().toStdString().c_str());
        }
    }

    QSpacerItem *verticalSpacer = new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Expanding);
    _layout->addSpacerItem(verticalSpacer);
}

GromacsOptionsGroup *GromacsTool::GenerateOptionGroup(QDomNode &optionGroupNode)
{
    QDomElement optionGroupElement(optionGroupNode.toElement());

    GromacsOptionsGroup *optionsGroup = new GromacsOptionsGroup(optionGroupElement.attribute("name"), this);

    _optionsGroups.push_back(optionsGroup);

    _layout->addWidget(optionsGroup);

    if(!optionGroupElement.attribute("name").contains("File", Qt::CaseInsensitive))
    {
        optionsGroup->HideOptions();
    }

    return optionsGroup;
}

void GromacsTool::GenerateOption(QDomNode &optionNode, GromacsOptionsGroup *optionsGroup)
{
    QDomElement optionElement(optionNode.toElement());
    const bool isMandatory = ("yes" == optionElement.attribute("mandatory"));
    const QString optionName(optionElement.attribute("name"));
    const bool isEnabled = ("no" != optionElement.attribute("enabled"));
    const QString optionValue(optionElement.attribute("value"));
    const QString optionTip(optionElement.text().simplified());

    if("InputFile" == optionNode.nodeName())
    {
        optionsGroup->AddFile(isMandatory, optionName, true, optionValue, isEnabled, optionTip);
    }
    else if("OutputFile" == optionNode.nodeName())
    {
        optionsGroup->AddFile(isMandatory, optionName, false, optionValue, isEnabled, optionTip);
    }
    else if("OptionChoose" == optionNode.nodeName())
    {
        const QString optionValues(optionElement.attribute("values").simplified());
        QList<QString> valuesList = optionValues.split(";");

        const QString value(optionElement.attribute("value"));

        optionsGroup->AddOption(isMandatory, optionName, valuesList, value, isEnabled, optionTip);
    }
    else if("OptionEnter" == optionNode.nodeName())
    {
        optionsGroup->AddOption(isMandatory, optionName, optionValue, isEnabled, optionTip);
    }
    else
    {
        qDebug("Unknown ooption! - \"%s\" in \"%s\"",
               optionNode.nodeName().toStdString().c_str(),
               _toolsDefinition.ToolName(_myId).toStdString().c_str());
    }
}

void GromacsTool::ShowContextMenu()
{
    const QString expandAll(tr("Expand All"));
    const QString collapseAll(tr("Collapse All"));

    QMenu menu;

    bool atLeastOneGroupExpanded = false;
    bool atLeastOneGroupCollapsed = false;
    for(unsigned i=0; i<_optionsGroups.size(); ++i)
    {
        _optionsGroups[i]->OptionsVisible() ? atLeastOneGroupExpanded = true : atLeastOneGroupCollapsed = true;
    }

    if(atLeastOneGroupExpanded && atLeastOneGroupCollapsed)
    {
        menu.addAction(expandAll);
        menu.addAction(collapseAll);
    }
    else if(atLeastOneGroupExpanded)
    {
        menu.addAction(collapseAll);
    }
    else
    {
        menu.addAction(expandAll);
    }

    QAction *selectedItem = menu.exec(QCursor::pos());
    if(selectedItem)
    {
        (expandAll == selectedItem->text()) ? ExpandAllGroups() : CollapseAllGroups();
    }
}
