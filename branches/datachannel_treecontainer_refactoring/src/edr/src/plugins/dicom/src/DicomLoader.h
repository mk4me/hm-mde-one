
/********************************************************************
	created:	2015/08/25
	created:	12:26
	filename: 	DicomLoader.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_DICOM__DicomLoader_H__
#define HEADER_GUARD_DICOM__DicomLoader_H__

#include <plugins/dicom/ILayeredImage.h>
#include "DicomInternalStruct.h"

namespace  dicom {
	//! Kalsa realizuj�ca odczyt danych USG
	class DicomLoader
	{
	public:
		//! \param from �cie�ka z kt�rej pr�bujemy czyta� wewn�trzn� reprezentacj� danych USG
		//! \return Drzewo danych USG
		static DicomInternalStructPtr load(const core::Filesystem::Path& from);
		//! \param p �cie�ka z k�rej pr�bujemy czyta� warstwy adnotacji opusuj�ce obraz USG
		//! \return Warstwy adnotacji opusuj�ce obraz USG
		static LayersVectorPtr loadLayers(const core::Filesystem::Path &p);
	};
	DEFINE_SMART_POINTERS(DicomLoader);
}

#endif // HEADER_GUARD 
