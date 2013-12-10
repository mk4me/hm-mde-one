/********************************************************************
	created:	2013/11/25
	created:	25:11:2013   8:52
	filename: 	Adnotations.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__ADNOTATIONS_H__
#define HEADER_GUARD_DICOM__ADNOTATIONS_H__

#include <string>
#include <boost/bimap.hpp>
#include "qstring_serialization.h"
#include <corelib/Filesystem.h>

namespace dicom {

namespace adnotations {
    typedef boost::bimap<int, QString> AdnotationsType;
    DEFINE_SMART_POINTERS(AdnotationsType);

    AdnotationsTypePtr getDefault();
    void save(const core::Filesystem::Path& p, AdnotationsTypeConstPtr adnotations);
    AdnotationsTypePtr load(const core::Filesystem::Path& p);

    AdnotationsTypePtr instance();
    void autoSave();

    core::Filesystem::Path adnotationsFile();
}
}

#endif
