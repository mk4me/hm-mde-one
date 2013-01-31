#ifndef TOOLBOXMAIN_H
#define TOOLBOXMAIN_H

#include <utils/Debug.h>
#include <osgViewer/CompositeViewer>
#include <QtCore/QVector>

#include <coreui/CoreMainWindow.h>

#include "ui_toolboxmaindeffile.h"
#include <corelib/Filesystem.h>

class ToolboxMain : public coreUI::CoreMainWindow, private Ui::EDRMain
{
public:
	Q_OBJECT
public:
	ToolboxMain(const CloseUpOperations & closeUpOperations);
	virtual ~ToolboxMain();

private:

	virtual void customViewInit(QWidget * console);

public slots:

	void onExit();

private slots:
	//! Wypełnia podmenu akcjami dla dostępnych okien.
	//! \param target Menu do wypełnienia.
	void populateWindowMenu();
	//! Wypełnia podmenu akcjami tworzącymi wizualizatory.
	void populateVisualizersMenu();
	//!
	void actionCreateVisualizer();

private:
	//!
	void populateWindowMenu( QMenu* menu );
	//!
	void populateVisualizersMenu( QMenu* menu );

	void initializeUI();
	
};

#endif // TOOLBOXMAIN_H
