/********************************************************************
	created:	2012/12/13
	created:	13:12:2012   11:59
	filename: 	NewVdfService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__NEWVDFSERVICE_H__
#define HEADER_GUARD_NEWVDF__NEWVDFSERVICE_H__

#include <core/IService.h>
#include <core/IDataManager.h>
#include "Command.h"

class NewVdfWidget;
class CanvasStyleEditorWidget;
class TypesWindow;

class NewVdfService : public core::IService
{
    UNIQUE_ID("{DF5B5B15-C591-4BCF-A205-FD995D2398DB}", "Data Flow Service");

public:
    NewVdfService();
    virtual ~NewVdfService();

public:
    //! 
    //! \param managersAccessor 
    virtual void init(core::IManagersAccessor * managersAccessor);
    //! 
    //! \param actions 
    virtual QWidget* getWidget( core::IActionsGroupManager * actionsManager );
    //! 
    //! \param actions 
    virtual QWidget* getSettingsWidget( core::IActionsGroupManager * actionsManager );
    //! 
    virtual const std::string& getName() const;
    //! 
    //! \param actions 
    virtual QWidget* getControlWidget( core::IActionsGroupManager * actionsManager );

private:
    std::string name;
    NewVdfWidget* newVdfWidget;
    CanvasStyleEditorWidget* canvasStyleEditorWidget;
    TypesWindow* typesWindow;
	QListWidget* commandStackDebug;
	CommandStackPtr commandStack;
};

#endif  //HEADER_GUARD___VDFSERVICE_H__

