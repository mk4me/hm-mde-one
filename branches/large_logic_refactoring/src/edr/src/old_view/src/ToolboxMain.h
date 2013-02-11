#ifndef TOOLBOXMAIN_H
#define TOOLBOXMAIN_H

#include <coreui/CoreMainWindow.h>
#include <corelib/Visualizer.h>

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

class ToolboxMain : public coreUI::CoreMainWindow, private core::Visualizer::IVisualizerObserver
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

	virtual void update(core::Visualizer::VisualizerSerie * serie, core::Visualizer::SerieModyfication modyfication );
	
private:
	QMainWindow * visualizersPlaceholder;
	Ui::EDRMain * ui;
	std::map<core::Visualizer::VisualizerSerie *, std::pair<int, core::shared_ptr<timeline::IChannel>>> seriesToChannels;
};

#endif // TOOLBOXMAIN_H
