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

class ToolboxMain : public QMainWindow
{
    Q_OBJECT

public:
    ToolboxMain(QWidget* parent = 0);

    void initializeUI();
    
    ~ToolboxMain();

    // TODO: Embedded widgets initializations - should be in plugins!!! 
    void InitializeOGSWidget();
    void InitializeConsoleWidget();
    void InitializeControlWidget();

    void Clear();

public slots:    
    //! Aktualizacja us�ug.
    void updateServices();

    void onOpen();
    void onExit();
    void onMaterial();
    void onBones();
    void onWireframe();

private:
    //! Tworzy siatk� rozci�gni�t� na p�aszczy�nie.
    osg::ref_ptr<osg::Node> createGrid(); 

    //! Rejestruje wbudowane us�ugi.
    void registerCoreServices(); 
    //! Rejestruje us�ugi pochodz�ce z plugin�w.
    void registerPluginsServices();

    //! Wype�nia podmenu akcjami dla dost�pnych okien.
    //! \param target
    void populateWindowMenu(QMenu* target);


protected:
    void closeEvent(QCloseEvent* event);

private: 
    void LoadConfiguration();

    void ReadSettings();
    void WriteSettings();



private:    
    ServiceManager* m_pServiceManager;
    
    UserInterfaceService* m_pUserInterfaceService;
    ModelService* m_pModelService;
    CORE_SHARED_PTR(RenderService) m_pRenderService;

    Ui::ToolboxMain* ui;

    // TODO: Embedded widgets - should be in plugins !!
    GridWidget* _gridWidget;
    ConsoleWidget* _consoleWidget; 

    // Stary bufor cout
    std::streambuf* _streambuf; 

    //! W�tek obliczaj�cy.
    ComputeThread* computeThread;

    //! Nazwa organizacji.
    static const QString organizationName;
    //! Nazwa aplikacji.
    static const QString applicationName;

    //! Timer wyznaczaj�cy update'y.
    QTimer updateTimer;
    //! Korze� sceny.
    osg::ref_ptr<osg::Node> sceneRoot;
    //! Pluginy.
    core::PluginLoader* pluginLoader;
};

#endif // TOOLBOXMAIN_H
