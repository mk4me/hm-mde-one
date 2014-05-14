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

	//! Indeksy adnotacji
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
		inflammatoryLevel = 9,
		fingerType = 10,
		jointType = 11,
		imageType = 12
    };

	//! Mapowanie indesków adnotacji do ich nazw
    typedef boost::bimap<annotationsIdx, QString> AdnotationsType;
    DEFINE_SMART_POINTERS(AdnotationsType);

	//! Tworzy domyœlne mapowanie indeksów
    AdnotationsTypePtr getDefault();
	//! \param p Œciazka pliku do którego zapisujemy  typy adnotacji
	//! \param adnotations Zapisywane typy adnotacji
    void save(const core::Filesystem::Path& p, AdnotationsTypeConstPtr adnotations);
	//! \param p Œcie¿ka z której wczytujemy typy adnotacji
    AdnotationsTypePtr load(const core::Filesystem::Path& p);
	//! Zwraca instancje typów adnotacji
    AdnotationsTypePtr instance();
	//! Zapisuje aktualne typy adnotacji
    void autoSave();
	//! \return Œcie¿ka pliku z typami adnotacji
    core::Filesystem::Path adnotationsFile();
	
	//! Typ poziomu krwi
	enum bloodLevelDescriptor {

		unknownBloodLevel = -1,
		bloodLevel0,
		bloodLevel1,
		bloodLevel2,
		bloodLevel3
	};

	//! Mapiowanie indeksów do nazw z ich opisem
	typedef boost::bimap<bloodLevelDescriptor, QString> BloodLevelsType;
	DEFINE_SMART_POINTERS(BloodLevelsType);

	//! \return Zwraca domyœlne poziomy krwi
	BloodLevelsTypePtr getDefaultBloodLevels();	
	//! Zwracaj instancjê poziomów krwi
	BloodLevelsTypePtr instanceBloodLevels();

	//! Typy poziomów schorzenia
	enum inflammatoryLevelDescriptor {

		unknownInflammatoryLevel = -1,
		inflammatoryLevel0,
		inflammatoryLevel1,
		inflammatoryLevel2,
		inflammatoryLevel3
	};

	//! Mapowanie typów poziomów schorzenia do ich opisów
	typedef boost::bimap<inflammatoryLevelDescriptor, QString> InflammatoryLevelsType;
	DEFINE_SMART_POINTERS(InflammatoryLevelsType);

	//! \return Domyœlne typy poziomów schorzenia
	InflammatoryLevelsTypePtr getDefaultInflammatoryLevels();	
	//! \return Instancja poziomów schorzenia
	InflammatoryLevelsTypePtr instanceInflammatoryLevels();

	//! Nowe rzeczy

	//! Typ palca
	enum fingerTypeDescriptor {

		unknownFinger = -1,
		finger1,
		finger2,
		finger3,
		finger4,
		finger5
	};

	//! Mapiowanie indeksów do nazw z ich opisem
	typedef boost::bimap<fingerTypeDescriptor, QString> FingerType;
	DEFINE_SMART_POINTERS(FingerType);

	//! \return Zwraca domyœlne poziomy krwi
	FingerTypePtr getDefaultFingerTypes();	
	//! Zwracaj instancjê poziomów krwi
	FingerTypePtr instanceFingerTypes();

	//! Typ jointa
	enum jointTypeDescriptor {

		unknownJoint = -1,
		LPIP,
		RPIP,
		LMCP,
		RMCP		
	};

	//! Mapiowanie indeksów do nazw z ich opisem
	typedef boost::bimap<jointTypeDescriptor, QString> JointType;
	DEFINE_SMART_POINTERS(JointType);

	//! \return Zwraca domyœlne poziomy krwi
	JointTypePtr getDefaultJointTypes();	
	//! Zwracaj instancjê poziomów krwi
	JointTypePtr instanceJointTypes();

	//! Typ jakoœci obrazu
	enum imageTypeDescriptor {
		incorrect = 0,
		different = 1,
		identical = 2,
	};

	//! Mapiowanie indeksów do nazw z ich opisem
	typedef boost::bimap<imageTypeDescriptor, QString> ImageType;
	DEFINE_SMART_POINTERS(ImageType);

	//! \return Zwraca domyœlne poziomy krwi
	ImageTypePtr getDefaultImageTypes();	
	//! Zwracaj instancjê poziomów krwi
	ImageTypePtr instanceImageTypes();

}

}

#endif
