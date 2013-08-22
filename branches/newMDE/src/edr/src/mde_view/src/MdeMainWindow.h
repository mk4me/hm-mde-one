#ifndef HMM_TOOLBOXMAIN_H
#define HMM_TOOLBOXMAIN_H

#include <map>
#include <QtGui/QToolButton>
#include "IMdeTab.h"
#include <corelib/PluginCommon.h>
#include <coreui/CoreMainWindow.h>
#include <coreui/CoreDockWidget.h>
#include "IAppUsageContextManager.h"
#include "ContextEventFilter.h"
#include "AnalisisModel.h"

class QWidget;
class MdeServiceWindow;
class MdeMainWindow;
namespace Ui { class HMMMain; }

class MdeMainWindowController : public QObject
{
    Q_OBJECT;
public:
    MdeMainWindowController(MdeMainWindow* mw);

public Q_SLOTS:
    void addTab(IMdeTabPtr tab);
    void activateTab(IMdeTabPtr tab);

private Q_SLOTS:
    void buttonClicked();

private:
    MdeMainWindow* window;
    //! mapa [przycisk -> zakładka]
    std::map<QToolButton*, IMdeTabPtr> button2TabWindow;
    //! mapa [zakładka -> jej kontekst]
    std::map<IMdeTabPtr, IAppUsageContextPtr> tab2Contex;
};
//! Klasa realizuje widok aplikacji dla medyków
//! Z czasem klasa zaczela się rozrastac, wymaga glebszej refaktoryzacji
class MdeMainWindow : public coreUI::CoreMainWindow, public IAppUsageContextManager//, private core::Visualizer::IVisualizerObserver
{
    Q_OBJECT;
    friend class MdeMainWindowController;

public:
    MdeMainWindow(const CloseUpOperations & closeUpOperations);
    virtual ~MdeMainWindow();

public:
    virtual void showSplashScreenMessage(const QString & message);
    AnalisisModelConstPtr getAnalysisModel() const { return analysisModel; }
    ContextEventFilterPtr getContextEventFilter() const { return contextEventFilter; }

Q_SIGNALS:
    void tabAdded(IMdeTabPtr tab);
    void activateTab(IMdeTabPtr tab);

private:	
	virtual void initializeSplashScreen(QSplashScreen * splashScreen);
	virtual void customViewInit(QWidget * console);

    void addPropertiesToServiceWindow( plugin::IServicePtr service, MdeServiceWindow * w );
    MdeServiceWindow* createServiceWidget( plugin::IServicePtr service );
    
    void addTab(IMdeTabPtr tab);

private:
    std::list<IMdeTabPtr> tabs;
    MdeMainWindowController controller;

    // TODO : utworzyc klase widoku i przeniesc to tam
    QTabWidget * contextPlaceholder;
    AnalisisModelPtr analysisModel;
    ContextEventFilterPtr contextEventFilter;
    Ui::HMMMain* ui;
};


#endif // TOOLBOXMAIN_H
