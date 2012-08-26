/********************************************************************
    created:  2012/02/16
    created:  16:2:2012   12:59
    filename: IDataSourceFilterManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEFILTERMANAGER_H__
#define HEADER_GUARD___IDATASOURCEFILTERMANAGER_H__

#include <plugins/newCommunication/DataSourceFilter.h>
#include <core/SmartPtr.h>

namespace communication
{

typedef core::shared_ptr<DataSourceFilter> DataFilterPtr;
typedef core::shared_ptr<const DataSourceFilter> DataFilterConstPtr;

typedef core::weak_ptr<DataSourceFilter> DataFilterWPtr;
typedef core::weak_ptr<const DataSourceFilter> DataFilterWConstPtr;


//! Interfejs zarządzający filtrami danych
class IDataSourceFilterManager
{
public:
    //Wirtualny destruktor
    virtual ~IDataSourceFilterManager() {}

    //! \param filter Filtr danych który rejestrujemy
    //! \return Indeks zarejestrowanego filtra
    virtual int registerFilter(DataSourceFilter * filter) = 0;

    //! \param idx Indeks filtra, zakres indeksu [-1; size()-1], -1 oznacza brak filtra
    virtual void setCurrentFilter(int idx) = 0;

    //! \return Ilość zarejestrowanych filtrów
    virtual int size() const = 0;

    //! \return Czy mamy zarejestrowane jakieś filtry
    virtual bool empty() const = 0;

    //! \return Indeks aktualnie aktywnego  filtru - -1 gdy żaden nieaktywny
    virtual int currentFilterIndex() const = 0;

	//! \return Wskaźnik do aktualnego filtra - nullptr jeśli nie ustawiono żadnego
	virtual DataFilterPtr currentFilter() const = 0;

    //! \param idx Indeks filtra
    //! \return Filtr o zadanym indeksie
    virtual DataFilterPtr dataFilter(int idx) const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCEFILTERMANAGER_H__
