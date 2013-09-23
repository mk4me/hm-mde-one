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
#include <corelib/IService.h>
#include "PythonLogic.h"

class QTextEdit;

class PythonService : public QObject, public plugin::IService
{
    Q_OBJECT;
	UNIQUE_ID("{2AB10F39-C5C9-4637-A562-85D15EAE118A}");

    CLASS_DESCRIPTION("Python Service", "Python Service");

public:
	//! 
	PythonService();
	//! 
	virtual ~PythonService();

//core::IService
public:
    virtual void init( core::ISourceManager * sourceManager, core::IVisualizerManager * visualizerManager, core::IMemoryDataManager * memoryDataManager, core::IStreamDataManager * streamDataManager, core::IFileDataManager * fileDataManager );
    virtual QWidget* getWidget();


private slots:
    void runScript();

public:
    virtual const bool lateInit();

    virtual void finalize();

    virtual void update( double deltaTime );

    virtual QWidgetList getPropertiesWidgets();

private:
    PythonLogic logic;
    QTextEdit* editor;
};

#endif //   HEADER_GUARD_SUBJECT__SUBJECTSERVICE_H__
