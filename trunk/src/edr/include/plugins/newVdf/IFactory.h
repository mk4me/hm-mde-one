/********************************************************************
    created:  2012/11/19
    created:  19:11:2012   8:36
    filename: IFactory.h
    author:   Mateusz Janiak
    
    purpose:  
*********************************************************************/
#ifndef HEADER_GUARD___IFACTORY_H__
#define HEADER_GUARD___IFACTORY_H__

#include <QtCore/QUuid>
#include <QtCore/QString>
#include <corelib/SmartPtr.h>

namespace vdf {

class IFactory
{
protected:
	IFactory() {}

public:

	virtual ~IFactory() {}

	virtual const QUuid id() const = 0;
	virtual const QString name() const = 0;
	virtual const QString description() const = 0;
};
typedef core::shared_ptr<IFactory> IFactoryPtr;
typedef core::shared_ptr<const IFactory> IFactoryConstPtr;

}

#endif	//	HEADER_GUARD___IFACTORY_H__
