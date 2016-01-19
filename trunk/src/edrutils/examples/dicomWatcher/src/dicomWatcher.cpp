#include "DicomWatcher.h"
#include <QtCore/QFileSystemWatcher>
#include <dicomLib/Dicom.h>
#include "dicomLib/DicomImporter.h"
#include "dicomLib/DicomInternalStruct.h"
#include <quazip/JlCompress.h>
#include "loglib/Logger.h"
#include <QtWidgets/QApplication>

DicomWatcher::DicomWatcher(const utils::Filesystem::Path& outputDir, const utils::Filesystem::Path& configPath, bool singleOutputFolder, bool runOnce) :
config(configPath),
outputDir(outputDir),
singleOutputFolder(singleOutputFolder),
runOnce(runOnce)
{
	//! wczytanie informacji o przetworzonych plikach
	config.loadOrCreate();
}

void DicomWatcher::import(const utils::Filesystem::Path& from, const utils::Filesystem::Path& to)
{
	dicomImporter::DicomImporter importer;
	importer.setCallBack([&](const std::string& s) {
		UTILS_LOG_INFO("processing: " << s);
	});

	// wyszukuje g³ownego pliku dicom, jeœli takiego nie ma to rzucany jest wyj¹tek
	auto rootDir = importer.findDicomRootDir(from);
	// wczytanie struktury, wyj¹tek przy niepowodzeniu
	auto inter = importer.import(rootDir);
	// jeœli wczytana struktura ma w sobie kilka sesji to nastêpuje rozdzielenie na wiele struktur z jednosesyjnych
	std::vector<dicomImporter::DicomInternalStructPtr> splits = importer.split(inter);

	dicomImporter::DicomSaver saver;
	for (auto it = splits.begin(); it != splits.end(); ++it) {
		// konwersja i zapis obrazów
		importer.convertImages(*it, rootDir, to);
		// zapis xml-a z metadanymi
		saver.save(to, *it, !singleOutputFolder);
	}
}


void DicomWatcher::dirChanged(const QString & path)
{
	try {
		// ta petla wynika z tego, ze metoda wyzwalana jest gdy skopiowany jest pierwszy plik
		// gdy ustawiona jest flaga run_once, to powinny zostac przetworzone wszystkie pliki
		// zostalo zrobione naiwne zalozenie, ze rozpakowywanie i konwertowanie dicoma bedzie trwalo dluzej niz jego kopiowanie
		bool processed = true;
		while (runOnce && processed) {
			processed = false;
			// wszystkie pliki *.zip z foleru, bez przeszukiwania podfolderów
			auto files = utils::Filesystem::listFiles(path.toStdString(), false, ".zip");
			for (auto& f : files) {
				// jeœli plik nie by³ wczeœniej przetwarzany...
				if (!config.wasProcessed(f)) {
					processed = true;
					UTILS_LOG_INFO("extracting: " << f.string());
					utils::Filesystem::Path out1(outputDir / f.stem() / "raw");
					utils::Filesystem::Path out2 = singleOutputFolder ? (outputDir / "converted") : (outputDir / f.stem() / "converted");
					//! wypakowanie archiwum, jeœli nie ma folderu, to zostanie utworzony
					QStringList sl = JlCompress::extractDir(QString::fromStdString(f.string()), QString::fromStdString(out1.string()));
					UTILS_LOG_INFO("processing: " << f.string());
					//! przetworzenie rozpakowanego dicoma (folder out1) na obrazy png, wynik trafia do folderu out2
					import(out1, out2 );
					//! aktualizacja statusów
					config.addProcessed(f);
					config.save();
					UTILS_LOG_INFO("processing: " << f.string() << " done.");
				}
			}
		}

		exitIfRequested();
	}
	catch (std::exception& e) {
		UTILS_LOG_ERROR(e.what());
		exitIfRequested(1);
	} 
	catch (...) {
		UTILS_LOG_ERROR("Unknown error");
		exitIfRequested(1);
	}
}

void DicomWatcher::exitIfRequested(int errorCode)
{
	if (runOnce) {
		QApplication::exit(errorCode);
	}
}



