/********************************************************************
	created:	2014/08/07
	created:	19:56
	filename: 	IMUPerspective.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_IMU__IMUPerspective_H__
#define HEADER_GUARD_IMU__IMUPerspective_H__

#include <plugins/newCommunication/IHierarchyPerspective.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/Visualizer.h>
#include <corelib/HierarchyDataItem.h>
#include "corelib/IService.h"
#include "plugins/c3d/C3DCollections.h"
#include "IMUCFGParser.h"

namespace IMU {

////! Klasa pomocnicza przy tworzeniu wizualizatorów
//class DicomHelper : public core::WrappedItemHelper
//{
//public:
//    DicomHelper(const core::VariantConstPtr & wrapper, const core::ConstVariantsList& layers,
//        const std::string& xmlFilename, const std::string& trialName);
//    virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series );
//
//private:
//    core::ConstVariantsList layers;
//    std::string xmlFilename; 
//    std::string trialName;
//};
//DEFINE_SMART_POINTERS(DicomHelper);

//! Klasa realizująca perspektywe danych
class IMUPerspective : public communication::IHierarchyPerspective
{
public:
	IMUPerspective(core::IMemoryDataManager * memoryDataManager);
	virtual ~IMUPerspective() {}

    virtual core::IHierarchyItemPtr getPerspective( PluginSubject::SubjectPtr subject );

	static void createIMUBranch(core::ConstVariantsList &framesV, IMU::IMUConfigConstPtr config, const std::string& sourceName, core::HierarchyItemPtr sessionItem, core::IMemoryDataManager * memoryDataManager = nullptr);

	static void createIMUBranch(core::ConstVariantsList &oList, core::HierarchyItemPtr root);

	static core::IHierarchyItemPtr createImuCollectionItem(int i, VectorChannelCollectionConstPtr collection);

    //core::HierarchyDataItemPtr tryGetHierarchyItem(const std::string& filename);

private:
    std::map<std::string, core::HierarchyDataItemWeakPtr> name2hierarchy;
	core::IMemoryDataManager * memoryDataManager;
};
DEFINE_SMART_POINTERS(IMUPerspective);


class IMUPerspectiveService : public plugin::IService 
{
	UNIQUE_ID("705AB109-B5CD-46C5-BE9E-181813D5BE89")
	CLASS_DESCRIPTION("IMUPerspectiveService", "IMUPerspectiveService");
public:
	virtual ~IMUPerspectiveService() { }

public:
	virtual const bool lateInit();

	virtual void init(core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager);

	virtual void finalize();

	virtual void update(double deltaTime);

	virtual QWidget* getWidget();

	virtual QWidgetList getPropertiesWidgets();
private:
	core::ISourceManager * sourceManager;
	core::IMemoryDataManager * memoryDataManager;
};

}
#endif
