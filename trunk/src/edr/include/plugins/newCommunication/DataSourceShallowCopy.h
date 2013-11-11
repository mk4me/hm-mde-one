/********************************************************************
    created:  2012/03/08
    created:  8:3:2012   11:01
    filename: DataSourceShallowCopy.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCESHALLOWCOPY_H__
#define HEADER_GUARD___DATASOURCESHALLOWCOPY_H__

#include <webserviceslib/ShallowCopy.h>
#include <utils/SmartPtr.h>

namespace communication
{
	typedef utils::shared_ptr<webservices::MotionShallowCopy::ShallowCopy> MotionShallowCopyPtr;
	typedef utils::shared_ptr<webservices::MedicalShallowCopy::ShallowCopy> MedicalShallowCopyPtr;

	//! Struktura zawierająca sparsowane płytkie kopie bazy danych medycznej i ruchu oraz ich metadane
	struct ShallowCopy
	{
		//! Płytka kopia bazy danych medycznych
		MedicalShallowCopyPtr medicalShallowCopy;
		//! Metadane medyczne
		webservices::MedicalMetaData::MetaData medicalMetaData;
		//! Płytka kopia bazy danych ruchu
		MotionShallowCopyPtr motionShallowCopy;
		//! Metadane ruchu
		webservices::MotionMetaData::MetaData motionMetaData;

		//! Konstruktor domyślny
		ShallowCopy() : medicalShallowCopy(new webservices::MedicalShallowCopy::ShallowCopy()), motionShallowCopy(new webservices::MotionShallowCopy::ShallowCopy()) {}
	};
}

#endif	//	HEADER_GUARD___DATASOURCESHALLOWCOPY_H__
