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

//! Interfejs podstawowych element�w logicznych aplikacji maj�cych operowa� na danych b�d� dostarcza� danych
class IWorkflowItemBase : 
    public IClonable<IWorkflowItemBase>, public ISerializable, public IIdentifiable, public IConfigurable
{
public:
    virtual ~IWorkflowItemBase() 
    {
    }


public:
    //! \return Nazwa elementu logicznego
    virtual const std::string & getName() const = 0;
    //! Defaultowo serializacja nic nie robi, kiedy� to wytniemy i b�dzie trzeba doimplementowa�!!
    virtual void serialize( std::ostream& output ) { }
    //! Defaultowo deserializacja nic nie robi, kiedy� to wytniemy i b�dzie trzeba doimplementowa�!!
    virtual void deserialize( std::istream& istream ) { }
    //! Defaultowo nie mamy widgeta konfiguracyjnego, kiedy� to wytniemy i b�dzie trzeba doimplementowa�!!
    virtual QWidget* getConfigurationWidget() { return nullptr; }
    //! Metoda resetuj�ca stan elementu po styczno�ci z jedn� porcj� danych
    //TODO
    //przejrzec ta metode - inne zachowanie dla �r�de� i inne dla wizualizator�w
    virtual void reset() = 0;

};

typedef shared_ptr<IWorkflowItemBase> IWorkflowItemBasePtr;
typedef shared_ptr<const IWorkflowItemBasePtr> IWorkflowItemBaseConstPtr;

}
#endif