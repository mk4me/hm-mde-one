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

//! Interfejs managera zawartości
class IDataSourceContentManager
{
public:
	//! Wirtualny destruktor
	virtual ~IDataSourceContentManager() {}
	//! \param content Generator zawartości który rejestrujemy
	//! \return indeks zarejestrowanego generatora zawartości
	virtual int registerContent(IDataSourceContent * content) = 0;
	//! \return Indeks aktualnego generatora zawartości
	virtual int currentContentIndex() const = 0;
	//! \return Aktualny generator zawartości
	virtual DataSourceContentPtr currentContent() const = 0;
	//! \param idx Indeks nowego generatora zawartości
	virtual void setCurrentContent(int idx) = 0;
	//! \return Czy jest zarejestrowany jakiś generator zawartości
	virtual bool empty() const = 0;
	//! \return Ilośc zarejestrowanych generatorów zawartości
	virtual int size() const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCECONTENTMANAGER_H__
