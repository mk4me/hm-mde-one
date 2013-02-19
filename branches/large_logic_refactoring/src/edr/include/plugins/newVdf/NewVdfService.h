/********************************************************************
	created:	2012/12/13
	created:	13:12:2012   11:59
	filename: 	NewVdfService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__NEWVDFSERVICE_H__
#define HEADER_GUARD_NEWVDF__NEWVDFSERVICE_H__

#include <corelib/IService.h>
#include <corelib/IDataManagerReader.h>
#include "Command.h"

class NewVdfWidget;
class CanvasStyleEditorWidget;
class TypesWindow;

class NewVdfService : public plugin::IService
{
    UNIQUE_ID("{DF5B5B15-C591-4BCF-A205-FD995D2398DB}")
	CLASS_DESCRIPTION("Data Flow Service", "Data Flow Service");

public:
    NewVdfService();
    virtual ~NewVdfService();

public:
    //! 
    //! \param managersAccessor 
	virtual void init(core::ISourceManager * sourceManager,
		core::IVisualizerManager * visualizerManager,
		core::IMemoryDataManager * memoryDataManager,
		core::IStreamDataManager * streamDataManager,
		core::IFileDataManager * fileDataManager);
    //! 
    //! \param actions 
    virtual QWidget* getWidget();
    //! 
    //! \param actions 
    virtual QWidget* getSettingsWidget();
    //! 
    //! \param actions 
    virtual QWidget* getControlWidget();

	virtual const bool lateInit();
	virtual void finalize();
	virtual void update(double time);

private:
    std::string name;
    NewVdfWidget* newVdfWidget;
    CanvasStyleEditorWidget* canvasStyleEditorWidget;
    TypesWindow* typesWindow;
	QListWidget* commandStackDebug;
	CommandStackPtr commandStack;
};

#endif  //HEADER_GUARD___VDFSERVICE_H__

