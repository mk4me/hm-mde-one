/********************************************************************
    created:  2012/03/14
    created:  14:3:2012   16:57
    filename: IDataSourcePerspectiveManager.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IDATASOURCEPERSPECTIVEMANAGER_H__
#define HEADER_GUARD___IDATASOURCEPERSPECTIVEMANAGER_H__

#include <core/SmartPtr.h>
#include <plugins/newCommunication/IDataSourcePerspective.h>

namespace communication
{

	typedef core::shared_ptr<IDataSourcePerspective> DataSourcePerspectivePtr;

//! Interfejs managera perspektyw danych
class IDataSourcePerspectiveManager
{
public:
	//! Wirtualny destruktor
	virtual ~IDataSourcePerspectiveManager() {}

	//! \param perspective Perspektywa do zarejestrowania
	virtual int registerPerspective(IDataSourcePerspective * perspective) = 0;
	//! \return ilośc zarejestrowanych perspektyw
	virtual int perspectivesCount() const = 0;
	//! \param idx Index aktualnej perspektywy
	virtual void setCurrentPerspective(int idx) = 0;
	//! \return Aktualna perspektywa
	virtual DataSourcePerspectivePtr currentPerspective() const = 0;
	//! \return Index aktualnej perspektywy
	virtual int currentPerspectiveIndex() const = 0;
};

}

#endif	//	HEADER_GUARD___IDATASOURCEPERSPECTIVEMANAGER_H__
