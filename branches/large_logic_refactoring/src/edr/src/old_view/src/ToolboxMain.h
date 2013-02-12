#ifndef TOOLBOXMAIN_H
#define TOOLBOXMAIN_H

#include <coreui/CoreMainWindow.h>
#include <corelib/Visualizer.h>
#include <plugins/newTimeline/VisualizerSerieTimelineChannel.h>

namespace timeline {

	class IChannel;

}

class QMenu;

namespace Ui {
	class EDRMain;
}

namespace coreUI {
	class CoreDockWidget;
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

	static coreUI::CoreDockWidget * embeddWidget(QWidget * widget, const QString & windowTitle, Qt::DockWidgetArea allowedAreas, bool permanent);	
	
private:
	QMainWindow * visualizersPlaceholder;
	Ui::EDRMain * ui;
	VisualizerTimelineHelper * helper;
};

#endif // TOOLBOXMAIN_H
