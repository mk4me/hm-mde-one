#include "DicomWatcher.h"
#include <QtCore/QFileSystemWatcher>
#include <dicomLib/Dicom.h>
#include "dicomLib/DicomImporter.h"
#include "dicomLib/DicomInternalStruct.h"
#include <quazip/JlCompress.h>
#include "loglib/Logger.h"

DicomWatcher::DicomWatcher(const utils::Filesystem::Path& outputDir, const utils::Filesystem::Path& configPath) :
config(configPath),
outputDir(outputDir)
{
	//! wczytanie informacji o przetworzonych plikach
	config.loadOrCreate();
}

void import(const utils::Filesystem::Path& from, const utils::Filesystem::Path& to)
{
	dicomImporter::DicomImporter importer;
	importer.setCallBack([&](const std::string& s) {
		UTILS_LOG_INFO("processing: " << s);
	});

	// wyszukuje g�ownego pliku dicom, je�li takiego nie ma to rzucany jest wyj�tek
	auto rootDir = importer.findDicomRootDir(from);
	// wczytanie struktury, wyj�tek przy niepowodzeniu
	auto inter = importer.import(rootDir);
	// je�li wczytana struktura ma w sobie kilka sesji to nast�puje rozdzielenie na wiele struktur z jednosesyjnych
	std::vector<dicomImporter::DicomInternalStructPtr> splits = importer.split(inter);

	dicomImporter::DicomSaver saver;
	for (auto it = splits.begin(); it != splits.end(); ++it) {
		// konwersja i zapis obraz�w
		importer.convertImages(*it, rootDir, to);
		// zapis xml-a z metadanymi
		saver.save(to, *it);
	}
}


void DicomWatcher::dirChanged(const QString & path)
{
	try {
		// wszystkie pliki *.zip z foleru, bez przeszukiwania podfolder�w
		auto files = utils::Filesystem::listFiles(path.toStdString(), false, ".zip");
		for (auto& f : files) {
			// je�li plik nie by� wcze�niej przetwarzany...
			if (!config.wasProcessed(f)) {
				UTILS_LOG_INFO("extracting: " << f.string());
				utils::Filesystem::Path out1(outputDir / f.stem() / "raw");
				utils::Filesystem::Path out2(outputDir / f.stem() / "converted");
				//! wypakowanie archiwum, je�li nie ma folderu, to zostanie utworzony
				QStringList sl  = JlCompress::extractDir(QString::fromStdString(f.string()), QString::fromStdString(out1.string()));
				UTILS_LOG_INFO("processing: " << f.string());
				//! przetworzenie rozpakowanego dicoma (folder out1) na obrazy png, wynik trafia do folderu out2
				import(out1, out2);
				//! aktualizacja status�w
				config.addProcessed(f);
				config.save();
				UTILS_LOG_INFO("processing: " << f.string() << " done.");
			}
		}
	}
	catch (std::exception& e) {
		UTILS_LOG_ERROR(e.what());
	} 
	catch (...) {
		UTILS_LOG_ERROR("Unknown error");
	}
}




