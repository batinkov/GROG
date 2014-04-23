#ifndef GROMACSTOOL_H
#define GROMACSTOOL_H

#include <QWidget>
#include <QString>
#include <vector>

class GromacsToolsDefinition;
class QVBoxLayout;
class GromacsOptionsGroup;
class QDomNode;

class GromacsTool : public QWidget
{
    Q_OBJECT
public:
    explicit GromacsTool(int toolId, QWidget *parent = 0);

    void mousePressEvent(QMouseEvent *event);

    void ExpandAllGroups();
    void CollapseAllGroups();

    bool CollectOptions(QStringList & listOfOptions);

    bool OptionsToXML();

private:
    void GenerateInterface();
    GromacsOptionsGroup * GenerateOptionGroup(QDomNode &optionGroupNode);
    void GenerateOption(QDomNode &optionNode, GromacsOptionsGroup *optionsGroup);

    void ShowContextMenu();

private:
    const int _myId;
    GromacsToolsDefinition &_toolsDefinition;
    QVBoxLayout *_layout;

    std::vector<GromacsOptionsGroup *> _optionsGroups;
};

#endif // GROMACSTOOL_H
