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

//! Interfejs managera zawartoœci
class IDataSourceContentManager
{
public:
	//! Wirtualny destruktor
	virtual ~IDataSourceContentManager() {}
	//! \param content Generator zawartoœci który rejestrujemy
	//! \return indeks zarejestrowanego generatora zawartoœci
	virtual int registerContent(IDataSourceContent * content) = 0;
	//! \return Indeks aktualnego generatora zawartoœci
	virtual int currentContentIndex() const = 0;
	//! \return Aktualny generator zawartoœci
	virtual DataSourceContentPtr currentContent() const = 0;
	//! \param idx Indeks nowego generatora zawartoœci
	virtual void setCurrentContent(int idx) = 0;
	//! \return Czy jest zarejestrowany jakiœ generator zawartoœci
	virtual bool empty() const = 0;
	//! \return Iloœc zarejestrowanych generatorów zawartoœci
	virtual int size() const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCECONTENTMANAGER_H__
