/********************************************************************
	created:	2014/04/30
	created:	30:4:2014   9:04
	filename: 	ExporterModel.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_MEDUSA_EXPORTER__EXPORTERMODEL_H__
#define HEADER_GUARD_MEDUSA_EXPORTER__EXPORTERMODEL_H__


#include <corelib/IMemoryDataManager.h>
#include <boost/function.hpp>
#include "IExporter.h"
#include "AnnotationData.h"
#include "plugins/dicom/IDicomInternalStruct.h"

namespace medusaExporter {

	//! Serce exportera danych MEDUSA
	class ExporterModel
	{
	public:
		typedef boost::function<void(float, const QString&)> CallbackFunction;
	public:
		ExporterModel(core::IFileDataManager* fileManager, core::IMemoryDataManager* dataManager);
		virtual ~ExporterModel() {}


	public:
		void addExporter(const IExporterConstPtr& exporter);
		//AnnotationDataConstPtr gatherData(const std::string& user);
		void packImages(const QString& dirPath, const QString& outFile);
		void packImages(const QString& dirPath, const QString& outFile, CallbackFunction fun);
		void downloadAll();
		void extractData(const QString& path);
		void extractData(const QString& path, CallbackFunction fun);
		void packMeta(const QString& dirPath, const QString& outFile);
		void packMeta(const QString& dirPath, const QString& outFile, CallbackFunction fun);
		void exportData(const QString& outFile, const QString& user, const QString& dirPath, const ExportConfig& config);
		void exportData(const QString& outFile, const QString& user, const QString& dirPath, const ExportConfig& config, CallbackFunction fun);
		QStringList getUsers(const QString& path) const;

	private:
		void pack(const QString& dirPath, const QString& outFile, boost::function<bool(const core::Filesystem::Path&)> filter, CallbackFunction fun);
		std::map<std::string, bool> gatherPowerDopplers(const QString& dirPath, CallbackFunction fun);
		void gatherPowerDopplers(dicom::IDicomInternalStructConstPtr inter, std::map<std::string, bool>& dopplers);
		std::vector<core::Filesystem::Path> gatherInternalSessionFiles(const QString &dirPath);

	private:
		std::vector<IExporterConstPtr> exporters;
		core::IMemoryDataManager* dataManager;
		core::IFileDataManager* fileManager;
	};
	DEFINE_SMART_POINTERS(ExporterModel);
}

#endif
