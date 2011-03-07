#include <QtGui/QWidget>
#include <QtGui/QApplication>

int main(int argc, char *argv[])
{
	QApplication application(argc, argv);
	QWidget window;
	window.show();
	return application.exec();
}