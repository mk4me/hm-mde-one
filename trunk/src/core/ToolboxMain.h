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

#include <core/PluginsInterfaces.h>


class PluginService;
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
    ~ToolboxMain();

    void LoadPlugins(); 
    // TODO: Embedded widgets initializations - should be in plugins!!! 
    void InitializeOGSWidget();
    void InitializeConsoleWidget();
    void InitializeControlWidget();

    void CreateMenus(); 
    void LoadWindowMenuWidget();
    osg::ref_ptr<osg::Node> CreateGrid(); 
    void InitializeCoreServices(); 
    void Clear();

public slots: 
    void Open();
    void SettingModel();
    void WireFrameView();
    void BoneView();
    void MaterialView();

    void PointViewModel();
    void LinesViewModel();
    void LineStripViewModel();
    void LineLoopViewModel();
    void TriangleViewModel();
    void TriangleStripViewModel();
    void TriangleFunViewModel();
    void QuadsViewModel();
    void QuadStripViewModel();
    void PolygonViewModel();

    //! Aktualizacja us³ug.
    void updateServices();


protected:
    void closeEvent(QCloseEvent* event);

private: 
    void LoadConfiguration();

    void ReadSettings();
    void WriteSettings();



private:    
    ServiceManager* m_pServiceManager;
    PluginService* m_pPluginService;
    UserInterfaceService* m_pUserInterfaceService;
    IAnimationService* m_pAnimationService;
    ModelService* m_pModelService;
    RenderService* m_pRenderService;

    Ui::ToolboxMain* _ui;
    QVector<QObject*> _plugins; 

    // TODO: Embedded widgets - should be in plugins !!
    GridWidget* _gridWidget;
    ConsoleWidget* _consoleWidget; 
    TimeLine* _timeLine; 

    // Stary bufor cout
    std::streambuf* _streambuf; 

    // Menu 
    QMenu* _windowMenu;
    ComputeThread* computeThread;

    friend class QMainWindowPrivate;

    // Application settings data
    static const QString _settingsOrganizationName;
    static const QString _settingsApplicationName;

    QTimer updateTimer;
};

#endif // TOOLBOXMAIN_H
