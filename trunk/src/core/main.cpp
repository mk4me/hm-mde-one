#include "CorePCH.h"
#include <QtGui/QApplication>
#include "ToolboxMain.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    ToolboxMain w;
    w.show();
    return a.exec();
}
