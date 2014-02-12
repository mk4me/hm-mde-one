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

    enum annotationsIdx {
        unknownAnnotation = -1,
        otherAnnotation = 0, 
        skin = 1,  
        bone = 2,  
        tendon = 3,
        joint = 4, 
        inflammatory = 5,
        intensity = 6,
        noise = 7,
		bloodLevel = 8,
		inflammatoryLevel = 9
    };

    typedef boost::bimap<annotationsIdx, QString> AdnotationsType;
    DEFINE_SMART_POINTERS(AdnotationsType);

    AdnotationsTypePtr getDefault();
    void save(const core::Filesystem::Path& p, AdnotationsTypeConstPtr adnotations);
    AdnotationsTypePtr load(const core::Filesystem::Path& p);

    AdnotationsTypePtr instance();
    void autoSave();

    core::Filesystem::Path adnotationsFile();


	enum bloodLevelDescriptor {

		unknownBloodLevel = -1,
		bloodLevel0,
		bloodLevel1,
		bloodLevel2,
		bloodLevel3
	};

	typedef boost::bimap<bloodLevelDescriptor, QString> BloodLevelsType;
	DEFINE_SMART_POINTERS(BloodLevelsType);

	BloodLevelsTypePtr getDefaultBloodLevels();	

	BloodLevelsTypePtr instanceBloodLevels();

	enum inflammatoryLevelDescriptor {

		unknownInflammatoryLevel = -1,
		inflammatoryLevel0,
		inflammatoryLevel1,
		inflammatoryLevel2,
		inflammatoryLevel3
	};

	typedef boost::bimap<inflammatoryLevelDescriptor, QString> InflammatoryLevelsType;
	DEFINE_SMART_POINTERS(InflammatoryLevelsType);

	InflammatoryLevelsTypePtr getDefaultInflammatoryLevels();	

	InflammatoryLevelsTypePtr instanceInflammatoryLevels();
}
}

#endif
