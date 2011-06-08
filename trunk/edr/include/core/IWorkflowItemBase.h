/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:10
	filename: 	IWorkflowItemBase.h
	
	purpose:	
*********************************************************************/


#ifndef HEADER_GUARD_CORE__IWORKFLOWITEMBASE_H__
#define HEADER_GUARD_CORE__IWORKFLOWITEMBASE_H__

#include <core/SmartPtr.h>
#include <core/IClonable.h>
#include <core/ISerializable.h>
#include <core/IIdentifiable.h>
#include <core/IConfigurable.h>

namespace core {

class IWorkflowItemBase : 
    public IClonable<IWorkflowItemBase>, public ISerializable, public IIdentifiable, public IConfigurable
{
public:
    virtual ~IWorkflowItemBase() 
    {
    }


public:
    virtual const std::string & getName() const = 0;
    virtual void serialize( std::ostream& output ) { }
    virtual void deserialize( std::istream& istream ) { }
    virtual QWidget* getConfigurationWidget(void) { return nullptr; }

};

typedef shared_ptr<IWorkflowItemBase> IWorkflowItemBasePtr;
typedef shared_ptr<const IWorkflowItemBasePtr> IWorkflowItemBaseConstPtr;

}
#endif