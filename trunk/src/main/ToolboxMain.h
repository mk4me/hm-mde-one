#ifndef TOOLBOXMAIN_H
#define TOOLBOXMAIN_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include <QMenu>
#include <QAction>
#include <osg/Node>
#include <osg/PositionAttitudeTransform>
#include <osg/Geometry>

#include <streambuf>
#include <QVector>

#include <PluginsInterfaces.h>

#include "ObjectService.h"
#include "ServiceManager.h"
#include "ModelWithSkeleton.h"

QT_BEGIN_NAMESPACE
class QObject; 
class OsgControlWidget;
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

    void CreateActions(); 
    void CreateMenus(); 
    osg::ref_ptr<osg::Node> CreateGrid(); 
    void InitializeCoreServices(); 

public slots: 
    void Open();
    void SettingModel();
    void WireFrameView();
    void NormalView();
    void BoneView();
    void MaterialView();

    // TODO:  refactorng KONIECZNIE - tymczasem dla testów, spróbowac zrobiæ jedna medote zarz¹cdaj¹ca tymi pozosta³ymi
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

protected:
    void closeEvent(QCloseEvent* event);
    ObjectService* _pScene;
    ModelWithSkeleton* _modelWithSkeleton;
    Model* _model;

private: 
    void InitializePlugin(QObject* plugin);
    void LoadConfiguration();

    void ReadSettings();
    void WriteSettings();

private:    
    ServiceManager* _pServiceManager;
    Ui::ToolboxMain* _ui;
    QVector<QObject*> _plugins; 

    // TODO: Embedded widgets - should be in plugins !!
    OsgControlWidget* _osgControlWidget; 
    GridWidget* _gridWidget;
    ConsoleWidget* _consoleWidget; 
    ViewerQT* _osgView; 
    TimeLine* _timeLine; 

    // Stary bufor cout
    std::streambuf* _streambuf; 

    // Menu 
    QMenu* _fileMenu; 
    QMenu* _modelMenu;

    // Actions 
    QAction* _openAct;
    QAction* _setModelTest;
    QAction* _viewWFAct;
    QAction* _viewMaterialAct;
    QAction* _viewBoneAct;

    // Model actions
    QAction* _NoneAct;
    QAction* _points;
    QAction* _lines;
    QAction* _line_strip;
    QAction* _line_loop;
    QAction* _triangle;
    QAction* _triangle_strip;
    QAction* _triangle_fun;
    QAction* _quads;
    QAction* _quad_strip;
    QAction* _polygon;

    // Application settings data
    static const QString _settingsOrganizationName;
    static const QString _settingsApplicationName;
};

#endif // TOOLBOXMAIN_H
