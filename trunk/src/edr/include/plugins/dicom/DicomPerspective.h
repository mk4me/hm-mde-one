/********************************************************************
	created:	2013/12/19
	created:	19:12:2013   19:15
	filename: 	DicomPerspective.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOMPERSPECTIVE_H__
#define HEADER_GUARD_DICOM__DICOMPERSPECTIVE_H__

#include <plugins/hmdbCommunication/IHierarchyPerspective.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/Visualizer.h>
#include <corelib/HierarchyDataItem.h>
#include "plugins/dicom/Export.h"

namespace dicom {

//! Klasa pomocnicza przy tworzeniu wizualizatorów
class PLUGIN_DICOM_EXPORT DicomHelper : public core::WrappedItemHelper
{
public:
    DicomHelper(const core::VariantConstPtr & wrapper, const core::ConstVariantsList& layers,
        const std::string& xmlFilename, const std::string& trialName);
    virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series );

private:
    core::ConstVariantsList layers;
    std::string xmlFilename; 
    std::string trialName;
};
DEFINE_SMART_POINTERS(DicomHelper);

//! Klasa pomocnicza przy tworzeniu wizualizatorów
class PLUGIN_DICOM_EXPORT DicomMultiHelper : public core::HierarchyHelper
{
public:
	DicomMultiHelper(const std::vector<DicomHelperPtr>& helpers);

protected:
	virtual void createSeries(const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series);
public:
	virtual core::VisualizerPtr createVisualizer(core::IVisualizerManager* manager);
	virtual std::vector<utils::TypeInfo> getTypeInfos() const;

private:
	std::vector<DicomHelperPtr> helpers;
};
DEFINE_SMART_POINTERS(DicomMultiHelper);

//! Klasa realizuj¹ca perspektywe danych
class PLUGIN_DICOM_EXPORT DicomPerspective : public hmdbCommunication::IHierarchyPerspective
{
public:
	virtual ~DicomPerspective() {}

    virtual core::IHierarchyItemPtr getPerspective( PluginSubject::SubjectPtr subject );

    core::HierarchyDataItemPtr tryGetHierarchyItem(const std::string& filename) const;

private:
    std::map<std::string, core::HierarchyDataItemWeakPtr> name2hierarchy;
};
DEFINE_SMART_POINTERS(DicomPerspective);

}
#endif
