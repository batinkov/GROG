#include "grogmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GrogMainWindow w;
    w.show();
    
    return a.exec();
}
