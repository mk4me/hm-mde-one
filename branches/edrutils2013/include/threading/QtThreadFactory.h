/********************************************************************
    created:  2013/07/02
    created:  2:7:2013   9:06
    filename: QtThreadFactory.h
    author:   Mateusz Janiak
    
    purpose:  Klasa realizuje funkcjonalnoœæ fabryki w¹tków w oparciu
				o bibliotekê Qt
*********************************************************************/
#ifndef HEADER_GUARD___QTTHREADFACTORY_H__
#define HEADER_GUARD___QTTHREADFACTORY_H__

#include <threading/IThreadFactory.h>

namespace utils
{
	class QtThreadFactory : public IThreadFactory
	{
	public:
		//! Konstruktor domyœlny
		QtThreadFactory();
		//! Destrutkor wirtualny
		virtual ~QtThreadFactory();
		//! \return WskaŸnik do nowoutworoznego w¹tku
		virtual IThreadPtr createThread();
	};
}

#endif	//	HEADER_GUARD___QTTHREADFACTORY_H__
