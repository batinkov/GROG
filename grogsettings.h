#ifndef GROGSETTINGS_H
#define GROGSETTINGS_H

#include <QDialog>

namespace Ui {
class GrogSettings;
}

class GromacsToolsDefinition;

class GrogSettings : public QDialog
{
    Q_OBJECT

public:
    explicit GrogSettings(QWidget *parent = 0);
    ~GrogSettings();
    
protected:
    void showEvent(QShowEvent * event);

private slots:
    void UpdateInterface();
    void UpdateToolsDefinition();
    void ResetButtonHandler();

private:
    Ui::GrogSettings *ui;

    GromacsToolsDefinition &_toolsDefinition;

    const int _toolColumn;
    const int _executableColumn;
    const int _workingDirColumn;

    const int _gridOffset;
};

#endif // GROGSETTINGS_H
