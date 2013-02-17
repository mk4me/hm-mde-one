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
#include <core/IDataProcessorManager.h>
#include <core/IDataSinkManager.h>
#include <core/IDataSourceManager.h>

#include "Command.h"
#include "ui_TypesWindow.h"
#include "CanvasStyleEditor.h"
#include "SceneModel.h"

class NewVdfWidget;


class TypesWindow : public QWidget, private Ui::TypesWindow,
    public utils::Observer<core::IDataProcessorManager>, public utils::Observer<core::IDataSinkManager>, utils::Observer<core::IDataSourceManager>
{
    Q_OBJECT;
public:
    explicit TypesWindow(CommandStackPtr stack, CanvasStyleEditorPtr canvas, NewVdfWidget* newVdf, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~TypesWindow() {}

public:
   // void addType();
    void insert(const QString& name, const QPointF& scenePos);

protected slots:
    void insert();
	void run();

protected:
    void update(const core::IDataProcessorManager* pm);
    void update(const core::IDataSourceManager* sm);
    void update(const core::IDataSinkManager* sm);

private:
    void addEntry(const QString& entry);
    SceneBuilder::VisualNodeWithPins createItemByEntry(const QString& entry);

private:
    CanvasStyleEditorPtr canvas;
	CommandStackPtr commmandStack;
    NewVdfWidget* newVdf;
    std::map<QString, core::IWorkflowItemBasePtr> name2node;
};



#endif
