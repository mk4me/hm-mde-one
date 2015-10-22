#include "DicomWatcher.h"
#include <dicomLib/Dicom.h>
#include <QtCore/QFileSystemWatcher>
#include "QtCore/QCoreApplication"
#include "QtWidgets/QApplication"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	QFileSystemWatcher fileWatcher;
	bool test = fileWatcher.addPath(QString("E:/Series"));
	DicomWatcher dicomWatcher;
	std::cout << "Welcome" << std::endl;
	test = QObject::connect(&fileWatcher, SIGNAL(directoryChanged(const QString &)), &dicomWatcher, SLOT(dirChanged(const QString &)));

	return app.exec();
}




void DicomWatcher::dirChanged(const QString & path)
{
	std::cout << "directory changed: " << path.toStdString() << "\n";
}

void DicomWatcher::fileChanged(const QString & path)
{
	std::cout << "file changed: " << path.toStdString() << "\n";
}
