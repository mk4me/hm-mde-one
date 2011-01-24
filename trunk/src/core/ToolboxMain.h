#ifndef TOOLBOXMAIN_H
#define TOOLBOXMAIN_H

#include <QtGui/QMainWindow>
#include <QtGui/QtGui>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>
#include <osgViewer/CompositeViewer>

#include <streambuf>
#include <QtCore/QVector>

#include <core/PluginLoader.h>
#include <core/Window.h>
#include <core/DataManager.h>

class UserInterfaceService;
class ServiceManager;
class IAnimationService;
class RenderService;
class ComputeThread;
class SceneGraphWidget;
class ConsoleWidget; 

namespace Ui
{
	class ToolboxMain;
}

class ToolboxMain : public QMainWindow, public core::Window, public osgViewer::CompositeViewer
{
    Q_OBJECT

private:
    enum WidgetsOrganization {
        WidgetsOrganizationTabbed,
        WidgetsOrganizationDocked,
    };



public:
    ToolboxMain(QWidget* parent = 0);

    void initializeUI();
    
    ~ToolboxMain();

    // TODO: Embedded widgets initializations - should be in plugins!!! 
    void InitializeOGSWidget();
    void initializeConsole();
    void InitializeControlWidget();

    void clear();

public:
    void openFile( const std::string& path );
    void openLayout( const QString& path );

	void loadData();

    ConsoleWidget* getConsole()
    {
        return _consoleWidget;
    }

public slots:    
    //! Aktualizacja us�ug.
    void updateServices();

    void onOpen();

    
    void onExit();
    void onMaterial();
    void onBones();
    void onWireframe();
    void onCustomAction();
    void onCustomAction(bool triggered);

    void onSaveLayout();
    void onOpenLayout();
    void onShowSavedLayouts();

    void addLayoutsToMenu( QDir &dir );
    void onTabbedViewSelected(bool toggled);
    void onDockableViewSelected(bool toggled);
    void onDockWidgetVisiblityChanged(bool visible);
    void onLayoutTriggered();

// core::Window
protected:    
    //! Natywne dodanie opcji do menu.
    virtual void onAddMenuItem( const std::string& path, bool checkable, bool initialState );
    //! Natywne usuni�cie opcji z menu.
    virtual void onRemoveMenuItem( const std::string& path );


private:
    //! Tworzy siatk� rozci�gni�t� na p�aszczy�nie.
    osg::ref_ptr<osg::Node> createGrid(); 

    //! Rejestruje wbudowane us�ugi.
    void registerCoreServices(); 
    //! Rejestruje us�ugi pochodz�ce z plugin�w.
    void registerPluginsServices();

    //! Wype�nia podmenu akcjami dla dost�pnych okien.
    //! \param target Menu do wype�nienia.
    void populateWindowMenu(QMenu* target);

    //! Opakowuje zadany widget QDockWidgetem.
    //! \param widget
    //! \param name
    //! \param style
    //! \param area
    QDockWidget* embeddWidget(QWidget* widget, const QString& name, const QString& style, const QString& sufix, Qt::DockWidgetArea area = Qt::AllDockWidgetAreas);

    void reorganizeWidgets(WidgetsOrganization organization);



protected:
    void closeEvent(QCloseEvent* event);

private: 
    void LoadConfiguration();

    void readSettings(const QSettings& settings, bool readGeometry);
    void WriteSettings();

private:    
    ServiceManager* m_pServiceManager;
	//zasoby (model, triale)
    DataManager* dataManager;
    UserInterfaceService* m_pUserInterfaceService;

    CORE_SHARED_PTR(RenderService) m_pRenderService;

    Ui::ToolboxMain* ui;

    // TODO: Embedded widgets - should be in plugins !!
    SceneGraphWidget* sceneGraphWidget;
    ConsoleWidget* _consoleWidget; 

    // Stary bufor cout
    std::streambuf* _streambuf; 

    //! W�tek obliczaj�cy.
    ComputeThread* computeThread;

    //! Nazwa pliku konfiguracyjnego.
    static const QString configName;
    //! Nazwa organizacji.
    static const QString organizationName;
    //! Nazwa aplikacji.
    static const QString applicationName;

    //!
    bool updateEnabled;
    //! Timer wyznaczaj�cy update'y.
    QTimer updateTimer;
    //! Korze� sceny.
    osg::ref_ptr<osg::Node> sceneRoot;
    //! Pluginy.
    core::PluginLoader* pluginLoader;


// kod testowo-tymczasowy
private:
    void onTestItemClicked(const std::string& sender, bool state);
    void onTestRemoveToggled(const std::string& sender, bool state);
    core::Window::ItemPressedPtr onTestItemClickedPtr;
    core::Window::ItemPressedPtr onTestRemoveToggledPtr;
    bool removeOnClick;

        QTimer _timer;

        virtual void paintEvent( QPaintEvent* event );
};

#endif // TOOLBOXMAIN_H
