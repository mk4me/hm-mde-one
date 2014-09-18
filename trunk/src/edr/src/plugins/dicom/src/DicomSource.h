/********************************************************************
	created:	2013/10/06
	created:	6:10:2013   14:34
	filename: 	DicomSource.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOMSOURCE_H__
#define HEADER_GUARD_DICOM__DICOMSOURCE_H__


#include <corelib/ISource.h>
#include <corelib/Filesystem.h>
#include <corelib/IHierarchyItem.h>
#include <corelib/HierarchyHelper.h>
#include "DicomInternalStruct.h"
#include "LayeredImage.h"

class DcmDirectoryRecord;

namespace hmdbCommunication
{
	class IHMDBSession;
}

namespace dicom {

//! Kalsa realizujπca odczyt danych USG
class DicomLoader
{
public:
	//! \param from åcieøka z ktÛrej prÛbujemy czytaÊ wewnÍtrznπ reprezentacjÍ danych USG
	//! \return Drzewo danych USG
    static DicomInternalStructPtr load( const core::Filesystem::Path& from);
	//! \param p åcieøka z kÛrej prÛbujemy czytaÊ warstwy adnotacji opusujπce obraz USG
	//! \return Warstwy adnotacji opusujπce obraz USG
    static LayersVectorPtr loadLayers(const core::Filesystem::Path &p );
};
DEFINE_SMART_POINTERS(DicomLoader);

//! Kalsa pomocnicza przy tworzeniu wizualizatora
class LayerHelper : public core::HierarchyHelper
{
public:
    LayerHelper(const core::VariantConstPtr& imgWrapper);
    virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series );
    virtual core::VisualizerPtr createVisualizer( core::IVisualizerManager* manager );
    virtual std::vector<utils::TypeInfo> getTypeInfos() const;

protected:
    LayerHelper() {}

private:
    core::VariantConstPtr imageWrapper;

};
DEFINE_SMART_POINTERS(LayerHelper);

//! èrÛd≥o danych DICOM
class DicomSource : public plugin::ISource
{
    UNIQUE_ID("{CB848CFE-5A1F-4D99-B06A-0249EA38B706}")
    CLASS_DESCRIPTION("Dicom Source", "Dicom Source")
public:
    DicomSource();
	virtual ~DicomSource() {}

public:
    virtual void init( core::IMemoryDataManager * memoryDM, core::IStreamDataManager * streamDM, core::IFileDataManager * fileDM );
    virtual bool lateInit();
    virtual void finalize();
    virtual void update( double deltaTime );
    virtual QWidget* getWidget();
    virtual QWidget* getControlWidget();
    virtual QWidget* getSettingsWidget();
    virtual void getOfferedTypes( utils::TypeInfoList & offeredTypes ) const;
    void addFile( const core::Filesystem::Path& path );
    void openInternalDataMainFile( core::Filesystem::Path path );

	static const bool userIsReviewer(hmdbCommunication::IHMDBSession * session);

private:
    void handlePatientRecord( DcmDirectoryRecord * patientRecord, core::IHierarchyItemPtr root, std::string basePath );
    void handleStudyRecord( DcmDirectoryRecord * studyRecord, core::IHierarchyItemPtr root, std::string basePath );
    void handleSeriesRecord( DcmDirectoryRecord * seriesRecord, core::IHierarchyItemPtr root, std::string basePath );
    void handleFileRecord( DcmDirectoryRecord * fileRecord, core::IHierarchyItemPtr root, std::string basePath, std::vector<LayerHelperPtr>& helpers );
    template <class Helper>
    core::IHierarchyItemPtr transactionPart( const core::Filesystem::Path &path, const QString& desc = QString(),
		const bool hasPowerDoppler = false);

public:
    static QString createDesc(const internalData::Patient& patient);
    static QString createDesc(const internalData::Study& study); 
    static QString createDesc(const internalData::Serie& serie); 
    static QString createDesc(const internalData::Image& image);
private:
    core::IMemoryDataManager* memoryDM;
    core::IFileDataManager* fileDM;
    core::IStreamDataManager* streamDM;
};
DEFINE_SMART_POINTERS(DicomSource);

}
#endif
