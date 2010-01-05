#ifndef TOOLBOXMAIN_H
#define TOOLBOXMAIN_H

#include <QtGui/QMainWindow>
#include <QtGui>
#include <QMenu>
#include <QAction>
#include <osg/Node>

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
    ToolboxMain(QWidget *parent = 0);

    void loadPlugins(); 
    void initializeOGSWidget();
    void initializeConsoleWidget();
    void initializeControlWidget();

    void createActions(); 
    void createMenus(); 
    osg::Node *createGrid(); 
    ~ToolboxMain();

public slots: 
  void open();

private: 
  void initializePlugin(QObject *plugin);
  void loadConfiguration();
private:    
  Ui::ToolboxMain *ui;

  QVector<QObject *> plugins_; 

  OsgControlWidget *osgControlWidget_; 
  ConsoleWidget *consoleWidget_; 

  ViewerQT *osgView_; 
  TimeLine *timeLine_; 

  // Stary bufor cout
  std::streambuf *streambuf_; 

  // Menu 
  QMenu *fileMenu_; 

  // Akcje 
  QAction *openAct_;
};

#endif // TOOLBOXMAIN_H
