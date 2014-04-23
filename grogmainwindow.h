#ifndef GROGMAINWINDOW_H
#define GROGMAINWINDOW_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class GrogMainWindow;
}

class GrogInfoBar;
class GromacsTools;
class GrogSettings;
class GromacsToolsDefinition;

class GrogMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit GrogMainWindow(QWidget *parent = 0);
    ~GrogMainWindow();

public slots:
    void ExecuteCurrentTool(const QString &execName, const QString &workingDir);
    void TerminateCurrentTool();
    void CurrentToolGetHelp(const QString &execName, const QString &workingDir);

private slots:
    void HandleSettings();
    void HandleAboutGROG();
    void HandleAboutQt();

    void HandleError(QProcess::ProcessError error);
    void HandleFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void HandleReadStderr();
    void HandleReadStdout();
    void HandleStateChanged(QProcess::ProcessState newState);
    void HandleStdin();

    void ShowTextEditContextMenu(const QPoint &pos);

private:
    Ui::GrogMainWindow *ui;
    GrogInfoBar *_grogInfoBar;
    GromacsTools *_gromacsTools;

    GrogSettings *_grogSettings;

    GromacsToolsDefinition &_toolsDefinition;

    const QString _mdgHelpMessage;
    void MdgFileGeneration(const QStringList &options);

    QProcess *_process;
    const QString _wrapper;
};

#endif // GROGMAINWINDOW_H
