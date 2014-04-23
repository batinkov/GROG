#include "gromacstoolsdefinition.h"
#include <cstddef>
#include <QFile>
#include <QTextStream>
#include <QDomDocument>

GromacsToolsDefinition & GromacsToolsDefinition::GetInstance()
{
    static GromacsToolsDefinition instance;

    return instance;
}

GromacsToolsDefinition::GromacsToolsDefinition() :
    _globalConfigDir(QDir::homePath() + "/.grog"),
    _defaultWorkingDirectory(QDir::home()),
    _serizlizationFileName("gromacs_tools_definition.xml"),
    _nonGromacsTool(QObject::tr("NA")),
    _toolsDefaultValues({
        {ToolsInfo("pdb2gmx",                                    "pdb2gmx_config.xml",   "pdb2gmx",       "1_pdb2gmx"  )},
        {ToolsInfo("editconf",                                   "editconf_config.xml",  "editconf",      "2_editconf" )},
        {ToolsInfo("genbox",                                     "genbox_config.xml",    "genbox",        "3_genbox"   )},
        {ToolsInfo("mdp file generator for energy minimization", "mdp-em_config.xml",    _nonGromacsTool, "4_grompp-em")},
        {ToolsInfo("grompp (energy minimization)",               "grompp-em_config.xml", "grompp",        "4_grompp-em")},
        {ToolsInfo("mdrun (energy minimization)",                "mdrun-em_config.xml",  "mdrun",         "5_mdrun-em" )},
        {ToolsInfo("mdp file generator for position restraints", "mdp-pr_config.xml",    _nonGromacsTool, "6_grompp-pr")},
        {ToolsInfo("grompp (position restraints)",               "grompp-pr_config.xml", "grompp",        "6_grompp-pr")},
        {ToolsInfo("mdrun (position restraints)",                "mdrun-pr_config.xml",  "mdrun",         "7_mdrun-pr" )},
        {ToolsInfo("mdp file generator",                         "mdp_config.xml",       _nonGromacsTool, "8_grompp"   )},
        {ToolsInfo("grompp",                                     "grompp_config.xml",    "grompp",        "8_grompp"   )},
        {ToolsInfo("mdrun",                                      "mdrun_config.xml",     "mdrun",         "9_mdrun"    )}
    }),
    _toolsDefinition(_toolsDefaultValues)
{
    if(_globalConfigDir.exists())
    {
        FromXML();
    }
    else
    {
        qDebug("The global config dir \"%s\" does not exist!", _globalConfigDir.absolutePath().toStdString().c_str());
    }
}

bool GromacsToolsDefinition::ToXML()
{
    bool exitStatus = true;

    QDomDocument document;
    QDomProcessingInstruction xmlProcessingInstruction = document.createProcessingInstruction("xml", "");
    xmlProcessingInstruction.setData("version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"");
    document.appendChild(xmlProcessingInstruction);

    QDomElement rootElement = document.createElement("GromacsToolsDefinition");
    document.appendChild(rootElement);

    QDomElement defaultWorkingDirectoryElement = document.createElement("DefaultWorkingDirectory");
    defaultWorkingDirectoryElement.setAttribute("value", _defaultWorkingDirectory.absolutePath());
    rootElement.appendChild(defaultWorkingDirectoryElement);

    for(unsigned int i = 0; i < _toolsDefinition.size(); ++i)
    {
        QDomElement gromacsToolElement = document.createElement("GromacsTool");
        gromacsToolElement.setAttribute("ToolName", _toolsDefinition[i]._toolName);
        gromacsToolElement.setAttribute("ExecName", _toolsDefinition[i]._execName);
        gromacsToolElement.setAttribute("WorkingDir", _toolsDefinition[i]._workingDir);
        rootElement.appendChild(gromacsToolElement);
    }

    // TODO: concatenate files/dirs in a better way
    QFile configFile(_globalConfigDir.absolutePath() + "/" + _serizlizationFileName);
    if(configFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        QTextStream textStream(&configFile);
        textStream << document.toString(4);
    }
    else
    {
        qDebug("Error! Cannot open \"%s\" properly!", configFile.fileName().toStdString().c_str());

        exitStatus = false;
    }

    configFile.close();

    return exitStatus;
}

bool GromacsToolsDefinition::FromXML()
{
    // TODO: concatenate files/dirs in a better way
    QFile configFile(_globalConfigDir.absolutePath() + "/" + _serizlizationFileName);
    if(!configFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug("Error! Cannot open \"%s\" properly!", configFile.fileName().toStdString().c_str());

        return false;
    }

    QString errorMessage;
    int row, col;

    QDomDocument domParser;
    if(!domParser.setContent(&configFile, &errorMessage, &row, &col))
    {
        qDebug("Error at row \"%d\", col \"%d\" - \"%s\"", row, col, errorMessage.toStdString().c_str());

        return false;
    }

    configFile.close();

    QDomElement rootElement = domParser.documentElement();
    if("GromacsToolsDefinition" != rootElement.tagName())
    {
        qDebug("The root element is \"%s\", but \"%s\" is expected!",
               rootElement.tagName().toStdString().c_str(),
               "GromacsToolsDefinition");

        return false;
    }

    for(QDomNode node = rootElement.firstChild(); !node.isNull(); node = node.nextSibling())
    {
        if("DefaultWorkingDirectory" == node.nodeName())
        {
            _defaultWorkingDirectory = node.toElement().attribute("value");
        }
        else if("GromacsTool" == node.nodeName())
        {
            for(unsigned int i=0; i<_toolsDefinition.size(); ++i)
            {
                QDomElement element = node.toElement();
                if(_toolsDefinition[i]._toolName == element.attribute("ToolName"))
                {
                    _toolsDefinition[i]._execName = element.attribute("ExecName");
                    _toolsDefinition[i]._workingDir = element.attribute("WorkingDir");

                    // break;
                }
            }
        }
        else
        {
            qDebug("Warning: unexpected node name!");
        }
    }

    return true;
}
