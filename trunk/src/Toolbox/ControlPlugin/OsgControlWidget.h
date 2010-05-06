#ifndef OSG_CONTROL_WIDGET_H
#define OSG_CONTROL_WIDGET_H

#include <QtGui/QWidget>
#include <osgViewer/Scene>
#include <osg/Group>
#include <osg/Geode>
#include <osg/NodeVisitor>

#include "ui_OsgWidget.h"

class OsgControlWidget: public QWidget, Ui::OsgTest
{
  Q_OBJECT
public:
  OsgControlWidget(void);
  ~OsgControlWidget(void);

  void setScene(osgViewer::Scene *scene); 
  void clearScene(); 

public slots: 
  virtual void makeTest();
  void addGroupToTreeView(osg::Group *group, QTreeWidgetItem *parentTreeItem);
  void addGeodeToTreeView(osg::Geode *geode, QTreeWidgetItem *parentTreeItem);
};


#endif
