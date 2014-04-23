#include "grogsettings.h"
#include "ui_grogsettings.h"
#include "gromacstoolsdefinition.h"
#include <QLabel>
#include <QLineEdit>
#include <QCloseEvent>

GrogSettings::GrogSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GrogSettings),
    _toolsDefinition(GromacsToolsDefinition::GetInstance()),
    _toolColumn(0),
    _executableColumn(1),
    _workingDirColumn(2),
    _gridOffset(1)
{
    ui->setupUi(this);

    QLabel *toolLabel = new QLabel(tr("Tool: "), this);
    ui->gridLayout->addWidget(toolLabel, 0, _toolColumn);

    QLabel *executableLable = new QLabel(tr("Executable: "), this);
    ui->gridLayout->addWidget(executableLable, 0, _executableColumn);

    QLabel *workingDirLabel = new QLabel(tr("Working Dir: "), this);
    ui->gridLayout->addWidget(workingDirLabel, 0, _workingDirColumn);

    for(int i=0; i<_toolsDefinition.NumberOfTools(); ++i)
    {
        QLabel *tool = new QLabel(_toolsDefinition.ToolName(i), this);
        ui->gridLayout->addWidget(tool, i+_gridOffset, _toolColumn);

        QLineEdit *execName = new QLineEdit(this);
        ui->gridLayout->addWidget(execName, i+_gridOffset, _executableColumn);
        if(!_toolsDefinition.IsNativeGromacsTool(i))
        {
            execName->setEnabled(false);
        }

        QLineEdit *workingDir = new QLineEdit(this);
        ui->gridLayout->addWidget(workingDir, i+_gridOffset, _workingDirColumn);
    }

    connect(this, SIGNAL(accepted()), this, SLOT(UpdateToolsDefinition()));
    connect(ui->pushButtonReset, SIGNAL(clicked()), this, SLOT(ResetButtonHandler()));
}

GrogSettings::~GrogSettings()
{
    delete ui;
}

void GrogSettings::showEvent(QShowEvent * event)
{
    event->accept();

    UpdateInterface();
}

void GrogSettings::UpdateInterface()
{
    ui->lineEdit->setText(_toolsDefinition.ProjectsDefaultDirectory());

    for(int i=0; i<_toolsDefinition.NumberOfTools(); ++i)
    {
        QLineEdit *execName = static_cast<QLineEdit *>(ui->gridLayout->itemAtPosition(i+_gridOffset, _executableColumn)->widget());
        execName->setText(_toolsDefinition.ExecutableName(i));

        QLineEdit *workingDir = static_cast<QLineEdit *>(ui->gridLayout->itemAtPosition(i+_gridOffset, _workingDirColumn)->widget());
        workingDir->setText(_toolsDefinition.WorkingDir(i));
    }
}

void GrogSettings::UpdateToolsDefinition()
{
    _toolsDefinition.SetProjectsDefaultDirectory(ui->lineEdit->text());

    for(int i=0; i<_toolsDefinition.NumberOfTools(); ++i)
    {
        QLineEdit *execName =  static_cast<QLineEdit *>(ui->gridLayout->itemAtPosition(i+_gridOffset, _executableColumn)->widget());
        _toolsDefinition.SetExecutableName(i, execName->text());

        QLineEdit *workingDirName = static_cast<QLineEdit *>(ui->gridLayout->itemAtPosition(i+_gridOffset, _workingDirColumn)->widget());
        _toolsDefinition.SetWorkingDir(i, workingDirName->text());
    }

    _toolsDefinition.ToXML();
}

void GrogSettings::ResetButtonHandler()
{
    _toolsDefinition.ResetToDefaults();
    UpdateInterface();
}
