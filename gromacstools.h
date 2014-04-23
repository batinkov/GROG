#ifndef GROMACSTOOLS_H
#define GROMACSTOOLS_H

#include <QDockWidget>
#include <QToolBox>

class GromacsToolsDefinition;

class GromacsTools : public QDockWidget
{
    Q_OBJECT
public:
    explicit GromacsTools(QWidget *parent = 0);

    bool CollectOptions(QStringList &listOfOptions);

    void InitGromacsTool(){ emit GromacsToolChanged(_toolBox->currentIndex()); }

    int CurrentToolIndex(){ return _toolBox->currentIndex(); }

public slots:
    void AllToolsOptionsToXML();
    void CurrentToolOptionsToXML();

private:
    QToolBox *_toolBox;
    GromacsToolsDefinition &_tools;

signals:
    void GromacsToolChanged(int index);
    void ExecutableNameChanged(QString name);
    void WorkingDirChanged(QString name);
};

#endif // GROMACSTOOLS_H
