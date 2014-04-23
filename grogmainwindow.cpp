#include "grogmainwindow.h"
#include "ui_grogmainwindow.h"
#include "gromacstools.h"
#include "groginfobar.h"
#include "gromacstoolsdefinition.h"
#include <grogsettings.h>
#include <QMessageBox>

GrogMainWindow::GrogMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GrogMainWindow),
    _grogInfoBar(new GrogInfoBar(this)),
    _gromacsTools(new GromacsTools(this)),
    _grogSettings(new GrogSettings(this)),
    _toolsDefinition(GromacsToolsDefinition::GetInstance()),
    _mdgHelpMessage(tr("This tool generates the MDP file needed as an input for the GROMPP tool.")),
    _process(new QProcess(this)),
    _wrapper(QCoreApplication::applicationDirPath() + "/wrapper")
{
    ui->setupUi(this);

    this->addToolBar(Qt::TopToolBarArea, _grogInfoBar);

    this->addDockWidget(Qt::LeftDockWidgetArea, _gromacsTools, Qt::Vertical);

    this->showMaximized();

    connect(_gromacsTools, SIGNAL(GromacsToolChanged(int)), _grogInfoBar, SLOT(ToolChanged(int)));
    _gromacsTools->InitGromacsTool();

    connect(_grogInfoBar, SIGNAL(StartClicked(QString,QString)), this, SLOT(ExecuteCurrentTool(QString,QString)));
    connect(_grogInfoBar, SIGNAL(StopClicked()), this, SLOT(TerminateCurrentTool()));
    connect(_grogInfoBar, SIGNAL(HelpClicked(QString, QString)), this, SLOT(CurrentToolGetHelp(QString, QString)));

    connect(ui->actionSaveCurrentToolOptions, SIGNAL(triggered()), _gromacsTools, SLOT(CurrentToolOptionsToXML()));
    connect(ui->actionSaveAllToolsOptions, SIGNAL(triggered()), _gromacsTools, SLOT(AllToolsOptionsToXML()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));

    connect(ui->actionSettings, SIGNAL(triggered()), this, SLOT(HandleSettings()));
    connect(ui->actionAboutGROG, SIGNAL(triggered()), this, SLOT(HandleAboutGROG()));
    connect(ui->actionAboutQt, SIGNAL(triggered()), this, SLOT(HandleAboutQt()));

    connect(_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(HandleError(QProcess::ProcessError)));
    connect(_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(HandleFinished(int, QProcess::ExitStatus)));
    connect(_process, SIGNAL(readyReadStandardError()), this, SLOT(HandleReadStderr()));
    connect(_process, SIGNAL(readyReadStandardOutput()), this, SLOT(HandleReadStdout()));
    connect(_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(HandleStateChanged(QProcess::ProcessState)));
    connect(ui->lineEdit, SIGNAL(returnPressed()), this, SLOT(HandleStdin()));

    connect(ui->textEdit, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(ShowTextEditContextMenu(const QPoint &)));
}

GrogMainWindow::~GrogMainWindow()
{
    _process->terminate();
    delete ui;
}

void GrogMainWindow::ExecuteCurrentTool(const QString &execName, const QString &workingDir)
{
    QStringList optionsList;

    if(!_grogInfoBar->WorkingDirExists())
    {
        ui->textEdit->append(tr("The working directory does not exist!"));

        return;
    }

    if(_toolsDefinition.IsNativeGromacsTool(_gromacsTools->CurrentToolIndex()))
    {
        optionsList << execName;
        if(_gromacsTools->CollectOptions(optionsList))
        {
            QString callLine = optionsList.join(" ");
            ui->textEdit->append("********************************************************************************");
            ui->textEdit->append(callLine);
            ui->textEdit->append("********************************************************************************\n");

            _process->setWorkingDirectory(workingDir);
            _process->start(_wrapper, optionsList);
        }
        else
        {
            ui->textEdit->append(tr("Error occured while collecting the options for the program!"));
        }
    }
    else
    {
        _gromacsTools->CollectOptions(optionsList);

        MdgFileGeneration(optionsList);
    }
}

void GrogMainWindow::TerminateCurrentTool()
{
    _process->terminate();
}

void GrogMainWindow::CurrentToolGetHelp(const QString &execName, const QString &workingDir)
{
    if(_toolsDefinition.IsNativeGromacsTool(_gromacsTools->CurrentToolIndex()))
    {
        QStringList optionsList;
        optionsList << execName;
        optionsList << "-h";

        _process->setWorkingDirectory(workingDir);
        _process->start(_wrapper, optionsList);
    }
    else
    {
        ui->textEdit->append(_mdgHelpMessage);
    }
}

void GrogMainWindow::HandleSettings()
{
    if(0 != _grogSettings->exec())
    {
        _gromacsTools->InitGromacsTool();
    }
}

void GrogMainWindow::HandleAboutGROG()
{
    QMessageBox::about(this, tr("About GROG"), tr("Version 0.2"));
}

void GrogMainWindow::HandleAboutQt()
{
    QMessageBox::aboutQt(this);
}

void GrogMainWindow::HandleError(QProcess::ProcessError error)
{
    const QString prefix(tr("Application Error: "));

    QColor defaultColour = ui->textEdit->textColor();
    ui->textEdit->setTextColor(QColor("red"));

    switch(error)
    {
    case QProcess::FailedToStart:
        ui->textEdit->append(prefix + "FailedToStart");
        break;
    case QProcess::Crashed:
        ui->textEdit->append(prefix + "Crashed");
        break;
    case QProcess::Timedout:
        ui->textEdit->append(prefix + "Timedout");
        break;
    case QProcess::WriteError:
        ui->textEdit->append(prefix + "WriteError");
        break;
    case QProcess::ReadError:
        ui->textEdit->append(prefix + "ReadError");
        break;
    case QProcess::UnknownError:
        ui->textEdit->append(prefix + "UnknownError");
        break;
    default:
        Q_ASSERT(false);
    }

    ui->textEdit->setTextColor(defaultColour);
}

void GrogMainWindow::HandleFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    QColor defaultColour = ui->textEdit->textColor();
    ui->textEdit->setTextColor(QColor("red"));

    if(QProcess::CrashExit == exitStatus)
    {
        ui->textEdit->append("The program crashed!");
    }
    else
    {
        if(0 != exitCode)
        {
            ui->textEdit->append("The program exited with non-zero code!");
        }
    }

    ui->textEdit->setTextColor(defaultColour);
}

void GrogMainWindow::HandleReadStderr()
{
    ui->textEdit->append(_process->readAllStandardError());
}

void GrogMainWindow::HandleReadStdout()
{
    ui->textEdit->append(_process->readAllStandardOutput());
}

void GrogMainWindow::HandleStateChanged(QProcess::ProcessState newState)
{
    switch(newState)
    {
    case QProcess::NotRunning:
        _grogInfoBar->EnableInterface(true);
        ui->lineEdit->setEnabled(false);
        ui->lineEdit->clear();
        break;
    case QProcess::Starting:
    case QProcess::Running:
        _grogInfoBar->EnableInterface(false);
        ui->lineEdit->setEnabled(true);
        ui->lineEdit->setFocus();
        break;
    default:
        Q_ASSERT(false);
    }
}

void GrogMainWindow::HandleStdin()
{
    if(QProcess::Running == _process->state())
    {
        // TODO: maybe something cleverer needs to be done here
        _process->write((ui->lineEdit->text() + "\n").toStdString().c_str());

        ui->lineEdit->clear();
    }
}

void GrogMainWindow::ShowTextEditContextMenu(const QPoint & pos)
{
    const QString actionName("Clear");

    QMenu *menu = ui->textEdit->createStandardContextMenu();
    menu->addAction(actionName);
    QAction *currentAction = menu->exec(ui->textEdit->mapToGlobal(pos));

    if(NULL != currentAction && actionName == currentAction->text())
    {
        ui->textEdit->clear();
    }

    delete menu;
}

void GrogMainWindow::MdgFileGeneration(const QStringList &options)
{
    QString mdgOutputText;

    for(int i=2; i<options.size(); i+=2) // start from s because 0 and 1 are related to the output file
    {
        mdgOutputText.append(options[i] + " = " + options[i+1] + "\n");
    }

    QFile mdgOutputFile(_grogInfoBar->CurrentWorkingDir() + "/" + options[1]);
    if(!mdgOutputFile.open(QFile::WriteOnly | QFile::Text))
    {
        ui->textEdit->append(tr("Cannot write the MDG output file!"));

        return;
    }

    if(-1 == mdgOutputFile.write(mdgOutputText.toUtf8()))
    {
        ui->textEdit->append(tr("Error while writing to the ouput MDG file!"));

        return;
    }

    mdgOutputFile.close();

    ui->textEdit->append(mdgOutputText);
}
