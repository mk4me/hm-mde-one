/********************************************************************
    created:  2014/02/18
    created:  18:2:2014   19:37
    filename: LanguageChangeFilter.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD_CORE__LANGUAGECHANGEFILTER_H__
#define HEADER_GUARD_CORE__LANGUAGECHANGEFILTER_H__

#include <QtCore/QObject>

namespace core
{
	//! RAII dla zmiany jezyka - filtr �apie fakt instalacji/usuni�cia translator�w
	//! i udaje �e obs�u�ono ju� te zmiany
	//! Na wyj�ciu emituje jeden komunikat o zmianie jesli wczesniej faktycznie
	//! dokonano zmian w ramach translator aplikacji
	class LanguageChangeFilter : public QObject
	{
		Q_OBJECT

	public:
		//! Konstruktor domy�lny
		LanguageChangeFilter();

		//! Destruktor wirtualny
		virtual ~LanguageChangeFilter();

		virtual bool eventFilter(QObject * object, QEvent * event);

	private:
		//! Czy faktycznie zmieniono j�zyk poprzez instalacj� b�d� usuni�ci� translator�w?
		bool languageChanged;
	};
}

#endif	//	HEADER_GUARD_CORE__LANGUAGECHANGEFILTER_H__
