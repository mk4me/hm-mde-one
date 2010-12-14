#ifndef TOOLBOXMAIN_H
#define TOOLBOXMAIN_H

#include <QtGui/QMainWindow>
#include <QtGui/QtGui>
#include <QtGui/QMenu>
#include <QtGui/QAction>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>

#include <streambuf>
#include <QtCore/QVector>

#include <core/PluginLoader.h>
#include <core/Window.h>
#include <core/DataManager.h>

class UserInterfaceService;
class ServiceManager;
class IAnimationService;
class ModelService;
class RenderService;
class ComputeThread;

QT_BEGIN_NAMESPACE
class QMainWindowPrivate;
class QObject; 
class ConsoleWidget; 
class ViewerQT; 
class TimeLine; 
class GridWidget;
QT_END_NAMESPACE

namespace Ui
{
	class ToolboxMain;
}

class ToolboxMain : public QMainWindow, core::Window
{
    Q_OBJECT

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


public slots:    
    //! Aktualizacja us³ug.
    void updateServices();

    void onOpen();

    
    void onExit();
    void onMaterial();
    void onBones();
    void onWireframe();
    void onCustomAction();
    void onCustomAction(bool triggered);


// core::Window
protected:
//     //! Natywne dodanie opcji do menu.
//     virtual void onAddMenuItem( const std::string& path );
// 
//     
//     //! Natywne dodanie opcji z flag¹ do menu.
//     virtual void onAddMenuToggle( const std::string& path, bool state );
    //! Natywne usuniêcie opcji z menu.
    virtual void onRemoveMenuItem( const std::string& path );

private:
    //! Tworzy siatkê rozci¹gniêt¹ na p³aszczyŸnie.
    osg::ref_ptr<osg::Node> createGrid(); 

    //! Rejestruje wbudowane us³ugi.
    void registerCoreServices(); 
    //! Rejestruje us³ugi pochodz¹ce z pluginów.
    void registerPluginsServices();

    //! Wype³nia podmenu akcjami dla dostêpnych okien.
    //! \param target
    void populateWindowMenu(QMenu* target);

    QDockWidget* embeddWidget(QWidget* widget, QString& name, QString& style, Qt::DockWidgetArea area = Qt::AllDockWidgetAreas);

    void onAddMenuItem( const std::string& path, bool checkable, bool initialState );


protected:
    void closeEvent(QCloseEvent* event);

private: 
    void LoadConfiguration();

    void ReadSettings();
    void WriteSettings();

private:    
    ServiceManager* m_pServiceManager;
	//zasoby (model, triale)
    DataManager* dataManager;
    UserInterfaceService* m_pUserInterfaceService;
    ModelService* m_pModelService;

    CORE_SHARED_PTR(RenderService) m_pRenderService;

    Ui::ToolboxMain* ui;

    // TODO: Embedded widgets - should be in plugins !!
    GridWidget* _gridWidget;
    ConsoleWidget* _consoleWidget; 

    // Stary bufor cout
    std::streambuf* _streambuf; 

    //! W¹tek obliczaj¹cy.
    ComputeThread* computeThread;

    //! Nazwa pliku konfiguracyjnego.
    static const QString configName;
    //! Nazwa organizacji.
    static const QString organizationName;
    //! Nazwa aplikacji.
    static const QString applicationName;

    //! Timer wyznaczaj¹cy update'y.
    QTimer updateTimer;
    //! Korzeñ sceny.
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
};

#endif // TOOLBOXMAIN_H
