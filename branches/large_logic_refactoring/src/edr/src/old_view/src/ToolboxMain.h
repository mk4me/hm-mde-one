#ifndef TOOLBOXMAIN_H
#define TOOLBOXMAIN_H

#include <coreui/CoreMainWindow.h>

class QMenu;

namespace Ui {
	class EDRMain;
}

class ToolboxMain : public coreUI::CoreMainWindow
{
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
	
private:

	Ui::EDRMain * ui;
};

#endif // TOOLBOXMAIN_H
