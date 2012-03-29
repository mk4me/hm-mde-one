/********************************************************************
    created:  2012/03/24
    created:  24:3:2012   10:12
    filename: IDataSourceContentManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCECONTENTMANAGER_H__
#define HEADER_GUARD___IDATASOURCECONTENTMANAGER_H__

#include <plugins/newCommunication/IDataSourceContent.h>

namespace communication {

//! Interfejs managera zawarto�ci
class IDataSourceContentManager
{
public:
	//! Wirtualny destruktor
	virtual ~IDataSourceContentManager() {}
	//! \param content Generator zawarto�ci kt�ry rejestrujemy
	//! \return indeks zarejestrowanego generatora zawarto�ci
	virtual int registerContent(IDataSourceContent * content) = 0;
	//! \return Indeks aktualnego generatora zawarto�ci
	virtual int currentContentIndex() const = 0;
	//! \return Aktualny generator zawarto�ci
	virtual DataSourceContentPtr currentContent() const = 0;
	//! \param idx Indeks nowego generatora zawarto�ci
	virtual void setCurrentContent(int idx) = 0;
	//! \return Czy jest zarejestrowany jaki� generator zawarto�ci
	virtual bool empty() const = 0;
	//! \return Ilo�c zarejestrowanych generator�w zawarto�ci
	virtual int size() const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCECONTENTMANAGER_H__
