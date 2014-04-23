#ifndef GROGINFOBAR_H
#define GROGINFOBAR_H

#include <QToolBar>
#include <QLabel>
#include <QDir>

class QLabel;
class QPushButton;
class GromacsToolsDefinition;

class GrogInfoBar : public QToolBar
{
    Q_OBJECT
public:
    explicit GrogInfoBar(QWidget *parent = 0);

    void EnableInterface(bool enabled);

    QString CurrentWorkingDir()
    {
        return _workingDir->text();
    }

    bool WorkingDirExists()
    {
        QDir workingDir(_workingDir->text());

        return workingDir.exists();
    }

public slots:
    void ToolChanged(int index = -1);

private slots:
    void HandleStartStopButton();
    void HandleHelpButton();

signals:
    void StartClicked(const QString &execName, const QString &workingDir);
    void StopClicked();
    void HelpClicked(const QString &execName, const QString &workingDir);

private:
    QLabel *_executableName;
    QLabel *_workingDir;
    QPushButton *_startStop;
    QPushButton *_help;

    GromacsToolsDefinition &_toolsDefinition;

    const QString _startText;
    const QString _stopText;
};

#endif // GROGINFOBAR_H
