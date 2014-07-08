/********************************************************************
    created:  2012/02/16
    created:  16:2:2012   13:11
    filename: DataSourceFilterManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___DATASOURCEFILTERMANAGER_H__
#define HEADER_GUARD___DATASOURCEFILTERMANAGER_H__

#include <plugins/newCommunication/IDataSourceFilterManager.h>
#include <plugins/newCommunication/DataSourceShallowCopy.h>

class DataSourceFilterManager : public communication::IDataSourceFilterManager
{
public:
    DataSourceFilterManager();
    virtual ~DataSourceFilterManager();

	//! Metoda usuwa wszystkie filtry
	void clearFilters();

    //! \param filter Filtr danych który rejestrujemy
    //! \return Indeks zarejestrowanego filtra
    virtual int registerFilter(DataSourceFilter * filter);

    //! \param idx Indeks filtra, zakres indeksu [-1; size()-1], -1 oznacza brak filtra
    virtual void setCurrentFilter(int idx);

    //! \return Ilość zarejestrowanych filtrów
    virtual int size() const;

    //! \return Czy mamy zarejestrowane jakieś filtry
    virtual bool empty() const;

    //! \return Aktualnie aktywny  filtr - -1 gdy żaden nieaktywny
    virtual int currentFilterIndex() const;

	//! \return Aktualnie aktywny  filtr - -1 gdy żaden nieaktywny
	virtual communication::DataFilterPtr currentFilter() const;

    //! \param idx Indeks filtra
    //! \return Filtr o zadanym indeksie
    virtual communication::DataFilterPtr dataFilter(int idx) const;

	//! \param inShallow Wejściowa płytka kopia do filtrowania
	//! \param outShallow Wyjściowa, przefiltrowana płytka kopia
	//! \param filter Filtr używany do filtrowania
	static void filterShallowCopy(const communication::ShallowCopy & inShallow, communication::ShallowCopy & outShallow, const communication::DataFilterPtr & filter);

private:
    //! Zarejestrowane filtry
    std::vector<communication::DataFilterPtr> filters;
    //! Indeks aktualnego filtra
    int currentFilter_;
};

#endif	//	HEADER_GUARD___DATASOURCEFILTERMANAGER_H__
