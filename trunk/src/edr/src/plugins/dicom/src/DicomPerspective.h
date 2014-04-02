/********************************************************************
	created:	2013/12/19
	created:	19:12:2013   19:15
	filename: 	DicomPerspective.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DICOMPERSPECTIVE_H__
#define HEADER_GUARD_DICOM__DICOMPERSPECTIVE_H__

#include <plugins/newCommunication/IHierarchyPerspective.h>
#include <corelib/HierarchyHelper.h>
#include <corelib/Visualizer.h>

namespace dicom {

//! Klasa pomocnicza przy tworzeniu wizualizatorów
class DicomHelper : public core::WrappedItemHelper
{
public:
    DicomHelper(const core::ObjectWrapperConstPtr & wrapper, const core::ConstObjectsList& layers,
        const std::string& xmlFilename, const std::string& trialName);
    virtual void createSeries( const core::VisualizerPtr & visualizer, const QString& path, std::vector<core::Visualizer::VisualizerSerie*>& series );

private:
    core::ConstObjectsList layers;
    std::string xmlFilename; 
    std::string trialName;
};
DEFINE_SMART_POINTERS(DicomHelper);

//! Klasa realizująca perspektywe danych
class DicomPerspective : public communication::IHierarchyPerspective
{
public:
	virtual ~DicomPerspective() {}

    virtual core::IHierarchyItemPtr getPerspective( PluginSubject::SubjectPtr subject );
    
};
DEFINE_SMART_POINTERS(DicomPerspective);

}
#endif
