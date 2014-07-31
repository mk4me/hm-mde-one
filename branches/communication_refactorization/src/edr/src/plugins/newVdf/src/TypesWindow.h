/********************************************************************
	created:	2012/12/17
	created:	17:12:2012   8:56
	filename: 	TypesWindow.h
	author:		Wojciech Kniec
	
	purpose:	
*********************************************************************/

#ifndef HEADER_GUARD_NEWVDF__TYPESWINDOW_H__
#define HEADER_GUARD_NEWVDF__TYPESWINDOW_H__


#include <QtWidgets/QWidget>
#include <utils/ObserverPattern.h>
#include <utils/CommandStack.h>
#include <plugins/newVdf/IDataProcessorManager.h>
#include <plugins/newVdf/IDataSinkManager.h>
#include <plugins/newVdf/IDataSourceManager.h>

#include "Command.h"
#include "CanvasStyleEditor.h"
#include "SceneModel.h"

namespace Ui { class TypesWindow; } 

namespace vdf {

class NewVdfWidget;


//! Widget z elementami do wstawienia na scen�
class TypesWindow : public QWidget, 
    public utils::Observer<vdf::IDataProcessorManager>, public utils::Observer<vdf::IDataSinkManager>, public utils::Observer<vdf::IDataSourceManager>
{
    Q_OBJECT;
public:
    //! Konstruktor
    //! \param stack Stos komend (umo�liwia m.in. undo)
    //! \param canvas Model ze stylami element�w
    //! \param sceneModel Scena, na kt�r� trafi� elementy 
    //! \param parent 
    //! \param f 
    explicit TypesWindow(utils::ICommandStackPtr stack, CanvasStyleEditorPtr canvas, SceneModelPtr sceneModel, QWidget* parent = 0, Qt::WindowFlags f = 0);
	virtual ~TypesWindow() {}

public:
    // TODO : zmienic idedtyfikacje z name na uid

    //! Tworzy element na podstawie nazwy i umieszcza go na scenie, komenda trafia na stos wywo�a�.
    //! \param name nazwa elementu, kt�ry zostanie utworzony
    //! \param scenePos pozycja na scenie, gdzie trafi obiekt
    void insert(const QString& name, const QPointF& scenePos);

    //! Tworzy element, kt�ry jest gotowy aby go wstawi� na scen�
    //! \param entry nazwa/identyfikator elementu
    SceneBuilder::VisualNodeWithPins createItemByEntry(const QString& entry);

    core::UniqueID getId(const QString& name) const;
        
protected:
    //! Wywo�ywane, gdy pojawi� si� nowe elementy przetwarzaj�ce
    //! \param pm 
    void update(const IDataProcessorManager* pm);
    //! Wywo�ywane, gdy pojawi� si� nowe �r�d�a
    //! \param sm 
    void update(const IDataSourceManager* sm);
    //! Wywo�ywane, gdy pojawi� si� nowe sinki
    //! \param sm 
    void update(const IDataSinkManager* sm);

private:
    //! Dodanie nowego elementu do odpwiedniego widgeta
    //! \param entry nazwa/identyfikator elementu
    //! \param icon ikona elementu
    //! \param type typ elementu, warunkuje, do kt�rego widgeta trafi wpis
    void addEntry(const QString& entry, const QIcon& icon, IVisualItem::Type type);

private:
    //! Model ze stylami elment�w
    CanvasStyleEditorPtr canvas;
    //! Stos komend (umo�liwia m.in. undo)
	utils::ICommandStackPtr commmandStack;
    //! Scena, na kt�r� trafi� elementy
    SceneModelPtr sceneModel;
    //! nazwa/identyfikator elementu -> prototyp elementu
    std::map<QString, IWorkflowItemBasePtr> name2node;
    //! ui tworzone w designerze
    Ui::TypesWindow* ui;
};

}

#endif
