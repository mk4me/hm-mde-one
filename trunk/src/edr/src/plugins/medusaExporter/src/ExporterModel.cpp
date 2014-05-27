#include "MedusaExporterPCH.h"
#include "ExporterModel.h"
#include "IExporter.h"
#include <plugins/dicom/ILayeredImage.h>
#include <quazip/quazip.h>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <quazip/quazipnewinfo.h>
#include <quazip/quazipfile.h>
#include <plugins/newCommunication/ICommunicationDataSource.h>
#include <corelib/PluginCommon.h>
#include <corelib/IFileDataManager.h>
#include <corelib/ISourceManager.h>
#include "CSVExporter.h"


using namespace medusaExporter;
typedef core::Filesystem fs;

// ponizsze dwie funkcje pochodz� z QuaZip, szkoda, ze byly private

bool copyData(QIODevice &inFile, QIODevice &outFile)
{
	while (!inFile.atEnd()) {
		char buf[4096];
		qint64 readLen = inFile.read(buf, 4096);
		if (readLen <= 0)
			return false;
		if (outFile.write(buf, readLen) != readLen)
			return false;
	}
	return true;
}

/**OK
* Comprime il file fileName, nell'oggetto zip, con il nome fileDest.
*
* La funzione fallisce se:
* * zip==NULL;
* * l'oggetto zip e stato aperto in una modalita non compatibile con l'aggiunta di file;
* * non e possibile aprire il file d'origine;
* * non e possibile creare il file all'interno dell'oggetto zip;
* * si e rilevato un errore nella copia dei dati;
* * non e stato possibile chiudere il file all'interno dell'oggetto zip;
*/
bool compressFile(QuaZip* zip, QString fileName, QString fileDest) {
	// zip: oggetto dove aggiungere il file
	// fileName: nome del file reale
	// fileDest: nome del file all'interno del file compresso

	// Controllo l'apertura dello zip
	if (!zip) return false;
	if (zip->getMode() != QuaZip::mdCreate &&
		zip->getMode() != QuaZip::mdAppend &&
		zip->getMode() != QuaZip::mdAdd) return false;

	// Apro il file originale
	QFile inFile;
	inFile.setFileName(fileName);
	if (!inFile.open(QIODevice::ReadOnly)) return false;

	// Apro il file risulato
	QuaZipFile outFile(zip);
	if (!outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileDest, inFile.fileName()))) return false;

	// Copio i dati
	if (!copyData(inFile, outFile) || outFile.getZipError() != UNZ_OK) {
		return false;
	}

	// Chiudo i file
	outFile.close();
	if (outFile.getZipError() != UNZ_OK) return false;
	inFile.close();

	return true;
}



medusaExporter::ExporterModel::ExporterModel(core::IFileDataManager* fileManager, core::IMemoryDataManager* dataManager):
	dataManager(dataManager),
	fileManager(fileManager)
{

}


void medusaExporter::ExporterModel::addExporter(const IExporterConstPtr& exporter)
{
	exporters.push_back(exporter);
}

//medusaExporter::AnnotationDataConstPtr medusaExporter::ExporterModel::gatherData(const std::string& user)
//{
//	AnnotationDataPtr annotations = utils::make_shared<AnnotationData>();
//	core::ConstVariantsList oList;
//	auto transaction = dataManager->transaction();
//	//transaction->getObjects(oList, typeid(dicom::ILayeredImage), false);
//
//	transaction->getObjects(oList, typeid(dicom::LayersVector), false);
//
//	for (auto imgIt = oList.begin(); imgIt != oList.end(); ++imgIt) {
//		AnnotationData::LayerInfo li;
//		li.imageName = "TODO.png";
//		dicom::LayersVectorConstPtr lv = (*imgIt)->get();
//		for (auto it = lv->cbegin(); it != lv->cend(); ++it) {
//			annotations->addAnnotation(li, *it);
//		}
//	}
//
//	return annotations;
//}



void medusaExporter::ExporterModel::downloadAll()
{
	communication::ICommunicationDataSourcePtr icomm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
	if (icomm) {
		if (icomm->isLogged()) {
			icomm->downloadAllFiles();
		} else {
            throw std::runtime_error("User not logged");
		}
	}
}

void medusaExporter::ExporterModel::extractData(const QString& path)
{
	auto dummy = [&](float f, const QString& s) {};
	extractData(path, dummy);
}

void medusaExporter::ExporterModel::extractData(const QString& path, CallbackFunction fun)
{
	fun(0.0f, QObject::tr("Extracting data"));
	communication::ICommunicationDataSourcePtr icomm = core::querySource<communication::ICommunicationDataSource>(plugin::getSourceManager());
	if (icomm) {
		core::Filesystem::Path dir(path.toStdString());
		if (core::Filesystem::isDirectory(dir)) {
            if (icomm->isLogged()) {
                icomm->extractDataFromLocalStorage(dir);
                fun(1.0f, QObject::tr("Done extracting"));
            } else {
                throw std::runtime_error("User not logged");
            }
		} else {
			throw std::runtime_error("Extracting error: invalid directory");
		}
	} else {
		throw std::runtime_error("Extracting error: communication plugin not found");
	}
	
}

void medusaExporter::ExporterModel::pack(const QString& dirPath, const QString& outFile, boost::function<bool(const core::Filesystem::Path&)> filter, CallbackFunction fun)
{
	fs::Path dir(dirPath.toStdString());
	if (fs::isDirectory(dir)) {
		std::vector<fs::Path> files = fs::listFilteredFiles(dir, true, filter);
		int count = files.size();
		if (count == 0) {
			throw std::runtime_error("No files to pack");
		}
		QuaZip zip(outFile);
		zip.open(QuaZip::mdCreate);
		QDir qdir(QString::fromStdString(dir.string()));
		float ratio = 0.0f;
		float delta = 1.0f / files.size();
		for (auto it = files.begin(); it != files.end(); ++it) {
			QString fileName = QString::fromStdString(it->string());
			QString fileDesc = qdir.relativeFilePath(fileName);
			fun(ratio, QString("Packing: %1").arg(fileName));
			ratio += delta;
			bool res = compressFile(&zip, fileName, fileDesc);
			if (!res) {
				// TODO
			}
		}
		zip.close();
		fun(1.0f, "Done");
	} else {
		throw std::runtime_error("Problem with packing: wrong directory");
	}
}

void medusaExporter::ExporterModel::packMeta(const QString& dirPath, const QString& outFile)
{
	auto dummy = [&](float f, const QString& q) {};
	packMeta(dirPath, outFile, dummy);
}

void medusaExporter::ExporterModel::packMeta(const QString& dirPath, const QString& outFile, CallbackFunction fun)
{
	auto filter = [&](const fs::Path& path) -> bool {
		return path.extension() == ".xml";
	};
	pack(dirPath, outFile, filter, fun);
}



void medusaExporter::ExporterModel::packImages(const QString& dirPath, const QString& outFile)
{
	auto dummy = [&](float f, const QString& q) {};
	packImages(dirPath, outFile, dummy);
}

void medusaExporter::ExporterModel::packImages(const QString& dirPath, const QString& outFile, CallbackFunction fun)
{
	// lambda pobiera pliki, ktore maja rozszerzenie png, lub sa plikami zawierajace zbiorcze metadane
	auto filter = [&](const fs::Path& path) -> bool {
		if (path.extension() == ".png") {
			return true;
		}
		else if (path.extension() == ".xml") {
			std::string stem = path.stem().string();
			return stem.find('.') == std::string::npos;
		}
		return false;
	};

	pack(dirPath, outFile, filter, fun);
}

void medusaExporter::ExporterModel::exportData(const QString& outFile, const QString& user, const QString& dirPath, const ExportConfig& config)
{
	auto dummy = [&](float f, const QString& q) {};
	exportData(outFile, user, dirPath, config, dummy);
}

void medusaExporter::ExporterModel::exportData(const QString& outFile, const QString& user, const QString& dirPath, const ExportConfig& config, CallbackFunction fun)
{
	auto filter = [&](const fs::Path& path) -> bool {
		if (path.extension() == ".xml") {
			std::string stem = path.stem().string();
			return stem.find(user.toStdString()) != std::string::npos;
		}
		return false;
	};

	fun(0.0f, QObject::tr("Export: gethering files"));
	auto files = fs::listFilteredFiles(fs::Path(dirPath.toStdString()), true, filter);
	auto transaction = fileManager->transaction();

	fun(0.05f, QObject::tr("Gathering power dopplers"));
	auto dopplers = gatherPowerDopplers(dirPath, fun);

	float ratio = 0.15f;
	float delta = (1.0f / files.size()) / (10.0f / 8.0f);
	AnnotationDataPtr annotations = utils::make_shared<AnnotationData>();
	for (auto it = files.begin(); it != files.end(); ++it) {
		core::ConstVariantsList oList;
		transaction->addFile(*it);
		fun(ratio, QObject::tr("Getting annotations: %1").arg(QString::fromStdString(it->string())));
		transaction->getObjects(*it, oList);
		for (auto imgIt = oList.begin(); imgIt != oList.end(); ++imgIt) {
			AnnotationData::ImageInfo li;
			li.imageName = it->stem().string();
			li.isPowerDoppler = dopplers[li.imageName];
			try {
				dicom::LayersVectorConstPtr lv = (*imgIt)->get();
				for (auto it = lv->cbegin(); it != lv->cend(); ++it) {
					annotations->addAnnotation(li, *it);
				}
			}
			catch (...) {}
		}
		transaction->removeFile(*it);
	}


	CSVExporter exporter;
	exporter.exportAnnotations(core::Filesystem::Path(outFile.toStdString()), *annotations, config);
	fun(1.0f, QObject::tr("Done"));
	//transaction->getObjects()
}

std::map<std::string, bool> medusaExporter::ExporterModel::gatherPowerDopplers(const QString& dirPath, CallbackFunction fun)
{
	std::map<std::string, bool> dopplers;

	auto files = gatherInternalSessionFiles(dirPath);
	auto transaction = fileManager->transaction();
	float ratio = 0.05f;
	if (files.empty()) {
		throw std::runtime_error("No power doppler info");
	}
	float delta = (1.0f / files.size()) / 10.0f;
	for (auto it = files.begin(); it != files.end(); ++it) {
		core::ConstVariantsList oList;
		transaction->addFile(*it);
		fun(ratio, QObject::tr("Parsing dopplers: %1").arg(QString::fromStdString(it->string())));
		ratio += delta;
		transaction->getObjects(*it, oList);
		for (auto imgIt = oList.begin(); imgIt != oList.end(); ++imgIt) {
			try {
				dicom::IDicomInternalStructConstPtr inter = (*imgIt)->get();
				gatherPowerDopplers(inter, dopplers);
			}
			catch (...) {}
		}
		transaction->removeFile(*it);
	}
	return dopplers;
}

void medusaExporter::ExporterModel::gatherPowerDopplers(dicom::IDicomInternalStructConstPtr inter, std::map<std::string, bool>& dopplers)
{
	int patCount = inter->getNumPatients();
	for (int iPat = 0; iPat < patCount; ++iPat) {
		auto patient = inter->getPatient(iPat);
		for (auto itSession = patient->sessions.begin(); itSession != patient->sessions.end(); ++itSession) {
			for (auto itSerie = (*itSession)->series.begin(); itSerie != (*itSession)->series.end(); ++itSerie) {
				for (auto itImg = (*itSerie)->images.begin(); itImg != (*itSerie)->images.end(); ++itImg) {
					dopplers[(*itImg)->imageFile] = (*itImg)->isPowerDoppler;
				}
			}
		}
	}
}

QStringList medusaExporter::ExporterModel::getUsers(const QString& path) const
{
	auto filter = [&](const fs::Path& path) -> bool {
		if (path.extension() == ".xml") {
			std::string stem = path.stem().string();
			return stem.find(".") != std::string::npos;
		}
		return false;
	};

	std::set<std::string> users;
	auto files = fs::listFilteredFiles(fs::Path(path.toStdString()), true, filter);
	for (auto it = files.begin(); it != files.end(); ++it) {
		std::string user = it->stem().extension().string();
		if (!user.empty()) {
			users.insert(user.erase(0,1));
		}
	}
	QStringList usersList;
	for (auto it = users.begin(); it != users.end(); ++it) {
		usersList.push_back(QString::fromStdString(*it));
	}
	return usersList;
}

std::vector<core::Filesystem::Path> medusaExporter::ExporterModel::gatherInternalSessionFiles(const QString &dirPath)
{
	auto filter = [&](const fs::Path& path) -> bool {
		if (path.extension() == ".xml") {
			std::string stem = path.stem().string();
			return stem.find(".") == std::string::npos;
		}
		return false;
	};

	return fs::listFilteredFiles(fs::Path(dirPath.toStdString()), true, filter);
}
