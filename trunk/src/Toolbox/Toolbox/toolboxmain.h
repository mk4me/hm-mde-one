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


QT_BEGIN_NAMESPACE
class QObject; 
class OsgControlWidget;
class ConsoleWidget; 
class ViewerQT; 
class TimeLine; 
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
		osg::Node* createGrid(); 
		~ToolboxMain();

	public slots: 
		void open();

	private: 
		void initializePlugin(QObject* plugin);
		void loadConfiguration();

		void drawSkeletonFromGroup(osg::Group* root);
		void drawBone(osg::PositionAttitudeTransform* bone, const osg::Vec3d* parentPos, const osg::Quat* parentRot, osg::Geode* geode);

	private:    
		Ui::ToolboxMain* m_ui;

		QVector<QObject*> m_plugins; 

		OsgControlWidget* m_osgControlWidget; 
		ConsoleWidget* m_consoleWidget; 

		ViewerQT* m_osgView; 
		TimeLine* m_timeLine; 

		// Stary bufor cout
		std::streambuf* m_streambuf; 

		// Menu 
		QMenu* m_fileMenu; 

		// Akcje 
		QAction* m_openAct;

		// scene root
		osg::Group* m_root;
};

#endif // TOOLBOXMAIN_H
