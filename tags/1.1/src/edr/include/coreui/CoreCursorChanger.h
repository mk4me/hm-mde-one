/********************************************************************
    created:  2014/01/09
    created:  9:1:2014   19:14
    filename: CoreCursorChanger.h
    author:   Mateusz Janiak
    
    purpose:  RAII dla chwilowej zmiany kursora w GUI
*********************************************************************/
#ifndef HEADER_GUARD___CORECURSORCHANGER_H__
#define HEADER_GUARD___CORECURSORCHANGER_H__

#include <coreui/Export.h>
#include <QtCore/qnamespace.h>

namespace coreUI
{
	//! Klasa u³atwiaj¹ca tymczasow¹ zmianê kursora, przywracaj¹c poprzedni automatycznie
	class COREUI_EXPORT CoreCursorChanger
	{
	public:
		//! \param cursorShape Tymczasowy kszta³t kursora
		CoreCursorChanger(const Qt::CursorShape cursorShape = Qt::BusyCursor);
		//! Destruktor
		~CoreCursorChanger();
	};
}

#endif	//	HEADER_GUARD___CORECURSORCHANGER_H__
