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

		void loadPlugins(); 
		void initializeOGSWidget();
		void initializeConsoleWidget();
		void initializeControlWidget();

		void createActions(); 
		void createMenus(); 
		osg::ref_ptr<osg::Node> createGrid(); 
		~ToolboxMain();

        void InitializeCoreServices(); 
        

	public slots: 
		void open();
        void setingModel();
        void wireFrameView();
        void normalView();
        void boneView();
        void materialView();

        inline void pointViewModel();
        inline void linesViewModel();
        inline void line_stripViewModel();
        inline void line_loopViewModel();
        inline void triangleViewModel();
        inline void triangle_stripViewModel();
        inline void triangle_funViewModel();
        inline void quadsViewModel();
        inline void quad_stripViewModel();
        inline void polygonViewModel();

    protected:
        void closeEvent(QCloseEvent* event);
        ObjectService* _pScene;
        CModelWithSkeleton* _model;

        CModel* _model2;
        

	private: 
		void initializePlugin(QObject* plugin);
		void loadConfiguration();

        void readSettings();
        void writeSettings();
	
	private:    
        ServiceManager* _pServiceManager;
		Ui::ToolboxMain* _ui;

		QVector<QObject*> _plugins; 

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


		// Akcje 
		QAction* _openAct;
        QAction* _setModelTest;
        QAction* _viewWFAct;
        QAction* _viewMaterialAct;
        QAction* _viewBoneAct;

        // Akcje Modelu
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
   

        // Dane u¿ywane przy przechowywaniu ustawieñ aplikacji
        static const QString _settingsOrganizationName;
        static const QString _settingsApplicationName;
};

#include "toolboxmain.inl"

#endif // TOOLBOXMAIN_H
