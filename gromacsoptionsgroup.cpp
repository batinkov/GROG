#include "gromacsoptionsgroup.h"
#include "ui_gromacsoptionsgroup.h"
#include <QMouseEvent>
#include <QCheckBox>
#include <QLabel>
#include <QFontMetrics>
#include "gromacsoption.h"
#include "gromacsoptionchoose.h"
#include "gromacsoptionenter.h"
#include "gromacsoptionfile.h"

enum ColumnPositions { PosCheckBox = 1, PosLabel, PosGromacsOption }; // TODO: define this as a member of GromacsOptionsGroup
const int RowBegin = 1; // TODO: define this as a member of GromacsOptionsGroup

GromacsOptionsGroup::GromacsOptionsGroup(const QString &name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GromacsOptionsGroup),
    _optionNameMaxWidth(-1)
{
    ui->setupUi(this);

    ui->toolButton->setText(name);

    connect(ui->toolButton, SIGNAL(clicked()), this, SLOT(ToggleOptionsVisibility()));
}

GromacsOptionsGroup::~GromacsOptionsGroup()
{
    delete ui;
}

void GromacsOptionsGroup::mousePressEvent(QMouseEvent *event)
{
    const QPoint eventGlobalPosition = event->globalPos();
    const QRect frameGlobalRect(mapToGlobal(ui->frame->pos()), QSize(ui->frame->width(), ui->frame->height()));
    const QRect buttonGlobalRect(mapToGlobal(ui->toolButton->pos()), QSize(ui->toolButton->width(), ui->toolButton->height()));

    if(!frameGlobalRect.contains(eventGlobalPosition) && !buttonGlobalRect.contains(eventGlobalPosition))
    {
        event->ignore();
    }
}

void GromacsOptionsGroup::AddFile(bool mandatory, const QString &name, bool inputFile, const QString &value, bool enabled, const QString &tip)
{
    QCheckBox *checkBox = NULL;
    const int currentRow = PrepareForNewOption(name, tip, &checkBox);

    GromacsOption *option;
    option = new GromacsOptionFile(inputFile, this, value);

    option->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->gridLayout->addWidget(option, currentRow, PosGromacsOption);

    SetCorrectState(checkBox, mandatory, enabled);
}

void GromacsOptionsGroup::AddOption(bool mandatory, const QString &name, const QString &value, bool enabled, const QString &tip)
{
    QCheckBox *checkBox = NULL;
    const int currentRow = PrepareForNewOption(name, tip, &checkBox);

    GromacsOption *option;
    option = new GromacsOptionEnter(this, value);

    option->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->gridLayout->addWidget(option, currentRow, PosGromacsOption);

    SetCorrectState(checkBox, mandatory, enabled);
}

void GromacsOptionsGroup::AddOption(bool mandatory, const QString &name, const QList<QString> &values, const QString &value, bool enabled, const QString &tip)
{
    QCheckBox *checkBox = NULL;
    const int currentRow = PrepareForNewOption(name, tip, &checkBox);

    GromacsOption *option = new GromacsOptionChoose(values, value, this);
    option->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    ui->gridLayout->addWidget(option, currentRow, PosGromacsOption);

    SetCorrectState(checkBox, mandatory, enabled);
}

void GromacsOptionsGroup::ShowOptions()
{
    ui->toolButton->setArrowType(Qt::DownArrow);
    ui->frame->show();
}

void GromacsOptionsGroup::HideOptions()
{
    ui->toolButton->setArrowType(Qt::RightArrow);
    ui->frame->hide();
}

bool GromacsOptionsGroup::OptionsVisible()
{
    return ui->frame->isVisible();
}

bool GromacsOptionsGroup::CollectOptions(QStringList & listOfOptions)
{
    for(int row=RowBegin; row<ui->gridLayout->rowCount(); ++row)
    {
        QCheckBox *currentCheckBox = static_cast<QCheckBox *>(ui->gridLayout->itemAtPosition(row, PosCheckBox)->widget());
        if(!currentCheckBox->isChecked())
        {
            continue;
        }

        QLabel *currentLabel = static_cast<QLabel *>(ui->gridLayout->itemAtPosition(row, PosLabel)->widget());
        if(!currentLabel->text().contains("/")) // TODO: think of some better logic
        {
            listOfOptions.push_back(currentLabel->text());
        }

        GromacsOption *currentOption = static_cast<GromacsOption *>(ui->gridLayout->itemAtPosition(row, PosGromacsOption)->widget());
        if(!currentOption->CollectOption(listOfOptions))
        {
            return false;
        }
    }

    return true;
}

void GromacsOptionsGroup::ToggleOptionsVisibility()
{
    (Qt::DownArrow == ui->toolButton->arrowType()) ? HideOptions() : ShowOptions();
}

void GromacsOptionsGroup::ToggleEnabledState()
{
    QCheckBox *checkBox = static_cast<QCheckBox *>(sender());
    const int checkBoxindex = ui->gridLayout->indexOf(checkBox);

    int row, unused; // only row will be used
    ui->gridLayout->getItemPosition(checkBoxindex, &row, &unused, &unused, &unused);

    for(int i=PosLabel; i<ui->gridLayout->columnCount(); ++i)
    {
        QWidget *currentWidget = ui->gridLayout->itemAtPosition(row, i)->widget();
        currentWidget->setEnabled(checkBox->isChecked());

        GromacsOptionFile *gromacsOptionFile = dynamic_cast<GromacsOptionFile *>(currentWidget);
        if(NULL != gromacsOptionFile)
        {
            gromacsOptionFile->HandleError();
        }
    }
}

int GromacsOptionsGroup::PrepareForNewOption(const QString &name, const QString &tip, QCheckBox ** createdCheckBox)
{
    const int currentRow = ui->gridLayout->rowCount();

    QCheckBox *checkBox = new QCheckBox(this);
    checkBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    checkBox->setChecked(true);
    connect(checkBox, SIGNAL(toggled(bool)), this, SLOT(ToggleEnabledState()));
    ui->gridLayout->addWidget(checkBox, currentRow, PosCheckBox);

    if(NULL != createdCheckBox)
    {
        *createdCheckBox = checkBox;
    }

    QLabel *label = new QLabel(name, this);
    label->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    label->setAlignment(Qt::AlignRight|Qt::AlignVCenter);
    label->setToolTip(tip);
    label->setWordWrap(true);
    ui->gridLayout->addWidget(label, currentRow, PosLabel);

    QFontMetrics fontMetrics(label->font());
    const int maxWidth = fontMetrics.width(name) + 1; // if you don't put this 1 the size doesn't always work correctly
    if(maxWidth > _optionNameMaxWidth)
    {
        _optionNameMaxWidth = maxWidth;
        SetOptionNameMaxWidth();
    }

    return currentRow;
}

void GromacsOptionsGroup::SetCorrectState(QCheckBox *checkBox, bool mandatory, bool enabled)
{
    if(mandatory)
    {
        checkBox->setEnabled(false);
    }
    else
    {
        checkBox->setChecked(enabled);
    }
}

void GromacsOptionsGroup::SetOptionNameMaxWidth()
{
    for(int row=RowBegin; row<ui->gridLayout->rowCount(); ++row)
    {
        QWidget *w = ui->gridLayout->itemAtPosition(row, PosLabel)->widget();
        w->setMaximumWidth(_optionNameMaxWidth);
    }
}
