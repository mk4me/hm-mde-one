/********************************************************************
	created:	2011/05/12
	created:	12:5:2011   9:10
	filename: 	IWorkflowItemBase.h
	
	purpose:	
*********************************************************************/


#ifndef HEADER_GUARD_CORE__IWORKFLOWITEMBASE_H__
#define HEADER_GUARD_CORE__IWORKFLOWITEMBASE_H__

#include <QtGui/QIcon>
#include <utils/Debug.h>
#include <corelib/SmartPtr.h>
#include <corelib/BaseDataTypes.h>

namespace df {
	class INode;
}

class QDialog;

namespace vdf {


// TODO : posprzatac tutaj

//! Interfejs podstawowych elementów logicznych aplikacji mających operować na danych bądź dostarczać danych
class IWorkflowItemBase // : public IClonable<IWorkflowItemBase>, public IIdentifiable
    //public IClonable<IWorkflowItemBase>, public ISerializable, public IIdentifiable, public IConfigurable
{
public:
    IWorkflowItemBase() {}
    virtual ~IWorkflowItemBase() 
    {
    }


public:
    //! \return Nazwa elementu logicznego
    virtual const std::string getName() const = 0;
    ////! Defaultowo serializacja nic nie robi, kiedyś to wytniemy i będzie trzeba doimplementować!!
    //virtual void serialize( std::ostream& output ) { }
    ////! Defaultowo deserializacja nic nie robi, kiedyś to wytniemy i będzie trzeba doimplementować!!
    //virtual void deserialize( std::istream& istream ) { }
    ////! Defaultowo nie mamy widgeta konfiguracyjnego, kiedyś to wytniemy i będzie trzeba doimplementować!!
    virtual QWidget* getConfigurationWidget() { return nullptr; }
    //! Metoda resetująca stan elementu po styczności z jedną porcją danych
    //TODO
    //przejrzec ta metode - inne zachowanie dla źródeł i inne dla wizualizatorów
    //virtual void reset() = 0;
	virtual df::INode* getPrototype() const { UTILS_ASSERT(false); return nullptr; }
	virtual df::INode* create() const { UTILS_ASSERT(false); return nullptr; }
	virtual core::UniqueID getID() const = 0;
	//virtual IWorkflowItemBase* createClone() const = 0;

    QIcon getIcon() const { return icon; }
    void setIcon(const QIcon& val) { icon = val; }
private:
    QIcon icon;
};

typedef core::shared_ptr<IWorkflowItemBase> IWorkflowItemBasePtr;
typedef core::shared_ptr<const IWorkflowItemBasePtr> IWorkflowItemBaseConstPtr;

}
#endif
