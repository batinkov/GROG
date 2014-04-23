#ifndef GROMACSTOOLSDEFINITION_H
#define GROMACSTOOLSDEFINITION_H

#include <QString>
#include <QDir>
#include <vector>

class GromacsToolsDefinition
{
public:
    static GromacsToolsDefinition & GetInstance();

    QString ProjectsDefaultDirectory(){ return _defaultWorkingDirectory.absolutePath(); }
    void SetProjectsDefaultDirectory(const QString &projectDirectory){ _defaultWorkingDirectory = projectDirectory; }

    int NumberOfTools(){ return _toolsDefinition.size(); }
    bool IsNativeGromacsTool(int index){ return _nonGromacsTool != _toolsDefinition[index]._execName; }
    QString ToolName(int index){ return _toolsDefinition[index]._toolName; }
    QString ConfigFile(int index, bool fullPath=false)
    {
        if(fullPath)
        {
            // TODO: find a better way to concatenate directories
            return _globalConfigDir.absolutePath() + "/gromacs_tools_config/" + _toolsDefinition[index]._configFile;
        }
        else
        {
            return _toolsDefinition[index]._configFile;
        }
    }
    QString ExecutableName(int index){ return _toolsDefinition[index]._execName; }
    QString WorkingDir(int index){ return _toolsDefinition[index]._workingDir; }

    bool SetExecutableName(int index, const QString &execName)
    {
        if(!IsNativeGromacsTool(index))
        {
            return false;
        }

        _toolsDefinition[index]._execName = execName;

        return true;
    }
    void SetWorkingDir(int index, const QString &workingDir){ _toolsDefinition[index]._workingDir = workingDir; }

    void ResetToDefaults()
    {
        _defaultWorkingDirectory = QDir::homePath();
        _toolsDefinition = _toolsDefaultValues;
    }

    bool ToXML();

private:
    GromacsToolsDefinition();
    ~GromacsToolsDefinition(){}
    GromacsToolsDefinition(GromacsToolsDefinition &);
    GromacsToolsDefinition & operator=(const GromacsToolsDefinition &);

    bool FromXML();

private:
    const QDir _globalConfigDir;
    QDir _defaultWorkingDirectory;
    const QString _serizlizationFileName;

    struct ToolsInfo
    {
        ToolsInfo(QString toolName, QString configFile, QString execName, QString workingDir) :
            _toolName(toolName),
            _configFile(configFile),
            _execName(execName),
            _workingDir(workingDir)
        {
        }

        const ToolsInfo & operator=(const ToolsInfo &initializer)
        {
            this->_execName = initializer._execName;
            this->_workingDir = initializer._workingDir;

            return *this;
        }

        const QString _toolName;
        const QString _configFile;
        QString       _execName;
        QString       _workingDir;
    };

    const QString _nonGromacsTool;
    const QString _mdpHelpMessage;

    const std::vector<ToolsInfo> _toolsDefaultValues;
    std::vector<ToolsInfo> _toolsDefinition;
};

#endif // GROMACSTOOLSDEFINITION_H
