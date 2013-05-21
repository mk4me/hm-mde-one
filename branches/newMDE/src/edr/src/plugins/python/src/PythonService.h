/********************************************************************
	created:	2012/12/04
	created:	4:12:2012   13:29
	filename: 	PythonService.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_PYTHON__PYTHONSERVICE_H__
#define HEADER_GUARD_PYTHON__PYTHONSERVICE_H__

#include <QtCore/QObject>
#include <core/IService.h>
#include "PythonLogic.h"

class QTextEdit;

class PythonService : public QObject, public core::IService
{
    Q_OBJECT;
	UNIQUE_ID("{2AB10F39-C5C9-4637-A562-85D15EAE118A}", "Python Service");

public:
	//! 
	PythonService();
	//! 
	virtual ~PythonService();

//core::IService
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

private slots:
    void runScript();

private:
    PythonLogic logic;
    QTextEdit* editor;
};

#endif //   HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__
