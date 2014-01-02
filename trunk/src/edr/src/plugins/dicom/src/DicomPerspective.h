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

namespace dicom {

class DicomPerspective : public communication::IHierarchyPerspective
{
public:
	virtual ~DicomPerspective() {}

    virtual core::IHierarchyItemPtr getPerspective( PluginSubject::SubjectPtr subject );

};
DEFINE_SMART_POINTERS(DicomPerspective);

}
#endif
