#include "SampleWindow.h"
#include "QtGui/QMessageBox"


SampleWindow::SampleWindow()
{
    setupUi(this);
}

void SampleWindow::showHelloWorld()
{
    QMessageBox::information( this, "Sample", "Hello world!" );
}
