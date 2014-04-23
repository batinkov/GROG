#ifndef GROMACSOPTIONSGROUP_H
#define GROMACSOPTIONSGROUP_H

#include <QWidget>

namespace Ui {
class GromacsOptionsGroup;
}

class QCheckBox;

class GromacsOptionsGroup : public QWidget
{
    Q_OBJECT
    
public:
    explicit GromacsOptionsGroup(const QString &name, QWidget *parent = 0);
    ~GromacsOptionsGroup();

    void mousePressEvent(QMouseEvent *event);

    void AddFile(bool mandatory, const QString &name, bool inputFile, const QString &value = "", bool enabled = true, const QString &tip = "");
    void AddOption(bool mandatory, const QString &name, const QString &value = "", bool enabled = true, const QString &tip = "");
    void AddOption(bool mandatory, const QString &name, const QList<QString> &values, const QString &value, bool enabled = true, const QString &tip = "");

    void ShowOptions();
    void HideOptions();
    bool OptionsVisible();

    bool CollectOptions(QStringList & listOfOptions);

private slots:
    void ToggleOptionsVisibility();
    void ToggleEnabledState();

private:
    int PrepareForNewOption(const QString &name, const QString &tip, QCheckBox ** createdCheckBox = NULL);
    void SetCorrectState(QCheckBox *checkBox, bool mandatory, bool enabled);

    void SetOptionNameMaxWidth();

private:
    Ui::GromacsOptionsGroup *ui;

    int _optionNameMaxWidth;
};

#endif // GROMACSOPTIONSGROUP_H
