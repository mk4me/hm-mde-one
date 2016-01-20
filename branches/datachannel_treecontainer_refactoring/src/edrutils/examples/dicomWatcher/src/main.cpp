#include <fstream>
#include <QtCore/QFileSystemWatcher>
#include <QtCore/QCoreApplication>
#include <QtWidgets/QApplication>
#include <dicomLib/Dicom.h>
#include "DicomWatcher.h"
#include "WatcherConfig.h"
#include "QtCore/QCommandLineParser"
#include "QtCore/QStringList"
#include "loglib/ILog.h"
#include "loglib/Logger.h"

// sk�adnia :
// dicomWatcher [opcje] inputFolder outputFolder 
// opcje :
// -s --status-file  -�cie�ka do pliku z informacjami o przetworzonych plikach
// -v --verbose		 -komunikaty b�d� wyswietlane w konsoli
// -l --log-file	 -�cie�ka do pliku z logiem
int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	QCoreApplication::setApplicationName("Dicom Watcher");
	QCoreApplication::setApplicationVersion("0.1");

	QCommandLineParser parser;
	parser.setApplicationDescription("Dicom Watcher");
	parser.addHelpOption();
	parser.addVersionOption();
	parser.addPositionalArgument("WatchFolder", QCoreApplication::translate("main", "Directory to watch."));
	parser.addPositionalArgument("OutputFolder", QCoreApplication::translate("main", "Destination directory."));


	QCommandLineOption configOption(QStringList() << "s" << "status-file",
								   QCoreApplication::translate("main", "Status <file>."),
								   QCoreApplication::translate("main", "file"));
	parser.addOption(configOption);
	QCommandLineOption verboseOption(QStringList() << "p" << "print-on-console",
									 QCoreApplication::translate("main", "Prints informations on standard output"));
	parser.addOption(verboseOption);

	QCommandLineOption logOption(QStringList() << "l" << "log-file",
									QCoreApplication::translate("main", "Log <file>."),
									QCoreApplication::translate("main", "file"));
	parser.addOption(logOption);

	parser.process(app);


	utils::Filesystem::Path sourceDir("input");
	utils::Filesystem::Path destDir("output");

	const QStringList args = parser.positionalArguments();
	if (args.size() < 2) {
		parser.showHelp(1);
	} else {
		sourceDir = args.at(0).toStdString();
		destDir = args.at(1).toStdString();
	}


	utils::Filesystem::Path configFile("status.dw");
	if (parser.isSet(configOption)) {
		QString targetDir = parser.value(configOption);
		configFile = targetDir.toStdString();
	}

	std::vector<loglib::ILogPtr> loggers;
	if (parser.isSet(verboseOption)) {
		loggers.push_back(utils::make_shared<loglib::CoutLogger>());
	}

	utils::Filesystem::Path logFile("log.txt");
	if (parser.isSet(logOption)) {
		QString targetDir = parser.value(logOption);
		logFile = targetDir.toStdString();
	}
	if (utils::Filesystem::pathExists(logFile)) {
		utils::Filesystem::deleteFile(logFile);
	}
	loglib::FileLogger::setPath(logFile);
	loggers.push_back(utils::make_shared<loglib::FileLogger>());

	loglib::ILogPtr multiLog = utils::make_shared<loglib::MultiLogger>(std::move(loggers));
	loglib::Logger::setLog(multiLog);
	
	QFileSystemWatcher fileWatcher;
	bool test = fileWatcher.addPath(QString::fromStdString(sourceDir.string()));

	DicomWatcher dicomWatcher(destDir, configFile);
	UTILS_LOG_INFO("Dicom watcher started");
	test = QObject::connect(&fileWatcher, SIGNAL(directoryChanged(const QString &)), &dicomWatcher, SLOT(dirChanged(const QString &)));

	return app.exec();
}


