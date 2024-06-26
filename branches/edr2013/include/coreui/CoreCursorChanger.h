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
	//! Klasa ułatwiająca tymczasową zmianę kursora, przywracając poprzedni automatycznie
	class COREUI_EXPORT CoreCursorChanger
	{
	public:
		//! \param cursorShape Tymczasowy kształt kursora
		CoreCursorChanger(const Qt::CursorShape cursorShape = Qt::BusyCursor);
		//! Destruktor
		~CoreCursorChanger();
	};
}

#endif	//	HEADER_GUARD___CORECURSORCHANGER_H__
