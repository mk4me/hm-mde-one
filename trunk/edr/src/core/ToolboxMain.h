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
#include <utils/Debug.h>

#include <QtCore/QVector>

#include <core/PluginLoader.h>
#include <core/Window.h>
#include <core/DataManager.h>

#include "ui_toolboxmaindeffile.h"

class UserInterfaceService;
class ServiceManager;
class IAnimationService;
//class RenderService;
//class ComputeThread;
class SceneGraphWidget;
//class ConsoleWidget;
class EDRConsoleWidget;
class VisualizerManager;
class DataProcessorManager;
class VisualizerWidget;


class ToolboxMain : public QMainWindow, public core::Window, public osgViewer::CompositeViewer, private Ui::EDRMain
{
    Q_OBJECT

private:    
    //! Widget ze scen� jakiego� grafu OSG.
    SceneGraphWidget* widgetSceneGraph;
    //! Widget konsoli.
    //ConsoleWidget* widgetConsole; 
    EDRConsoleWidget* widgetConsole; 
    //! W�tek obliczeniowy.
    //ComputeThread* computeThread;
    //! Czy update jest w��czony?
    bool updateEnabled;
    //! Timer wyznaczaj�cy update'y.
    QTimer updateTimer;
    //! Korze� sceny.
    osg::ref_ptr<osg::Node> sceneRoot;
    //! Pluginy.
    core::PluginLoader* pluginLoader;
    //! Timer u�ywany gdy wy�wietlamy w trybie composite.
    QTimer viewerFrameTimer;


public:
    ToolboxMain(core::PluginLoader* pluginLoader);
    virtual ~ToolboxMain();

    void initializeUI();
    
    

    // TODO: Embedded widgets initializations - should be in plugins!!! 
    void InitializeOGSWidget();
    void initializeConsole();
    void InitializeControlWidget();

public:
    void openFile( const std::string& path );
    void openLayout( const QString& path );

	void loadData();

    //ConsoleWidget* getConsole()
    EDRConsoleWidget* getConsole()
    {
        return widgetConsole;
    }

public slots:    
    void refreshVisualizerWidgetsNames();
    void visualizerWidgetClosed(VisualizerWidget* widget);
    //! Aktualizacja us�ug.
    void updateServices();
    //! Aktualizacja wizualizatorow.
    void updateVisualizers();

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
    void onDockWidgetVisiblityChanged(bool visible);
    void onLayoutTriggered();

// core::Window
protected:    
    //! Natywne dodanie opcji do menu.
    virtual void onAddMenuItem( const std::string& path, bool checkable, bool initialState );
    //! Natywne usuni�cie opcji z menu.
    virtual void onRemoveMenuItem( const std::string& path );

private slots:
    //! Wype�nia podmenu akcjami dla dost�pnych okien.
    //! \param target Menu do wype�nienia.
    void populateWindowMenu();
    //! Wype�nia podmenu akcjami tworz�cymi wizualizatory.
    void populateVisualizersMenu();
    //!
    void actionCreateVisualizer();

    void createWorkflow();

private:

    //!
    void populateWindowMenu( QMenu* menu );
    //!
    void populateVisualizersMenu( QMenu* menu );

    //! Tworzy siatk� rozci�gni�t� na p�aszczy�nie.
    osg::ref_ptr<osg::Node> createGrid(); 

    //! Rejestruje wbudowane us�ugi.
    void registerCoreServices();
    //! Rejestruje wbudowane �r�d�a danych.
    void registerCoreDataSources();

    //! Rejestruje us�ugi pochodz�ce z plugin�w.
    void registerPluginsServices();
    //! Rejestruje parsery pochodz�ce z plugin�w.
    void registerPluginsParsers();
    //!
    void registerPluginsVisualizers();
    //!
    void registerPluginsDataProcessors();

    void registerPluginsDataSources();

    void registerPluginsWrapperFactories();


    //! Opakowuje zadany widget QDockWidgetem.
    //! \param widget
    //! \param name
    //! \param style
    //! \param area
    QDockWidget* embeddWidget(QWidget* widget, std::vector<QObject*>& widgetActions, const QString& name, const QString& style, const QString& sufix, Qt::DockWidgetArea area = Qt::AllDockWidgetAreas);

    void safeRegisterPlugin(const core::PluginPtr & plugin);
    void safeRegisterService(const core::IServicePtr & service);
    void safeRegisterParser(const core::IParserPtr & parser);
    void safeRegisterObjectFactory(const core::IObjectWrapperFactoryPtr & factory);
    void safeRegisterVisualizer(const core::IVisualizerPtr & visualizer);
    void safeRegisterDataProcessor(const core::IDataProcessorPtr & dataProcessor);
    void safeRegisterDataSource(const core::IDataSourcePtr & dataSource);

// QWidget
protected:
    //!
    virtual void closeEvent(QCloseEvent* event);
    //!
    virtual void paintEvent( QPaintEvent* event );

private: 
    //! Odczytuje ustawienia aplikacji.
    //! \param settings
    //! \param readGeometry
    void readSettings(const QSettings& settings, bool readGeometry);
    //! Zapisuje ustawienia aplikacji.
    void writeSettings();





// kod testowo-tymczasowy
#ifdef UTILS_DEBUG
private:
    void onTestItemClicked(const std::string& sender, bool state);
    void onTestRemoveToggled(const std::string& sender, bool state);
    core::Window::ItemPressedPtr onTestItemClickedPtr;
    core::Window::ItemPressedPtr onTestRemoveToggledPtr;
    bool removeOnClick;
#endif // UTILS_DEBUG

};

#endif // TOOLBOXMAIN_H
