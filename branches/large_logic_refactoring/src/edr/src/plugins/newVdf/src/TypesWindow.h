/********************************************************************
	created:	2012/12/17
	created:	17:12:2012   8:56
	filename: 	TypesWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__TYPESWINDOW_H__
#define HEADER_GUARD_NEWVDF__TYPESWINDOW_H__


#include <QtGui/QWidget>
#include <utils/ObserverPattern.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <plugins/newVdf/IDataSourceManager.h>

#include "Command.h"
#include "ui_TypesWindow.h"
#include "CanvasStyleEditor.h"
#include "SceneModel.h"

namespace vdf {

class NewVdfWidget;


class TypesWindow : public QWidget, private Ui::TypesWindow,
    public utils::Observer<vdf::IDataProcessorManager>, public utils::Observer<vdf::IDataSinkManager>, public utils::Observer<vdf::IDataSourceManager>
{
    Q_OBJECT;
public:
    explicit TypesWindow(CommandStackPtr stack, CanvasStyleEditorPtr canvas, NewVdfWidget* newVdf, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~TypesWindow() {}

public:
    void insert(const QString& name, const QPointF& scenePos);

public slots:
    void onNodeSelected(IVisualNodePtr node);
    
protected:
    void update(const IDataProcessorManager* pm);
    void update(const IDataSourceManager* sm);
    void update(const IDataSinkManager* sm);

private:
    void addEntry(const QString& entry, const QIcon& icon, IVisualItem::Type type);
    SceneBuilder::VisualNodeWithPins createItemByEntry(const QString& entry);

private:
    CanvasStyleEditorPtr canvas;
	CommandStackPtr commmandStack;
    NewVdfWidget* newVdf;
    std::map<QString, IWorkflowItemBasePtr> name2node;
};

}

#endif
